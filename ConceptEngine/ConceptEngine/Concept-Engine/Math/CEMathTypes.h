#pragma once
#include "../Graphics/Main/Common/API.h"

#include <DirectXMath.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

struct CEMatrixFloat4X4 {

	CEMatrixFloat4X4() = default;

	CEMatrixFloat4X4(const CEMatrixFloat4X4&) = default;
	CEMatrixFloat4X4& operator=(const CEMatrixFloat4X4&) = default;

	CEMatrixFloat4X4(CEMatrixFloat4X4&&) = default;
	CEMatrixFloat4X4& operator=(CEMatrixFloat4X4&&) = default;

	CEMatrixFloat4X4(float m00, float m01, float m02, float m03,
	                 float m10, float m11, float m12, float m13,
	                 float m20, float m21, float m22, float m23,
	                 float m30, float m31, float m32, float m33)
		: _11(m00), _12(m01), _13(m02), _14(m03),
		  _21(m10), _22(m11), _23(m12), _24(m13),
		  _31(m20), _32(m21), _33(m22), _34(m23),
		  _41(m30), _42(m31), _43(m32), _44(m33),
		  Native(PrepareNative(m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33)) {
	}

	float operator()(size_t Row, size_t Column) const {
		return m[Row][Column];
	}

	float& operator()(size_t Row, size_t Column) {
		return m[Row][Column];
	}

	union {
		struct {
			float _11, _12, _13, _14;
			float _21, _22, _23, _24;
			float _31, _32, _33, _34;
			float _41, _42, _43, _44;
		};

		float m[4][4]{};
	};


#if DIRECTX_API
		const CEMatrixFloat4X4& operator=(const DirectX::XMFLOAT4X4& native) const {
			return CEMatrixFloat4X4(
				native.m[0][0], native.m[0][1], native.m[0][2], native.m[0][3],
				native.m[1][0], native.m[1][1], native.m[1][2], native.m[1][3],
				native.m[2][0], native.m[2][1], native.m[2][2], native.m[2][3],
				native.m[3][0], native.m[3][1], native.m[3][2], native.m[3][3]
			);
		};
		
		DirectX::XMFLOAT4X4 Native;

	private:
		void SetNative(const DirectX::XMFLOAT4X4& native) {
			Native = native;
		};

		DirectX::XMFLOAT4X4 PrepareNative(float m00, float m01, float m02, float m03,
		                                  float m10, float m11, float m12, float m13,
		                                  float m20, float m21, float m22, float m23,
		                                  float m30, float m31, float m32, float m33) {
			return DirectX::XMFLOAT4X4(m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33);
		}
#elif VULKAN_API || OPENGL_API
		const CEMatrixFloat4X4& operator=(const glm::mat4x4& native) const {
			return CEMatrixFloat4X4(
				native[0][0], native[0][1], native[0][2], native[0][3],
				native[1][0], native[1][1], native[1][2], native[1][3],
				native[2][0], native[2][1], native[2][2], native[2][3],
				native[3][0], native[3][1], native[3][2], native[3][3]
			);
		};
		
		glm::mat4x4 Native;

	private:
		void SetNative(const glm::mat4x4& native) {
			Native = native;
		};

		glm::mat4x4 PrepareNative(float m00, float m01, float m02, float m03,
		                          float m10, float m11, float m12, float m13,
		                          float m20, float m21, float m22, float m23,
		                          float m30, float m31, float m32, float m33) {
			return glm::mat4x4(m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33);
		}
#else
	const CEMatrixFloat4X4& operator=(const DirectX::XMFLOAT4X4& native) const {
		return CEMatrixFloat4X4(
			native.m[0][0], native.m[0][1], native.m[0][2], native.m[0][3],
			native.m[1][0], native.m[1][1], native.m[1][2], native.m[1][3],
			native.m[2][0], native.m[2][1], native.m[2][2], native.m[2][3],
			native.m[3][0], native.m[3][1], native.m[3][2], native.m[3][3]
		);
	};

	DirectX::XMFLOAT4X4 Native;

private:
	void SetNative(const DirectX::XMFLOAT4X4& native) {
		Native = native;
	};

	DirectX::XMFLOAT4X4 PrepareNative(float m00, float m01, float m02, float m03,
	                                  float m10, float m11, float m12, float m13,
	                                  float m20, float m21, float m22, float m23,
	                                  float m30, float m31, float m32, float m33) {
		return DirectX::XMFLOAT4X4(m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33);
	}
#endif
};

