#include "ConceptEngine.h"

#include <d3dcompiler.h>


#include "CECommandList.h"
#include "CECommandQueue.h"
#include "CED3DCubeObject.h"
#include "CEHelper.h"
#include "CEIndexBuffer.h"
#include "CERootSignature.h"
#include "CESwapChain.h"
#include "d3dx12.h"

using namespace Concept;

ConceptEngine::ConceptEngine(HINSTANCE hInstance) {
#if defined(_DEBUG)
	CEDevice::EnableDebugLayer();
#endif
	auto& pGame = CEGame::Create(hInstance);
	{
		/*
		 * Create logger for logging messages
		 */
		logger = pGame.CreateLogger("Concept Engine");

		/*
		 * Create GPU device using default adapter selection
		 */
		pDevice = CEDevice::Create();

		auto description = pDevice->GetDescription();
		logger->info("Device Created: {}", description);

		/*
		 * Use copy queue to copy GPU resources.
		 * TODO: Move to CED3DObject class to create basic objects in one function call
		 */
		auto& commandQueue = pDevice->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
		auto commandList = commandQueue.GetCommandList();

		/*
		 * Cube object
		 */
		pCube = std::make_shared<Objects::CED3DCubeObject>();

		auto vertices = pCube->GetVertices();
		auto vertexStride = pCube->GetVertexStride();
		auto indices = pCube->GetIndices();
		auto indexStride = pCube->GetIndexStride();

		/*
		 * Load vertex data:
		 */
		pVertexBuffer = commandList->CopyVertexBuffer(vertices.size(), vertexStride, vertices.data());

		/*
		 * Load index data:
		 */
		pIndexBuffer = commandList->CopyIndexBuffer(indices.size(), DXGI_FORMAT_R16_UINT, indices.data());

		/*
		 * Execute command list to upload resources to GPU.
		 */
		commandQueue.ExecuteCommandList(commandList);

		/*
		 * Create window
		 */
		pGameWindow = pGame.CreateWindow(L"Concept Engine Editor", 1920, 1080);
		spdlog::info("Concept Engine Screen class created.");

		/*
		 * Create Swap chain for window
		 */
		pSwapChain = pDevice->CreateSwapChain(pGameWindow->GetWindowHandle());
		pSwapChain->SetVSync(false);

		/*
		 * Register events.
		 */
		pGameWindow->KeyPressed += &OnKeyPressed;
		pGameWindow->MouseWheel += &OnMouseWheel;
		pGameWindow->MouseButtonPressed += &OnMouseButtonPress;
		pGameWindow->Resize += &OnWindowResized;
		pGameWindow->Update += &OnUpdate;
		pGameWindow->Close += &OnWindowClose;

		/*
		 * Create vertex input layout
		 */
		D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
			{
				"POSITION",
				0,
				DXGI_FORMAT_R32G32B32_FLOAT,
				0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
				0
			},
			{
				"COLOR",
				0,
				DXGI_FORMAT_R32G32B32_FLOAT,
				0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
				0
			}
		};

		/*
		 * Create root signature
		 * Allow input layout and deny unnecessary access to certain pipeline stages.
		 */
		D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

		/*
		 * Single 32-bit constant root parameter that is used by vertex shader.
		 */
		CD3DX12_ROOT_PARAMETER1 rootParameters[1];
		rootParameters[0].InitAsConstants(sizeof(DirectX::XMMATRIX) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription(
			_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);

		pRootSignature = pDevice->CreateRootSignature(rootSignatureDescription.Desc_1_1);

		/*
		 * Load shaders
		 * Load vertex shader.
		 */
		Microsoft::WRL::ComPtr<ID3DBlob> vertexShaderBlob;
		ThrowIfFailed(D3DReadFileToBlob(L"CEGEBasicVertexShader.cso", &vertexShaderBlob));

		/*
		 * Load index shader.
		 */
		Microsoft::WRL::ComPtr<ID3DBlob> pixelShaderBlob;
		ThrowIfFailed(D3DReadFileToBlob(L"CEGEBasicPixelShader.cso", &pixelShaderBlob));

		/*
		 * Create pipeline state object
		 */
		struct PipelineStateStream {
			CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
			CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
			CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
			CD3DX12_PIPELINE_STATE_STREAM_VS VS;
			CD3DX12_PIPELINE_STATE_STREAM_PS PS;
			CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
			CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
		} pipelineStateStream;

		pipelineStateStream.pRootSignature = pRootSignature->GetD3D12RootSignature().Get();
		pipelineStateStream.InputLayout = {inputLayout, _countof(inputLayout)};
		pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(vertexShaderBlob.Get());
		pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(pixelShaderBlob.Get());
		pipelineStateStream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		pipelineStateStream.RTVFormats = pSwapChain->GetRenderTarget().GetRenderTargetFormats();

		pPipelineStateObject = pDevice->CreatePipelineStateObject(pipelineStateStream);

		/*
		 * Make sure index/vertex buffers are loaded before rendering first frame.
		 */
		commandQueue.Flush();

		pGameWindow->Show();
	}
}

