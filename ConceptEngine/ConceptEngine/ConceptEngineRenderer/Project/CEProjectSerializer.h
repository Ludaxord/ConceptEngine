#pragma once
#include "CEProject.h"

class CEProjectSerializer {
public:
	CEProjectSerializer(CEProject* Project);

	void Serialize(const std::string& FilePath);
	bool Deserialize(const std::string& FilePath);

private:
	CEProject* CurrentProject;
};
