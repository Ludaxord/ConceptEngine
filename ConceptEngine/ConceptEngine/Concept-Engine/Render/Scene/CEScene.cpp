#include "CEScene.h"

#include <tiny_obj_loader.h>

#include "Components/CEMeshComponent.h"
#include "../../Graphics/Main/Common/CEMaterial.h"

#include "../../Core/Platform/Generic/Managers/CECastManager.h"

using namespace ConceptEngine::Render::Scene;

CEScene::CEScene(): Actors() {
}

CEScene::~CEScene() {
	for (CEActor* CurrentActor : Actors) {
		SafeDelete(CurrentActor);
	}

	Actors.Clear();

	for (Lights::CELight* CurrentLight : Lights) {
		SafeDelete(CurrentLight);
	}

	Lights.Clear();

	SafeDelete(CurrentCamera);
}

void CEScene::Update(Time::CETimestamp deltaTime) {

}

void CEScene::AddCamera(CECamera* camera) {
	if (CurrentCamera) {
		SafeDelete(CurrentCamera);
	}

	CurrentCamera = camera;
}

void CEScene::AddActor(CEActor* actor) {
	Assert(actor != nullptr);
	Actors.EmplaceBack(actor);

	actor->OnAddedToScene(this);

	Components::CEMeshComponent* Component = actor->GetComponentOfType<Components::CEMeshComponent>();
	if (Component) {
		AddMeshComponent(Component);
	}
}

void CEScene::AddLight(Lights::CELight* Light) {
	Assert(Light != nullptr);
	Lights.EmplaceBack(Light);
}

void CEScene::OnAddedComponent(CEComponent* component) {
	Components::CEMeshComponent* Component = Cast<Components::CEMeshComponent>(component);
	if (Component) {
		AddMeshComponent(Component);
	}
}


//TODO: Change to LoadObjectFromFile (maybe move it to different class) LoadFromFile should load all scene with attributes but for that it is required to create some file format for store information
CEScene* CEScene::LoadFromFile(const std::string& filePath) {

	std::string Warning;
	std::string Error;
	std::vector<tinyobj::shape_t> Shapes;
	std::vector<tinyobj::material_t> Materials;
	tinyobj::attrib_t Attributes;

	std::string MTLFileDir = std::string(filePath.begin(), filePath.begin() + filePath.find_last_of('/'));
	if (!tinyobj::LoadObj(&Attributes,
	                      &Shapes,
	                      &Materials,
	                      &Warning,
	                      &Error,
	                      filePath.c_str(),
	                      MTLFileDir.c_str(),
	                      true,
	                      false)) {
		CE_LOG_WARNING("[CEScene] Failed to load Scene '" + filePath + "'. Warning: " + Warning + " Error: " + Error);
		return nullptr;
	}

	CE_LOG_INFO("[CEScene]: Loaded Scene '" + filePath + "'.");

	uint8 Pixels[] = {255, 255, 255, 255};
	CERef WhiteTexture = CastTextureManager()->LoadFromMemory(Pixels,
	                                                          1,
	                                                          1,
	                                                          0,
	                                                          Graphics::Main::RenderLayer::CEFormat::R8G8B8A8_Unorm);
	if (!WhiteTexture) {
		return nullptr;
	}

	WhiteTexture->SetName("[CEScene] WhiteTexture");

	Pixels[0] = 127;
	Pixels[1] = 127;
	Pixels[2] = 255;

	CERef NormalMap = CastTextureManager()->LoadFromMemory(Pixels,
	                                                       1,
	                                                       1,
	                                                       0,
	                                                       Graphics::Main::RenderLayer::CEFormat::R8G8B8A8_Unorm);

	if (!NormalMap) {
		return nullptr;
	}

	NormalMap->SetName("[CEScene]: NormalMap");

	Graphics::Main::Common::CEMaterialProperties Properties;
	Properties.AO = 1.0f;
	Properties.Metallic = 0.0f;
	Properties.Roughness = 1.0f;

	CESharedPtr<Graphics::Main::Common::CEMaterial> BaseMaterial = MakeShared<
		Graphics::Main::Common::CEMaterial>(Properties);
	BaseMaterial->AlbedoMap = WhiteTexture;
	BaseMaterial->AOMap = WhiteTexture;
	BaseMaterial->HeightMap = WhiteTexture;
	BaseMaterial->MetallicMap = WhiteTexture;
	BaseMaterial->RoughnessMap = WhiteTexture;
	BaseMaterial->NormalMap = NormalMap;
	BaseMaterial->Create();

	//TODO: Finish

	return nullptr;
}

void CEScene::AddMeshComponent(Components::CEMeshComponent* component) {
	Graphics::Main::Rendering::CEMeshDrawCommand Command;
	Command.CurrentActor = component->GetOwningActor();
	Command.Geometry = component->Mesh->RayTracingGeometry.Get();
	Command.VertexBuffer = component->Mesh->VertexBuffer.Get();
	Command.IndexBuffer = component->Mesh->IndexBuffer.Get();
	Command.Material = component->Material.Get();
	Command.Mesh = component->Mesh.Get();
	MeshDrawCommands.PushBack(Command);
}
