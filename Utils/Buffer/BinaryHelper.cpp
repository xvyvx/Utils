#include "BinaryHelper.h"

void BinaryHelper::PushBinDataLE(LinearBuffer& buf, us32 data, us8 size)
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

void BinaryHelper::PushBinDataBE(LinearBuffer& buf, us32 data, us8 size)
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

void BinaryHelper::PushBinDataLE(LinearBuffer& buf, f32 data)
{
    us8* ptr = reinterpret_cast<us8*>(&data);
#ifdef IS_LITTLE_ENDIAN
    buf.insert(buf.end(), ptr, ptr + 4);
#else
    buf.push_back(ptr[3]);
    buf.push_back(ptr[2]);
    buf.push_back(ptr[1]);
    buf.push_back(ptr[0]);
#endif
}

void BinaryHelper::PushBinDataBE(LinearBuffer& buf, f32 data)
{
    us8* ptr = reinterpret_cast<us8*>(&data);
#ifdef IS_LITTLE_ENDIAN
    buf.push_back(ptr[3]);
    buf.push_back(ptr[2]);
    buf.push_back(ptr[1]);
    buf.push_back(ptr[0]);
#else
    buf.insert(buf.end(), ptr, ptr + 4);
#endif
}

void BinaryHelper::PushBinDataLE_Signed(LinearBuffer& buf, s32 data, us8 size)
{
    us32& temp = *reinterpret_cast<us32*>(&data);
    switch (size)
    {
    case 1:
        buf.push_back(((temp & 0x80000000) >> 24) | (temp & 0x7F));
        break;
    case 2:
        buf.push_back(temp & 0xFF);
        buf.push_back(((temp & 0x80000000) >> 24) | ((temp & 0x7F00) >> 8));
        break;
    case 3:
        buf.push_back(temp & 0xFF);
        buf.push_back((temp & 0xFF00) >> 8);
        buf.push_back(((temp & 0x80000000) >> 24) | ((temp & 0x7F0000) >> 16));
        break;
    case 4:
        buf.push_back(temp & 0xFF);
        buf.push_back((temp & 0xFF00) >> 8);
        buf.push_back((temp & 0xFF0000) >> 16);
        buf.push_back((temp & 0xFF000000) >> 24);
        break;
    }
}

void BinaryHelper::PushBinDataBE_Signed(LinearBuffer& buf, s32 data, us8 size)
{
    us32& temp = *reinterpret_cast<us32*>(&data);
    switch (size)
    {
    case 1:
        buf.push_back(((temp & 0x80000000) >> 24) | (temp & 0x7F));
        break;
    case 2:
        buf.push_back(((temp & 0x80000000) >> 24) | ((temp & 0x7F00) >> 8));
        buf.push_back(temp & 0xFF);
        break;
    case 3:
        buf.push_back(((temp & 0x80000000) >> 24) | ((temp & 0x7F0000) >> 16));
        buf.push_back((temp & 0xFF00) >> 8);
        buf.push_back(temp & 0xFF);
        break;
    case 4:
        buf.push_back((temp & 0xFF000000) >> 24);
        buf.push_back((temp & 0xFF0000) >> 16);
        buf.push_back((temp & 0xFF00) >> 8);
        buf.push_back(temp & 0xFF);
        break;
    }
}

void BinaryHelper::SetBinDataLE(us8* buf, size_t offset, us32 data, us8 size)
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
            union Val
            {
                us16 m_val16;
                us8 m_val8[2];
            } *temp = reinterpret_cast<Val*>(buf + offset);
            temp->m_val16 = data;
        }
        else
#endif
        {
            us8* temp = buf + offset;
            *temp = data;
            *(temp + 1) = (data & 0xFF00) >> 8;
        }
        break;
    case 3:
    {
        us8* temp = buf + offset;
        *temp = data;
        *(temp + 1) = (data & 0xFF00) >> 8;
        *(temp + 2) = (data & 0xFF0000) >> 16;
    }
    break;
    case 4:
#ifdef IS_LITTLE_ENDIAN
        if (offset % 4 == 0)
        {
            union Val
            {
                us32 m_val32;
                us8 m_val8[4];
            } *temp = reinterpret_cast<Val*>(buf + offset);
            temp->m_val32 = data;
        }
        else
#endif
        {
            us8* temp = buf + offset;
            *temp = data;
            *(temp + 1) = (data & 0xFF00) >> 8;
            *(temp + 2) = (data & 0xFF0000) >> 16;
            *(temp + 3) = (data & 0xFF000000) >> 24;
        }
        break;
    }
}

void BinaryHelper::SetBinDataBE(us8* buf, size_t offset, us32 data, us8 size)
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
            union Val
            {
                us16 m_val16;
                us8 m_val8[2];
            } *temp = reinterpret_cast<Val*>(buf + offset);
            temp->m_val16 = data;
        }
        else
