#include "CEProject.h"

CEProject::CEProject() {
}

CEProject::~CEProject() {
}

//TODO: Implement..
void CEProject::SetActive(CEProject* Project) {

	//TODO: Create Manager of Assets (To modify project file)
	//TODO: Close Current Asset Manager. It means that all changes will by saved to project file...

	ActiveProject = Project;

	//TODO: Read data from new Project File, Call it from Asset Manager...

}

//TODO: Implement..
void CEProject::OnDeserialized() {
}