int ConceptEngine::Run() {
	// return RunGraphics();
	return RunEngine();
}

int ConceptEngine::RunEngine() {
	int returnCode = 0;
	{
		returnCode = CEGame::Get().Run();
		/*
		 * Release global variables
		 */
		pIndexBuffer.reset();
		pVertexBuffer.reset();
		pPipelineStateObject.reset();
		pRootSignature.reset();
		pDepthTexture.reset();
		pDevice.reset();
		pSwapChain.reset();
		pGameWindow.reset();
	}

	/*
	 * Destroy CEGame resource.
	 */
	CEGame::Destroy();

	atexit(&CEDevice::ReportLiveObjects);

	return returnCode;
}

void OnUpdate(UpdateEventArgs& e) {
	static uint64_t frameCount = 0;
	static double totalTime = 0.0;

	totalTime += e.DeltaTime;
	++frameCount;

	if (totalTime > 1.0) {
		auto fps = frameCount / totalTime;
		frameCount = 0;
		totalTime = 0.0;

		logger->info("FPS: {:.7}", fps);

		wchar_t buffer[256];
		::swprintf_s(buffer, L"Concept Engine [FPS: %f]", fps);
		pGameWindow->SetWindowTitle(buffer);
	}

	/*
	 * Use render target from swapchain
	 */
	auto renderTarget = pSwapChain->GetRenderTarget();

	/*
	 * Set render target (with depth texture)
	 */
	renderTarget.AttachTexture(AttachmentPoint::DepthStencil, pDepthTexture);

	auto viewPort = renderTarget.GetViewPort();

	/*
	 * Update model matrix
	 */
	float angle = static_cast<float>(e.TotalTime * 90.0);
	const DirectX::XMVECTOR rotationAxis = DirectX::XMVectorSet(0, 1, 1, 0);
	DirectX::XMMATRIX modelMatrix = DirectX::XMMatrixRotationAxis(rotationAxis, DirectX::XMConvertToRadians(angle));

	/*
	 * Update view matrix
	 */
	const DirectX::XMVECTOR cameraPosition = DirectX::XMVectorSet(0, 0, -10, 1);
	const DirectX::XMVECTOR focusPoint = DirectX::XMVectorSet(0, 0, 0, 1);
	const DirectX::XMVECTOR upDirection = DirectX::XMVectorSet(0, 1, 0, 0);
	DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookAtLH(cameraPosition, focusPoint, upDirection);

	/*
	 * Update projection matrix.
	 */
	float aspectRatio = viewPort.Width / viewPort.Height;
	DirectX::XMMATRIX projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XMConvertToRadians(pCube->GetFieldOfView()), aspectRatio, 0.1f, 100.0f);
	DirectX::XMMATRIX mvpMatrix = DirectX::XMMatrixMultiply(modelMatrix, viewMatrix);
	mvpMatrix = DirectX::XMMatrixMultiply(mvpMatrix, projectionMatrix);

	auto& commandQueue = pDevice->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
	auto commandList = commandQueue.GetCommandList();

	/*
	 * Set pipeline state object
	 */
	commandList->SetPipelineState(pPipelineStateObject);
	/*
	 * set root signatures
	 */
	commandList->SetGraphicsRootSignature(pRootSignature);

	/*
	 * Set root parameters
	 */
	commandList->SetGraphics32BitConstants(0, mvpMatrix);

	/*
	 * Create clear color
	 */
	// auto& renderTarget = pSwapChain->GetRenderTarget();
	const FLOAT clearColor[] = {0.4f, 0.6f, 0.9f, 1.0f};
	commandList->ClearTexture(renderTarget.GetTexture(AttachmentPoint::Color0), clearColor);
	commandList->ClearDepthStencilTexture(pDepthTexture, D3D12_CLEAR_FLAG_DEPTH);

	commandList->SetRenderTarget(renderTarget);
	commandList->SetViewport(renderTarget.GetViewPort());
	commandList->SetScissorRect(CD3DX12_RECT(0, 0, LONG_MAX, LONG_MAX));

	/*
	 * Render Cube
	 */
	commandList->SetVertexBuffer(0, pVertexBuffer);
	commandList->SetIndexBuffer(pIndexBuffer);
	commandList->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->DrawIndexed(pIndexBuffer->GetNumIndices());

	commandQueue.ExecuteCommandList(commandList);

	/*
	 * Present image to window
	 */
	pSwapChain->Present();
}

