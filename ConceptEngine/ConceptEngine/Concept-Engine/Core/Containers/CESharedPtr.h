#pragma once

#include "CEUniquePtr.h"

struct CEPtrControlBlock {
public:
	typedef uint32 RefType;

	CEPtrControlBlock() noexcept : WeakRefs(0), StrongRefs(0) {

	}

	RefType AddWeakRef() noexcept {
		return WeakRefs++;
	}

	RefType AddStrongRef() noexcept {
		return StrongRefs++;
	}

	RefType ReleaseWeakRef() noexcept {
		return WeakRefs--;
	}

	RefType ReleaseStrongRef() noexcept {
		return StrongRefs--;
	}

	RefType GetWeakReferences() const noexcept {
		return WeakRefs;
	}

	RefType GetStrongReferences() const noexcept {
		return StrongRefs;
	}

private:
	RefType WeakRefs;
	RefType StrongRefs;

};
