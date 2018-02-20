#ifndef FRAMERECVHELPER_H
#define FRAMERECVHELPER_H

#include "CircularBuffer.h"

/**
 * Frame probe helper.
 */
class FrameRecvHelper
{
public:

	/**
	 * Values that represent frame probe result.
	 */
	enum class ProbeHint :int
	{
		Successful = 0, /**< Probe successful. */
		Continue, /**< Maybe a partial frame or no data to probe. */
		Failed  /**< Probe failed. */
	};

	/**
	 * Encapsulates the result data of a probe operation.
	 */
	struct ProbeResult
	{
		ProbeHint m_hint;   /**< Probe result. */

		size_t m_predicatorIndex;   /**< Indicate an index of predicator array when m_hint is Successful,otherwise not used.当m_hint为Successful时，指示predicator数组的索引，m_hint为其他值时无意义 */

		size_t m_beg;   /**< Indicate begin offset of a frame in buffer when m_hint is Successful,otherwise not used. */

		size_t m_size;  /**< Indicate a frame size in buffer when m_hint is Successful,otherwise not used. */
	};

	/**
	 * Probe frame a frame in buf with predicators in predicator array.
	 *
	 * @tparam PredicateType Type of the predicators.
	 * @param [in,out] buf	 The buffer.
	 * @param predicator The predicator array.
	 * @param predicatorSize Size of the predicator array.
	 * 	
	 * @note The function signature of predicator must be:
	 * @code ProbeHint(CircularBuffer &buf, //Data buffer
	 * 		 size_t beg, //Current probe begin offset
	 * 		 size_t &end //Frame end offset in buffer when probe successful,otherwise not used.
	 * 	); @endcode
	 *
	 * @return A ProbeResult.
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

	/**
	 * Move a range of data from CircularBuffer to LinearBuffer.
	 *
	 * @param [in,out] buf Buffer that will be move from.
	 * @param beg Begin offset in the CircularBuffer.
	 * @param size Move size.
	 * @param [in,out] seqBuf Buffer that will be move to.
	 */
	inline static void AdjustBuffer(CircularBuffer &buf, size_t beg, size_t size, LinearBuffer &seqBuf)
	{
		buf.copy_to(seqBuf, beg, size);
		buf.pop_front(beg + size);
	}
};

#endif /* FRAMERECVHELPER_H */