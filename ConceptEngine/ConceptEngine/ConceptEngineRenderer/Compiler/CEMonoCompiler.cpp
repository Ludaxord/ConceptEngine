#include "CEMonoCompiler.h"

#include <mono/metadata/image.h>
#include <mono/metadata/object-forward.h>
#include <mono/metadata/object.h>
#include <mono/utils/mono-forward.h>

#include <Windows.h>
#include <winioctl.h>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>

#include "RenderLayer/Buffer.h"

static MonoDomain* CurrentMonoDomain = nullptr;
static MonoDomain* NewMonoDomain = nullptr;
static std::string CoreAssemblyPath;
static CEScene* SceneContext;

MonoImage* AppAssemblyImage = nullptr;
MonoImage* CoreAssemblyImage = nullptr;

static MonoMethod* GetMethod(MonoImage* Image, const std::string& MethodDesc);

static MonoMethod* ExceptionMethod = nullptr;
static MonoClass* EntityClass = nullptr;

static MonoAssembly* AppAssembly = nullptr;
static MonoAssembly* CoreAssembly = nullptr;

static bool PostLoadCleanup = false;

struct CEMonoScriptClass {
	std::string FullName;
	std::string ClassName;
	std::string NamespaceName;

	MonoClass* Class = nullptr;
	MonoMethod* Constructor = nullptr;
	MonoMethod* OnCreate = nullptr;
	MonoMethod* OnRelease = nullptr;
	MonoMethod* OnUpdate = nullptr;
	MonoMethod* OnPhysicsUpdate = nullptr;

	MonoMethod* OnCollisionBegin = nullptr;
	MonoMethod* OnCollisionEnd = nullptr;
	MonoMethod* OnTriggerBegin = nullptr;
	MonoMethod* OnTriggerEnd = nullptr;
	MonoMethod* OnCollision2DBegin = nullptr;
	MonoMethod* OnCollision2DEnd = nullptr;

	void CreateClassMethods(MonoImage* Image) {
		Constructor = GetMethod(CoreAssemblyImage, "ConceptEngine.CEActor:.ctr(ulong)");
		OnCreate = GetMethod(Image, ":OnCreate()");
		OnRelease = GetMethod(Image, ":OnRelease()");
		OnUpdate = GetMethod(Image, ":OnUpdate(Single)");
		OnPhysicsUpdate = GetMethod(Image, ":OnPhysicsUpdate(Single)");

		OnCollisionBegin = GetMethod(CoreAssemblyImage, "ConceptEngine.CEActor:OnCollisionBegin(Single)");
		OnCollisionEnd = GetMethod(CoreAssemblyImage, "ConceptEngine.CEActor:OnCollisionEnd(Single)");
		OnTriggerBegin = GetMethod(CoreAssemblyImage, "ConceptEngine.CEActor:OnTriggerBegin(Single)");
		OnTriggerEnd = GetMethod(CoreAssemblyImage, "ConceptEngine.CEActor:OnTriggerEnd(Single)");
		OnCollision2DBegin = GetMethod(CoreAssemblyImage, "ConceptEngine.CEActor:OnCollision2DBegin(Single)");
		OnCollision2DEnd = GetMethod(CoreAssemblyImage, "ConceptEngine.CEActor:OnCollision2DEnd(Single)");
	}
};

static std::unordered_map<std::string, CEMonoScriptClass> ScriptClassMap;

