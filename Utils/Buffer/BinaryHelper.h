#ifndef BINARYHELPER_H
#define BINARYHELPER_H

#include "BufferDescriptor.h"
#include "CircularBuffer.h"

/**
 * Buildin type binary operation utilities.
 */
class UTILS_EXPORTS_API BinaryHelper
{
public:

	/**
	 * Pushes a unsigned integer at the end of a buf(at most 4 bytes).
	 *
	 * @param [in,out] buf	 The buffer.
	 * @param data Pushed data.
	 * @param size Pushed size.
	 * @param isLittleEndian True if push as little endian, false if as big endian.
	 */
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

	/**
	 * Sets a unsigned integer at the specific position of a buf(at most 4 bytes).
	 *
	 * @param [in,out] buf The buffer.
	 * @param offset Set offset in the buffer.
	 * @param data Setted data.
	 * @param size Setted size.
	 * @param isLittleEndian True if set as little endian, false if as big endian.
	 */
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

	/**
	 * Sets a unsigned integer at the specific position of a buf(at most 4 bytes).
	 *
	 * @param [in,out] buf	 The buffer.
	 * @param offset	 Set offset in the buffer.
	 * @param data Setted data.
	 * @param size Setted size.
	 * @param isLittleEndian True if set as little endian, false if as big endian.
	 */
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

	/**
	 * Reads a unsigned integer at the specific position of a buf(at most 4 bytes).
	 *
	 * @param buf	 The buffer.
	 * @param offset Read offset in the buffer.
	 * @param size Read size.
	 * @param isLittleEndian True if read as little endian, false if as big endian.
	 *
	 * @return The readed data.
	 */
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

	/**
	 * Reads a unsigned integer at the specific position of a buf(at most 4 bytes).
	 *
	 * @param buf The buffer.
	 * @param offset Read offset in the buffer.
	 * @param size Read size.
	 * @param isLittleEndian True if read as little endian, false if as big endian.
	 *
	 * @return The readed data.
	 */
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

	/**
	 * Reads a unsigned integer at the specific position of a buf(at most 4 bytes).
	 *
	 * @param [in,out] buf	 The buffer.
	 * @param offset Read offset in the buffer.
	 * @param size Read size.
	 * @param isLittleEndian True if read as little endian, false if as big endian.
	 *
	 * @return The readed data.
	 */
	static us32 ReadBinData(CircularBuffer &buf, size_t offset, us8 size, bool isLittleEndian);

private:

	/**
	 * Pushes a unsigned integer at the end of a buf as little endian(at most 4 bytes).
	 *
	 * @param [in,out] buf The buffer.
	 * @param data Pushed data.
	 * @param size Pushed size.
	 */
	static void PushBinDataLE(LinearBuffer &buf, us32 data, us8 size);

	/**
	 * Pushes a unsigned integer at the end of a buf as big endian(at most 4 bytes).
	 *
	 * @param [in,out] buf The buffer.
	 * @param data Pushed data.
	 * @param size Pushed size.
	 */
	static void PushBinDataBE(LinearBuffer &buf, us32 data, us8 size);

	/**
	 * Sets a unsigned integer at the specific position of a buf as little endian(at most 4 bytes).
	 *
	 * @param [in,out] buf If non-null, the buffer.
	 * @param offset Set offset in the buffer.
	 * @param data Setted data.
	 * @param size Setted size.
	 */
	static void SetBinDataLE(us8 *buf, size_t offset, us32 data, us8 size);

	/**
	 * Sets a unsigned integer at the specific position of a buf as big endian(at most 4 bytes).
	 *
	 * @param [in,out] buf If non-null, the buffer.
	 * @param offset Set offset in the buffer.
	 * @param data Setted data.
	 * @param size Setted size.
	 */
	static void SetBinDataBE(us8 *buf, size_t offset, us32 data, us8 size);

	/**
	 * Reads a unsigned integer at the specific position of a buf as little endian(at most 4 bytes).
	 *
	 * @param buf The buffer.
	 * @param offset Read offset in the buffer.
	 * @param size Read size.
	 *
	 * @return The readed data.
	 */
	static us32 ReadBinDataLE(const us8 *buf, size_t offset, us8 size);

	/**
	 * Reads a unsigned integer at the specific position of a buf as big endian(at most 4 bytes).
	 *
	 * @param buf The buffer.
	 * @param offset Read offset in the buffer.
	 * @param size Read size.
	 *
	 * @return The readed data.
	 */
	static us32 ReadBinDataBE(const us8 *buf, size_t offset, us8 size);
};

#endif /* BINARYHELPER_H */