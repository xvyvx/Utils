#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

#include <limits>
#include <iterator>
#include "../Common/CommonHdr.h"
#include "BufferDescriptor.h"

class UTILS_EXPORTS_API CircularBuffer;

class UTILS_EXPORTS_API CircularBufferIterator;

namespace std
{
	template<> struct iterator_traits<CircularBufferIterator>
	{
		typedef std::ptrdiff_t difference_type;

		typedef us8 value_type;

		typedef us8* pointer;

		typedef us8& reference;

		typedef const us8& const_reference;

		typedef std::random_access_iterator_tag iterator_category;
	};
}

class UTILS_EXPORTS_API CircularBufferIterator
{
public:
	typedef std::size_t size_type;

	typedef std::iterator_traits<CircularBufferIterator>::difference_type difference_type;

	typedef std::iterator_traits<CircularBufferIterator>::value_type value_type;

	typedef std::iterator_traits<CircularBufferIterator>::pointer pointer;

	typedef const pointer const_pointer;

	typedef std::iterator_traits<CircularBufferIterator>::reference reference;

	typedef std::iterator_traits<CircularBufferIterator>::const_reference const_reference;

	CircularBufferIterator(difference_type index, CircularBuffer *buf) :m_index(index), m_buf(buf) {}

	reference operator*()
	{
		return this->operator[](m_index);
	}

	pointer operator->()
	{
		return &this->operator[](m_index);
	}

	CircularBufferIterator& operator++();

	CircularBufferIterator operator++(int)
	{
		CircularBufferIterator ret = *this;
		this->operator++();
		return ret;
	}

	CircularBufferIterator& operator--();

	CircularBufferIterator operator--(int)
	{
		CircularBufferIterator ret = *this;
		this->operator--();
		return ret;
	}

	CircularBufferIterator& operator+=(difference_type diff);

	CircularBufferIterator& operator-=(difference_type diff);

	reference operator[](size_t index);

	void swap(CircularBufferIterator &rhs) noexcept
	{
		CircularBufferIterator temp = *this;
		*this = rhs;
		rhs = temp;
	}

	bool Equal(const CircularBufferIterator &rhs) const
	{
		return m_index == rhs.m_index&&m_buf == rhs.m_buf;
	}

	difference_type Index() const
	{
		return m_index;
	}

private:
	difference_type m_index;

	CircularBuffer *m_buf;
};

UTILS_EXPORTS_API bool operator==(const CircularBufferIterator &lhs, const CircularBufferIterator &rhs);

inline bool operator!=(const CircularBufferIterator &lhs, const CircularBufferIterator &rhs)
{
	return !(lhs == rhs);
}

UTILS_EXPORTS_API CircularBufferIterator operator+(const CircularBufferIterator &lhs, CircularBufferIterator::difference_type diff);

inline CircularBufferIterator operator+(CircularBufferIterator::difference_type diff, const CircularBufferIterator &rhs)
{
	return rhs + diff;
}

UTILS_EXPORTS_API CircularBufferIterator operator-(const CircularBufferIterator &lhs, CircularBufferIterator::difference_type diff);

UTILS_EXPORTS_API CircularBufferIterator::difference_type operator-(const CircularBufferIterator &lhs, const CircularBufferIterator &rhs);

UTILS_EXPORTS_API bool operator<(const CircularBufferIterator &lhs, const CircularBufferIterator &rhs);

inline bool operator>(const CircularBufferIterator &lhs, const CircularBufferIterator &rhs)
{
	return rhs < lhs;
}

inline bool operator<=(const CircularBufferIterator &lhs, const CircularBufferIterator &rhs)
{
	return !(lhs > rhs);
}

inline bool operator>=(const CircularBufferIterator &lhs, const CircularBufferIterator &rhs)
{
	return !(lhs < rhs);
}

namespace std
{
	inline void swap(CircularBufferIterator &a, CircularBufferIterator &b) noexcept
	{
		a.swap(b);
	}
}

class UTILS_EXPORTS_API CircularBuffer
{
public:
	typedef CircularBufferIterator::size_type size_type;

	typedef CircularBufferIterator::difference_type difference_type;

	typedef CircularBufferIterator::value_type value_type;

	typedef CircularBufferIterator::pointer pointer;

	typedef const pointer const_pointer;

	typedef CircularBufferIterator::reference reference;

	typedef CircularBufferIterator::const_reference const_reference;

	typedef CircularBufferIterator iterator;

	CircularBuffer(size_t capacity = 0);

	CircularBuffer(const CircularBuffer &rhs);

	CircularBuffer(CircularBuffer &&rhs) noexcept;

	~CircularBuffer();

	CircularBuffer& operator=(const CircularBuffer &rhs);

	CircularBuffer& operator=(CircularBuffer &&rhs) noexcept;

	us8& operator[](size_t index);

	const us8& operator[](size_t index) const;

	CircularBufferIterator begin();

	CircularBufferIterator end();

	size_t size() const
	{
		return m_subSize;
	}

	size_t capacity() const
	{
		return m_capacity;
	}

	void reserve(std::size_t newCapacity);

	void inc_size(size_t incSize)
	{
		m_subSize += incSize;
	}

	void pop_front(size_t popSize);

	void clear()
	{
		m_beg = m_buf;
		m_subSize = 0;
	}

	size_t free_buffers(BufDescriptor bufs[2]);

	void copy_to(LinearBuffer &buf, size_t beg = 0, size_t size = std::numeric_limits<size_t>::max());

	void swap(CircularBuffer &buf) noexcept;

private:
	us8 *m_buf;//整个buffer的首字节指针

	us8 *m_beg;//当前buffer位置的首字节指针

	size_t m_subSize;//当前buffer的大小

	size_t m_capacity;//整个buffer的大小
};

UTILS_EXPORTS_API bool operator==(const CircularBuffer &lhs, const CircularBuffer &rhs);

inline bool operator!=(const CircularBuffer &lhs, const CircularBuffer &rhs)
{
	return !(lhs == rhs);
}

UTILS_EXPORTS_API bool operator<(const CircularBuffer &lhs, const CircularBuffer &rhs);

inline bool operator>(const CircularBuffer &lhs, const CircularBuffer &rhs)
{
	return rhs < lhs;
}

inline bool operator<=(const CircularBuffer &lhs, const CircularBuffer &rhs)
{
	return !(lhs > rhs);
}

inline bool operator>=(const CircularBuffer &lhs, const CircularBuffer &rhs)
{
	return !(lhs < rhs);
}

#endif /* CIRCULARBUFFER_H */