void OnKeyPressed(KeyEventArgs& e) {
	switch (e.Key) {
	case KeyCode::V:
		pSwapChain->ToggleVSync();
		break;
	case KeyCode::Escape:
		/*
		 * Stop application if Escape key is pressed.
		 */
		CEGame::Get().Stop();
		break;
	case KeyCode::Enter:
		if (e.Alt) {
			[[fallthrough]];
		case KeyCode::F11:
			pGameWindow->ToggleFullscreen();
			break;
		}
	}
}

void OnMouseWheel(MouseWheelEventArgs& e) {

}

void OnMouseButtonPress(MouseButtonEventArgs& e) {

}

void OnWindowResized(ResizeEventArgs& e) {
	logger->info("Window Resize: {}, {}", e.Width, e.Height);
	CEGame::Get().SetDisplaySize(e.Width, e.Height);

	/*
	 * Flush any pending commands before resizing resources
	 */
	pDevice->Flush();

	/*
	 * Resize swap chain
	 */
	pSwapChain->Resize(e.Width, e.Height);

	/*
	 * Resize depth texture
	 */
	auto depthTextureDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, e.Width, e.Height);
	/*
	 * Must be set on textures that will be used as a depth-stencil buffer
	 */
	depthTextureDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	/*
	 * Specify optimized clear values for depth buffer
	 */
	D3D12_CLEAR_VALUE optimizedClearValue = {};
	optimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	optimizedClearValue.DepthStencil = {1.0f, 0};

	pDepthTexture = pDevice->CreateTexture(depthTextureDesc, &optimizedClearValue);
}

void OnWindowClose(WindowCloseEventArgs& e) {
	/*
	 * Stop application if window is closed
	 */
	CEGame::Get().Stop();
}


/** =========================================================================
 *  ========== Deprecated - will be removed in upcoming versions ============
 *  =========================================================================
 */

ConceptEngine::ConceptEngine(CEOSTools::CEGraphicsApiTypes graphicsApiType): apiType_(graphicsApiType) {
	Init();
	window_ = std::make_unique<CEWindow>(1920, 1080, "Concept Engine Editor", apiType_);
}

ConceptEngine::ConceptEngine(int windowWidth, int windowHeight, const char* windowName) {
	Init();
	window_ = std::make_unique<CEWindow>(windowWidth, windowHeight, windowName);
}

ConceptEngine::ConceptEngine(int width, int height, const char* name,
                             CEOSTools::CEGraphicsApiTypes graphicsApiType) : ConceptEngine(width, height, name) {
	apiType_ = graphicsApiType;
}

