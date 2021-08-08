#include "CERigidDynamic.h"

CERigidDynamic::CERigidDynamic(
	std::string& InParentName,
	const CERigidTransform& InParentTransform,
	const CERigidTransform& InLocalTransform
): CERigid(InParentName, InParentTransform, InLocalTransform) {

}

CERigidDynamic::~CERigidDynamic() {
	CERigid::~CERigid();
}

void CERigidDynamic::AddRigidDynamic() {
	CERigid::AddRigidDynamic();
}

void CERigidDynamic::Release() {
	CERigid::Release();
}
