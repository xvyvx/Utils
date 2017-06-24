#ifndef BINARYHELPER_H
#define BINARYHELPER_H

#include "BufferDescriptor.h"
#include "CircularBuffer.h"

class UTILS_EXPORTS_API BinaryHelper
{
public:
	static void PushBinData(LinearBuffer &buf, us32 data, us8 size, bool isLittleEndian)
	{
		if (isLittleEndian)
		{
			PushBinDataLE(buf, data, size);
		}
		else
		{
			PushBinDataBE(buf, data, size);
		}
	}

	static void SetBinData(LinearBuffer &buf, size_t offset, us32 data, us8 size, bool isLittleEndian)
	{
		if (isLittleEndian)
		{
			SetBinDataLE(buf.data(), offset, data, size);
		}
		else
		{
			SetBinDataBE(buf.data(), offset, data, size);
		}
	}

	static void SetBinData(us8 *buf, size_t offset, us32 data, us8 size, bool isLittleEndian)
	{
		if (isLittleEndian)
		{
			SetBinDataLE(buf, offset, data, size);
		}
		else
		{
			SetBinDataBE(buf, offset, data, size);
		}
	}

	static us32 ReadBinData(const LinearBuffer &buf, size_t offset, us8 size, bool isLittleEndian)
	{
		if (isLittleEndian)
		{
			return ReadBinDataLE(&buf[0], offset, size);
		}
		else
		{
			return ReadBinDataBE(&buf[0], offset, size);
		}
	}

	static us32 ReadBinData(const us8 *buf, size_t offset, us8 size, bool isLittleEndian)
	{
		if (isLittleEndian)
		{
			return ReadBinDataLE(buf, offset, size);
		}
		else
		{
			return ReadBinDataBE(buf, offset, size);
		}
	}

	static us32 ReadBinData(CircularBuffer &buf, size_t offset, us8 size, bool isLittleEndian);

private:
	static void PushBinDataLE(LinearBuffer &buf, us32 data, us8 size);

	static void PushBinDataBE(LinearBuffer &buf, us32 data, us8 size);

	static void SetBinDataLE(us8 *buf, size_t offset, us32 data, us8 size);

	static void SetBinDataBE(us8 *buf, size_t offset, us32 data, us8 size);

	static us32 ReadBinDataLE(const us8 *buf, size_t offset, us8 size);

	static us32 ReadBinDataBE(const us8 *buf, size_t offset, us8 size);
};

#endif /* BINARYHELPER_H */