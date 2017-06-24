#include "BinaryHelper.h"

void BinaryHelper::PushBinDataLE(LinearBuffer &buf, us32 data, us8 size)
{
	switch (size)
	{
	case 1:
		buf.push_back(data);
		break;
	case 2:
		buf.push_back(data & 0xFF);
		buf.push_back((data & 0xFF00) >> 8);
		break;
	case 3:
		buf.push_back(data & 0xFF);
		buf.push_back((data & 0xFF00) >> 8);
		buf.push_back((data & 0xFF0000) >> 16);
		break;
	case 4:
		buf.push_back(data & 0xFF);
		buf.push_back((data & 0xFF00) >> 8);
		buf.push_back((data & 0xFF0000) >> 16);
		buf.push_back((data & 0xFF000000) >> 24);
		break;
	}
}

void BinaryHelper::PushBinDataBE(LinearBuffer &buf, us32 data, us8 size)
{
	switch (size)
	{
	case 4:
		buf.push_back((data & 0xFF000000) >> 24);
	case 3:
		buf.push_back((data & 0xFF0000) >> 16);
	case 2:
		buf.push_back((data & 0xFF00) >> 8);
	case 1:
		buf.push_back(data);
	}
}

void BinaryHelper::SetBinDataLE(us8 *buf, size_t offset, us32 data, us8 size)
{
	switch (size)
	{
	case 1:
		buf[offset] = data;
		break;
	case 2:
#ifdef IS_LITTLE_ENDIAN
		if (offset % 2 == 0)
		{
			*reinterpret_cast<us16*>(buf+offset) = data;
		}
		else
#endif
		{
			us8 *temp = buf + offset;
			*temp = data;
			*(temp + 1) = (data & 0xFF00) >> 8;
		}
		break;
	case 3:
		{
			us8 *temp = buf + offset;
			*temp = data;
			*(temp + 1) = (data & 0xFF00) >> 8;
			*(temp + 2) = (data & 0xFF0000) >> 16;
		}
		break;
	case 4:
#ifdef IS_LITTLE_ENDIAN
		if (offset % 4 == 0)
		{
			*reinterpret_cast<us32*>(buf + offset) = data;
		}
		else
#endif
		{
			us8 *temp = buf + offset;
			*temp = data;
			*(temp + 1) = (data & 0xFF00) >> 8;
			*(temp + 2) = (data & 0xFF0000) >> 16;
			*(temp + 3) = (data & 0xFF000000) >> 24;
		}
		break;
	}
}

void BinaryHelper::SetBinDataBE(us8 *buf, size_t offset, us32 data, us8 size)
{
	switch (size)
	{
	case 1:
		buf[offset] = data;
		break;
	case 2:
#ifndef IS_LITTLE_ENDIAN
		if (offset % 2 == 0)
		{
			*reinterpret_cast<us16*>(buf + offset) = data;
		}
		else
#endif
		{
			us8 *temp = buf + offset;
			*temp = (data & 0xFF00) >> 8;
			*(temp + 1) = data;
		}
		break;
	case 3:
	{
		us8 *temp = buf + offset;
		*temp = (data & 0xFF0000) >> 16;
		*(temp + 1) = (data & 0xFF00) >> 8;
		*(temp + 2) = data;
	}
	break;
	case 4:
#ifndef IS_LITTLE_ENDIAN
		if (offset % 4 == 0)
		{
			*reinterpret_cast<us32*>(buf + offset) = data;
		}
		else
#endif
		{
			us8 *temp = buf + offset;
			*temp = (data & 0xFF000000) >> 24;
			*(temp + 1) = (data & 0xFF0000) >> 16;
			*(temp + 2) = (data & 0xFF00) >> 8;
			*(temp + 3) = data;
		}
		break;
	}
}

us32 BinaryHelper::ReadBinDataLE(const us8 *buf, size_t offset, us8 size)
{
	us32 ret = 0;
	switch (size)
	{
	case 1:
		ret = buf[offset];
		break;
	case 2:
#ifdef IS_LITTLE_ENDIAN
		if (offset % 2 == 0)
		{
			ret = *reinterpret_cast<const us16*>(&buf[offset]);
		}
		else
#endif
		{
			const us8 *temp = &buf[offset];
			ret = *temp | *(temp + 1) << 8;
		}
		break;
	case 3:
	{
		const us8 *temp = &buf[offset];
		ret = *temp | *(temp + 1) << 8 | *(temp + 2) << 16;
	}
	break;
	case 4:
#ifdef IS_LITTLE_ENDIAN
		if (offset % 4 == 0)
		{
			ret = *reinterpret_cast<const us32*>(&buf[offset]);
		}
		else
#endif
		{
			const us8 *temp = &buf[offset];
			ret = *temp | *(temp + 1) << 8 | *(temp + 2) << 16 | *(temp + 3) << 24;
		}
		break;
	}
	return ret;
}

us32 BinaryHelper::ReadBinDataBE(const us8 *buf, size_t offset, us8 size)
{
	us32 ret = 0;
	switch (size)
	{
	case 1:
		ret = buf[offset];
		break;
	case 2:
#ifndef IS_LITTLE_ENDIAN
		if (offset % 2 == 0)
		{
			ret = *reinterpret_cast<const us16*>(&buf[offset]);
		}
		else
#endif
		{
			const us8 *temp = &buf[offset];
			ret = *temp << 8 | *(temp + 1);
		}
		break;
	case 3:
	{
		const us8 *temp = &buf[offset];
		ret = *temp << 16 | *(temp + 1) << 8 | *(temp + 2);
	}
	break;
	case 4:
#ifndef IS_LITTLE_ENDIAN
		if (offset % 4 == 0)
		{
			ret = *reinterpret_cast<const us32*>(&buf[offset]);
		}
		else
#endif
		{
			const us8 *temp = &buf[offset];
			ret = *temp << 24 | *(temp + 1) << 16 | *(temp + 2) << 8 | *(temp + 3);
		}
		break;
	}
	return ret;
}

us32 BinaryHelper::ReadBinData(CircularBuffer& buf, size_t offset, us8 size, bool isLittleEndian)
{
	us32 ret = 0;
	switch(size)
	{
	case 1:
		ret = buf[offset];
		break;
	case 2:
		ret = isLittleEndian ? buf[offset] | (buf[offset + 1] << 8) : (buf[offset] << 8) | buf[offset + 1];
		break;
	case 3:
		ret = isLittleEndian ? buf[offset] | (buf[offset + 1] << 8) | (buf[offset + 2] << 16) : (buf[offset] << 16) | (buf[offset + 1] << 8) | buf[offset + 2];
		break;
	case 4:
		ret = isLittleEndian ? buf[offset] | (buf[offset + 1] << 8) | (buf[offset + 2] << 16) | (buf[offset + 3] << 24) : (buf[offset] << 24) | (buf[offset + 1] << 16) | (buf[offset + 2] << 8) | buf[offset + 3];
		break;
	}
	return ret;
}