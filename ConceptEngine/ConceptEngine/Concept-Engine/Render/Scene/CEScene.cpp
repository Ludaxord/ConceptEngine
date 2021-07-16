#include "CEScene.h"

#include <tiny_obj_loader.h>

#include "Components/CEMeshComponent.h"
#include "../../Graphics/Main/Common/CEMaterial.h"
#include "../../Graphics/DirectX12/Common/CEDXMaterial.h"
#include "../../Utilities/CEStringUtilities.h"

#include "../../Core/Platform/Generic/Managers/CECastManager.h"

using namespace ConceptEngine::Render::Scene;

using namespace ConceptEngine::Graphics::Main::Common;

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

#ifdef WINDOWS_PLATFORM
	using namespace Graphics::DirectX12::Common;
	CESharedPtr<CEDXMaterial> BaseMaterial = MakeShared<CEDXMaterial>(Properties);
#else
	CESharedPtr<CEMaterial> BaseMaterial = MakeShared<CEMaterial>(Properties);
#endif

	BaseMaterial->AlbedoMap = WhiteTexture;
	BaseMaterial->AOMap = WhiteTexture;
	BaseMaterial->HeightMap = WhiteTexture;
	BaseMaterial->MetallicMap = WhiteTexture;
	BaseMaterial->RoughnessMap = WhiteTexture;
	BaseMaterial->NormalMap = NormalMap;
	BaseMaterial->Create();

#ifdef WINDOWS_PLATFORM
	using namespace Graphics::DirectX12::Common;
	CEArray<CESharedPtr<CEDXMaterial>> LoadedMaterials;
#else
	CEArray<CESharedPtr<Graphics::Main::Common::CEMaterial>> LoadedMaterials;
