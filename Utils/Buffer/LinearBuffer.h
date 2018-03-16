#ifndef LINEARBUFFER_H
#define LINEARBUFFER_H

#include "../Common/CommonHdr.h"
#include <initializer_list>
#include <iterator>
#include <stdexcept>

/**
 * Fixed size linear buffer.
 */
class UTILS_EXPORTS_API LinearBuffer
{
public:

	/**
	 * Defines an alias representing type of the value.
	 */
	typedef us8 value_type;

	/**
	 * Defines an alias representing type of the size.
	 */
	typedef size_t size_type;

	/**
	 * Defines an alias representing type of the difference.
	 */
	typedef ptrdiff_t difference_type;

	/**
	 * Defines an alias representing the value reference.
	 */
	typedef value_type& reference;

	/**
	 * Defines an alias representing the constant value reference.
	 */
	typedef const value_type& const_reference;

	/**
	 * Defines an alias representing the value pointer.
	 */
	typedef value_type* pointer;

	/**
	 * Defines an alias representing the constant value pointer.
	 */
	typedef const value_type* const_pointer;

	/**
	 * Defines an alias representing the iterator.
	 */
	typedef pointer iterator;

	/**
	 * Defines an alias representing the constant iterator.
	 */
	typedef const_pointer const_iterator;

	/**
	 * Defines an alias representing the reverse iterator.
	 */
	typedef std::reverse_iterator<pointer> reverse_iterator;

	/**
	 * Defines an alias representing the constant reverse iterator.
	 */
	typedef std::reverse_iterator<const_pointer> const_reverse_iterator;

	/**
	 * Constructor
	 *
	 * @param capacity (Optional) The capacity(0 representing an empty buffer).
	 */
	LinearBuffer(size_type capacity = 0);

	/**
	 * Copy constructor
	 *
	 * @param rhs The right hand side.
	 */
	LinearBuffer(const LinearBuffer &rhs);

	/**
	 * Move constructor
	 *
	 * @param [in,out] rhs The right hand side.
	 */
	LinearBuffer(LinearBuffer &&rhs) noexcept;

	/**
	 * Destructor
	 */
	~LinearBuffer();

	/**
	 * Assignment operator
	 *
	 * @param rhs The right hand side.
	 *
	 * @return Equal to *this.
	 */
	LinearBuffer& operator=(const LinearBuffer &rhs);

	/**
	 * Move assignment operator
	 *
	 * @param [in,out] rhs The right hand side.
	 *
	 * @return Equal to *this.
	 */
	LinearBuffer& operator=(LinearBuffer &&rhs) noexcept;

	/**
	 * Assigns buffer data.
	 *
	 * @param count Number of byte to fiil.
	 * @param value Filled value.
	 */
	void assign(size_type count, const_reference value);

	/**
	 * Assigns a range of data to buffer.
	 *
	 * @param first The begin position.
	 * @param last  The end position.
	 */
	void assign(const_iterator first, const_iterator last);

	/**
	 * Assigns buffer data from a given list.
	 *
	 * @param ilist The data.
	 */
	void assign(std::initializer_list<value_type> ilist);

	/**
	 * Get data reference from the given position.
	 *
	 * @exception std::out_of_range Thrown when an out of range error condition occurs.
	 *
	 * @param pos The position.
	 *
	 * @return Data reference.
	 */
	reference at(size_type pos)
	{
		if (pos + m_beg >= m_last)
		{
			throw std::out_of_range("out of range.");
		}
		return m_beg[pos];
	}

	/**
	 * @see LinearBuffer::at(size_type pos)
	 */
	const_reference at(size_type pos) const
	{
		if (pos + m_beg >= m_last)
		{
			throw std::out_of_range("out of range.");
		}
		return m_beg[pos];
	}

	/**
	 * Array indexer operator
	 *
	 * @param pos The position.
	 *
	 * @return The indexed value reference.
	 */
	reference operator[](size_type pos)
	{
#ifndef NDEBUG
		return at(pos);
#else
		return m_beg[pos];
#endif
	}