#endif
        {
            us8* temp = buf + offset;
            *temp = (data & 0xFF00) >> 8;
            *(temp + 1) = data;
        }
        break;
    case 3:
    {
        us8* temp = buf + offset;
        *temp = (data & 0xFF0000) >> 16;
        *(temp + 1) = (data & 0xFF00) >> 8;
        *(temp + 2) = data;
    }
    break;
    case 4:
#ifndef IS_LITTLE_ENDIAN
        if (offset % 4 == 0)
        {
            union Val
            {
                us32 m_val32;
                us8 m_val8[4];
            } *temp = reinterpret_cast<Val*>(buf + offset);
            temp->m_val32 = data;
        }
        else
#endif
        {
            us8* temp = buf + offset;
            *temp = (data & 0xFF000000) >> 24;
            *(temp + 1) = (data & 0xFF0000) >> 16;
            *(temp + 2) = (data & 0xFF00) >> 8;
            *(temp + 3) = data;
        }
        break;
    }
}

void BinaryHelper::SetBinDataLE(us8* buf, size_t offset, f32 data)
{
#ifdef IS_LITTLE_ENDIAN
    if (offset % 4 == 0)
    {
        union Val
        {
            f32 m_val32;
            us8 m_val8[4];
        } *temp = reinterpret_cast<Val*>(buf + offset);
        temp->m_val32 = data;
    }
    else
    {
        us8* temp = buf + offset;
        us8* tempF32 = reinterpret_cast<us8*>(&data);
        *temp = tempF32[0];
        *(temp + 1) = tempF32[1];
        *(temp + 2) = tempF32[2];
        *(temp + 3) = tempF32[3];
    }
#else
    us8* temp = buf + offset;
    us8* tempF32 = reinterpret_cast<us8*>(&data);
    *temp = tempF32[3];
    *(temp + 1) = tempF32[2];
    *(temp + 2) = tempF32[1];
    *(temp + 3) = tempF32[0];
#endif
}

void BinaryHelper::SetBinDataBE(us8* buf, size_t offset, f32 data)
{
#ifndef IS_LITTLE_ENDIAN
    if (offset % 4 == 0)
    {
        union Val
        {
            f32 m_val32;
            us8 m_val8[4];
        } *temp = reinterpret_cast<Val*>(buf + offset);
        temp->m_val32 = data;
    }
    else
    {
        us8* temp = buf + offset;
        us8* tempF32 = reinterpret_cast<us8*>(&data);
        *temp = tempF32[0];
        *(temp + 1) = tempF32[1];
        *(temp + 2) = tempF32[2];
        *(temp + 3) = tempF32[3];
    }
#else
    us8* temp = buf + offset;
    us8* tempF32 = reinterpret_cast<us8*>(&data);
    *temp = tempF32[3];
    *(temp + 1) = tempF32[2];
    *(temp + 2) = tempF32[1];
    *(temp + 3) = tempF32[0];
#endif
}

void BinaryHelper::SetBinDataLE_Signed(us8* buf, size_t offset, s32 data, us8 size)
{
    switch (size)
    {
    case 1:
    {
        us32& tempData = *reinterpret_cast<us32*>(&data);
        buf[offset] = ((tempData & 0x80000000) >> 24) | (tempData & 0x7F);
    }
    break;
    case 2:
#ifdef IS_LITTLE_ENDIAN
        if (offset % 2 == 0)
        {
            union Val
            {
                s16 m_val16;
                us8 m_val8[2];
            } *temp = reinterpret_cast<Val*>(buf + offset);
            temp->m_val16 = data;
        }
        else
#endif
        {
            us32& tempData = *reinterpret_cast<us32*>(&data);
            us8* temp = buf + offset;
            *temp = tempData;
            *(temp + 1) = ((tempData & 0x80000000) >> 24) | ((tempData & 0x7F00) >> 8);
        }
        break;
    case 3:
    {
        us32& tempData = *reinterpret_cast<us32*>(&data);
        us8* temp = buf + offset;
        *temp = tempData;
        *(temp + 1) = (tempData & 0xFF00) >> 8;
        *(temp + 2) = ((tempData & 0x80000000) >> 24) | ((tempData & 0x7F0000) >> 16);
    }
    break;
    case 4:
#ifdef IS_LITTLE_ENDIAN
        if (offset % 4 == 0)
        {
            union Val
            {
                s32 m_val32;
                us8 m_val8[4];
            } *temp = reinterpret_cast<Val*>(buf + offset);
            temp->m_val32 = data;
        }
        else
#endif
        {
            us32& tempData = *reinterpret_cast<us32*>(&data);
            us8* temp = buf + offset;
            *temp = tempData;
            *(temp + 1) = (tempData & 0xFF00) >> 8;
            *(temp + 2) = (tempData & 0xFF0000) >> 16;
            *(temp + 3) = (tempData & 0xFF000000) >> 24;
        }
        break;
    }
}

