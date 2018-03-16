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
	/**
	 * CircularBuffer iterator traits.
	 */
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

/**
 * CircularBuffer iterator.
 */
class UTILS_EXPORTS_API CircularBufferIterator
{
public:

	/**
	 * Defines an alias representing type of the size.
	 */
	typedef std::size_t size_type;

	/**
	 * Defines an alias representing type of the difference.
	 */
	typedef std::iterator_traits<CircularBufferIterator>::difference_type difference_type;

	/**
	 * Defines an alias representing type of the value.
	 */
	typedef std::iterator_traits<CircularBufferIterator>::value_type value_type;

	/**
	 * Defines an alias representing type of the pointer.
	 */
	typedef std::iterator_traits<CircularBufferIterator>::pointer pointer;

	/**
	 * Defines an alias representing type of the constant pointer.
	 */
	typedef const pointer const_pointer;

	/**
	 * Defines an alias representing type of the reference.
	 */
	typedef std::iterator_traits<CircularBufferIterator>::reference reference;

	/**
	 * Defines an alias representing type of the constant reference.
	 */
	typedef std::iterator_traits<CircularBufferIterator>::const_reference const_reference;

	/**
	 * Constructor
	 *
	 * @param index Zero-based index of the buffer.
	 * @param buf The buffer.
	 */
	CircularBufferIterator(difference_type index, CircularBuffer *buf) :m_index(index), m_buf(buf) {}

	/**
	 * Indirection operator
	 *
	 * @return The element reference pointted by this iterator.
	 */
	reference operator*()
	{
		return this->operator[](m_index);
	}

	/**
	 * Member dereference operator
	 *
	 * @return The element pointer pointted by this iterator.
	 */
	pointer operator->()
	{
		return &this->operator[](m_index);
	}

	/**
	 * Increment operator
	 *
	 * @return Iterator reference after increment.
	 */
	CircularBufferIterator& operator++();

	/**
	 * Increment operator
	 *
	 * @param parameter1 The first parameter.
	 *
	 * @return Iterator before increment.
	 */
	CircularBufferIterator operator++(int)
	{
		CircularBufferIterator ret = *this;
		this->operator++();
		return ret;
	}

	/**
	 * Decrement operator
	 *
	 * @return Iterator reference after decrement.
	 */
	CircularBufferIterator& operator--();

	/**
	 * Decrement operator
	 *
	 * @param parameter1 The first parameter.
	 *
	 * @return Iterator before decrement.
	 */
	CircularBufferIterator operator--(int)
	{
		CircularBufferIterator ret = *this;
		this->operator--();
		return ret;
	}

	/**
	 * Addition assignment operator
	 *
	 * @param diff The difference value.
	 *
	 * @return Iterator reference after addition.
	 */
	CircularBufferIterator& operator+=(difference_type diff);

	/**
	 * Subtraction assignment operator
	 *
	 * @param diff The difference value.
	 *
	 * @return Iterator reference after subtraction.
	 */
	CircularBufferIterator& operator-=(difference_type diff);

	/**
	 * Array indexer operator
	 *
	 * @param index Zero-based index of the.
	 *
	 * @return The element reference pointted by this iterator.
	 */
	reference operator[](size_t index);

	/**
	 * Swaps the given right hand side
	 *
	 * @param [in,out] rhs The right hand side.
	 */
	void swap(CircularBufferIterator &rhs) noexcept
	{
		CircularBufferIterator temp = *this;
		*this = rhs;
		rhs = temp;
	}

	/**
	 * Query the given right hand side and left hand side is equal.
	 *
	 * @param rhs The right hand side.
	 *
	 * @return True if it is equal, false if is not equal.
	 */
	bool Equal(const CircularBufferIterator &rhs) const
	{
		return m_index == rhs.m_index&&m_buf == rhs.m_buf;
	}

	/**
	 * Gets internal index of this iterator.
	 *
	 * @return Internal index.
	 */
	difference_type Index() const
	{
		return m_index;
	}

private:
	difference_type m_index;	/**< Zero-based index of the buffer.  */

	CircularBuffer *m_buf;  /**< The buffer. */
};

/**
 * Equality operator
 *
 * @param lhs The first instance to compare.
 * @param rhs The second instance to compare.
 *
 * @return True if the parameters are considered equivalent.
 */
UTILS_EXPORTS_API bool operator==(const CircularBufferIterator &lhs, const CircularBufferIterator &rhs);

/**
 * Inequality operator
 *
 * @param lhs The first instance to compare.
 * @param rhs The second instance to compare.
 *
 * @return True if the parameters are not considered equivalent.
 */
inline bool operator!=(const CircularBufferIterator &lhs, const CircularBufferIterator &rhs)
{
	return !(lhs == rhs);
}

