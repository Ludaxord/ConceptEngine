#pragma once
#include <memory>
#include "../Math/CEOperators.h"

template <typename T, typename THashType = size_t>
inline void HashCombine(THashType& hash, const T& value) {
	std::hash<T> hasher;
	hash ^= (THashType)hasher(value) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
}

namespace std {
	template <>
	struct hash<CEVectorFloat4> {
		size_t operator()(const CEVectorFloat4& ceFloat) const {
			std::hash<float> hasher;

			size_t hash = hasher(ceFloat.x);
			HashCombine<float>(hash, ceFloat.y);
			HashCombine<float>(hash, ceFloat.z);
			HashCombine<float>(hash, ceFloat.w);

			return hash;
		}
	};

	template <>
	struct hash<CEVectorFloat3> {
		size_t operator()(const CEVectorFloat3& ceFloat) const {
			std::hash<float> hasher;

			size_t hash = hasher(ceFloat.x);
			HashCombine<float>(hash, ceFloat.y);
			HashCombine<float>(hash, ceFloat.z);

			return hash;
		}
	};

	template <>
	struct hash<CEVectorFloat2> {
		size_t operator()(const CEVectorFloat2& ceFloat) const {
			std::hash<float> hasher;

			size_t hash = hasher(ceFloat.x);
			HashCombine<float>(hash, ceFloat.y);

			return hash;
		}
	};

#if defined(WINDOWS_PLATFORM)
	
    template<> struct hash<DirectX::XMFLOAT4>
    {
        size_t operator()(const DirectX::XMFLOAT4& XmFloat) const
        {
            std::hash<float> Hasher;

            size_t Hash = Hasher(XmFloat.x);
            HashCombine<float>(Hash, XmFloat.y);
            HashCombine<float>(Hash, XmFloat.z);
            HashCombine<float>(Hash, XmFloat.w);

            return Hash;
        }
    };

    template<> struct hash<DirectX::XMFLOAT3>
    {
        size_t operator()(const DirectX::XMFLOAT3& XmFloat) const
        {
            std::hash<float> Hasher;

            size_t Hash = Hasher(XmFloat.x);
            HashCombine<float>(Hash, XmFloat.y);
            HashCombine<float>(Hash, XmFloat.z);

            return Hash;
        }
    };

    template<> struct hash<DirectX::XMFLOAT2>
    {
        size_t operator()(const DirectX::XMFLOAT2& XmFloat) const
        {
            std::hash<float> Hasher;

            size_t Hash = Hasher(XmFloat.x);
            HashCombine<float>(Hash, XmFloat.y);

            return Hash;
        }
    };
#endif
}