struct CEMatrixFloat3X4 {

	CEMatrixFloat3X4() = default;

	CEMatrixFloat3X4(const CEMatrixFloat3X4&) = default;
	CEMatrixFloat3X4& operator=(const CEMatrixFloat3X4&) = default;

	CEMatrixFloat3X4(CEMatrixFloat3X4&&) = default;
	CEMatrixFloat3X4& operator=(CEMatrixFloat3X4&&) = default;

	CEMatrixFloat3X4(float m00, float m01, float m02, float m03,
	                 float m10, float m11, float m12, float m13,
	                 float m20, float m21, float m22, float m23)
		: _11(m00), _12(m01), _13(m02), _14(m03),
		  _21(m10), _22(m11), _23(m12), _24(m13),
		  _31(m20), _32(m21), _33(m22), _34(m23),
		  Native(PrepareNative(m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23)) {
	}

	float operator()(size_t Row, size_t Column) const {
		return m[Row][Column];
	}

	float& operator()(size_t Row, size_t Column) {
		return m[Row][Column];
	}

	union {
		struct {
			float _11, _12, _13, _14;
			float _21, _22, _23, _24;
			float _31, _32, _33, _34;
		};

		float m[3][4];
		float f[12];
	};


#if DIRECTX_API
		const CEMatrixFloat3X4& operator=(const DirectX::XMFLOAT3X4& native) const {
			return CEMatrixFloat3X4(
				native.m[0][0], native.m[0][1], native.m[0][2], native.m[0][3],
				native.m[1][0], native.m[1][1], native.m[1][2], native.m[1][3],
				native.m[2][0], native.m[2][1], native.m[2][2], native.m[2][3]
			);
		};
		
		DirectX::XMFLOAT3X4 Native;

	private:
		void SetNative(const DirectX::XMFLOAT3X4& native) {
			Native = native;
		};

		DirectX::XMFLOAT3X4 PrepareNative(float m00, float m01, float m02, float m03,
		                                  float m10, float m11, float m12, float m13,
		                                  float m20, float m21, float m22, float m23) {
			return DirectX::XMFLOAT3X4(m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23);
		}
#elif VULKAN_API || OPENGL_API
		const CEMatrixFloat3X4& operator=(const glm::mat3x4& native) const {
			return CEMatrixFloat3X4(
				native[0][0], native[0][1], native[0][2], native[0][3],
				native[1][0], native[1][1], native[1][2], native[1][3],
				native[2][0], native[2][1], native[2][2], native[2][3]
			);
		};
		
		glm::mat3x4 Native;

	private:
		void SetNative(const glm::mat3x4& native) {
			Native = native;
		};

		glm::mat3x4 PrepareNative(float m00, float m01, float m02, float m03,
		                          float m10, float m11, float m12, float m13,
		                          float m20, float m21, float m22, float m23) {
			return glm::mat3x4(m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23);
		}
#else
	const CEMatrixFloat3X4& operator=(const DirectX::XMFLOAT3X4& native) const {
		return CEMatrixFloat3X4(
			native.m[0][0], native.m[0][1], native.m[0][2], native.m[0][3],
			native.m[1][0], native.m[1][1], native.m[1][2], native.m[1][3],
			native.m[2][0], native.m[2][1], native.m[2][2], native.m[2][3]
		);
	};

	DirectX::XMFLOAT3X4 Native;

private:
	void SetNative(const DirectX::XMFLOAT3X4& native) {
		Native = native;
	};

	DirectX::XMFLOAT3X4 PrepareNative(float m00, float m01, float m02, float m03,
	                                  float m10, float m11, float m12, float m13,
	                                  float m20, float m21, float m22, float m23) {
		return DirectX::XMFLOAT3X4(m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23);
	}
#endif
};

struct CEMatrixFloat3X3 {

	CEMatrixFloat3X3() = default;

	CEMatrixFloat3X3(const CEMatrixFloat3X3&) = default;
	CEMatrixFloat3X3& operator=(const CEMatrixFloat3X3&) = default;

	CEMatrixFloat3X3(CEMatrixFloat3X3&&) = default;
	CEMatrixFloat3X3& operator=(CEMatrixFloat3X3&&) = default;