/**
 * Addition operator
 *
 * @param lhs The iterator.
 * @param diff A value to be added to iterator.
 *
 * @return The iterator after addition.
 */
UTILS_EXPORTS_API CircularBufferIterator operator+(const CircularBufferIterator &lhs, CircularBufferIterator::difference_type diff);

/**
 * Addition operator
 *
 * @param diff A value to be added to iterator.
 * @param rhs  The iterator.
 *
 * @return The iterator after addition.
 */
inline CircularBufferIterator operator+(CircularBufferIterator::difference_type diff, const CircularBufferIterator &rhs)
{
	return rhs + diff;
}

/**
 * Subtraction operator
 *
 * @param lhs The iterator.
 * @param diff A value to be subtractted to iterator.
 *
 * @return The iterator after subtraction.
 */
UTILS_EXPORTS_API CircularBufferIterator operator-(const CircularBufferIterator &lhs, CircularBufferIterator::difference_type diff);

/**
 * Subtraction operator
 *
 * @param lhs The left hand side.
 * @param rhs The right hand side.
 *
 * @return The difference between two iterator.
 */
UTILS_EXPORTS_API CircularBufferIterator::difference_type operator-(const CircularBufferIterator &lhs, const CircularBufferIterator &rhs);

/**
 * Less-than comparison operator
 *
 * @param lhs The first instance to compare.
 * @param rhs The second instance to compare.
 *
 * @return True if the first parameter is less than the second.
 */
UTILS_EXPORTS_API bool operator<(const CircularBufferIterator &lhs, const CircularBufferIterator &rhs);

/**
 * Greater-than comparison operator
 *
 * @param lhs The first instance to compare.
 * @param rhs The second instance to compare.
 *
 * @return True if the first parameter is greater than to the second.
 */
inline bool operator>(const CircularBufferIterator &lhs, const CircularBufferIterator &rhs)
{
	return rhs < lhs;
}

/**
 * Less-than-or-equal comparison operator
 *
 * @param lhs The first instance to compare.
 * @param rhs The second instance to compare.
 *
 * @return True if the first parameter is less than or equal to the second.
 */
inline bool operator<=(const CircularBufferIterator &lhs, const CircularBufferIterator &rhs)
{
	return !(lhs > rhs);
}

/**
 * Greater-than-or-equal comparison operator
 *
 * @param lhs The first instance to compare.
 * @param rhs The second instance to compare.
 *
 * @return True if the first parameter is greater than or equal to the second.
 */
inline bool operator>=(const CircularBufferIterator &lhs, const CircularBufferIterator &rhs)
{
	return !(lhs < rhs);
}

namespace std
{
	/**
	 * Swap two CircularBufferIterator.
	 *
	 * @param [in,out] a A CircularBufferIterator to process.
	 * @param [in,out] b A CircularBufferIterator to process.
	 */
	inline void swap(CircularBufferIterator &a, CircularBufferIterator &b) noexcept
	{
		a.swap(b);
	}
}

/**
 * Circular buffer utility class.
 */
class UTILS_EXPORTS_API CircularBuffer
{
public:

	/**
	 * Defines an alias representing type of the size.
	 */
	typedef CircularBufferIterator::size_type size_type;

	/**
	 * Defines an alias representing type of the difference.
	 */
	typedef CircularBufferIterator::difference_type difference_type;

	/**
	 * Defines an alias representing type of the value.
	 */
	typedef CircularBufferIterator::value_type value_type;

	/**
	 * Defines an alias representing the value pointer.
	 */
	typedef CircularBufferIterator::pointer pointer;

	/**
	 * Defines an alias representing the constant value pointer.
	 */
	typedef const pointer const_pointer;

	/**
	 * Defines an alias representing the value reference.
	 */
	typedef CircularBufferIterator::reference reference;

	/**
	 * Defines an alias representing the constant value reference.
	 */
	typedef CircularBufferIterator::const_reference const_reference;

	/**
	 * Defines an alias representing the iterator.
	 */
	typedef CircularBufferIterator iterator;

	/**
	 * Constructor
	 *
	 * @param capacity (Optional) The buffer capacity.
	 */
	CircularBuffer(size_t capacity = 0);

	/**
	 * Copy constructor
	 *
	 * @param rhs The right hand side.
	 */
	CircularBuffer(const CircularBuffer &rhs);

	/**
	 * Move constructor
	 *
	 * @param [in,out] rhs The right hand side.
	 */
	CircularBuffer(CircularBuffer &&rhs) noexcept;

	/**
	 * Destructor
	 */
	~CircularBuffer();

	/**
	 * Assignment operator
	 *
	 * @param rhs The right hand side.
	 *
	 * @return Equal to *this.
	 */
	CircularBuffer& operator=(const CircularBuffer &rhs);

	/**
	 * Move assignment operator
	 *
	 * @param [in,out] rhs The right hand side.
	 *
	 * @return Equal to *this.
	 */
	CircularBuffer& operator=(CircularBuffer &&rhs) noexcept;

