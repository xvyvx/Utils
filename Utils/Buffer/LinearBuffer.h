#ifndef LINEARBUFFER_H
#define LINEARBUFFER_H

#include "../Common/CommonHdr.h"
#include <initializer_list>
#include <iterator>
#include <stdexcept>

class UTILS_EXPORTS_API LinearBuffer
{
public:
	typedef us8 value_type;

	typedef size_t size_type;

	typedef ptrdiff_t difference_type;

	typedef value_type& reference;

	typedef const value_type& const_reference;

	typedef value_type* pointer;

	typedef const value_type* const_pointer;

	typedef pointer iterator;

	typedef const_pointer const_iterator;

	typedef std::reverse_iterator<pointer> reverse_iterator;

	typedef std::reverse_iterator<const_pointer> const_reverse_iterator;

	LinearBuffer(size_type capacity = 0);

	LinearBuffer(const LinearBuffer &rhs);

	LinearBuffer(LinearBuffer &&rhs) noexcept;

	~LinearBuffer();

	LinearBuffer& operator=(const LinearBuffer &rhs);

	LinearBuffer& operator=(LinearBuffer &&rhs) noexcept;

	void assign(size_type count, const_reference value);

	void assign(const_iterator first, const_iterator last);

	void assign(std::initializer_list<value_type> ilist);

	reference at(size_type pos)
	{
		if (pos + m_beg >= m_last)
		{
			throw std::out_of_range("out of range.");
		}
		return m_beg[pos];
	}

	const_reference at(size_type pos) const
	{
		if (pos + m_beg >= m_last)
		{
			throw std::out_of_range("out of range.");
		}
		return m_beg[pos];
	}

	reference operator[](size_type pos)
	{
#ifndef NDEBUG
		return at(pos);
#endif
		return m_beg[pos];
	}

	const_reference operator[](size_type pos) const
	{
#ifndef NDEBUG
		return at(pos);
#endif
		return m_beg[pos];
	}

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

	pointer data() noexcept
	{
		return m_beg;
	}

	const_pointer data() const noexcept
	{
		return m_beg;
	}

	iterator begin() noexcept
	{
		return m_beg;
	}

	const_iterator begin() const noexcept
	{
		return m_beg;
	}

	const_iterator cbegin() const noexcept
	{
		return begin();
	}

	iterator end() noexcept
	{
		return m_last;
	}

	const_iterator end() const noexcept
	{
		return m_last;
	}

	const_iterator cend() const noexcept
	{
		return end();
	}

	reverse_iterator rbegin() noexcept
	{
		return reverse_iterator(m_last);
	}

	const_reverse_iterator rbegin() const noexcept
	{
		return const_reverse_iterator(m_last);
	}

	const_reverse_iterator crbegin() const noexcept
	{
		return rbegin();
	}

	reverse_iterator rend() noexcept
	{
		return reverse_iterator(m_beg);
	}

	const_reverse_iterator rend() const noexcept
	{
		return const_reverse_iterator(m_beg);
	}

	const_reverse_iterator crend() const noexcept
	{
		return rend();
	}

	bool empty() const noexcept
	{
		return size() == 0;
	}

	size_type size() const noexcept
	{
		return m_last - m_beg;
	}

	size_type max_size() const noexcept
	{
		return capacity();
	}

	void reserve(size_type newCapacity);

	size_type capacity() const noexcept
	{
		return m_capacityLast - m_beg;
	}

	void clear() noexcept
	{
		m_last = m_beg;
	}

	iterator insert(const_iterator pos, const_reference value)
	{
		return insert(pos, 1, value);
	}

	iterator insert(const_iterator pos, size_type count, const_reference value);

	iterator insert(const_iterator pos, const_iterator first, const_iterator last);

	iterator insert(const_iterator pos, std::initializer_list<value_type> ilist);

	iterator erase(const_iterator pos);

	iterator erase(const_iterator first, const_iterator last);

	void push_back(const_reference value);

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

	void resize(size_type count)
	{
		resize(count, 0);
	}

	void resize(size_type count, const value_type& value);

	void swap(LinearBuffer &other) noexcept;

private:
	iterator m_beg;

	iterator m_last;

	iterator m_capacityLast;
};

namespace std
{
	inline void swap(LinearBuffer& lhs, LinearBuffer& rhs) noexcept
	{
		lhs.swap(rhs);
	}
}

#endif /* LINEARBUFFER_H */