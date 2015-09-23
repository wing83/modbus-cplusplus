#include "modbus.h"

#include <string.h>

/* Table of CRC values for high-order byte */
static unsigned char table_crc_hi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};
/* Table of CRC values for low-order byte */
static unsigned char table_crc_lo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
    0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
    0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
    0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
    0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
    0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
    0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
    0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
    0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
    0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
    0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
    0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
    0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
    0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
    0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
    0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

/* Fast CRC */
//返回的CRC校验结果,高低位未互换
static unsigned short getCRC16(unsigned char *buffer, int buffer_length)
{
    unsigned char crc_hi = 0xFF; /* high CRC byte initialized */
    unsigned char crc_lo = 0xFF; /* low CRC byte initialized */
    unsigned int i; /* will index into CRC lookup */

    /* pass through message buffer */
    while (buffer_length--)
    {
        i = crc_hi ^ *buffer++; /* calculate the CRC  */
        crc_hi = crc_lo ^ table_crc_hi[i];
        crc_lo = table_crc_lo[i];
    }
    return (crc_hi << 8 | crc_lo);
}

static unsigned short changeEndian(unsigned short value)
{
    return ((value&0xFF)<<8) | (value>>8);
}

ModBus::ModBus(unsigned char *ptrBuff, int nLen)
{
    init(ptrBuff, nLen, 0x00, 0x00);
}

ModBus::ModBus(unsigned char *ptrBuff, int nLen, unsigned char nMeterNumber, unsigned char nFunctionCode)
{
    init(ptrBuff, nLen, nMeterNumber, nFunctionCode);
}

void ModBus::init(unsigned char *ptrBuff, int nLen, unsigned char nMeterNumber, unsigned char nFunctionCode)
{
    m_ptrDataBuff = ptrBuff;
    m_nDataBuffLen = nLen;
    m_nMeterNumber = nMeterNumber;
    m_nFunctionCode = nFunctionCode;
    m_nStartRegister = 0x00;
    m_nRegisterCount = 0x00;
    m_nDataLength = 0x00;
    m_nErrorCode = 0x00;
    m_nStartIndex = 0;
    m_nCmdLength = 0;

    //memset(m_ptrDataBuff, 0, m_nDataBuffLen);
}

void ModBus::resetSettings()
{
    init(m_ptrDataBuff, m_nDataBuffLen, 0, 0);
}

void ModBus::resetSettings(unsigned char *ptrBuff, int nLen)
{
    init(ptrBuff, nLen, 0, 0);
}

unsigned char ModBus::getMeterNumber()
{
    return m_nMeterNumber;
}

unsigned char ModBus::getFunctionCode()
{
    return m_nFunctionCode;
}

unsigned short ModBus::getStartRegister()
{
    return m_nStartRegister;
}

unsigned short ModBus::getRegisterCount()
{
    return m_nRegisterCount;
}

unsigned char ModBus::getDataLength()
{
    return m_nDataLength;
}

unsigned char ModBus::getErrorCode()
{
    return m_nErrorCode;
}

void ModBus::setFunctionCode(unsigned char nFunctionCode)
{
    m_nFunctionCode = nFunctionCode;
    if((ModBus::Write_Single_Coil==m_nFunctionCode)
            ||(ModBus::Write_Single_Reg==m_nFunctionCode))
    {
        m_nRegisterCount = 1;
    }else if((ModBus::Write_Serval_Coil==m_nFunctionCode)
             ||(ModBus::Write_Serval_Reg==m_nFunctionCode))
    {
        if(m_nDataBuffLen>7)
        {
            memset(m_ptrDataBuff+7, 0, m_nDataBuffLen-7);
        }
    }
}

void ModBus::setMeterNumber(unsigned char nMeterNumber)
{
    m_nMeterNumber = nMeterNumber;
}

void ModBus::setStartRegister(unsigned short nRegister)
{
    m_nStartRegister = nRegister;
}

void ModBus::setRegisterCount(unsigned short nCount)
{
    m_nRegisterCount = nCount;
}

