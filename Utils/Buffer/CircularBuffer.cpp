#include "CircularBuffer.h"
#include <assert.h>
#include <string.h>
#include <algorithm>
#include "../Common/RunTimeLibraryHelper.h"

CircularBufferIterator& CircularBufferIterator::operator++()
{
	++m_index;
	return *this;
}

CircularBufferIterator& CircularBufferIterator::operator--()
{
	--m_index;
	return *this;
}

CircularBufferIterator& CircularBufferIterator::operator+=(difference_type diff)
{
	m_index += diff;
	return *this;
}

CircularBufferIterator& CircularBufferIterator::operator-=(difference_type diff)
{
	m_index -= diff;
	return *this;
}

CircularBufferIterator::reference CircularBufferIterator::operator[](size_t index)
{
	return m_buf->operator[](index);
}

bool operator==(const CircularBufferIterator &lhs, const CircularBufferIterator &rhs)
{
	return lhs.Equal(rhs);
}

CircularBufferIterator operator+(const CircularBufferIterator &lhs, CircularBufferIterator::difference_type diff)
{
	CircularBufferIterator ret = lhs;
	ret += diff;
	return ret;
}

CircularBufferIterator operator-(const CircularBufferIterator &lhs, CircularBufferIterator::difference_type diff)
{
	CircularBufferIterator ret = lhs;
	ret -= diff;
	return ret;
}

CircularBufferIterator::difference_type operator-(const CircularBufferIterator &lhs, const CircularBufferIterator &rhs)
{
	return lhs.Index() - rhs.Index();
}

bool operator<(const CircularBufferIterator &lhs, const CircularBufferIterator &rhs)
{
	return lhs.Index() < rhs.Index();
}

CircularBuffer::CircularBuffer(size_t capacity) :m_buf(capacity ? new us8[capacity] : nullptr), m_beg(m_buf), m_subSize(0), m_capacity(capacity)
{
}

CircularBuffer::~CircularBuffer()
{
	delete[] m_buf;
}

CircularBuffer::CircularBuffer(const CircularBuffer &rhs) :m_buf(rhs.m_capacity ? new us8[rhs.m_capacity] : nullptr), m_beg(m_buf ? m_buf + (rhs.m_beg - rhs.m_buf) : nullptr)
	, m_subSize(rhs.m_subSize), m_capacity(rhs.m_capacity)
{
	if (m_buf)
	{
		RunTimeLibraryHelper::MemCpy(m_buf, m_capacity, rhs.m_buf, m_capacity);
	}
}

CircularBuffer::CircularBuffer(CircularBuffer &&rhs) noexcept : m_buf(rhs.m_buf), m_beg(rhs.m_beg), m_subSize(rhs.m_subSize), m_capacity(rhs.m_capacity)
{
	rhs.m_buf = nullptr;
	rhs.m_beg = nullptr;
	rhs.m_subSize = 0;
	rhs.m_capacity = 0;
}

CircularBuffer& CircularBuffer::operator=(const CircularBuffer &rhs)
{
	CircularBuffer temp(rhs);
	swap(temp);
	return *this;
}

CircularBuffer& CircularBuffer::operator=(CircularBuffer &&rhs) noexcept
{
	swap(rhs);
	return *this;
}

const us8& CircularBuffer::operator[](size_t index) const
{
	assert(m_buf);
	return const_cast<CircularBuffer*>(this)->operator[](index);
}

us8& CircularBuffer::operator[](size_t index)
{
	assert(m_buf);
	assert(index < m_capacity);
	us8 *end = m_buf + m_capacity;
	return m_beg + index < end ? m_beg[index] : m_buf[index - (end - m_beg)];
}

CircularBufferIterator CircularBuffer::begin()
{
	return CircularBufferIterator(0, this);
}

CircularBufferIterator CircularBuffer::end()
{
	return CircularBufferIterator(static_cast<CircularBufferIterator::difference_type>(size()), this);
}