void BinaryHelper::SetBinDataBE_Signed(us8* buf, size_t offset, s32 data, us8 size)
{
    switch (size)
    {
    case 1:
    {
        us32& tempData = *reinterpret_cast<us32*>(&data);
        buf[offset] = ((tempData & 0x80000000) >> 24) | (tempData & 0x7F);
    }
    break;
    case 2:
#ifndef IS_LITTLE_ENDIAN
        if (offset % 2 == 0)
        {
            union Val
            {
                s16 m_val16;
                us8 m_val8[2];
            } *temp = reinterpret_cast<Val*>(buf + offset);
            temp->m_val16 = data;
        }
        else
#endif
        {
            us32& tempData = *reinterpret_cast<us32*>(&data);
            us8* temp = buf + offset;
            *temp = ((tempData & 0x80000000) >> 24) | ((tempData & 0x7F00) >> 8);
            *(temp + 1) = tempData;
        }
        break;
    case 3:
    {
        us32& tempData = *reinterpret_cast<us32*>(&data);
        us8* temp = buf + offset;
        *temp = ((tempData & 0x80000000) >> 24) | ((tempData & 0x7F0000) >> 16);
        *(temp + 1) = (tempData & 0xFF00) >> 8;
        *(temp + 2) = tempData;
    }
    break;
    case 4:
#ifndef IS_LITTLE_ENDIAN
        if (offset % 4 == 0)
        {
            union Val
            {
                s32 m_val32;
                us8 m_val8[4];
            } *temp = reinterpret_cast<Val*>(buf + offset);
            temp->m_val32 = data;
        }
        else
#endif
        {
            us32& tempData = *reinterpret_cast<us32*>(&data);
            us8* temp = buf + offset;
            *temp = (tempData & 0xFF000000) >> 24;
            *(temp + 1) = (tempData & 0xFF0000) >> 16;
            *(temp + 2) = (tempData & 0xFF00) >> 8;
            *(temp + 3) = tempData;
        }
        break;
    }
}

us32 BinaryHelper::ReadBinDataLE(const us8* buf, size_t offset, us8 size)
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
            union Val
            {
                us16 m_val16;
                us8 m_val8[2];
            } const* temp = reinterpret_cast<const Val*>(buf + offset);
            ret = temp->m_val16;
        }
        else
#endif
        {
            const us8* temp = &buf[offset];
            ret = *temp | *(temp + 1) << 8;
        }
        break;
    case 3:
    {
        const us8* temp = &buf[offset];
        ret = *temp | *(temp + 1) << 8 | *(temp + 2) << 16;
    }
    break;
    case 4:
#ifdef IS_LITTLE_ENDIAN
        if (offset % 4 == 0)
        {
            union Val
            {
                us32 m_val32;
                us8 m_val8[4];
            } const* temp = reinterpret_cast<const Val*>(buf + offset);
            ret = temp->m_val32;
        }
        else
#endif
        {
            const us8* temp = &buf[offset];
            ret = *temp | *(temp + 1) << 8 | *(temp + 2) << 16 | *(temp + 3) << 24;
        }
        break;
    }
    return ret;
}

us32 BinaryHelper::ReadBinDataBE(const us8* buf, size_t offset, us8 size)
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
            union Val
            {
                us16 m_val16;
                us8 m_val8[2];
            } *temp = reinterpret_cast<Val*>(buf + offset);
            ret = temp->m_val16;
        }
        else
#endif
        {
            const us8* temp = &buf[offset];
            ret = *temp << 8 | *(temp + 1);
        }
        break;
    case 3:
    {
        const us8* temp = &buf[offset];
        ret = *temp << 16 | *(temp + 1) << 8 | *(temp + 2);
    }
    break;
    case 4:
#ifndef IS_LITTLE_ENDIAN
        if (offset % 4 == 0)
        {
            union Val
            {
                us32 m_val32;
                us8 m_val8[4];
            } *temp = reinterpret_cast<Val*>(buf + offset);
            ret = temp->m_val32;
        }
        else
#endif
        {
            const us8* temp = &buf[offset];
            ret = *temp << 24 | *(temp + 1) << 16 | *(temp + 2) << 8 | *(temp + 3);
        }
        break;
    }
    return ret;
}

