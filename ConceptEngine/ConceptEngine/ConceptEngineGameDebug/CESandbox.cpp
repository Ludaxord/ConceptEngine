#include "CESandbox.h"

#include <random>

#include "Core/Platform/Generic/Managers/CECastManager.h"
#include "Graphics/DirectX12/Common/CEDXMaterial.h"
#include "Render/Scene/CEActor.h"
#include "Render/Scene/CEScene.h"
#include "Render/Scene/Components/CEMeshComponent.h"
#include "Render/Scene/Lights/CEDirectionalLight.h"
#include "Render/Scene/Lights/CEPointLight.h"
#include "Utilities/CEDirectoryUtilities.h"

using namespace ConceptEngine::Render::Scene;
using namespace ConceptEngine::Graphics::Main;
using namespace ConceptEngine::Core::Generic::Platform;
using namespace ConceptEngine::Core::Platform::Generic::Input;
using namespace ConceptEngine::Graphics::DirectX12::Common;

#define ENABLE_LIGHT_TEST 0

Common::CEPlayground* CreatePlayground() {
	return new CESandbox();
}

bool CESandbox::Create() {
	if (!CEPlayground::Create()) {
		return false;
	}

	CEActor* Actor = nullptr;
	Components::CEMeshComponent* MeshComponent = nullptr;
	Scene = CEScene::LoadFromFile(GetEngineSourceDirectory("Assets/Scenes/Sponza/Sponza.obj"));

	CEMeshData SphereMeshData = CastMeshManager()->CreateSphere(3);
	CESharedPtr<Common::CEMesh> SphereMesh = Common::CEMesh::Make(SphereMeshData);
	SphereMesh->ShadowOffset = 0.05f;

	uint8 Pixels[] = {
		255,
		255,
		255,
		255
	};

	CERef BaseTexture = CastTextureManager()->LoadFromMemory(Pixels, 1, 1, 0, RenderLayer::CEFormat::R8G8B8A8_Unorm);
	if (!BaseTexture) {
		return false;
	}

	BaseTexture->SetName("BaseTexture");

	Pixels[0] = 127;
	Pixels[1] = 127;
	Pixels[2] = 255;

	CERef BaseNormal = CastTextureManager()->LoadFromMemory(Pixels, 1, 1, 0, RenderLayer::CEFormat::R8G8B8A8_Unorm);
	if (!BaseNormal) {
		return false;
	}

	BaseNormal->SetName("BaseNormal");

	Pixels[0] = 255;
	Pixels[1] = 255;
	Pixels[2] = 255;

	CERef WhiteTexture = CastTextureManager()->LoadFromMemory(Pixels, 1, 1, 0, RenderLayer::CEFormat::R8G8B8A8_Unorm);
	if (!WhiteTexture) {
		return false;
	}

	WhiteTexture->SetName("WhiteTexture");

	constexpr float SphereOffset = 1.25f;
	constexpr uint32 SphereCountX = 8;
	constexpr float StartPositionX = (-static_cast<float>(SphereCountX) * SphereOffset) / 2.0f;
	constexpr uint32 SphereCountY = 8;
	constexpr float StartPositionY = (-static_cast<float>(SphereCountY) * SphereOffset) / 2.0f;
	constexpr float MetallicDelta = 1.0f / SphereCountY;
	constexpr float RoughnessDelta = 1.0f / SphereCountX;

	Common::CEMaterialProperties MaterialProperties;
	MaterialProperties.AO = 1.0f;

	uint32 SphereIndex = 0;
	for (uint32 y = 0; y < SphereCountY; y++) {
		for (uint32 x = 0; x < SphereCountX; x++) {
			Actor = new CEActor();
			Actor->GetTransform().SetTranslation(StartPositionX + (x * SphereOffset), 1.0f,
			                                     40.0f + StartPositionY + (y * SphereOffset));

			Actor->SetName("Sphere [" + std::to_string(SphereIndex) + "]");
			SphereIndex++;

			Scene->AddActor(Actor);

			MeshComponent = new Components::CEMeshComponent(Actor);
			MeshComponent->Mesh = SphereMesh;
			MeshComponent->Material = MakeShared<CEDXMaterial>(MaterialProperties);

			MeshComponent->Material->AlbedoMap = BaseTexture;
			MeshComponent->Material->NormalMap = BaseNormal;
			MeshComponent->Material->RoughnessMap = WhiteTexture;
			MeshComponent->Material->HeightMap = WhiteTexture;
			MeshComponent->Material->AOMap = WhiteTexture;
			MeshComponent->Material->MetallicMap = WhiteTexture;
			MeshComponent->Material->Create();

			Actor->AddComponent(MeshComponent);

			MaterialProperties.Roughness += RoughnessDelta;
		}

		MaterialProperties.Roughness = 0.05f;
		MaterialProperties.Metallic = MetallicDelta;
	}

	CEMeshData CubeMeshData = CastMeshManager()->CreateCube();

	Actor = new CEActor();
	Scene->AddActor(Actor);

	Actor->SetName("Cube");
	Actor->GetTransform().SetTranslation(0.0f, 2.0f, 50.0f);

	MaterialProperties.AO = 1.0f;
	MaterialProperties.Metallic = 1.0f;
	MaterialProperties.Roughness = 1.0f;
	MaterialProperties.EnableHeight = 1;

	MeshComponent = new Components::CEMeshComponent(Actor);
	MeshComponent->Mesh = Common::CEMesh::Make(CubeMeshData);
	MeshComponent->Material = MakeShared<CEDXMaterial>(MaterialProperties);

	CERef AlbedoMap = CastTextureManager()->LoadFromFile(GetEngineSourceDirectory("Assets/Textures/Gate_Albedo.png"), Managers::TextureFlag_GenerateMips,
	                                                     RenderLayer::CEFormat::R8G8B8A8_Unorm);
	if (!AlbedoMap) {
		return false;
	}
	else {
		AlbedoMap->SetName("AlbedoMap");
	}

	CERef NormalMap = CastTextureManager()->LoadFromFile(GetEngineSourceDirectory("Assets/Textures/Gate_Normal.png"), Managers::TextureFlag_GenerateMips,
	                                                     RenderLayer::CEFormat::R8G8B8A8_Unorm);
	if (!NormalMap) {
		return false;
	}
	else {
		NormalMap->SetName("NormalMap");
	}

	CERef AOMap = CastTextureManager()->LoadFromFile(GetEngineSourceDirectory("Assets/Textures/Gate_AO.png"), Managers::TextureFlag_GenerateMips,
	                                                 RenderLayer::CEFormat::R8_Unorm);
	if (!AOMap) {
		return false;

	}
	else {
		AOMap->SetName("AOMap");
	}

	CERef RoughnessMap = CastTextureManager()->LoadFromFile(GetEngineSourceDirectory("Assets/Textures/Gate_Roughness.png"), Managers::TextureFlag_GenerateMips,
	                                                        RenderLayer::CEFormat::R8_Unorm);
	if (!RoughnessMap) {
		return false;
	}
	else {
		RoughnessMap->SetName("RoughnessMap");
	}

	CERef HeightMap = CastTextureManager()->LoadFromFile(GetEngineSourceDirectory("Assets/Textures/Gate_Height.png"), Managers::TextureFlag_GenerateMips,
	                                                     RenderLayer::CEFormat::R8_Unorm);
	if (!HeightMap) {
		return false;
	}
	else {
		HeightMap->SetName("HeightMap");
	}

	CERef MetallicMap = CastTextureManager()->LoadFromFile(GetEngineSourceDirectory("Assets/Textures/Gate_Metallic.png"), Managers::TextureFlag_GenerateMips,
	                                                       RenderLayer::CEFormat::R8_Unorm);
	if (!MetallicMap) {
		return false;
	}
	else {
		HeightMap->SetName("MetallicMap");
	}

	MeshComponent->Material->AlbedoMap = AlbedoMap;
	MeshComponent->Material->NormalMap = NormalMap;
	MeshComponent->Material->RoughnessMap = RoughnessMap;
	MeshComponent->Material->HeightMap = HeightMap;
	MeshComponent->Material->AOMap = AOMap;
	MeshComponent->Material->MetallicMap = MetallicMap;
	MeshComponent->Material->Create();
	Actor->AddComponent(MeshComponent);

	Actor = new CEActor();
	Scene->AddActor(Actor);

	Actor->SetName("Plane");
	Actor->GetTransform().SetRotation(0.0f, 0.0f, ConceptEngine::Math::CEMath::HALF_PI);
	Actor->GetTransform().SetUniformScale(40.f);
	Actor->GetTransform().SetTranslation(0.0f, 0.0f, 42.0f);

	MaterialProperties.AO = 1.0f;
	MaterialProperties.Metallic = 1.0f;
	MaterialProperties.Roughness = 0.25f;
	MaterialProperties.EnableHeight = 0;
	MaterialProperties.Albedo = CEVectorFloat3(1.0f, 1.0f, 1.0f).Native;

	MeshComponent = new Components::CEMeshComponent(Actor);
	MeshComponent->Mesh = Common::CEMesh::Make(CastMeshManager()->CreatePlane(10, 10));
	MeshComponent->Material = MakeShared<CEDXMaterial>(MaterialProperties);
	MeshComponent->Material->AlbedoMap = BaseTexture;
	MeshComponent->Material->NormalMap = BaseNormal;
	MeshComponent->Material->RoughnessMap = WhiteTexture;
	MeshComponent->Material->HeightMap = WhiteTexture;
	MeshComponent->Material->AOMap = WhiteTexture;
	MeshComponent->Material->MetallicMap = WhiteTexture;
	MeshComponent->Material->Create();
	Actor->AddComponent(MeshComponent);

	CurrentCamera = new CECamera();
	Scene->AddCamera(CurrentCamera);

	const float Intensity = 50.0f;

	Lights::CEPointLight* Light0 = new Lights::CEPointLight();
	Light0->SetPosition(16.5f, 1.0f, 0.0f);
	Light0->SetColor(1.0f, 1.0f, 1.0f);
	Light0->SetShadowBias(0.001f);
	Light0->SetMaxShadowBias(0.009f);
	Light0->SetShadowFarPlane(50.0f);
	Light0->SetIntensity(Intensity);
	Light0->SetShadowCaster(true);
	Scene->AddLight(Light0);

	Lights::CEPointLight* Light1 = new Lights::CEPointLight();
	Light1->SetPosition(-17.5f, 1.0f, 0.0f);
	Light1->SetColor(1.0f, 1.0f, 1.0f);
	Light1->SetShadowBias(0.001f);
	Light1->SetMaxShadowBias(0.009f);
	Light1->SetShadowFarPlane(50.0f);
	Light1->SetIntensity(Intensity);
	Light1->SetShadowCaster(true);
	Scene->AddLight(Light1);

	Lights::CEPointLight* Light2 = new Lights::CEPointLight();
	Light2->SetPosition(16.5f, 11.0f, 0.0f);
	Light2->SetColor(1.0f, 1.0f, 1.0f);
	Light2->SetShadowBias(0.001f);
	Light2->SetMaxShadowBias(0.009f);
	Light2->SetShadowFarPlane(50.0f);
	Light2->SetIntensity(Intensity);
	Light2->SetShadowCaster(true);
	Scene->AddLight(Light2);

	Lights::CEPointLight* Light3 = new Lights::CEPointLight();
	Light3->SetPosition(-17.5f, 11.0f, 0.0f);
	Light3->SetColor(1.0f, 1.0f, 1.0f);
	Light3->SetShadowBias(0.001f);
	Light3->SetMaxShadowBias(0.009f);
	Light3->SetShadowFarPlane(50.0f);
	Light3->SetIntensity(Intensity);
	Light3->SetShadowCaster(true);
	Scene->AddLight(Light3);

#if ENABLE_LIGHT_TEST
	std::uniform_real_distribution<float> RandomFloats(0.0f, 1.0f);
    std::default_random_engine Generator;

    for (uint32 i = 0; i < 256; i++)
    {
        float x = RandomFloats(Generator) * 35.0f - 17.5f;
        float y = RandomFloats(Generator) * 22.0f;
        float z = RandomFloats(Generator) * 16.0f - 8.0f;
        float Intentsity = RandomFloats(Generator) * 5.0f + 1.0f;

        Lights::CEPointLight* Light = new Lights::CEPointLight();
        Light->SetPosition(x, y, z);
        Light->SetColor(RandomFloats(Generator), RandomFloats(Generator), RandomFloats(Generator));
        Light->SetIntensity(Intentsity);
        Scene->AddLight(Light);
	}
#endif

	Lights::CEDirectionalLight* Light4 = new Lights::CEDirectionalLight();
	Light4->SetShadowBias(0.0008f);
	Light4->SetMaxShadowBias(0.008f);
	Light4->SetShadowNearPlane(0.01f);
	Light4->SetShadowFarPlane(140.0f);
	Light4->SetColor(1.0f, 1.0f, 1.0f);
	Light4->SetIntensity(10.0f);
	Scene->AddLight(Light4);

	return true;
}