	CEMatrixFloat3X3(float m00, float m01, float m02,
	                 float m10, float m11, float m12,
	                 float m20, float m21, float m22)
		: _11(m00), _12(m01), _13(m02),
		  _21(m10), _22(m11), _23(m12),
		  _31(m20), _32(m21), _33(m22),
		  Native(PrepareNative(m00, m01, m02, m10, m11, m12, m20, m21, m22)) {
	}

	float operator()(size_t Row, size_t Column) const {
		return m[Row][Column];
	}

	float& operator()(size_t Row, size_t Column) {
		return m[Row][Column];
	}

	union {
		struct {
			float _11, _12, _13;
			float _21, _22, _23;
			float _31, _32, _33;
		};

		float m[3][3];
	};


#if DIRECTX_API
		const CEMatrixFloat3X3& operator=(const DirectX::XMFLOAT3X3& native) const {
			return CEMatrixFloat3X3(
				native.m[0][0], native.m[0][1], native.m[0][2], 
				native.m[1][0], native.m[1][1], native.m[1][2], 
				native.m[2][0], native.m[2][1], native.m[2][2]
			);
		};
		
		DirectX::XMFLOAT3X3 Native;

	private:
		void SetNative(const DirectX::XMFLOAT3X3& native) {
			Native = native;
		};

		DirectX::XMFLOAT3X3 PrepareNative(float m00, float m01, float m02,
		                                  float m10, float m11, float m12,
		                                  float m20, float m21, float m22) {
			return DirectX::XMFLOAT3X3(m00, m01, m02, m10, m11, m12, m20, m21, m22);
		}
#elif VULKAN_API || OPENGL_API
		const CEMatrixFloat3X3& operator=(const glm::mat3x3& native) const {
			return CEMatrixFloat3X3(
				native[0][0], native[0][1], native[0][2],
				native[1][0], native[1][1], native[1][2],
				native[2][0], native[2][1], native[2][2]
			);
		};
		
		glm::mat3x3 Native;

	private:
		void SetNative(const glm::mat4x4& native) {
			Native = native;
		};

		glm::mat3x3 PrepareNative(float m00, float m01, float m02,
		                          float m10, float m11, float m12,
		                          float m20, float m21, float m22) {
			return glm::mat3x3(m00, m01, m02, m10, m11, m12, m20, m21, m22);
		}
#else
	const CEMatrixFloat3X3& operator=(const DirectX::XMFLOAT3X3& native) const {
		return CEMatrixFloat3X3(
			native.m[0][0], native.m[0][1], native.m[0][2],
			native.m[1][0], native.m[1][1], native.m[1][2],
			native.m[2][0], native.m[2][1], native.m[2][2]
		);
	};

	DirectX::XMFLOAT3X3 Native;

private:
	void SetNative(const DirectX::XMFLOAT3X3& native) {
		Native = native;
	};

	DirectX::XMFLOAT3X3 PrepareNative(float m00, float m01, float m02,
	                                  float m10, float m11, float m12,
	                                  float m20, float m21, float m22) {
		return DirectX::XMFLOAT3X3(m00, m01, m02, m10, m11, m12, m20, m21, m22);
	}
#endif
};

struct CEVectorFloat4 {

	CEVectorFloat4() = default;

	CEVectorFloat4(const CEVectorFloat4&) = default;
	CEVectorFloat4& operator=(const CEVectorFloat4&) = default;

	CEVectorFloat4(CEVectorFloat4&&) = default;
	CEVectorFloat4& operator=(CEVectorFloat4&&) = default;

	CEVectorFloat4(float _x, float _y, float _z, float _w) : x(_x),
	                                                         y(_y),
	                                                         z(_z),
	                                                         w(_w),
	                                                         Native(PrepareNative(_x, _y, _z, _w)) {
	}

	float x;
	float y;
	float z;
	float w;


#if DIRECTX_API
		const CEVectorFloat4& operator=(const DirectX::XMFLOAT4& matrix) const {
			return CEVectorFloat4(matrix.x, matrix.y, matrix.z, matrix.w);
		};
		
		DirectX::XMFLOAT4 Native;

	private:
		void SetNative(const DirectX::XMFLOAT4& native) {
			Native = native;
		};

		DirectX::XMFLOAT4 PrepareNative(float _x, float _y, float _z, float _w) {
			return DirectX::XMFLOAT4(_x, _y, _z, _w);
		}
#elif VULKAN_API || OPENGL_API
		const CEVectorFloat4& operator=(const glm::vec4& matrix) const {
			return CEVectorFloat4(matrix.x, matrix.y, matrix.z, matrix.w);
		};
		
