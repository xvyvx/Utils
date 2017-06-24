#include "LinearBuffer.h"
#include "../Common/RunTimeLibraryHelper.h"

LinearBuffer::LinearBuffer(size_type capacity) :m_beg(capacity == 0 ? nullptr : new us8[capacity]), m_last(m_beg), m_capacityLast(m_beg + capacity)
{
}

LinearBuffer::LinearBuffer(const LinearBuffer &rhs):LinearBuffer(rhs.capacity())
{
	size_type rhsSize = rhs.size();
	if(rhsSize)
	{
		RunTimeLibraryHelper::MemCpy(m_beg, m_capacityLast - m_beg, rhs.m_beg, rhsSize);
		m_last = m_beg + rhsSize;
	}
}

LinearBuffer::LinearBuffer(LinearBuffer &&rhs) noexcept :LinearBuffer(0)
{
	swap(rhs);
}

LinearBuffer::~LinearBuffer()
{
	delete[] m_beg;
}

LinearBuffer& LinearBuffer::operator=(const LinearBuffer &rhs)
{
	LinearBuffer temp(rhs);
	swap(temp);
	return *this;
}

LinearBuffer& LinearBuffer::operator=(LinearBuffer &&rhs) noexcept
{
	swap(rhs);
	return *this;
}

void LinearBuffer::assign(size_type count, const_reference value)
{
#ifndef NDEBUG
	if(count > capacity())
	{
		throw std::out_of_range("assign out of range.");
	}
#endif
	memset(m_beg, value, count);
	m_last = m_beg + count;
}

void LinearBuffer::assign(const_iterator first, const_iterator last)
{
#ifndef NDEBUG
	if (last < first || static_cast<size_type>(last - first) > capacity())
	{
		throw std::out_of_range("assign out of range.");
	}
#endif
	size_type size = last - first;
	RunTimeLibraryHelper::MemCpy(m_beg, size, first, size);
	m_last = m_beg + size;
}

void LinearBuffer::assign(std::initializer_list<value_type> ilist)
{
	size_type realSize = ilist.size();
#ifndef NDEBUG
	if (realSize > capacity())
	{
		throw std::out_of_range("assign out of range.");
	}
#endif
	RunTimeLibraryHelper::MemCpy(m_beg, realSize, ilist.begin(), realSize);
	m_last = m_beg + realSize;
}

void LinearBuffer::reserve(size_type newCapacity)
{
	if(newCapacity > capacity())
	{
		LinearBuffer rhs(newCapacity);
		rhs.insert(rhs.begin(), begin(), end());
		swap(rhs);
	}
}

LinearBuffer::iterator LinearBuffer::insert(const_iterator pos, size_type count, const_reference value)
{
#ifndef NDEBUG
	if (size() + count > capacity() || pos > m_last || pos < m_beg)
	{
		throw std::out_of_range("insert out of range.");
	}
#endif
	iterator iter = const_cast<iterator>(pos);
	RunTimeLibraryHelper::MemMove(iter + count, m_capacityLast - (pos + count), pos, m_last - pos);
	memset(iter, value, count);
	m_last += count;
	return iter;
}

LinearBuffer::iterator LinearBuffer::insert(const_iterator pos, const_iterator first, const_iterator last)
{
	size_type inserttedSize = last - first;
#ifndef NDEBUG
	if (last<first || size() + inserttedSize > capacity() || pos > m_last || pos < m_beg)
	{
		throw std::out_of_range("insert out of range.");
	}
	if ((first >= m_beg&&first < m_last) || (last >= m_beg&&last < m_last))
	{
		throw std::range_error("insert range overlaped.");
	}
#endif
	iterator iter = const_cast<iterator>(pos);
	RunTimeLibraryHelper::MemMove(iter + inserttedSize, m_capacityLast - (pos + inserttedSize), pos, m_last - pos);
	RunTimeLibraryHelper::MemCpy(iter, inserttedSize, first, inserttedSize);
	m_last += inserttedSize;
	return iter;
}

LinearBuffer::iterator LinearBuffer::insert(const_iterator pos, std::initializer_list<value_type> ilist)
{
#ifndef NDEBUG
	if (size() + ilist.size() > capacity() || pos > m_last || pos < m_beg)
	{
		throw std::out_of_range("insert out of range.");
	}
	if ((ilist.begin() >= m_beg&&ilist.begin() < m_last) || (ilist.end() >= m_beg&&ilist.end() < m_last))
	{
		throw std::range_error("insert range overlaped.");
	}
#endif
	iterator iter = const_cast<iterator>(pos);
	size_type realSize = ilist.size();
	RunTimeLibraryHelper::MemMove(iter + realSize, m_capacityLast - (pos + realSize), pos, m_last - pos);
	RunTimeLibraryHelper::MemCpy(iter, realSize, ilist.begin(), realSize);
	m_last += realSize;
	return iter;
}

LinearBuffer::iterator LinearBuffer::erase(const_iterator pos)
{
#ifndef NDEBUG
	if (empty() || pos >= m_last || pos < m_beg)
	{
		throw std::out_of_range("erase out of range.");
	}
#endif
	iterator iter = const_cast<iterator>(pos);
	RunTimeLibraryHelper::MemMove(iter, m_capacityLast - pos, pos + 1, m_last - (pos + 1));
	--m_last;
	return iter;
}

LinearBuffer::iterator LinearBuffer::erase(const_iterator first, const_iterator last)
{
#ifndef NDEBUG
	if (empty() || first >= m_last || first < m_beg || last > m_last || last < m_beg || last < first)
	{
		throw std::out_of_range("erase out of range.");
	}
#endif
	iterator iterBeg = const_cast<iterator>(first);
	iterator iterEnd = const_cast<iterator>(last);
	RunTimeLibraryHelper::MemMove(iterBeg, m_capacityLast - iterBeg, iterEnd, m_last - iterEnd);
	m_last -= iterEnd - iterBeg;
	return iterEnd;
}

void LinearBuffer::push_back(const_reference value)
{
#ifndef NDEBUG
	if (size() == capacity())
	{
		throw std::out_of_range("push_back out of range.");
	}
#endif
	*m_last++ = value;
}

void LinearBuffer::resize(size_type count, const value_type& value)
{
#ifndef NDEBUG
	if (count > capacity())
	{
		throw std::out_of_range("resize out of range.");
	}
#endif
	size_type currentSize = size();
	if (count > currentSize)
	{
		memset(m_last, value, count);
		m_last += count;
	}
	else
	{
		m_last -= currentSize - count;
	}
}

void LinearBuffer::swap(LinearBuffer &other) noexcept
{
	std::swap(m_beg, other.m_beg);
	std::swap(m_last, other.m_last);
	std::swap(m_capacityLast, other.m_capacityLast);
}