bool ModBus::setCoilValue(unsigned short nRegister, bool bValue)
{
    if(NULL == m_ptrDataBuff)
    {
        return false;
    }

    if(ModBus::Write_Single_Coil == m_nFunctionCode)
    {
        if((8 > m_nDataBuffLen)
                ||(nRegister != m_nStartRegister))
        {
            return false;
        }

        unsigned short nTmp = 0;
        if(bValue)
        {
            nTmp = 0xFF;
        }
        memcpy(m_ptrDataBuff+4, &nTmp, 2);
        return true;
    }else if(ModBus::Write_Serval_Coil == m_nFunctionCode)
    {
        int nDataLen = m_nRegisterCount/8;
        if(0!=(m_nRegisterCount%8))
        {
            nDataLen ++;
        }
        int nRegisterIndex = nRegister - m_nStartRegister;
        if(((9+nDataLen)>m_nDataBuffLen)
            ||(nRegisterIndex<0)
                ||(nRegisterIndex>=m_nRegisterCount))
        {
            return false;
        }
        int nDataIndex = nRegisterIndex/8;
        int nDataOffset = nRegisterIndex%8;
        unsigned nTmp = m_ptrDataBuff[7+nDataIndex];
        if(bValue)
        {
            nTmp = nTmp | (0x1<<nDataOffset);
        }else
        {
            nTmp = nTmp & (~(0x1<<nDataOffset));
        }
        m_ptrDataBuff[7+nDataIndex] = nTmp;
        return true;
    }else
    {
        return false;
    }
}

bool ModBus::setRegisterValue(unsigned short nRegister, unsigned short nValue)
{
    if(NULL == m_ptrDataBuff)
    {
        return false;
    }

    if(ModBus::Write_Single_Reg == m_nFunctionCode)
    {
        if((8>m_nDataBuffLen)
                ||(nRegister != m_nStartRegister))
        {
            return false;
        }

        unsigned short nTmp = changeEndian(nValue);
        memcpy(m_ptrDataBuff+4, &nTmp, 2);
        return true;
    }else if(ModBus::Write_Serval_Reg == m_nFunctionCode)
    {
        int nRegisterIndex = nRegister - m_nStartRegister;
        if(((9+2*m_nRegisterCount)>m_nDataBuffLen)
                ||(nRegisterIndex<0)
                ||(nRegisterIndex>=m_nRegisterCount))
        {
            return false;
        }

        unsigned short nTmp = changeEndian(nValue);
        memcpy(m_ptrDataBuff+7+nRegisterIndex*2, &nTmp, 2);
        return true;
    }else
    {
        return false;
    }
}

bool ModBus::setRawRegistsValue(unsigned short nStartRegister, unsigned char *dataBuff, int nLen)
{
    if(NULL == m_ptrDataBuff)
    {
        return false;
    }

    if(ModBus::Write_Serval_Reg == m_nFunctionCode)
    {
        int nRegisterIndex = nStartRegister - m_nStartRegister;
        if(((9+2*m_nRegisterCount)>m_nDataBuffLen)
                ||(nRegisterIndex<0)
                ||(nRegisterIndex>=m_nRegisterCount)
                ||(2*(m_nRegisterCount-nRegisterIndex)<nLen))
        {
            return false;
        }
        memcpy(m_ptrDataBuff+7+2*nRegisterIndex, dataBuff, nLen);
        return true;
    }else
    {
        return false;
    }
}

bool ModBus::getCoilValue(int nRegisterIndex, bool &bValue)
{
    if((ModBus::Read_Coil == m_nFunctionCode)
            ||(ModBus::Read_DI == m_nFunctionCode))
    {
        int nDataIndex = nRegisterIndex/8;
        int nDataOffset = nRegisterIndex%8;
        if(nDataIndex>=m_nDataLength)
        {
            return false;
        }
        unsigned char nData = m_ptrDataBuff[m_nStartIndex+3+nDataIndex];
        if((nData&(1<<nDataOffset))>0)
        {
            bValue = true;
        }else
        {
            bValue = false;
        }
        return true;
    }else if(ModBus::Write_Single_Coil)
    {
        if((nRegisterIndex != 0)
                ||(8 != m_nCmdLength))
        {
            return false;
        }

        if(0xFF == m_ptrDataBuff[m_nStartIndex+4])
        {
            bValue = true;
        }else
        {
            bValue = false;
        }
        return true;
    }

    return false;
}