void CircularBuffer::reserve(std::size_t newCapacity)
{
	if (newCapacity > m_capacity)
	{
		CircularBuffer temp(newCapacity);
		if (m_subSize)
		{
			us8 *capacityEnd = m_buf + m_capacity;
			us8 *end = m_beg + m_subSize;
			if (end > capacityEnd)
			{
				size_t leftSize1 = capacityEnd - m_beg;
				RunTimeLibraryHelper::MemCpy(temp.m_buf, leftSize1, m_beg, leftSize1);
				RunTimeLibraryHelper::MemCpy(temp.m_buf + leftSize1, m_subSize - leftSize1, m_buf, m_subSize - leftSize1);
			}
			else
			{
				RunTimeLibraryHelper::MemCpy(temp.m_buf, m_subSize, m_beg, m_subSize);
			}
		}
		swap(temp);
	}
}

void CircularBuffer::pop_front(size_t popSize)
{
	assert(m_buf);
	if (popSize == m_subSize)
	{
		clear();
	}
	else
	{
		us8 *end = m_buf + m_capacity;
		size_t leftSize = end - m_beg;
		if (leftSize>popSize)
		{
			m_beg += popSize;
		}
		else
		{
			m_beg = m_buf + (popSize - leftSize);
		}
		m_subSize -= popSize;
	}
}

size_t CircularBuffer::free_buffers(BufDescriptor bufs[2])
{
	assert(m_buf);
	us8 *end = m_buf + m_capacity;
	if (m_buf == m_beg)
	{
		us8 *tempEnd = m_buf + m_subSize;
		bufs[0].m_beg = tempEnd;
		bufs[0].m_size = static_cast<size_t>(end - tempEnd);
		return 1;
	}
	else if (m_beg + m_subSize >= end)
	{
		size_t tempSize = m_subSize - (end - m_beg);
		bufs[0].m_beg = m_buf + tempSize;
		bufs[0].m_size = static_cast<size_t>(m_beg - (m_buf + tempSize));
		return 1;
	}
	else
	{
		us8 *tempBeg = m_beg + m_subSize;
		bufs[0].m_beg = tempBeg;
		bufs[0].m_size = static_cast<size_t>(end - tempBeg);
		bufs[1].m_beg = m_buf;
		bufs[1].m_size = static_cast<size_t>(m_beg - m_buf);
		return 2;
	}
}

void CircularBuffer::copy_to(LinearBuffer &buf, size_t beg, size_t size)
{
	assert(m_buf);
	us8 *capacityEnd = m_buf + m_capacity;
	size_t realSize = size == std::numeric_limits<size_t>::max() ? m_subSize : size;
	assert(size <= m_subSize);
	us8 *realBeg = &this->operator[](beg);
	assert(realBeg >= m_beg&&realBeg < capacityEnd);
	us8 *tempEnd = realBeg + realSize;
	if (tempEnd <= capacityEnd)
	{
		buf.assign(realBeg, tempEnd);
	}
	else
	{
		buf.assign(realBeg, capacityEnd);
		buf.insert(buf.end(), m_buf, m_buf + (realSize - (capacityEnd - realBeg)));
	}
}

void CircularBuffer::swap(CircularBuffer &buf) noexcept
{
	std::swap(m_buf, buf.m_buf);
	std::swap(m_beg, buf.m_beg);
	std::swap(m_subSize, buf.m_subSize);
	std::swap(m_capacity, buf.m_capacity);
}

bool operator==(const CircularBuffer &lhs, const CircularBuffer &rhs)
{
	return lhs.size() == rhs.size() && std::equal(const_cast<CircularBuffer&>(lhs).begin(), const_cast<CircularBuffer&>(lhs).end(), const_cast<CircularBuffer&>(rhs).begin());
}

bool operator<(const CircularBuffer &lhs, const CircularBuffer &rhs)
{
	if (lhs.size() < rhs.size())
	{
		return true;
	}
	else if (lhs.size() > rhs.size())
	{
		return false;
	}
	for (int i = 0; i < lhs.size(); ++i)
	{
		us8 lVal = lhs[i];
		us8 rVal = rhs[i];
		if (lVal < rVal)
		{
			return true;
		}
		else if (lVal > rVal)
		{
			return false;
		}
	}
	return false;
}