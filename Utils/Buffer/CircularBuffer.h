#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

#include <limits>
#include <iterator>
#include <type_traits>
#include "../Common/CommonHdr.h"
#include "BufferDescriptor.h"

template<bool IsConst> class CircularBufferIterator;

/**
 * Circular buffer utility class.
 */
class UTILS_EXPORTS_API CircularBuffer
{
public:

    /**
     * Defines an alias representing type of the size.
     */
    typedef std::size_t size_type;

    /**
     * Defines an alias representing type of the difference.
     */
    typedef std::ptrdiff_t difference_type;

    /**
     * Defines an alias representing type of the value.
     */
    typedef us8 value_type;

    /**
     * Defines an alias representing the value pointer.
     */
    typedef us8* pointer;

    /**
     * Defines an alias representing the constant pointer
     */
    typedef const pointer const_pointer;

    /**
     * Defines an alias representing the value reference.
     */
    typedef us8& reference;

    /**
     * Defines an alias representing the constant value reference.
     */
    typedef const us8& const_reference;

    /**
     * Defines an alias representing the iterator.
     */
    typedef CircularBufferIterator<false> iterator;

    /**
     * Defines an alias representing the constant iterator
     */
    typedef CircularBufferIterator<true> const_iterator;

    /**
     * Defines an alias representing the reverse iterator
     */
    typedef std::reverse_iterator<CircularBufferIterator<false>> reverse_iterator;

    /**
     * Defines an alias representing the constant reverse iterator
     */
    typedef std::reverse_iterator<CircularBufferIterator<true>> const_reverse_iterator;

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
    iterator begin();

    /**
     * @copydoc cbegin() const
     */
    const_iterator begin() const;

    /**
    * Const overload of begin.
    *
    * @sa begin()
    *
    * @return A const_iterator.
    */
    const_iterator cbegin() const;

    /**
     * Gets the begin reverse iterator
     *
     * @return Reverse iterator point to begin position.
     */
    reverse_iterator rbegin();

    /**
     * @copydoc crbegin() const
     */
    const_reverse_iterator rbegin() const;

    /**
     * Gets the constant begin reverse iterator.
     *
     * @return Constant reverse iterator point to begin position.
     */
    const_reverse_iterator crbegin() const;

    /**
     * Gets the end iterator.
     *
     * @return The end CircularBufferIterator.
     */
    iterator end();

    /**
    * @copydoc cend() const
    */
    const_iterator end() const;

    /**
    * Const overload of end.
    *
    * @sa end()
    *
    * @return A const_iterator.
    */
    const_iterator cend() const;

    /**
    * Gets the end reverse iterator
    *
    * @return Reverse iterator point to begin position.
    */
    reverse_iterator rend();

    /**
    * @copydoc crend() const
    */
    const_reverse_iterator rend() const;

    /**
    * Gets the constant end reverse iterator.
    *
    * @return Constant reverse iterator point to end position.
    */
    const_reverse_iterator crend() const;

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
     * Get content space of this buffer.
     *
     * @param [out] bufs Content spaces.
     *
     * @return The size of content space array.
     */
    size_t content_buffers(BufDescriptor bufs[2]);