bool ModBus::getRegisterValue(int nRegisterIndex, unsigned short &nValue)
{
    if(NULL == m_ptrDataBuff)
    {
        return false;
    }

    if((ModBus::Read_Hold_Reg==m_nFunctionCode)
            ||(ModBus::Read_Input_Reg==m_nFunctionCode))
    {
        int nStartIndex = 2*nRegisterIndex;
        if((nStartIndex+2)>m_nDataLength)
        {
            return false;
        }
        memcpy(&nValue, m_ptrDataBuff+m_nStartIndex+nStartIndex+3, 2);
        nValue = changeEndian(nValue);
        return true;
    }else if(ModBus::Write_Single_Reg==m_nFunctionCode)
    {
        if((0 != nRegisterIndex)
                ||(8 != m_nCmdLength))
        {
            return false;
        }

        memcpy(&nValue, m_ptrDataBuff+m_nStartIndex+4, 2);
        nValue = changeEndian(nValue);
        return true;
    }

    return false;
}

bool ModBus::getRawRegisterValue(int nRegisterIndex, unsigned char *ptrDataBuff, unsigned nBuffLen)
{
    if(NULL == m_ptrDataBuff)
    {
        return false;
    }

    if((ModBus::Read_Hold_Reg==m_nFunctionCode)
            ||(ModBus::Read_Input_Reg==m_nFunctionCode))
    {
        int nStartIndex = 2*nRegisterIndex;
        if((nStartIndex+nBuffLen)>m_nDataLength)
        {
            return false;
        }
        memcpy(ptrDataBuff, m_ptrDataBuff+m_nStartIndex+3, nBuffLen);
        return true;
    }else
    {
        return false;
    }
}

bool ModBus::getCmd(int &nLen)
{
    if(NULL == m_ptrDataBuff)
    {
        return false;
    }

    if((ModBus::Read_Coil == m_nFunctionCode)
            ||(ModBus::Read_DI == m_nFunctionCode)
            ||(ModBus::Read_Hold_Reg == m_nFunctionCode)
            ||(ModBus::Read_Input_Reg == m_nFunctionCode))
    {
        if(8>m_nDataBuffLen)
        {
            return false;
        }
        m_ptrDataBuff[0] = m_nMeterNumber;
        m_ptrDataBuff[1] = m_nFunctionCode;
        unsigned short nTmp = changeEndian(m_nStartRegister);
        memcpy(m_ptrDataBuff + 2, &nTmp, 2);
        nTmp = changeEndian(m_nRegisterCount);
        memcpy(m_ptrDataBuff + 4, &nTmp, 2);
        nTmp = getCRC16(m_ptrDataBuff, 6);
        nTmp = changeEndian(nTmp);
        memcpy(m_ptrDataBuff + 6, &nTmp, 2);
        nLen = 8;
        return true;
    }else if((ModBus::Write_Single_Coil == m_nFunctionCode)
             ||(ModBus::Write_Single_Reg == m_nFunctionCode))
    {
        if(8>m_nDataBuffLen)
        {
            return false;
        }

        m_ptrDataBuff[0] = m_nMeterNumber;
        m_ptrDataBuff[1] = m_nFunctionCode;
        unsigned short nTmp = changeEndian(m_nStartRegister);
        memcpy(m_ptrDataBuff+2, &nTmp, 2);

        nTmp = getCRC16(m_ptrDataBuff, 6);
        nTmp = changeEndian(nTmp);
        memcpy(m_ptrDataBuff+6, &nTmp, 2);
        return true;
    }else if((ModBus::Write_Serval_Coil == m_nFunctionCode)
             ||(ModBus::Write_Serval_Reg == m_nFunctionCode))
    {
        int nDataLen = 0;
        if(ModBus::Write_Serval_Coil == m_nFunctionCode)
        {
            nDataLen = m_nRegisterCount/8;
            if(0 != (m_nRegisterCount%8))
            {
                nDataLen++;
            }
        }else
        {
            nDataLen = 2*m_nRegisterCount;
        }

        if((nDataLen+9) > m_nDataBuffLen)
        {
            return false;
        }

        m_ptrDataBuff[0] = m_nMeterNumber;
        m_ptrDataBuff[1] = m_nFunctionCode;
        unsigned short nTmp = changeEndian(m_nStartRegister);
        memcpy(m_ptrDataBuff+2, &nTmp, 2);
        nTmp = changeEndian(m_nRegisterCount);
        memcpy(m_ptrDataBuff+4, &nTmp, 2);
        m_ptrDataBuff[6] = (unsigned char)nDataLen;

        nTmp = getCRC16(m_ptrDataBuff, 7+nDataLen);
        nTmp = changeEndian(nTmp);
        memcpy(m_ptrDataBuff+7+nDataLen, &nTmp, 2);
        nLen = 9+nDataLen;
        return true;
    }else
    {
        return false;
    }
}