	/**
	 * @see LinearBuffer::operator[](size_type pos)
	 */
	const_reference operator[](size_type pos) const
	{
#ifndef NDEBUG
		return at(pos);
#else
		return m_beg[pos];
#endif
	}

	/**
	 * Gets the front value reference.
	 *
	 * @exception std::logic_error Raised when a logic error condition occurs.
	 *
	 * @return Data reference.
	 */
	reference front()
	{
#ifndef NDEBUG
		if(empty())
		{
			throw std::logic_error("access front() when empty() is true.");
		}
#endif
		return *m_beg;
	}

	/**
	 * @see LinearBuffer::front()
	 */
	const_reference front() const
	{
#ifndef NDEBUG
		if (empty())
		{
			throw std::logic_error("access front() when empty() is true.");
		}
#endif
		return *m_beg;
	}

	/**
	* Gets the back value reference.
	*
	* @exception std::logic_error Raised when a logic error condition occurs.
	*
	* @return Data const_reference.
	*/
	reference back()
	{
#ifndef NDEBUG
		if (empty())
		{
			throw std::logic_error("access front() when empty() is true.");
		}
#endif
		return *(m_last - 1);
	}

	/**
	 * @see LinearBuffer::back()
	 */
	const_reference back() const
	{
#ifndef NDEBUG
		if (empty())
		{
			throw std::logic_error("access front() when empty() is true.");
		}
#endif
		return *(m_last - 1);
	}

	/**
	 * Gets the internal data pointer.
	 *
	 * @return Internal data pointer.
	 */
	pointer data() noexcept
	{
		return m_beg;
	}

	/**
	 * @see LinearBuffer::data() noexcept
	 */
	const_pointer data() const noexcept
	{
		return m_beg;
	}

	/**
	 * Gets the begin iterator.
	 *
	 * @return Begin iterator.
	 */
	iterator begin() noexcept
	{
		return m_beg;
	}

	/**
	 * Gets the begin const_iterator.
	 *
	 * @return Begin const_iterator.
	 */
	const_iterator begin() const noexcept
	{
		return m_beg;
	}

	/**
	 * @see LinearBuffer::begin() const noexcept
	 */
	const_iterator cbegin() const noexcept
	{
		return begin();
	}

	/**
	 * Gets the end iterator.
	 *
	 * @return End iterator.
	 */
	iterator end() noexcept
	{
		return m_last;
	}

	/**
	 * Gets the end const_iterator.
	 *
	 * @return End const_iterator.
	 */
	const_iterator end() const noexcept
	{
		return m_last;
	}

	/**
	 * @see LinearBuffer::end() const noexcept
	 */
	const_iterator cend() const noexcept
	{
		return end();
	}

	/**
	 * Gets the reverse begin iterator.
	 *
	 * @return Reverse begin iterator.
	 */
	reverse_iterator rbegin() noexcept
	{
		return reverse_iterator(m_last);
	}

	/**
	 * @see LinearBuffer::rbegin() noexcept
	 */
	const_reverse_iterator rbegin() const noexcept
	{
		return const_reverse_iterator(m_last);
	}

	/**
	 * Gets the constant reverse begin iterator.
	 *
	 * @return Constant reverse begin iterator.
	 */
	const_reverse_iterator crbegin() const noexcept
	{
		return rbegin();
	}

	/**
	 * Gets the reverse end iterator.
	 *
	 * @return Reverse end iterator.
	 */
	reverse_iterator rend() noexcept
	{
		return reverse_iterator(m_beg);
	}

	/**
	 * @see LinearBuffer::rend() noexcept
	 */
	const_reverse_iterator rend() const noexcept
	{
		return const_reverse_iterator(m_beg);
	}

	/**
	 * Gets the constant reverse end iterator.
	 *
	 * @return Constant reverse end iterator.
	 */
	const_reverse_iterator crend() const noexcept
	{
		return rend();
	}

	/**
	 * Query if this buffer is empty.
	 *
	 * @return True if it empty, false if otherwise.
	 */
	bool empty() const noexcept
	{
		return size() == 0;
	}

