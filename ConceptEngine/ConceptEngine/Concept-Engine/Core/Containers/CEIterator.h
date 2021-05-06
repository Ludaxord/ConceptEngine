#pragma once
namespace ConceptEngine::Core::Containers {
	template <typename TIteratorType>
	class CEReverseIterator {

	public:
		CEReverseIterator() noexcept
			: Iterator(nullptr) {
		}

		CEReverseIterator(TIteratorType* InIterator) noexcept
			: Iterator(InIterator) {
		}

		CEReverseIterator(const CEReverseIterator& Other) noexcept
			: Iterator(Other.Raw()) {
		}

		template <typename U>
		CEReverseIterator(const CEReverseIterator<U>& Other) noexcept
			: Iterator(Other.Raw()) {
		}

		~CEReverseIterator() = default;

		TIteratorType* Raw() const {
			return Iterator;
		}

		TIteratorType* operator->() const noexcept {
			return (Iterator - 1);
		}

		TIteratorType& operator*() const noexcept {
			return *(Iterator - 1);
		}

		CEReverseIterator operator++() noexcept {
			Iterator--;
			return *this;
		}

		CEReverseIterator operator++(int32) noexcept {
			CEReverseIterator Temp = *this;
			Iterator--;
			return Temp;
		}

		CEReverseIterator operator--() noexcept {
			Iterator++;
			return *this;
		}

		CEReverseIterator operator--(int32) noexcept {
			CEReverseIterator Temp = *this;
			Iterator++;
			return Temp;
		}

		CEReverseIterator operator+(int32 Offset) const noexcept {
			CEReverseIterator Temp = *this;
			return Temp += Offset;
		}

		CEReverseIterator operator-(int32 Offset) const noexcept {
			CEReverseIterator Temp = *this;
			return Temp -= Offset;
		}

		CEReverseIterator& operator+=(int32 Offset) noexcept {
			Iterator -= Offset;
			return *this;
		}

		CEReverseIterator& operator-=(int32 Offset) noexcept {
			Iterator += Offset;
			return *this;
		}

		bool operator==(const CEReverseIterator& Other) const noexcept {
			return (Iterator == Other.Iterator);
		}

		bool operator!=(const CEReverseIterator& Other) const noexcept {
			return (Iterator != Other.Iterator);
		}

		operator CEReverseIterator<const TIteratorType>() const noexcept {
			return CEReverseIterator<const TIteratorType>(Iterator);
		}

	private:
		TIteratorType* Iterator;
	};
}
