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
     * Pushes a float(four-bytes) at the end of a buf.
     * 
     * @param [in,out] buf The buffer.
     * @param data Pushed data.
     * @param isLittleEndian True if push as little endian, false if as big endian.
     */
    static void PushBinData(LinearBuffer &buf, f32 data, bool isLittleEndian)
    {
        if (isLittleEndian)
        {
            PushBinDataLE(buf, data);
        }
        else
        {
            PushBinDataBE(buf, data);
        }
    }

    /**
     * Pushes a signed integer at the end of a buf(at most 4 bytes).
     * 
     * @param [in,out] buf The buffer.
     * @param data Pushed data.
     * @param size Pushed size.
     * @param isLittleEndian True if push as little endian, false if as big endian.
     */
    static void PushBinDataSigned(LinearBuffer &buf, s32 data, us8 size, bool isLittleEndian)
    {
        if (isLittleEndian)
        {
            PushBinDataLE_Signed(buf, data, size);
        }
        else
        {
            PushBinDataBE_Signed(buf, data, size);
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
     * Sets a float(four-bytes) at the specific position of a buf.
     * 
     * @param [in,out] buf The buffer.
     * @param offset Set offset in the buffer.
     * @param data Setted data.
     * @param isLittleEndian True if set as little endian, false if as big endian.
     */
    static void SetBinData(us8 *buf, size_t offset, f32 data, bool isLittleEndian)
    {
        if (isLittleEndian)
        {
            SetBinDataLE(buf, offset, data);
        }
        else
        {
            SetBinDataBE(buf, offset, data);
        }
    }

    /**
     * Sets a signed integer at the specific position of a buf(at most 4 bytes).
     * 
     * @param [in,out] buf The buffer.
     * @param offset Set offset in the buffer.
     * @param data Setted data.
     * @param size Setted size.
     * @param isLittleEndian True if set as little endian, false if as big endian.
     */
    static void SetBinDataSigned(LinearBuffer &buf, size_t offset, s32 data, us8 size, bool isLittleEndian)
    {
        if (isLittleEndian)
        {
            SetBinDataLE_Signed(buf.data(), offset, data, size);
        }
        else
        {
            SetBinDataBE_Signed(buf.data(), offset, data, size);
        }
    }

    /**
     * Sets a signed integer at the specific position of a buf(at most 4 bytes).
     * 
     * @param [in,out] buf The buffer.
     * @param offset Set offset in the buffer.
     * @param data Setted data.
     * @param size Setted size.
     * @param isLittleEndian True if set as little endian, false if as big endian.
     */
    static void SetBinDataSigned(us8 *buf, size_t offset, s32 data, us8 size, bool isLittleEndian)
    {
        if (isLittleEndian)
        {
            SetBinDataLE_Signed(buf, offset, data, size);
        }
        else
        {
            SetBinDataBE_Signed(buf, offset, data, size);
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
     * Reads a float(four-bytes) at the specific position of a buf.
     * 
     * @param buf The buffer.
     * @param offset Read offset in the buffer.
     * @param isLittleEndian True if read as little endian, false if as big endian.
     * 
     * @return The readed data.
     */
    static f32 ReadBinData(const us8 *buf, size_t offset, bool isLittleEndian)
    {
        if (isLittleEndian)
        {
            return ReadBinDataLE(buf, offset);
        }
        else
        {
            return ReadBinDataBE(buf, offset);
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
    static s32 ReadBinDataSigned(const LinearBuffer &buf, size_t offset, s8 size, bool isLittleEndian)
    {
        if (isLittleEndian)
        {
            return ReadBinDataLE_Signed(&buf[0], offset, size);
        }
        else
        {
            return ReadBinDataBE_Signed(&buf[0], offset, size);
        }
    }

    /**
     * Reads a signed integer at the specific position of a buf(at most 4 bytes).
     * 
     * @param buf The buffer.
     * @param offset Read offset in the buffer.
     * @param size Read size.
     * @param isLittleEndian True if read as little endian, false if as big endian.
     * 
     * @return The readed data.
     */
    static s32 ReadBinDataSigned(const us8 *buf, size_t offset, us8 size, bool isLittleEndian)
    {
        if (isLittleEndian)
        {
            return ReadBinDataLE_Signed(buf, offset, size);
        }
        else
        {
            return ReadBinDataBE_Signed(buf, offset, size);
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
     * Pushes a float(four-bytes) at the end of a buf as little endian.
     *
     * @param [in,out] buf The buffer.
     * @param data Pushed data.
     */
    static void PushBinDataLE(LinearBuffer &buf, f32 data);

    /**
     * Pushes a float(four-bytes) at the end of a buf as big endian.
     *
     * @param [in,out] buf The buffer.
     * @param data Pushed data.
     */
    static void PushBinDataBE(LinearBuffer &buf, f32 data);

    /**
     * Pushes a signed integer at the end of a buf as little endian(at most 4 bytes).
     *
     * @param [in,out] buf The buffer.
     * @param data Pushed data.
     * @param size Pushed size.
     */
    static void PushBinDataLE_Signed(LinearBuffer &buf, s32 data, us8 size);

    /**
     * Pushes a signed integer at the end of a buf as big endian(at most 4 bytes).
     *
     * @param [in,out] buf The buffer.
     * @param data Pushed data.
     * @param size Pushed size.
     */
    static void PushBinDataBE_Signed(LinearBuffer &buf, s32 data, us8 size);

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
     * Sets a float(four-bytes) at the specific position of a buf as little endian.
     *
     * @param [in,out] buf If non-null, the buffer.
     * @param offset Set offset in the buffer.
     * @param data Setted data.
     */
    static void SetBinDataLE(us8 *buf, size_t offset, f32 data);

    /**
     * Sets a float(four-bytes) at the specific position of a buf as big endian.
     *
     * @param [in,out] buf If non-null, the buffer.
     * @param offset Set offset in the buffer.
     * @param data Setted data.
     */
    static void SetBinDataBE(us8 *buf, size_t offset, f32 data);

    /**
     * Sets a signed integer at the specific position of a buf as little endian(at most 4 bytes).
     *
     * @param [in,out] buf If non-null, the buffer.
     * @param offset Set offset in the buffer.
     * @param data Setted data.
     * @param size Setted size.
     */
    static void SetBinDataLE_Signed(us8 *buf, size_t offset, s32 data, us8 size);

    /**
     * Sets a unsigned integer at the specific position of a buf as big endian(at most 4 bytes).
     *
     * @param [in,out] buf If non-null, the buffer.
     * @param offset Set offset in the buffer.
     * @param data Setted data.
     * @param size Setted size.
     */
    static void SetBinDataBE_Signed(us8 *buf, size_t offset, s32 data, us8 size);

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

    /**
     * Reads a float(four-bytes) at the specific position of a buf as little endian.
     *
     * @param buf The buffer.
     * @param offset Read offset in the buffer.
     *
     * @return The readed data.
     */
    static f32 ReadBinDataLE(const us8 *buf, size_t offset);

    /**
     * Reads a float(four-bytes) at the specific position of a buf as big endian.
     *
     * @param buf The buffer.
     * @param offset Read offset in the buffer.
     *
     * @return The readed data.
     */
    static f32 ReadBinDataBE(const us8 *buf, size_t offset);

    /**
     * Reads a signed integer at the specific position of a buf as little endian(at most 4 bytes).
     *
     * @param buf The buffer.
     * @param offset Read offset in the buffer.
     * @param size Read size.
     *
     * @return The readed data.
     */
    static s32 ReadBinDataLE_Signed(const us8 *buf, size_t offset, us8 size);

    /**
     * Reads a signed integer at the specific position of a buf as big endian(at most 4 bytes).
     *
     * @param buf The buffer.
     * @param offset Read offset in the buffer.
     * @param size Read size.
     *
     * @return The readed data.
     */
    static s32 ReadBinDataBE_Signed(const us8 *buf, size_t offset, us8 size);
};

#endif /* BINARYHELPER_H */