void CESandbox::Update(ConceptEngine::Time::CETimestamp DeltaTime) {
	CEPlayground::Update(DeltaTime);

	const float Delta = static_cast<float>(DeltaTime.AsSeconds());
	const float RotationSpeed = 45.0f;
	//TODO: Replace it with mouse movement!!!
	if (CEPlatform::GetInputManager()->IsKeyDown(Key_Right)) {
		CurrentCamera->Rotate(0.0f, XMConvertToRadians(RotationSpeed * Delta), 0.0f);
	}
	else if (CEPlatform::GetInputManager()->IsKeyDown(Key_Left)) {
		CurrentCamera->Rotate(0.0f, XMConvertToRadians(-RotationSpeed * Delta), 0.0f);
	}

	if (CEPlatform::GetInputManager()->IsKeyDown(Key_Up)) {
		CurrentCamera->Rotate(XMConvertToRadians(-RotationSpeed * Delta), 0.0f, 0.0f);
	}
	else if (CEPlatform::GetInputManager()->IsKeyDown(Key_Down)) {
		CurrentCamera->Rotate(XMConvertToRadians(RotationSpeed * Delta), 0.0f, 0.0f);
	}

	float Acceleration = 15.0f;
	if (CEPlatform::GetInputManager()->IsKeyDown(CEKey::Key_LeftShift)) {
		Acceleration = Acceleration * 3;
	}

	auto CameraAcceleration = CEVectorFloat3(0.0f, 0.0f, 0.0f).Native;
	if (CEPlatform::GetInputManager()->IsKeyDown(Key_W)) {
		CameraAcceleration.z = Acceleration;
	}
	else if (CEPlatform::GetInputManager()->IsKeyDown(Key_S)) {
		CameraAcceleration.z = -Acceleration;
	}

	if (CEPlatform::GetInputManager()->IsKeyDown(CEKey::Key_A)) {
		CameraAcceleration.x = Acceleration;
	}
	else if (CEPlatform::GetInputManager()->IsKeyDown(Key_D)) {
		CameraAcceleration.x = -Acceleration;
	}

	if (CEPlatform::GetInputManager()->IsKeyDown(CEKey::Key_Q)) {
		CameraAcceleration.y = Acceleration;
	}
	else if (CEPlatform::GetInputManager()->IsKeyDown(Key_E)) {
		CameraAcceleration.y = -Acceleration;
	}

	const float DeAcceleration = -5.0f;
	CameraSpeed = CameraSpeed + (CameraSpeed * DeAcceleration) * Delta;
	CameraSpeed = CameraSpeed + (CameraAcceleration * Delta);

	XMFLOAT3 Speed = CameraSpeed * Delta;
	CurrentCamera->Move(Speed.x, Speed.y, Speed.z);
	CurrentCamera->UpdateMatrices();
}