		glm::vec4 Native;

	private:
		void SetNative(const glm::vec4& native) {
			Native = native;
		};

		glm::vec4 PrepareNative(float _x, float _y, float _z, float _w) {
			return glm::vec4(_x, _y, _z, _w);
		}
#else
	const CEVectorFloat4& operator=(const DirectX::XMFLOAT4& matrix) const {
		return CEVectorFloat4(matrix.x, matrix.y, matrix.z, matrix.w);
	};


	DirectX::XMFLOAT4 Native;

private:
	void SetNative(const DirectX::XMFLOAT4& native) {
		Native = native;
	};

	DirectX::XMFLOAT4 PrepareNative(float _x, float _y, float _z, float _w) {
		return DirectX::XMFLOAT4(_x, _y, _z, _w);
	}
#endif
};

struct CEVectorFloat3 {

	CEVectorFloat3() = default;

	CEVectorFloat3(const CEVectorFloat3&) = default;
	CEVectorFloat3& operator=(const CEVectorFloat3&) = default;

	CEVectorFloat3(CEVectorFloat3&&) = default;
	CEVectorFloat3& operator=(CEVectorFloat3&&) = default;

	CEVectorFloat3(float _x, float _y, float _z) : x(_x),
	                                               y(_y),
	                                               z(_z),
	                                               Native(PrepareNative(_x, _y, _z)) {
	}

	float x;
	float y;
	float z;


#if DIRECTX_API
		const CEVectorFloat3& operator=(const DirectX::XMFLOAT3& matrix) const {
			return CEVectorFloat3(matrix.x, matrix.y, matrix.z);
		};
		
		DirectX::XMFLOAT3 Native;

	private:
		void SetNative(const DirectX::XMFLOAT3& native) {
			Native = native;
		};

		DirectX::XMFLOAT3 PrepareNative(float _x, float _y, float _z) {
			return DirectX::XMFLOAT3(_x, _y, _z);
		}
#elif VULKAN_API || OPENGL_API
		const CEVectorFloat3& operator=(const glm::vec3& matrix) const {
			return CEVectorFloat3(matrix.x, matrix.y, matrix.z);
		};
		
	glm::vec3 Native;

	private:
		void SetNative(const glm::vec3& native) {
			Native = native;
		};

		glm::vec3 PrepareNative(float _x, float _y, float _z) {
			return glm::vec3(_x, _y, _z);
		}
#else

	const CEVectorFloat3& operator=(const DirectX::XMFLOAT3& matrix) const {
		return CEVectorFloat3(matrix.x, matrix.y, matrix.z);
	};

	DirectX::XMFLOAT3 Native;

private:
	void SetNative(const DirectX::XMFLOAT3& native) {
		Native = native;
	};

	DirectX::XMFLOAT3 PrepareNative(float _x, float _y, float _z) {
		return DirectX::XMFLOAT3(_x, _y, _z);
	}
#endif

};

struct CEVectorFloat2 {

	CEVectorFloat2() = default;

	CEVectorFloat2(const CEVectorFloat2&) = default;
	CEVectorFloat2& operator=(const CEVectorFloat2&) = default;

	CEVectorFloat2(CEVectorFloat2&&) = default;
	CEVectorFloat2& operator=(CEVectorFloat2&&) = default;

	CEVectorFloat2(float _x, float _y) : x(_x),
	                                     y(_y),
	                                     Native(PrepareNative(_x, _y)) {
	}

	float x;
	float y;


#if DIRECTX_API
		const CEVectorFloat2& operator=(const DirectX::XMFLOAT2& matrix) const {
			return CEVectorFloat2(matrix.x, matrix.y);
		};
		
		DirectX::XMFLOAT2 Native;

	private:
		void SetNative(const DirectX::XMFLOAT2& native) {
			Native = native;
		};

		DirectX::XMFLOAT2 PrepareNative(float _x, float _y) {
			return DirectX::XMFLOAT2(_x, _y);
		}
#elif VULKAN_API || OPENGL_API
		const CEVectorFloat2& operator=(const glm::vec2& matrix) const {
			return CEVectorFloat2(matrix.x, matrix.y);
		};
		
		glm::vec2 Native;

	private:
		void SetNative(const glm::vec2& native) {
			Native = native;
		};