MonoAssembly* LoadAssemblyFromFile(const char* FilePath) {
	if (FilePath == NULL) {
		return NULL;
	}

	HANDLE File = CreateFileA(FilePath, FILE_READ_ACCESS, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (File == INVALID_HANDLE_VALUE) {
		return NULL;
	}

	DWORD FileSize = GetFileSize(File, NULL);
	if (FileSize == INVALID_FILE_SIZE) {
		CloseHandle(File);
		return NULL;
	}

	void* FileData = malloc(FileSize);
	if (FileData == NULL) {
		CloseHandle(File);
		return NULL;
	}

	DWORD Read = 0;
	ReadFile(File, FileData, FileSize, &Read, NULL);
	if (FileSize != Read) {
		free(FileData);
		CloseHandle(File);
		return NULL;
	}

	MonoImageOpenStatus Status;
	MonoImage* Image = mono_image_open_from_data_full(reinterpret_cast<char*>(FileData), FileSize, 1, &Status, 0);
	if (Status != MONO_IMAGE_OK) {
		return NULL;
	}

	auto Assembly = mono_assembly_load_from_full(Image, FilePath, &Status, 0);
	free(FileData);
	CloseHandle(File);
	mono_image_close(Image);

	return Assembly;
}

static void CreateMono() {
	Assert(!CurrentMonoDomain);
	mono_set_assemblies_path("mono/lib");
	auto Domain = mono_jit_init("ConceptEngine");
}

//TODO: Implement...
static void ReleaseMono() {
}

static MonoAssembly* LoadAssembly(const std::string& Path) {
	MonoAssembly* Assembly = LoadAssemblyFromFile(Path.c_str());
	if (!Assembly)
		CE_LOG_ERROR("[CEMonoCompiler]: Could not load Assembly: " + Path)
	else
		CE_LOG_INFO("[CEMonoCompiler]: Successfully loaded Assembly: " + Path)

	return Assembly;
}

static MonoImage* GetAssemblyImage(MonoAssembly* Assembly) {
	MonoImage* Image = mono_assembly_get_image(Assembly);
	if (!Image)
		CE_LOG_ERROR("[CEMonoCompiler]: mono_assembly_get_image Failed");

	return Image;
}

static MonoClass* GetClass(MonoImage* Image, const CEMonoScriptClass& ScriptClass) {
	MonoClass* MonoClass =
		mono_class_from_name(Image, ScriptClass.NamespaceName.c_str(), ScriptClass.ClassName.c_str());
	if (!MonoClass)
		CE_LOG_ERROR("[CEMonoCompiler]: mono_class_from_name Failed");

	return MonoClass;
}

static uint32 Instantiate(CEMonoScriptClass& ScriptClass) {
	MonoObject* Instance = mono_object_new(CurrentMonoDomain, ScriptClass.Class);
	if (!Instance)
		CE_LOG_ERROR("[CEMonoCompiler]: mono_object_new Failed");

	mono_runtime_object_init(Instance);
	uint32 Handle = mono_gchandle_new(Instance, false);
	return Handle;
}

//TODO: Implement...
static MonoMethod* GetMethod(MonoImage* Image, const std::string& MethodDesc) {
	MonoMethodDesc* Desc = mono_method_desc_new(MethodDesc.c_str(), NULL);
	if (!Desc)
		CE_LOG_ERROR("[CEMonoCompiler]: mono_method_desc_new Failed "+ MethodDesc);

	MonoMethod* Method = mono_method_desc_search_in_image(Desc, Image);
	if (!Method)
		CE_LOG_WARNING("[CEMonoCompiler]: mono_method_desc_search_in_image Failed " + MethodDesc)

	return Method;
}

//TODO: Implement...
static std::string GetStringProperty(const char* PropertyName, MonoClass* ClassType, MonoObject* Object) {

}

//TODO: Implement...
static MonoObject* CallMethod(MonoObject* Object, MonoMethod* Method, void** Params = nullptr) {

}

//TODO: Implement...
static MonoString* GetName() {

}

//TODO: Implement...
MonoObject* CEActorInstance::GetInstance() {
	Assert(Handle);
	return mono_gchandle_get_target(Handle);
}

//TODO: Implement...
bool CEActorInstance::IsRuntimeAvailable() const {
	return Handle != 0;
}

//TODO: Implement...
bool CEMonoCompiler::Create(const std::string& SourcePath) {
	return false;
}

//TODO: Implement...
void CEMonoCompiler::Release() {
}

//TODO: Implement...
void CEMonoCompiler::OnSceneDestruct(CEUUID SceneID) {
}

//TODO: Implement...
bool CEMonoCompiler::LoadRuntimeScript(const std::string& ScriptPath) {
	return false;
}

//TODO: Implement...
bool CEMonoCompiler::LoadAppScript(const std::string& ScriptPath) {
	return false;
}

//TODO: Implement...
bool CEMonoCompiler::ReloadScript(const std::string& ScriptPath) {
	return false;
}

//TODO: Implement...
void CEMonoCompiler::SetSceneContext(const CEScene* scene) {
	CECompiler::SetSceneContext(scene);
}

//TODO: Implement...
const CEScene* CEMonoCompiler::GetCurrentSceneContext() {
	return CECompiler::GetCurrentSceneContext();
}

//TODO: Implement...
void CEMonoCompiler::OnCreateActor(Actor* in_actor) {
	CECompiler::OnCreateActor(in_actor);
}

//TODO: Implement...
void CEMonoCompiler::OnUpdateActor(Actor* in_actor, CETimestamp timestamp) {
	CECompiler::OnUpdateActor(in_actor, timestamp);
}

//TODO: Implement...
void CEMonoCompiler::OnPhysicsUpdate(Actor* in_actor) {
	CECompiler::OnPhysicsUpdate(in_actor);
}

//TODO: Implement...
void CEMonoCompiler::OnCollisionBegin(Actor* in_actor) {
	CECompiler::OnCollisionBegin(in_actor);
}

//TODO: Implement...
void CEMonoCompiler::OnCollisionEnd(Actor* in_actor) {
	CECompiler::OnCollisionEnd(in_actor);
}

//TODO: Implement...
void CEMonoCompiler::OnTriggerBegin(Actor* in_actor) {
	CECompiler::OnTriggerBegin(in_actor);
}

//TODO: Implement...
void CEMonoCompiler::OnTriggerEnd(Actor* in_actor) {
	CECompiler::OnTriggerEnd(in_actor);
}

//TODO: Implement...
bool CEMonoCompiler::IsActorModuleValid(Actor* actor) {
	return CECompiler::IsActorModuleValid(actor);
}

//TODO: Implement...
void CEMonoCompiler::OnScriptComponentDestroyed(CEUUID scene_id, CEUUID actor_id) {
	CECompiler::OnScriptComponentDestroyed(scene_id, actor_id);
}

//TODO: Implement...
bool CEMonoCompiler::ModuleExists(const std::string& module_name) {
	return CECompiler::ModuleExists(module_name);
}

//TODO: Implement...
void CEMonoCompiler::CreateActorScript(Actor* in_actor) {
	CECompiler::CreateActorScript(in_actor);
}

//TODO: Implement...
void CEMonoCompiler::ReleaseActorScript(Actor* in_actor, const std::string& module_name) {
	CECompiler::ReleaseActorScript(in_actor, module_name);
}

//TODO: Implement...
void CEMonoCompiler::InstanceActorClass(Actor* in_actor) {
	CECompiler::InstanceActorClass(in_actor);
}

//TODO: Implement...
void CEMonoCompiler::OnEditorRender() {
	CECompiler::OnEditorRender();
}

//TODO: Implement...
CEMonoPublicField::CEMonoPublicField(const std::string& InName, const std::string& InTypeName,
                                     CEMonoFieldType InFieldType) {
}

//TODO: Implement...
CEMonoPublicField::CEMonoPublicField(const CEMonoPublicField& PublicField) {
}

//TODO: Implement...
CEMonoPublicField::CEMonoPublicField(CEMonoPublicField&& PublicField) {
}

//TODO: Implement...
CEMonoPublicField::~CEMonoPublicField() {
}

//TODO: Implement...
CEMonoPublicField& CEMonoPublicField::operator=(const CEMonoPublicField& PublicField) {
}

//TODO: Implement...
void CEMonoPublicField::CopyStoredValueToRuntime(CEActorInstance& Instance) {
}

//TODO: Implement...
bool CEMonoPublicField::IsRuntimeAvailable() const {
}

//TODO: Implement...
void CEMonoPublicField::SetStoredValueRaw(void* Src) {
}

//TODO: Implement...
uint8* CEMonoPublicField::AllocateBuffer(CEMonoFieldType Type) {
}

//TODO: Implement...
void CEMonoPublicField::SetStoredValueInternal(void* Value) const {
}

//TODO: Implement...
void CEMonoPublicField::GetStoredValueInternal(void* OutValue) const {
}

//TODO: Implement...
void CEMonoPublicField::SetRuntimeValueInternal(CEActorInstance& Instance, void* Value) const {
}

//TODO: Implement...
void CEMonoPublicField::GetRuntimeValueInternal(CEActorInstance& Instance, void* OutValue) const {
}
