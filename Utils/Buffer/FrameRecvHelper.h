#ifndef FRAMERECVHELPER_H
#define FRAMERECVHELPER_H

#include "CircularBuffer.h"

class FrameRecvHelper
{
public:
	enum class ProbeHint :int
	{
		Successful = 0,
		Continue,
		Failed
	};

	struct ProbeResult
	{
		ProbeHint m_hint;

		size_t m_predicatorIndex;//当m_hint为Successful时，指示predicator数组的索引，m_hint为其他值时无意义

		size_t m_beg;

		size_t m_size;
	};

	/*
	 * PredicateType=ProbeHint(CircularBuffer &buf,size_t beg,size_t &end)
	 * buf-数据缓冲区
	 * beg-当前探测的起始索引
	 * end-如果成功则输出结束索引，否则无意义
	 */
	template<typename PredicateType> static ProbeResult ProbeFrame(CircularBuffer &buf, PredicateType predicator[], size_t predicatorSize)
	{
		size_t currentBeg = 0;
		size_t end = 0;
		while (currentBeg < buf.size())
		{
			for (size_t i = 0; i < predicatorSize; ++i)
			{
				ProbeHint hint = predicator[i](buf, currentBeg, end);
				if (hint == ProbeHint::Successful)
				{
					buf.pop_front(currentBeg);
					return{ hint ,i,0,end - currentBeg };
				}
				else if (hint == ProbeHint::Continue)
				{
					buf.pop_front(currentBeg);
					return{ hint ,0,0,0 };
				}
			}
			++currentBeg;
		}
		buf.clear();
		return { ProbeHint::Continue,0,0,0 };
	}

	inline static void AdjustBuffer(CircularBuffer &buf, size_t beg, size_t size, LinearBuffer &seqBuf)
	{
		buf.copy_to(seqBuf, beg, size);
		buf.pop_front(beg + size);
	}
};

#endif /* FRAMERECVHELPER_H */