		glm::vec2 PrepareNative(float _x, float _y) {
			return glm::vec2(_x, _y);
		}
#else
	const CEVectorFloat2& operator=(const DirectX::XMFLOAT2& matrix) const {
		return CEVectorFloat2(matrix.x, matrix.y);
	};

	DirectX::XMFLOAT2 Native;

private:
	void SetNative(const DirectX::XMFLOAT2& native) {
		Native = native;
	};

	DirectX::XMFLOAT2 PrepareNative(float _x, float _y) {
		return DirectX::XMFLOAT2(_x, _y);
	}
#endif

};

struct CEVectorUint4 {

	CEVectorUint4() = default;

	CEVectorUint4(const CEVectorUint4&) = default;
	CEVectorUint4& operator=(const CEVectorUint4&) = default;

	CEVectorUint4(CEVectorUint4&&) = default;
	CEVectorUint4& operator=(CEVectorUint4&&) = default;

	CEVectorUint4(uint32_t _x, uint32_t _y, uint32_t _z, uint32_t _w) : x(_x),
	                                                                    y(_y),
	                                                                    z(_z),
	                                                                    w(_w),
	                                                                    Native(PrepareNative(_x, _y, _z, _w)) {
	}

	uint32_t x;
	uint32_t y;
	uint32_t z;
	uint32_t w;


#if DIRECTX_API
		const CEVectorUint4& operator=(const DirectX::XMUINT4& matrix) const {
			return CEVectorUint4(matrix.x, matrix.y, matrix.z, matrix.w);
		};
		
		DirectX::XMUINT4 Native;

	private:
		void SetNative(const DirectX::XMUINT4& native) {
			Native = native;
		};

		DirectX::XMUINT4 PrepareNative(uint32_t _x, uint32_t _y, uint32_t _z, uint32_t _w) {
			return DirectX::XMUINT4(_x, _y, _z, _w);
		}
#elif VULKAN_API || OPENGL_API
		const CEVectorUint4& operator=(const glm::vec4& matrix) const {
			return CEVectorUint4(matrix.x, matrix.y, matrix.z, matrix.w);
		};
		
		glm::vec4 Native;

	private:
		void SetNative(const glm::vec4& native) {
			Native = native;
		};

		glm::vec4 PrepareNative(uint32_t _x, uint32_t _y, uint32_t _z, uint32_t _w) {
			return glm::vec4(_x, _y, _z, _w);
		}
#else
	const CEVectorUint4& operator=(const DirectX::XMUINT4& matrix) const {
		return CEVectorUint4(matrix.x, matrix.y, matrix.z, matrix.w);
	};

	DirectX::XMUINT4 Native;

private:
	void SetNative(const DirectX::XMUINT4& native) {
		Native = native;
	};

	DirectX::XMUINT4 PrepareNative(uint32_t _x, uint32_t _y, uint32_t _z, uint32_t _w) {
		return DirectX::XMUINT4(_x, _y, _z, _w);
	}
#endif

};

struct CEVectorUint3 {

	CEVectorUint3() = default;

	CEVectorUint3(const CEVectorUint3&) = default;
	CEVectorUint3& operator=(const CEVectorUint3&) = default;

	CEVectorUint3(CEVectorUint3&&) = default;
	CEVectorUint3& operator=(CEVectorUint3&&) = default;

	CEVectorUint3(uint32_t _x, uint32_t _y, uint32_t _z) : x(_x),
	                                                       y(_y),
	                                                       z(_z),
	                                                       Native(PrepareNative(_x, _y, _z)) {
	}

	uint32_t x;
	uint32_t y;
	uint32_t z;


#if DIRECTX_API
		const CEVectorFloat3& operator=(const DirectX::XMFLOAT3& matrix) const {
			return CEVectorFloat3(matrix.x, matrix.y, matrix.z);
		};
		
		DirectX::XMFLOAT3 Native;

	private:
		void SetNative(const DirectX::XMFLOAT3& native) {
			Native = native;
		};

		DirectX::XMFLOAT3 PrepareNative(uint32_t _x, uint32_t _y, uint32_t _z) {
			return DirectX::XMFLOAT3(_x, _y, _z);
		}
#elif VULKAN_API || OPENGL_API
		const CEVectorFloat3& operator=(const glm::vec3& matrix) const {
			return CEVectorFloat3(matrix.x, matrix.y, matrix.z);
		};
		