bool ModBus::parseCmd(int &nStartIndex, int &nCmdLen)
{
    if(m_nDataBuffLen < 4)
    {
        return false;
    }

    int nIndex = 0;
    int nMaxIndex = m_nDataBuffLen - 4;
    while(nIndex < nMaxIndex)
    {
        unsigned char nFunctionCode = m_ptrDataBuff[nIndex+1];
        if(nFunctionCode>0x80)
        {
            nFunctionCode -= 0x80;
            if((ModBus::Read_Coil==nFunctionCode)
                    ||(ModBus::Read_DI==nFunctionCode)
                    ||(ModBus::Read_Hold_Reg==nFunctionCode)
                    ||(ModBus::Read_Input_Reg==nFunctionCode)
                    ||(ModBus::Write_Single_Coil==nFunctionCode)
                    ||(ModBus::Write_Single_Reg==nFunctionCode)
                    ||(ModBus::Write_Serval_Coil==nFunctionCode)
                    ||(ModBus::Write_Serval_Reg==nFunctionCode))
            {
                if((nIndex+5)>m_nDataBuffLen)
                {
                    nIndex++;
                    continue;
                }
                unsigned short nCRC = 0;
                memcpy(&nCRC, m_ptrDataBuff+nIndex+3, 2);
                nCRC = changeEndian(nCRC);
                if(nCRC != getCRC16(m_ptrDataBuff+nIndex, 3))
                {
                    nIndex++;
                    continue;
                }
                m_nMeterNumber = m_ptrDataBuff[nIndex];
                m_nFunctionCode = m_ptrDataBuff[nIndex+1];
                m_nErrorCode = m_ptrDataBuff[nIndex+2];
                nStartIndex = m_nStartIndex = nIndex;
                nCmdLen = m_nCmdLength = 5;
                return true;
            }else
            {
                nIndex++;
                continue;
            }
        }else if((ModBus::Read_Coil==nFunctionCode)
                 ||(ModBus::Read_DI==nFunctionCode)
                 ||(ModBus::Read_Hold_Reg==nFunctionCode)
                 ||(ModBus::Read_Input_Reg==nFunctionCode))
        {
            unsigned char nDataLen = m_ptrDataBuff[nIndex+2];
            if((nIndex+5+nDataLen)>m_nDataBuffLen)
            {
                nIndex++;
                continue;
            }

            unsigned short nCRC = 0;
            memcpy(&nCRC, m_ptrDataBuff+nIndex+nDataLen+3, 2);
            nCRC = changeEndian(nCRC);
            if(nCRC != getCRC16(m_ptrDataBuff+nIndex, nDataLen+3))
            {
                nIndex++;
                continue;
            }
            m_nMeterNumber = m_ptrDataBuff[nIndex];
            m_nFunctionCode = m_ptrDataBuff[nIndex+1];
            m_nDataLength = m_ptrDataBuff[nIndex+2];
            nStartIndex = m_nStartIndex = nIndex;
            nCmdLen = m_nCmdLength = nDataLen+5;
            return true;
        }else if((ModBus::Write_Single_Coil == nFunctionCode)
                 ||(ModBus::Write_Single_Reg == nFunctionCode)
                 ||(ModBus::Write_Serval_Coil == nFunctionCode)
                 ||(ModBus::Write_Serval_Reg == nFunctionCode))
        {
            if((nIndex+8)>m_nDataBuffLen)
            {
                nIndex++;
                continue;
            }

            unsigned short nCRC = 0;
            memcpy(&nCRC, m_ptrDataBuff+nIndex+6, 2);
            nCRC = changeEndian(nCRC);
            if(nCRC != getCRC16(m_ptrDataBuff+nIndex, 6))
            {
                nIndex++;
                continue;
            }

            m_nMeterNumber = m_ptrDataBuff[0];
            m_nFunctionCode = m_ptrDataBuff[nIndex+1];
            memcpy(&m_nStartRegister, m_ptrDataBuff+nIndex+2, 2);
            m_nStartRegister = changeEndian(m_nStartRegister);

            if((ModBus::Write_Single_Coil == nFunctionCode)
                    ||(ModBus::Write_Single_Reg == nFunctionCode))
            {
                m_nRegisterCount = 1;
            }else
            {
                memcpy(&m_nRegisterCount, m_ptrDataBuff+nIndex+4, 2);
                m_nRegisterCount = changeEndian(m_nRegisterCount);
            }
            nCmdLen = m_nCmdLength = 8;
            return true;
        }

        nIndex++;
    }

    return false;
}
