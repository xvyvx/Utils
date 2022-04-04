#include <boost/test/unit_test.hpp>
#include "Buffer/BinaryHelper.h"

BOOST_AUTO_TEST_SUITE(BinaryHelperTest)

BOOST_AUTO_TEST_CASE(GeneralTest)
{
    CircularBuffer circularBuf(512);
    BufDescriptor bufs[2];
    circularBuf.free_buffers(bufs);
    LinearBuffer linearBuf(512);
    for (size_t i = 0; i < bufs[0].m_size; ++i)
    {
        bufs[0].m_beg[i] = static_cast<us8>(i);
    }
    circularBuf.inc_size(512);
    circularBuf.copy_to(linearBuf, 0, circularBuf.size());
    us32 result[2][4] = { {0x00,0x0001,0x000102,0x00010203},{0x00,0x0100,0x020100,0x03020100} };

    for (us8 i = 1; i <= 4; ++i)
    {
        us32 temp = BinaryHelper::ReadBinData(linearBuf, 0, i, true);
        us32 temp2 = BinaryHelper::ReadBinData(linearBuf.data(), 0, i, true);
        us32 temp3 = BinaryHelper::ReadBinData(circularBuf, 0, i, true);
        s32 tempSigned = BinaryHelper::ReadBinDataSigned(linearBuf, 0, i, true);
        s32 tempSigned2 = BinaryHelper::ReadBinDataSigned(linearBuf.data(), 0, i, true);
        BOOST_TEST((temp == temp2&&temp == temp3&&temp == result[1][i - 1]));
        BOOST_TEST((tempSigned == tempSigned2 && tempSigned == *reinterpret_cast<s32*>(&result[1][i - 1])));
        temp = BinaryHelper::ReadBinData(linearBuf, 0, i, false);
        temp2 = BinaryHelper::ReadBinData(linearBuf.data(), 0, i, false);
        temp3 = BinaryHelper::ReadBinData(circularBuf, 0, i, false);
        tempSigned = BinaryHelper::ReadBinDataSigned(linearBuf, 0, i, false);
        tempSigned2 = BinaryHelper::ReadBinDataSigned(linearBuf.data(), 0, i, false);
        BOOST_TEST((temp == temp2&&temp == temp3&&temp == result[0][i - 1]));
        BOOST_TEST((tempSigned == tempSigned2 && tempSigned == *reinterpret_cast<s32*>(&result[0][i - 1])));
    }
    f32 tempFloat = BinaryHelper::ReadBinData(linearBuf.data(), 0, true);
    BOOST_TEST((tempFloat == *reinterpret_cast<f32*>(&result[1][3])));
    tempFloat = BinaryHelper::ReadBinData(linearBuf.data(), 0, false);
    BOOST_TEST((tempFloat == *reinterpret_cast<f32*>(&result[0][3])));

    for (us8 i = 1; i <= 4; ++i)
    {
        linearBuf.clear();
        BinaryHelper::PushBinData(linearBuf, 0x01, i, true);
        BinaryHelper::PushBinData(linearBuf, 0x01, i, false);
        BinaryHelper::PushBinDataSigned(linearBuf, -1, i, true);
        BinaryHelper::PushBinDataSigned(linearBuf, -1, i, false);
        us32 temp = BinaryHelper::ReadBinData(linearBuf, 0, i, true);
        us32 temp2 = BinaryHelper::ReadBinData(linearBuf.data(), 1 * i, i, false);
        s32 tempSigned = BinaryHelper::ReadBinDataSigned(linearBuf, 2 * i, i, true);
        s32 tempSigned2 = BinaryHelper::ReadBinDataSigned(linearBuf.data(), 3 * i, i, false);
        BOOST_TEST((temp == temp2&&temp == 0x01));
        BOOST_TEST((tempSigned == tempSigned2&&tempSigned == -1));
        linearBuf.assign(i * 8, 0);
        BinaryHelper::SetBinData(linearBuf, 0, 0x01, i, true);
        BinaryHelper::SetBinData(linearBuf.data(), 1*i, 0x01, i, true);
        BinaryHelper::SetBinData(linearBuf, 2 * i, 0x01, i, false);
        BinaryHelper::SetBinData(linearBuf.data(), 3 * i, 0x01, i, false);
        BinaryHelper::SetBinDataSigned(linearBuf, 4*i, -1, i, true);
        BinaryHelper::SetBinDataSigned(linearBuf.data(), 5*i, -1, i, true);
        BinaryHelper::SetBinDataSigned(linearBuf, 6*i, -1, i, false);
        BinaryHelper::SetBinDataSigned(linearBuf.data(), 7*i, -1, i, false);
        temp = BinaryHelper::ReadBinData(linearBuf, 0, i, true);
        temp2 = BinaryHelper::ReadBinData(linearBuf, 1 * i, i, true);
        us32 temp3 = BinaryHelper::ReadBinData(linearBuf, 2 * i, i, false);
        us32 temp4 = BinaryHelper::ReadBinData(linearBuf, 3 * i, i, false);
        tempSigned = BinaryHelper::ReadBinDataSigned(linearBuf, 4 * i, i, true);
        tempSigned2 = BinaryHelper::ReadBinDataSigned(linearBuf, 5 * i, i, true);
        s32 tempSigned3 = BinaryHelper::ReadBinDataSigned(linearBuf, 6 * i, i, false);
        s32 tempSigned4 = BinaryHelper::ReadBinDataSigned(linearBuf, 7 * i, i, false);
        BOOST_TEST((temp == 0x01 && temp2 == 0x01 && temp3 == 0x01 && temp4 == 0x01));
        BOOST_TEST((tempSigned == -1 && tempSigned2 == -1 && tempSigned3 == -1 && tempSigned4 == -1));
    }
    linearBuf.clear();
    BinaryHelper::PushBinData(linearBuf, -1.0, true);
    BinaryHelper::PushBinData(linearBuf, -1.0, false);
    tempFloat = BinaryHelper::ReadBinData(linearBuf.data(), 0, true);
    f32 tempFloat2 = BinaryHelper::ReadBinData(linearBuf.data(), 4, false);
    BOOST_TEST((tempFloat == tempFloat2 && tempFloat == -1.0), boost::test_tools::tolerance(0.1));

    for (us8 i = 1; i <= 4; ++i)
    {
        us32 unalignData[2] = { 0,0 };
        us8 *unalignPtr = reinterpret_cast<us8*>(unalignData) + 1;
        BinaryHelper::SetBinData(unalignPtr, 0, 0x01, i, true);
        us32 temp = BinaryHelper::ReadBinData(unalignPtr, 0, i, true);
        BOOST_TEST(temp == 0x01);
    }
}

BOOST_AUTO_TEST_SUITE_END()