	glm::vec3 Native;

	private:
		void SetNative(const glm::vec3& native) {
			Native = native;
		};

		glm::vec3 PrepareNative(uint32_t _x, uint32_t _y, uint32_t _z) {
			return glm::vec3(_x, _y, _z);
		}
#else
	const CEVectorUint3& operator=(const DirectX::XMUINT3& matrix) const {
		return CEVectorUint3(matrix.x, matrix.y, matrix.z);
	};

	DirectX::XMUINT3 Native;

private:
	void SetNative(const DirectX::XMUINT3& native) {
		Native = native;
	};

	DirectX::XMUINT3 PrepareNative(uint32_t _x, uint32_t _y, uint32_t _z) {
		return DirectX::XMUINT3(_x, _y, _z);
	}
#endif
};

struct CEVectorUint2 {
	CEVectorUint2() = default;

	CEVectorUint2(const CEVectorUint2&) = default;
	CEVectorUint2& operator=(const CEVectorUint2&) = default;

	CEVectorUint2(CEVectorUint2&&) = default;
	CEVectorUint2& operator=(CEVectorUint2&&) = default;

	CEVectorUint2(uint32_t _x, uint32_t _y) : x(_x),
	                                          y(_y),
	                                          Native(PrepareNative(_x, _y)) {
	}

	uint32_t x;
	uint32_t y;


#if DIRECTX_API
		const CEVectorUint2& operator=(const DirectX::XMUINT2& matrix) const {
			return CEVectorUint2(matrix.x, matrix.y);
		};
		
		DirectX::XMUINT2 Native;

	private:
		void SetNative(const DirectX::XMUINT2& native) {
			Native = native;
		};

		DirectX::XMUINT2 PrepareNative(uint32_t _x, uint32_t _y) {
			return DirectX::XMUINT2(_x, _y);
		}
#elif VULKAN_API || OPENGL_API
		const CEVectorUint2& operator=(const glm::vec2& matrix) const {
			return CEVectorUint2(matrix.x, matrix.y);
		};
		
		glm::vec2 Native;

	private:
		void SetNative(const glm::vec2& native) {
			Native = native;
		};

		glm::vec2 PrepareNative(uint32_t _x, uint32_t _y) {
			return glm::vec2(_x, _y);
		}
#else
	const CEVectorUint2& operator=(const DirectX::XMUINT2& matrix) const {
		return CEVectorUint2(matrix.x, matrix.y);
	};

	DirectX::XMUINT2 Native;

private:
	void SetNative(const DirectX::XMUINT2& native) {
		Native = native;
	};

	DirectX::XMUINT2 PrepareNative(uint32_t _x, uint32_t _y) {
		return DirectX::XMUINT2(_x, _y);
	}
#endif

};

template <int L, typename T>
struct CEVector {
	CEVector() = default;

	CEVector(const CEVector&) = default;
	CEVector& operator=(const CEVector&) = default;

	CEVector(CEVector&&) = default;
	CEVector& operator=(CEVector&&) = default;


#if DIRECTX_API
		DirectX::XMVECTOR Native;
		
		CEVector(const DirectX::XMVECTOR& matrix): Native(matrix) {
		};
#elif VULKAN_API || OPENGL_API
		glm::vec<L, T> Native;

		
		CEVector(const glm::vec<L, T>& matrix): Native(matrix) {
		};
#else
	DirectX::XMVECTOR Native;

	CEVector(const DirectX::XMVECTOR& matrix): Native(matrix) {
	};
#endif
};

template <int L, int R, typename T>
struct CEMatrix {
	CEMatrix() = default;

	CEMatrix(const CEMatrix&) = default;
	CEMatrix& operator=(const CEMatrix&) = default;

	CEMatrix(CEMatrix&&) = default;
	CEMatrix& operator=(CEMatrix&&) = default;


#if DIRECTX_API
		DirectX::XMMATRIX Native;

		CEMatrix(const DirectX::XMMATRIX& matrix): Native(matrix) {
		};
#elif VULKAN_API || OPENGL_API
		glm::mat<L, R, T> Native;


		CEMatrix(const glm::mat<L, R, T>& matrix): Native(matrix) {
		};
#else
	DirectX::XMMATRIX Native;

	CEMatrix(const DirectX::XMMATRIX& matrix): Native(matrix) {
	};
#endif

};

