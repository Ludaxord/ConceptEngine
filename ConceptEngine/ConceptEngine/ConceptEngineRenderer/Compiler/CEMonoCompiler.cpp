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

static CEActorInstanceMap ActorInstanceMap;

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

static void ReleaseMono() {
	// mono_jit_cleanup(MonoDomain);
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

static MonoMethod* GetMethod(MonoImage* Image, const std::string& MethodDesc) {
	MonoMethodDesc* Desc = mono_method_desc_new(MethodDesc.c_str(), NULL);
	if (!Desc)
		CE_LOG_ERROR("[CEMonoCompiler]: mono_method_desc_new Failed "+ MethodDesc);

	MonoMethod* Method = mono_method_desc_search_in_image(Desc, Image);
	if (!Method)
		CE_LOG_WARNING("[CEMonoCompiler]: mono_method_desc_search_in_image Failed " + MethodDesc)

	return Method;
}

static std::string GetStringProperty(const char* PropertyName, MonoClass* ClassType, MonoObject* Object) {
	MonoProperty* Property = mono_class_get_property_from_name(ClassType, PropertyName);
	MonoMethod* GetterMethod = mono_property_get_get_method(Property);
	MonoString* String = (MonoString*)mono_runtime_invoke(GetterMethod, Object, NULL, NULL);
	return String != nullptr ? std::string(mono_string_to_utf8(String)) : "";
}

static MonoObject* CallMethod(MonoObject* Object, MonoMethod* Method, void** Params = nullptr) {
	MonoObject* Exception = nullptr;
	MonoObject* Result = mono_runtime_invoke(Method, Object, Params, &Exception);
	if (Exception) {
		MonoClass* ExceptionClass = mono_object_get_class(Exception);
		MonoType* ExceptionType = mono_class_get_type(ExceptionClass);
		const char* TypeName = mono_type_get_name(ExceptionType);
		std::string Message = GetStringProperty("Message", ExceptionClass, Exception);
		std::string StackTrace = GetStringProperty("StackTrace", ExceptionClass, Exception);

		CE_LOG_ERROR(std::string(TypeName) + " :" + Message + ". Stack Trace: " + StackTrace);

		void* Args[] = {Exception};
		Result = mono_runtime_invoke(ExceptionMethod, nullptr, Args, nullptr);
	}

	return Result;
}

static MonoString* GetName() {
	return mono_string_new(CurrentMonoDomain, "Concept Engine...");
}

MonoObject* CEActorInstance::GetInstance() {
	Assert(Handle);
	return mono_gchandle_get_target(Handle);
}

bool CEActorInstance::IsRuntimeAvailable() const {
	return Handle != 0;
}

bool CEMonoCompiler::Create(const std::string& SourcePath) {
	CreateMono();
	if (!LoadRuntimeScript(SourcePath)) {
		return false;
	}

	return true;
}

void CEMonoCompiler::Release() {
	ReleaseMono();
	SceneContext = nullptr;
	ActorInstanceMap.clear();
}

void CEMonoCompiler::OnSceneDestruct(CEUUID SceneID) {
	if (ActorInstanceMap.find(SceneID) != ActorInstanceMap.end()) {
		ActorInstanceMap.at(SceneID).clear();
		ActorInstanceMap.erase(SceneID);
	}
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

static std::unordered_map<std::string, MonoClass*> Classes;

void CEMonoCompiler::SetSceneContext(CEScene* scene) {
	CECompiler::SetSceneContext(scene);
	Classes.clear();
	SceneContext = scene;
}

const CEScene* CEMonoCompiler::GetCurrentSceneContext() {
	return SceneContext;
}

void CEMonoCompiler::OnCreateActor(Actor* InActor) {
	CECompiler::OnCreateActor(InActor);
	CEActorInstance& ActorInstance = GetInstanceData(InActor->GetSceneUUID(), InActor->GetUUID()).Instance;
	if (ActorInstance.ScriptClass->OnCreate)
		CallMethod(ActorInstance.GetInstance(), ActorInstance.ScriptClass->OnCreate);
}

void CEMonoCompiler::OnUpdateActor(Actor* InActor, CETimestamp Timestamp) {
	CECompiler::OnUpdateActor(InActor, Timestamp);
	//TODO: Add Profiler
	CEActorInstance& ActorInstance = GetInstanceData(InActor->GetSceneUUID(), InActor->GetUUID()).Instance;
	//TODO: Profiler Scope
	if (ActorInstance.ScriptClass->OnUpdate) {
		void* Args[] = {&Timestamp};
		CallMethod(ActorInstance.GetInstance(), ActorInstance.ScriptClass->OnUpdate, Args);
	}

}

void CEMonoCompiler::OnPhysicsUpdate(Actor* InActor, float FixedTimestamp) {
	CECompiler::OnPhysicsUpdate(InActor, FixedTimestamp);
	//TODO: Add Profiler
	CEActorInstance& ActorInstance = GetInstanceData(InActor->GetSceneUUID(), InActor->GetUUID()).Instance;
	if (ActorInstance.ScriptClass->OnPhysicsUpdate) {
		void* Args[] = {&FixedTimestamp};
		CallMethod(ActorInstance.GetInstance(), ActorInstance.ScriptClass->OnPhysicsUpdate, Args);
	}
}

void CEMonoCompiler::OnCollisionBegin(Actor* InActor) {
	CECompiler::OnCollisionBegin(InActor);
	//TODO: Add Profiler
	CEActorInstance& ActorInstance = GetInstanceData(InActor->GetSceneUUID(), InActor->GetUUID()).Instance;
	if (ActorInstance.ScriptClass->OnCollisionBegin) {
		float Value = 5.0f;
		void* Args[] = {&Value};
		CallMethod(ActorInstance.GetInstance(), ActorInstance.ScriptClass->OnCollisionBegin, Args);
	}
}

void CEMonoCompiler::OnCollisionEnd(Actor* InActor) {
	CECompiler::OnCollisionEnd(InActor);
	//TODO: Add Profiler
	CEActorInstance& ActorInstance = GetInstanceData(InActor->GetSceneUUID(), InActor->GetUUID()).Instance;
	if (ActorInstance.ScriptClass->OnCollisionEnd) {
		float Value = 5.0f;
		void* Args[] = {&Value};
		CallMethod(ActorInstance.GetInstance(), ActorInstance.ScriptClass->OnCollisionEnd, Args);
	}
}

void CEMonoCompiler::OnTriggerBegin(Actor* InActor) {
	CECompiler::OnTriggerBegin(InActor);
	//TODO: Add Profiler
	CEActorInstance& ActorInstance = GetInstanceData(InActor->GetSceneUUID(), InActor->GetUUID()).Instance;
	if (ActorInstance.ScriptClass->OnTriggerBegin) {
		float Value = 5.0f;
		void* Args[] = {&Value};
		CallMethod(ActorInstance.GetInstance(), ActorInstance.ScriptClass->OnTriggerBegin, Args);
	}
}

void CEMonoCompiler::OnTriggerEnd(Actor* InActor) {
	CECompiler::OnTriggerEnd(InActor);
	//TODO: Add Profiler
	CEActorInstance& ActorInstance = GetInstanceData(InActor->GetSceneUUID(), InActor->GetUUID()).Instance;
	if (ActorInstance.ScriptClass->OnTriggerEnd) {
		float Value = 5.0f;
		void* Args[] = {&Value};
		CallMethod(ActorInstance.GetInstance(), ActorInstance.ScriptClass->OnTriggerEnd, Args);
	}
}

MonoObject* CEMonoCompiler::Construct(std::string& FullName, bool CallConstructor, void** Params) {
	std::string NamespaceName;
	std::string ClassName;
	std::string ParameterList;

	if (FullName.find(".") != std::string::npos) {
		NamespaceName = FullName.substr(0, FullName.find_first_of('.'));
		ClassName = FullName.substr(FullName.find_first_of(".") + 1,
		                            (FullName.find_first_of(":") - FullName.find_first_of('.')) - 1);
	}

	if (FullName.find(":") != std::string::npos) {
		ParameterList = FullName.substr(FullName.find_first_of(":"));
	}

	MonoClass* MClass = mono_class_from_name(CoreAssemblyImage, NamespaceName.c_str(), ClassName.c_str());
	MonoObject* MObject = mono_object_new(mono_domain_get(), MClass);

	if (CallConstructor) {
		MonoMethodDesc* Desc = mono_method_desc_new(ParameterList.c_str(), NULL);
		MonoMethod* Constructor = mono_method_desc_search_in_class(Desc, MClass);
		MonoObject* Exception = nullptr;
		mono_runtime_invoke(Constructor, MObject, Params, &Exception);
	}

	return MObject;
}

//TODO: Implement...
MonoClass* CEMonoCompiler::GetCoreClass(std::string& FullName) {
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
void CEMonoCompiler::CreateActorScript(Actor* InActor) {
	CECompiler::CreateActorScript(InActor);
}

//TODO: Implement...
void CEMonoCompiler::ReleaseActorScript(Actor* in_actor, const std::string& module_name) {
	CECompiler::ReleaseActorScript(in_actor, module_name);
}

//TODO: Implement...
void CEMonoCompiler::InstantiateActorClass(Actor* InActor) {
	CECompiler::InstantiateActorClass(InActor);
}

//TODO: Implement...
void CEMonoCompiler::OnEditorRender() {
	CECompiler::OnEditorRender();
}

CEActorInstanceMap CEMonoCompiler::GetActorInstanceMap() {
	return ActorInstanceMap;
}

CEInstanceData& CEMonoCompiler::GetInstanceData(CEUUID SceneID, CEUUID ActorID) {
	Assert(ActorInstanceMap.find(SceneID) != ActorInstanceMap.end());
	auto& ActorIDMap = ActorInstanceMap.at(SceneID);

	if (ActorIDMap.find(ActorID) == ActorIDMap.end())
		CreateActorScript(SceneContext->GetActorByUUID(ActorID));

	return ActorIDMap.at(ActorID);
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