f32 BinaryHelper::ReadBinDataLE(const us8* buf, size_t offset)
{
    f32 ret;
#ifdef IS_LITTLE_ENDIAN
    if (offset % 4 == 0)
    {
        union Val
        {
            f32 m_val32;
            us8 m_val8[4];
        } const* temp = reinterpret_cast<const Val*>(buf + offset);
        ret = temp->m_val32;
    }
    else
#endif
    {
        const us8* temp = &buf[offset];
        union Val
        {
            f32 m_valFloat;
            us32 m_valInt;
        } tempVal;
        tempVal.m_valInt = *temp | *(temp + 1) << 8 | *(temp + 2) << 16 | *(temp + 3) << 24;
        ret = tempVal.m_valFloat;
    }
    return ret;
}

f32 BinaryHelper::ReadBinDataBE(const us8* buf, size_t offset)
{
    f32 ret;
#ifndef IS_LITTLE_ENDIAN
    if (offset % 4 == 0)
    {
        union Val
        {
            f32 m_val32;
            us8 m_val8[4];
        } *temp = reinterpret_cast<Val*>(buf + offset);
        ret = temp->m_val32;
    }
    else
#endif
    {
        const us8* temp = &buf[offset];
        union Val
        {
            f32 m_valFloat;
            us32 m_valInt;
        } tempVal;
        tempVal.m_valInt = *temp << 24 | *(temp + 1) << 16 | *(temp + 2) << 8 | *(temp + 3);
        ret = tempVal.m_valFloat;
    }
    return ret;
}

s32 BinaryHelper::ReadBinDataLE_Signed(const us8* buf, size_t offset, us8 size)
{
    s32 ret = 0;
    switch (size)
    {
    case 1:
        ret = *reinterpret_cast<const s8*>(buf + offset);
        break;
    case 2:
#ifdef IS_LITTLE_ENDIAN
        if (offset % 2 == 0)
        {
            union Val
            {
                s16 m_val16;
                us8 m_val8[2];
            } const* temp = reinterpret_cast<const Val*>(buf + offset);
            ret = temp->m_val16;
        }
        else
#endif
        {
            const us8* temp = &buf[offset];
            us16 tempValue = *temp | *(temp + 1) << 8;
            ret = *reinterpret_cast<s16*>(&tempValue);
        }
        break;
    case 3:
    {
        const us8* temp = &buf[offset];
        us32 tempValue = *temp | *(temp + 1) << 8 | (*(temp + 2) & 0x80 ? 0xFF80 : 0x0000) << 16
            | (*(temp + 2) & 0x7F) << 16;
        ret = *reinterpret_cast<s32*>(&tempValue);
    }
    break;
    case 4:
#ifdef IS_LITTLE_ENDIAN
        if (offset % 4 == 0)
        {
            union Val
            {
                s32 m_val32;
                us8 m_val8[4];
            } const* temp = reinterpret_cast<const Val*>(buf + offset);
            ret = temp->m_val32;
        }
        else
#endif
        {
            const us8* temp = &buf[offset];
            us32 tempValue = *temp | *(temp + 1) << 8 | *(temp + 2) << 16 | *(temp + 3) << 24;
            ret = *reinterpret_cast<s32*>(&tempValue);
        }
        break;
    }
    return ret;
}

s32 BinaryHelper::ReadBinDataBE_Signed(const us8* buf, size_t offset, us8 size)
{
    s32 ret = 0;
    switch (size)
    {
    case 1:
        ret = *reinterpret_cast<const s8*>(buf + offset);
        break;
    case 2:
#ifndef IS_LITTLE_ENDIAN
        if (offset % 2 == 0)
        {
            union Val
            {
                s16 m_val16;
                us8 m_val8[2];
            } *temp = reinterpret_cast<Val*>(buf + offset);
            ret = temp->m_val16;
        }
        else
#endif
        {
            const us8* temp = &buf[offset];
            us16 tempValue = *temp << 8 | *(temp + 1);
            ret = *reinterpret_cast<s16*>(&tempValue);
        }
        break;
    case 3:
    {
        const us8* temp = &buf[offset];
        us32 tempValue = *(temp + 2) | *(temp + 1) << 8 | (*temp & 0x80 ? 0xFF80 : 0x0000) << 16
            | (*temp & 0x7F) << 16;
        ret = *reinterpret_cast<s32*>(&tempValue);
    }
    break;
    case 4:
#ifndef IS_LITTLE_ENDIAN
        if (offset % 4 == 0)
        {
            union Val
            {
                s32 m_val32;
                us8 m_val8[4];
            } *temp = reinterpret_cast<Val*>(buf + offset);
            ret = temp->m_val32;
        }
        else
#endif
        {
            const us8* temp = &buf[offset];
            us32 tempValue = *temp << 24 | *(temp + 1) << 16 | *(temp + 2) << 8 | *(temp + 3);
            ret = *reinterpret_cast<s32*>(&tempValue);
        }
        break;
    }
    return ret;
}

us32 BinaryHelper::ReadBinData(CircularBuffer& buf, size_t offset, us8 size, bool isLittleEndian)
{
    us32 ret = 0;
    switch (size)
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