#pragma once
#include <xhash>

class CEUUID {
public:
	CEUUID();
	CEUUID(uint64 uuid);
	CEUUID(const CEUUID& uuid);

	operator uint64() {
		return UUID;
	}

	operator const uint64() const {
		return UUID;
	}

private:
	uint64 UUID;
};

namespace std {
	template <>
	struct hash<CEUUID> {
		std::size_t operator()(const CEUUID& uuid) const noexcept {
			return hash<uint64>()((uint64)uuid);
		}
	};
}