void ConceptEngine::Init() {
	InitSpdLog();
	static_logger_ = CreateLogger("ConceptEngine");
}

int ConceptEngine::RunGraphics() {
	window_->GetGraphics().LogSystemInfo();
	while (true) {
		if (const auto ecode = CEWindow::ProcessMessages()) {
			return *ecode;
		}

		MakeFrame();
	}
}

void ConceptEngine::MakeFrame() {
	window_->GetGraphics().OnUpdate();
	window_->GetGraphics().OnRender();
}

Logger ConceptEngine::CreateLogger(const std::string& name) const {
	Logger logger = spdlog::get(name);
	if (!logger) {
		logger = logger_->clone(name);
		spdlog::register_logger(logger);
	}

	return logger;
}

Logger ConceptEngine::GetLogger() {
	return static_logger_;
}

void ConceptEngine::InitSpdLog() {

#if defined( _DEBUG )
	// Create a console window for std::cout
	CreateConsoleWindow();
#endif

	// Init spdlog.
	spdlog::init_thread_pool(8192, 1);
	auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
		"logs/log.txt", 1024 * 1024 * 5, 3,
		true); // Max size: 5MB, Max files: 3, Rotate on open: true
	auto msvc_sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();

	std::vector<spdlog::sink_ptr> sinks{stdout_sink, rotating_sink, msvc_sink};
	logger_ = std::make_shared<spdlog::async_logger>("ConceptEngine", sinks.begin(), sinks.end(),
	                                                 spdlog::thread_pool(), spdlog::async_overflow_policy::block);
	spdlog::register_logger(logger_);
	spdlog::set_default_logger(logger_);
	spdlog::flush_on(spdlog::level::info);
}

constexpr int MAX_CONSOLE_LINES = 500;

void ConceptEngine::CreateConsoleWindow() {
	// Allocate a console.
	if (AllocConsole()) {
		HANDLE lStdHandle = GetStdHandle(STD_OUTPUT_HANDLE);

		// Increase screen buffer to allow more lines of text than the default.
		CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
		GetConsoleScreenBufferInfo(lStdHandle, &consoleInfo);
		consoleInfo.dwSize.Y = MAX_CONSOLE_LINES;
		SetConsoleScreenBufferSize(lStdHandle, consoleInfo.dwSize);
		SetConsoleCursorPosition(lStdHandle, {0, 0});

		// Redirect unbuffered STDOUT to the console.
		int hConHandle = _open_osfhandle((intptr_t)lStdHandle, _O_TEXT);
		FILE* fp = _fdopen(hConHandle, "w");
		freopen_s(&fp, "CONOUT$", "w", stdout);
		setvbuf(stdout, nullptr, _IONBF, 0);

		// Redirect unbuffered STDIN to the console.
		lStdHandle = GetStdHandle(STD_INPUT_HANDLE);
		hConHandle = _open_osfhandle((intptr_t)lStdHandle, _O_TEXT);
		fp = _fdopen(hConHandle, "r");
		freopen_s(&fp, "CONIN$", "r", stdin);
		setvbuf(stdin, nullptr, _IONBF, 0);

		// Redirect unbuffered STDERR to the console.
		lStdHandle = GetStdHandle(STD_ERROR_HANDLE);
		hConHandle = _open_osfhandle((intptr_t)lStdHandle, _O_TEXT);
		fp = _fdopen(hConHandle, "w");
		freopen_s(&fp, "CONOUT$", "w", stderr);
		setvbuf(stderr, nullptr, _IONBF, 0);

		// Clear the error state for each of the C++ standard stream objects. We
		// need to do this, as attempts to access the standard streams before
		// they refer to a valid target will cause the iostream objects to enter
		// an error state. In versions of Visual Studio after 2005, this seems
		// to always occur during startup regardless of whether anything has
		// been read from or written to the console or not.
		std::wcout.clear();
		std::cout.clear();
		std::wcerr.clear();
		std::cerr.clear();
		std::wcin.clear();
		std::cin.clear();
	}
}