    /**
     * Get free space of this buffer.
     *
     * @param [out] bufs Free spaces.
     *
     * @return The size of free space array.
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

/**
* CircularBuffer iterator.
*/
template<bool IsConst> class CircularBufferIterator
{
public:

    /**
    * Defines an alias representing type of the size.
    */
    typedef CircularBuffer::size_type size_type;

    /**
    * Defines an alias representing type of the difference.
    */
    typedef CircularBuffer::difference_type difference_type;

    /**
    * Defines an alias representing type of the value.
    */
    typedef CircularBuffer::value_type value_type;

    /**
    * Defines an alias representing type of the pointer.
    */
    typedef typename std::conditional<IsConst, CircularBuffer::const_pointer, CircularBuffer::pointer>::type pointer;

    /**
    * Defines an alias representing type of the reference.
    */
    typedef typename std::conditional<IsConst, CircularBuffer::const_reference, CircularBuffer::reference>::type reference;

    /**
    * Constructor
    *
    * @param index Zero-based index of the buffer.
    * @param buf The buffer.
    */
    CircularBufferIterator(difference_type index, typename std::conditional<IsConst, const CircularBuffer*, CircularBuffer*>::type buf) :m_index(index), m_buf(buf) {}

    CircularBufferIterator(const CircularBufferIterator<false> &rhs) :m_index(rhs.Index()), m_buf(rhs.Buf()) {}

    /**
    * Indirection operator
    *
    * @return The element reference pointted by this iterator.
    */
    reference operator*()
    {
        return this->operator[](0);
    }

    /**
    * Member dereference operator
    *
    * @return The element pointer pointted by this iterator.
    */
    pointer operator->()
    {
        return &this->operator[](0);
    }

    /**
    * Increment operator
    *
    * @return Iterator reference after increment.
    */
    CircularBufferIterator& operator++()
    {
        ++m_index;
        return *this;
    }

    /**
    * Increment operator
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
    CircularBufferIterator& operator--()
    {
        --m_index;
        return *this;
    }

    /**
    * Decrement operator
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
    CircularBufferIterator& operator+=(difference_type diff)
    {
        m_index += diff;
        return *this;
    }

    /**
    * Subtraction assignment operator
    *
    * @param diff The difference value.
    *
    * @return Iterator reference after subtraction.
    */
    CircularBufferIterator& operator-=(difference_type diff)
    {
        m_index -= diff;
        return *this;
    }

    /**
    * Array indexer operator
    *
    * @param index Zero-based index of the.
    *
    * @return The element reference pointted by this iterator.
    */
    reference operator[](size_t index)
    {
        return m_buf->operator[](m_index + index);
    }

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

    /**
    * Gets the internal buffer point of this iterator.
    *
    * @return A pointer to internal CircularBuffer.
    */
    CircularBuffer* Buf() const
    {
        return m_buf;
    }

private:
    difference_type m_index;	/**< Zero-based index of the buffer.  */

    typename std::conditional<IsConst, const CircularBuffer*, CircularBuffer*>::type m_buf;  /**< The buffer. */
};

namespace std
{
    /**
    * CircularBuffer iterator traits.
    *
    * @tparam IsConst Wether the iterator is constant.
    */
    template<bool IsConst> struct iterator_traits<CircularBufferIterator<IsConst>>
    {
        typedef typename CircularBufferIterator<IsConst>::difference_type difference_type;

        typedef typename CircularBufferIterator<IsConst>::value_type value_type;

        typedef typename CircularBufferIterator<IsConst>::pointer pointer;

        typedef typename CircularBufferIterator<IsConst>::reference reference;

        typedef std::random_access_iterator_tag iterator_category;
    };
}

/**
* Equality operator
*
* @tparam IsConst Wether the iterator is constant.
* @param lhs The first instance to compare.
* @param rhs The second instance to compare.
*
* @return True if the parameters are considered equivalent.
*/
template<bool IsConst> bool operator==(const CircularBufferIterator<IsConst> &lhs, const CircularBufferIterator<IsConst> &rhs)
{
    return lhs.Equal(rhs);
}

/**
* Inequality operator
*
* @tparam IsConst Wether the iterator is constant.
* @param lhs The first instance to compare.
* @param rhs The second instance to compare.
*
* @return True if the parameters are not considered equivalent.
*/
template<bool IsConst> bool operator!=(const CircularBufferIterator<IsConst> &lhs, const CircularBufferIterator<IsConst> &rhs)
{
    return !(lhs == rhs);
}

/**
* Addition operator
*
* @tparam IsConst Wether the iterator is constant.
* @param lhs  The iterator.
* @param diff A value to be added to iterator.
*
* @return The iterator after addition.
*/
template<bool IsConst> CircularBufferIterator<IsConst> operator+(const CircularBufferIterator<IsConst> &lhs, typename CircularBufferIterator<IsConst>::difference_type diff)
{
    CircularBufferIterator<IsConst> ret = lhs;
    ret += diff;
    return ret;
}

/**
* Addition operator
*
* @tparam IsConst Wether the iterator is constant.
* @param diff A value to be added to iterator.
* @param rhs  The iterator.
*
* @return The iterator after addition.
*/
template<bool IsConst> CircularBufferIterator<IsConst> operator+(typename CircularBufferIterator<IsConst>::difference_type diff, const CircularBufferIterator<IsConst> &rhs)
{
    return rhs + diff;
}

/**
* Subtraction operator
*
* @tparam IsConst Wether the iterator is constant.
* @param lhs  The iterator.
* @param diff A value to be subtractted to iterator.
*
* @return The iterator after subtraction.
*/
template<bool IsConst> CircularBufferIterator<IsConst> operator-(const CircularBufferIterator<IsConst> &lhs, typename CircularBufferIterator<IsConst>::difference_type diff)
{
    CircularBufferIterator<IsConst> ret = lhs;
    ret -= diff;
    return ret;
}

/**
* Subtraction operator
*
* @tparam IsConst Wether the iterator is constant.
* @param lhs The left hand side.
* @param rhs The right hand side.
*
* @return The difference between two iterator.
*/
template<bool IsConst> typename CircularBufferIterator<IsConst>::difference_type operator-(const CircularBufferIterator<IsConst> &lhs, const CircularBufferIterator<IsConst> &rhs)
{
    return lhs.Index() - rhs.Index();
}

/**
* Less-than comparison operator
*
* @tparam IsConst Wether the iterator is constant.
* @param lhs The first instance to compare.
* @param rhs The second instance to compare.
*
* @return True if the first parameter is less than the second.
*/
template<bool IsConst> bool operator<(const CircularBufferIterator<IsConst> &lhs, const CircularBufferIterator<IsConst> &rhs)
{
    return lhs.Index() < rhs.Index();
}

/**
* Greater-than comparison operator
*
* @tparam IsConst Wether the iterator is constant.
* @param lhs The first instance to compare.
* @param rhs The second instance to compare.
*
* @return True if the first parameter is greater than to the second.
*/
template<bool IsConst> bool operator>(const CircularBufferIterator<IsConst> &lhs, const CircularBufferIterator<IsConst> &rhs)
{
    return rhs < lhs;
}

/**
* Less-than-or-equal comparison operator
*
* @tparam IsConst Wether the iterator is constant.
* @param lhs The first instance to compare.
* @param rhs The second instance to compare.
*
* @return True if the first parameter is less than or equal to the second.
*/
template<bool IsConst> bool operator<=(const CircularBufferIterator<IsConst> &lhs, const CircularBufferIterator<IsConst> &rhs)
{
    return !(lhs > rhs);
}

/**
* Greater-than-or-equal comparison operator
*
* @tparam IsConst Wether the iterator is constant.
* @param lhs The first instance to compare.
* @param rhs The second instance to compare.
*
* @return True if the first parameter is greater than or equal to the second.
*/
template<bool IsConst> bool operator>=(const CircularBufferIterator<IsConst> &lhs, const CircularBufferIterator<IsConst> &rhs)
{
    return !(lhs < rhs);
}

namespace std
{
    /**
    * Swap two CircularBufferIterator.
    *
    * @tparam IsConst Wether the iterator is constant.
    * @param [in,out] a A CircularBufferIterator to process.
    * @param [in,out] b A CircularBufferIterator to process.
    */
    template<bool IsConst> void swap(CircularBufferIterator<IsConst> &a, CircularBufferIterator<IsConst> &b) noexcept
    {
        a.swap(b);
    }
}

#endif /* CIRCULARBUFFER_H */