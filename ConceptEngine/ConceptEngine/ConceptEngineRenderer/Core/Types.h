#pragma once

// Unsigned
using uint8 = unsigned char;
using uint16 = unsigned short;
using uint32 = unsigned int;
using uint64 = unsigned long long;

// Signed
using int8 = char;
using int16 = short;
using int32 = int;
using int64 = long long;

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