#endif

	std::unordered_map<std::string, CERef<Graphics::Main::RenderLayer::CETexture2D>> MaterialTextures;
	for (tinyobj::material_t& Mat : Materials) {
		CEMaterialProperties MaterialProperties;
		MaterialProperties.Metallic = Mat.ambient[0];
		MaterialProperties.AO = 1.0f;
		MaterialProperties.Roughness = 1.0f;

#ifdef WINDOWS_PLATFORM
		using namespace Graphics::DirectX12::Common;
		CESharedPtr<CEDXMaterial>& NewMaterial = LoadedMaterials.EmplaceBack(
			MakeShared<CEDXMaterial>(MaterialProperties));
#else
		CESharedPtr<CEMaterial>& NewMaterial = LoadedMaterials.EmplaceBack(MakeShared<CEMaterial>(MaterialProperties));
#endif

		CE_LOG_INFO("[CEScene] Loaded Material ID = " + std::to_string(LoadedMaterials.Size() - 1));

		NewMaterial->AlbedoMap = WhiteTexture;
		NewMaterial->AOMap = WhiteTexture;
		NewMaterial->HeightMap = WhiteTexture;
		NewMaterial->MetallicMap = WhiteTexture;
		NewMaterial->RoughnessMap = WhiteTexture;
		NewMaterial->NormalMap = WhiteTexture;

		//Metallic
		if (!Mat.ambient_texname.empty()) {
			ConvertBackslashes(Mat.ambient_texname);
			if (MaterialTextures.count(Mat.ambient_texname) == 0) {
				std::string TexName = MTLFileDir + '/' + Mat.ambient_texname;
				CERef Texture = CastTextureManager()->LoadFromFile(
					TexName, Graphics::Main::Managers::TextureFlag_GenerateMips,
					Graphics::Main::RenderLayer::CEFormat::R8_Unorm);
				if (Texture) {
					Texture->SetName(Mat.ambient_texname);
					MaterialTextures[Mat.ambient_texname] = Texture;
				}
				else {
					MaterialTextures[Mat.ambient_texname] = WhiteTexture;
				}
			}

			NewMaterial->MetallicMap = MaterialTextures[Mat.ambient_texname];
		}

		//Albedo
		if (!Mat.diffuse_texname.empty()) {
			ConvertBackslashes(Mat.diffuse_texname);
			if (MaterialTextures.count(Mat.diffuse_texname) == 0) {
				std::string TexName = MTLFileDir + '/' + Mat.diffuse_texname;
				CERef Texture = CastTextureManager()->LoadFromFile(
					TexName, Graphics::Main::Managers::TextureFlag_GenerateMips,
					Graphics::Main::RenderLayer::CEFormat::R8G8B8A8_Unorm);
				if (Texture) {
					Texture->SetName(Mat.diffuse_texname);
					MaterialTextures[Mat.diffuse_texname] = Texture;
				}
				else {
					MaterialTextures[Mat.diffuse_texname] = WhiteTexture;
				}
			}

			NewMaterial->AlbedoMap = MaterialTextures[Mat.diffuse_texname];
		}

		//Roughness
		if (!Mat.specular_highlight_texname.empty()) {
			ConvertBackslashes(Mat.specular_highlight_texname);
			if (MaterialTextures.count(Mat.specular_highlight_texname) == 0) {
				std::string TexName = MTLFileDir + '/' + Mat.specular_highlight_texname;
				CERef Texture = CastTextureManager()->LoadFromFile(
					TexName, Graphics::Main::Managers::TextureFlag_GenerateMips,
					Graphics::Main::RenderLayer::CEFormat::R8_Unorm);
				if (Texture) {
					Texture->SetName(Mat.specular_highlight_texname);
					MaterialTextures[Mat.specular_highlight_texname] = Texture;
				}
				else {
					MaterialTextures[Mat.specular_highlight_texname] = WhiteTexture;
				}
			}

			NewMaterial->RoughnessMap = MaterialTextures[Mat.specular_highlight_texname];
		}

		//Normal
		if (!Mat.bump_texname.empty()) {
			ConvertBackslashes(Mat.bump_texname);
			if (MaterialTextures.count(Mat.bump_texname) == 0) {
				std::string TexName = MTLFileDir + '/' + Mat.bump_texname;
				CERef<Graphics::Main::RenderLayer::CETexture2D> Texture = CastTextureManager()->LoadFromFile(
					TexName, Graphics::Main::Managers::TextureFlag_GenerateMips,
					Graphics::Main::RenderLayer::CEFormat::R8G8B8A8_Unorm);
				if (Texture) {
					Texture->SetName(Mat.bump_texname);
					MaterialTextures[Mat.bump_texname] = Texture;
				}
				else {
					MaterialTextures[Mat.bump_texname] = WhiteTexture;
				}
			}

			NewMaterial->NormalMap = MaterialTextures[Mat.bump_texname];
		}

		//Alpha
		if (!Mat.alpha_texname.empty()) {
			ConvertBackslashes(Mat.alpha_texname);
			if (MaterialTextures.count(Mat.alpha_texname) == 0) {
				std::string TexName = MTLFileDir + '/' + Mat.alpha_texname;
				CERef Texture = CastTextureManager()->LoadFromFile(
					TexName, Graphics::Main::Managers::TextureFlag_GenerateMips,
					Graphics::Main::RenderLayer::CEFormat::R8_Unorm);
				if (Texture) {
					Texture->SetName(Mat.alpha_texname);
					MaterialTextures[Mat.alpha_texname] = Texture;
				}
				else {
					MaterialTextures[Mat.alpha_texname] = WhiteTexture;
				}
			}

			NewMaterial->AlphaMask = MaterialTextures[Mat.alpha_texname];
		}

		NewMaterial->Create();
	}


	//Construct Scene.
	Graphics::Main::CEMeshData Data;
	CEUniquePtr<CEScene> LoadedScene = MakeUnique<CEScene>();
	std::unordered_map<Graphics::Main::CEVertex, uint32, Graphics::Main::CEVertexHasher> UniqueVertices;

	for (const tinyobj::shape_t& Shape : Shapes) {
		uint32 i = 0;
		while (i < Shape.mesh.indices.size()) {
			Data.Indices.Clear();
			Data.Vertices.Clear();

			UniqueVertices.clear();

			uint32 Face = i / 3;
			const int32 MaterialID = Shape.mesh.material_ids[Face];
			for (; i < Shape.mesh.indices.size(); i++) {
				Face = i / 3;
				if (Shape.mesh.material_ids[Face] != MaterialID) {
					break;
				}

				const tinyobj::index_t& Index = Shape.mesh.indices[i];
				Graphics::Main::CEVertex TempVertex;

				//Normals and texcoods are optional, Positions are required.
				Assert(Index.vertex_index >= 0);

				size_t PositionIndex = 3 * static_cast<size_t>(Index.vertex_index);
				TempVertex.Position = {
					Attributes.vertices[PositionIndex + 0],
					Attributes.vertices[PositionIndex + 1],
					Attributes.vertices[PositionIndex + 2],
				};

				if (Index.normal_index >= 0) {
					size_t NormalIndex = 3 * static_cast<size_t>(Index.normal_index);
					TempVertex.Normal = {
						Attributes.normals[NormalIndex + 0],
						Attributes.normals[NormalIndex + 1],
						Attributes.normals[NormalIndex + 2],
					};
				}

				if (Index.texcoord_index >= 0) {
					size_t TexCoordIndex = 2 * static_cast<size_t>(Index.texcoord_index);
					TempVertex.TexCoord = {
						Attributes.texcoords[TexCoordIndex + 0],
						Attributes.texcoords[TexCoordIndex + 1],
					};
				}

				if (UniqueVertices.count(TempVertex) == 0) {
					UniqueVertices[TempVertex] = static_cast<uint32>(Data.Vertices.Size());
					Data.Vertices.PushBack(TempVertex);
				}

				Data.Indices.EmplaceBack(UniqueVertices[TempVertex]);
			}

			CastMeshManager()->CalculateTangent(Data);
			CESharedPtr<CEMesh> NewMesh = CEMesh::Make(Data);

			CEActor* NewActor = new CEActor();
			NewActor->SetName(Shape.name);
			NewActor->GetTransform().SetScale(0.015f, 0.015f, 0.015f);

			Components::CEMeshComponent* NewComponent = new Components::CEMeshComponent(NewActor);
			NewComponent->Mesh = NewMesh;
			if (MaterialID >= 0) {
				CE_LOG_INFO(Shape.name + " Has Material ID = " + std::to_string(MaterialID));
				NewComponent->Material = LoadedMaterials[MaterialID];
			}
			else {
				NewComponent->Material = BaseMaterial;
			}

			NewActor->AddComponent(NewComponent);
			LoadedScene->AddActor(NewActor);
		}
	}

	return LoadedScene.Release();
}

void CEScene::AddMeshComponent(Components::CEMeshComponent* component) {
	Graphics::Main::Rendering::CEMeshDrawCommand Command;
	Command.CurrentActor = component->GetOwningActor();
	Command.Geometry = component->Mesh->RTGeometry.Get();
	Command.VertexBuffer = component->Mesh->VertexBuffer.Get();
	Command.IndexBuffer = component->Mesh->IndexBuffer.Get();
	Command.Material = component->Material.Get();
	Command.Mesh = component->Mesh.Get();
	MeshDrawCommands.PushBack(Command);
}
