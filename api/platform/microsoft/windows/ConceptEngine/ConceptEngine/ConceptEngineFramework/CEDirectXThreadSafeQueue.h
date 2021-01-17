#pragma once
#include <wrl.h>

namespace ConceptEngine::GraphicsEngine::DirectX {
	namespace wrl = Microsoft::WRL;

	template <typename T>
	class CEDirectXThreadSafeQueue {
		CEDirectXThreadSafeQueue();
		CEDirectXThreadSafeQueue(const CEDirectXThreadSafeQueue& copy);

		/*
		 * Push value into back of queue
		 */
		void Push(T value);

		/*
		 * Try to pop value from front of the queue
		 * @return false if queue is empty
		 */
		bool TryPop(T& value);

		/*
		 * Check to se e if there are any items in queue;
		 */
		bool Empty() const;

		/*
		 * Retrieve number of items in the queue;
		 */
		size_t Size() const;

	private:
		std::queue<T> m_queue;
		mutable std::mutex m_mutex;
	};

	template <typename T>
	CEDirectXThreadSafeQueue<T>::CEDirectXThreadSafeQueue() {
	}

	template <typename T>
	CEDirectXThreadSafeQueue<T>::CEDirectXThreadSafeQueue(const CEDirectXThreadSafeQueue& copy) {
		std::lock_guard<std::mutex> lock(copy.m_mutex);
		m_queue = copy.m_queue;
	}

	template <typename T>
	void CEDirectXThreadSafeQueue<T>::Push(T value) {
		std::lock_guard<std::mutex> lock(m_mutex);
		m_queue.push(std::move(value));
	}

	template <typename T>
	bool CEDirectXThreadSafeQueue<T>::TryPop(T& value) {
		std::lock_guard<std::mutex> lock(m_mutex);
		if (m_queue.empty()) {
			return false;
		}
		value = m_queue.front();
		m_queue.pop();
		return true;
	}

	template <typename T>
	bool CEDirectXThreadSafeQueue<T>::Empty() const {
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_queue.empty();
	}

	template <typename T>
	size_t CEDirectXThreadSafeQueue<T>::Size() const {
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_queue.size();
	}
}
