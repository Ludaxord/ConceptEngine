#pragma once

struct CEFileBuffer {
	void* Data;
	uint32 Size;

	CEFileBuffer() : Data(nullptr), Size(0) {

	}

	CEFileBuffer(void* InData, uint32 InSize) : Data(InData), Size(InSize) {

	}

	static CEFileBuffer Copy(const void* InData, uint32 InSize) {
		CEFileBuffer Buffer;
		Buffer.Allocate(InSize);
		memcpy(Buffer.Data, InData, InSize);
		return Buffer;
	}

	void Allocate(uint32 InSize) {
		delete[] Data;
		Data = nullptr;

		if (InSize == 0)
			return;

		Data = new byte[InSize];
		Size = InSize;
	}

	void Release() {
		delete[] Data;
		Data = nullptr;
		Size = 0;
	}

	void ZeroCreate() {
		if (Data)
			memset(Data, 0, Size);
	}

	template <typename T>
	T& Read(uint32 Offset = 0) {
		return *(T*)((byte*)Data + Offset);
	}

	byte* ReadBytes(uint32 InSize, uint32 Offset) {
		Assert(Offset + InSize <= Size);
		byte* Buffer = new byte[InSize];
		memcpy(Buffer, (byte*)Data + Offset, InSize);
		return Buffer;
	}

	void Write(void* InData, uint32 InSize, uint32 Offset = 0) {
		Assert(Offset + InSize <= Size);
		memcpy((byte*)Data + Offset, InData, InSize);
	}

	operator bool() const {
		return Data;
	}

	byte& operator[](int Index) {
		return ((byte*)Data)[Index];
	}

	byte operator[](int Index) const {
		return ((byte*)Data)[Index];
	}

	template <typename T>
	T& As() {
		return (T*)Data;
	}

	inline uint32 GetSize() const {
		return Size;
	}
};