	/**
	 * Array indexer operator
	 *
	 * @param index Zero-based index of the.
	 *
	 * @return The indexed value.
	 */
	us8& operator[](size_t index);

	/**
	 * Array indexer operator
	 *
	 * @param index Zero-based index of the.
	 *
	 * @return The indexed value.
	 */
	const us8& operator[](size_t index) const;

	/**
	 * Gets the begin iterator.
	 *
	 * @return The begin CircularBufferIterator.
	 */
	CircularBufferIterator begin();

	/**
	 * Gets the end iterator.
	 *
	 * @return The end CircularBufferIterator.
	 */
	CircularBufferIterator end();

	/**
	 * Gets the current size of this buffer.
	 *
	 * @return Size of this buffer.
	 */
	size_t size() const
	{
		return m_subSize;
	}

	/**
	 * Gets the current capacity of this buffer.
	 *
	 * @return Capacity of this buffer.
	 */
	size_t capacity() const
	{
		return m_capacity;
	}

	/**
	 * Reserves the given new capacity.
	 *
	 * @param newCapacity The new capacity.
	 */
	void reserve(std::size_t newCapacity);

	/**
	 * Increase size by adding specific value.
	 *
	 * @param incSize increase value.
	 */
	void inc_size(size_t incSize)
	{
		m_subSize += incSize;
	}

	/**
	 * Pops the front elements described by popSize.
	 *
	 * @param popSize Size to be poped.
	 */
	void pop_front(size_t popSize);

	/**
	 * Clears all elements.
	 */
	void clear()
	{
		m_beg = m_buf;
		m_subSize = 0;
	}

	/**
	 * Get free space of this buffer.
	 *
	 * @param [out] bufs Free spaces.
	 *
	 * @return A The size of free space array.
	 */
	size_t free_buffers(BufDescriptor bufs[2]);

	/**
	 * Copy elements to specific linear buffer.
	 *
	 * @param [in,out] buf Copy destination buffer.
	 * @param beg  (Optional) The beg position to be copied.
	 * @param size (Optional) Number of elements to be copied.
	 */
	void copy_to(LinearBuffer &buf, size_t beg = 0, size_t size = std::numeric_limits<size_t>::max());

	/**
	 * Swaps the given buffer
	 *
	 * @param [in,out] buf The right hand side buffer.
	 */
	void swap(CircularBuffer &buf) noexcept;

private:
	us8 *m_buf; /**< The begin pointer of whole memory block.整个buffer的首字节指针 */

	us8 *m_beg; /**< The begin pointer of used memory block.当前buffer位置的首字节指针 */

	size_t m_subSize;   /**< The size of used memory block.当前buffer的大小 */

	size_t m_capacity;  /**< The size of whole memory block.整个buffer的大小 */
};

/**
 * Equality operator
 *
 * @param lhs The first instance to compare.
 * @param rhs The second instance to compare.
 *
 * @return True if the parameters are considered equivalent.
 */
UTILS_EXPORTS_API bool operator==(const CircularBuffer &lhs, const CircularBuffer &rhs);

/**
 * Inequality operator
 *
 * @param lhs The first instance to compare.
 * @param rhs The second instance to compare.
 *
 * @return True if the parameters are not considered equivalent.
 */
inline bool operator!=(const CircularBuffer &lhs, const CircularBuffer &rhs)
{
	return !(lhs == rhs);
}

/**
 * Less-than comparison operator
 *
 * @param lhs The first instance to compare.
 * @param rhs The second instance to compare.
 *
 * @return True if the first parameter is less than the second.
 */
UTILS_EXPORTS_API bool operator<(const CircularBuffer &lhs, const CircularBuffer &rhs);

/**
 * Greater-than comparison operator
 *
 * @param lhs The first instance to compare.
 * @param rhs The second instance to compare.
 *
 * @return True if the first parameter is greater than to the second.
 */
inline bool operator>(const CircularBuffer &lhs, const CircularBuffer &rhs)
{
	return rhs < lhs;
}

/**
 * Less-than-or-equal comparison operator
 *
 * @param lhs The first instance to compare.
 * @param rhs The second instance to compare.
 *
 * @return True if the first parameter is less than or equal to the second.
 */
inline bool operator<=(const CircularBuffer &lhs, const CircularBuffer &rhs)
{
	return !(lhs > rhs);
}

/**
 * Greater-than-or-equal comparison operator
 *
 * @param lhs The first instance to compare.
 * @param rhs The second instance to compare.
 *
 * @return True if the first parameter is greater than or equal to the second.
 */
inline bool operator>=(const CircularBuffer &lhs, const CircularBuffer &rhs)
{
	return !(lhs < rhs);
}

#endif /* CIRCULARBUFFER_H */