	/**
	 * Gets the size of this buffer.
	 *
	 * @return Buffer size.
	 */
	size_type size() const noexcept
	{
		return m_last - m_beg;
	}

	/**
	 * Gets maximum size of this buffer.
	 *
	 * @return Maximum buffer size.
	 */
	size_type max_size() const noexcept
	{
		return capacity();
	}

	/**
	 * Reserves the given new capacity
	 *
	 * @param newCapacity The new capacity.
	 */
	void reserve(size_type newCapacity);

	/**
	 * Gets the capacity of this buffer.
	 *
	 * @return Capacity of this buffer.
	 */
	size_type capacity() const noexcept
	{
		return m_capacityLast - m_beg;
	}

	/**
	 * Clears contents of this buffer.
	 */
	void clear() noexcept
	{
		m_last = m_beg;
	}

	/**
	 * Inserts a value at specified position.
	 *
	 * @param pos Position to be insertted.
	 * @param value Insertted value.
	 *
	 * @return An iterator point to insertted value.
	 */
	iterator insert(const_iterator pos, const_reference value)
	{
		return insert(pos, 1, value);
	}

	/**
	 * Inserts the specified number of content to this buffer.
	 *
	 * @param pos Position to be insertted.
	 * @param count Number of value.
	 * @param value Insertted value.
	 *
	 * @return An iterator point to begin of the insertted values.
	 */
	iterator insert(const_iterator pos, size_type count, const_reference value);

	/**
	 * Inserts a range of data to this buffer.
	 *
	 * @param pos Position to be insertted.
	 * @param first Begin position of the range.
	 * @param last  End position of the range.
	 *
	 * @return An iterator point to begin of the insertted values.
	 */
	iterator insert(const_iterator pos, const_iterator first, const_iterator last);

	/**
	 * Inserts a list of data to this buffer.
	 *
	 * @param pos Position to be insertted.
	 * @param ilist The data.
	 *
	 * @return An iterator point to begin of the insertted values.
	 */
	iterator insert(const_iterator pos, std::initializer_list<value_type> ilist);

	/**
	 * Erases the given position data.
	 *
	 * @param pos The position.
	 *
	 * @return Same as pos.
	 */
	iterator erase(const_iterator pos);

	/**
	 * Erases a range of data.
	 *
	 * @param first The begin position of the range.
	 * @param last The end position of the range.
	 *
	 * @return Same as last.
	 */
	iterator erase(const_iterator first, const_iterator last);

	/**
	 * Pushes a value at the end of the buffer.
	 *
	 * @param value The value.
	 */
	void push_back(const_reference value);

	/**
	 * Pops a value at the end of the buffer.
	 *
	 * @exception std::logic_error Raised when a logic error condition occurs.
	 */
	void pop_back()
	{
#ifndef NDEBUG
		if (empty())
		{
			throw std::logic_error("pop_back() when empty() is true.");
		}
#endif
		--m_last;
	}

	/**
	 * Resizes this buffer to given size.
	 *
	 * @param count New buffer size.
	 */
	void resize(size_type count)
	{
		resize(count, 0);
	}

	/**
	 * Resizes this buffer to given size.
	 *
	 * If count is greater than current size,value wiil be used to fill addition elements.
	 * 
	 * @param count New buffer size.
	 * @param value Filled value when current size is less than count.
	 */
	void resize(size_type count, const value_type& value);

	/**
	 * Swaps contents with the given buffer.
	 *
	 * @param [in,out] other The buffer.
	 */
	void swap(LinearBuffer &other) noexcept;

private:
	iterator m_beg; /**< The beg pointer of the memory block. */

	iterator m_last;	/**< One past last pointer base on current size. */

	iterator m_capacityLast;	/**< One past last pointer of whole memory block. */
};

namespace std
{
	/**
	 * Swaps contents with the given buffers.
	 *
	 * @param [in,out] lhs The left hand side.
	 * @param [in,out] rhs The right hand side.
	 */
	inline void swap(LinearBuffer& lhs, LinearBuffer& rhs) noexcept
	{
		lhs.swap(rhs);
	}
}

#endif /* LINEARBUFFER_H */