inline CEVector<3, float> CELoadFloat3(const CEVectorFloat3* source) {
#if DIRECTX_API
	return CEVector<3, float>(XMLoadFloat3(&source->Native));
#elif VULKAN_API || OPENGL_API
	//TODO: find equivalent for GLM
#else
	return CEVector<3, float>(XMLoadFloat3(&source->Native));
#endif
}

inline CEVector<4, float> CEVectorSet(float x, float y, float z, float w) {
#if DIRECTX_API
		return CEVector<4, float>(DirectX::XMVectorSet(x, y, z, w));
#elif VULKAN_API || OPENGL_API
	//TODO: find equivalent for GLM
#else
	return CEVector<4, float>(DirectX::XMVectorSet(x, y, z, w));
#endif
}

inline CEMatrix<4, 4, float> CEMatrixMultiply(const CEMatrix<4, 4, float> m1, const CEMatrix<4, 4, float>& m2) {
#if DIRECTX_API
		return CEMatrix<4, 4, float>(DirectX::XMMatrixMultiply(m1.Native, m2.Native));
#elif VULKAN_API || OPENGL_API
	//TODO: find equivalent for GLM
#else
	return CEMatrix<4, 4, float>(DirectX::XMMatrixMultiply(m1.Native, m2.Native));
#endif
}

inline CEMatrix<4, 4, float> CEMatrixScalingFromVector(CEVector<3, float> scale) {
#if DIRECTX_API
		return CEMatrix<4, 4, float>(DirectX::XMMatrixScalingFromVector(scale.Native));
#elif VULKAN_API || OPENGL_API
	//TODO: find equivalent for GLM
#else
	return CEMatrix<4, 4, float>(DirectX::XMMatrixScalingFromVector(scale.Native));
#endif
}

inline CEMatrix<4, 4, float> CEMatrixRotationRollPitchYawFromVector(CEVector<4, float> angles) {
#if DIRECTX_API
		return CEMatrix<4, 4, float>(DirectX::XMMatrixRotationRollPitchYawFromVector(angles.Native));
#elif VULKAN_API || OPENGL_API
	//TODO: find equivalent for GLM
#else
	return CEMatrix<4, 4, float>(DirectX::XMMatrixRotationRollPitchYawFromVector(angles.Native));
#endif
}

inline CEMatrix<4, 4, float> CEMatrixTranslationFromVector(CEVector<3, float> offset) {
#if DIRECTX_API
		return CEMatrix<4, 4, float>(DirectX::XMMatrixTranslationFromVector(offset.Native));
#elif VULKAN_API || OPENGL_API
	//TODO: find equivalent for GLM
#else
	return CEMatrix<4, 4, float>(DirectX::XMMatrixTranslationFromVector(offset.Native));
#endif
}

inline void CEStoreFloat3x4(CEMatrixFloat3X4* destination, const CEMatrix<4, 4, float> M) {
#if DIRECTX_API
		DirectX::XMStoreFloat3x4(&destination->Native, M.Native);
#elif VULKAN_API || OPENGL_API
	//TODO: find equivalent for GLM
#else
	DirectX::XMStoreFloat3x4(&destination->Native, M.Native);
#endif
}

inline void CEStoreFloat4x4(CEMatrixFloat4X4* destination, const CEMatrix<4, 4, float> M) {
#if DIRECTX_API
		DirectX::XMStoreFloat4x4(&destination->Native, M.Native);
#elif VULKAN_API || OPENGL_API
	//TODO: find equivalent for GLM
#else
	DirectX::XMStoreFloat4x4(&destination->Native, M.Native);
#endif
}

inline CEMatrix<4, 4, float> CEMatrixTranspose(const CEMatrix<4, 4, float> M) {
#if DIRECTX_API
		return DirectX::XMMatrixTranspose(M.Native);
#elif VULKAN_API || OPENGL_API
	//TODO: find equivalent for GLM
#else
	return DirectX::XMMatrixTranspose(M.Native);
#endif
}

inline CEMatrix<4, 4, float> CEMatrixInverse(CEVector<3, float>* determinant, const CEMatrix<4, 4, float> M) {
#if DIRECTX_API
		return DirectX::XMMatrixTranspose(M.Native);
#elif VULKAN_API || OPENGL_API
	//TODO: find equivalent for GLM
#else
	return DirectX::XMMatrixInverse(&determinant->Native, M.Native);
#endif
}
