#include "CERigidDynamic.h"

CERigidDynamic::CERigidDynamic() {
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
