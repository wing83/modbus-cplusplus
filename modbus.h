#ifndef MODBUS_H
#define MODBUS_H



class ModBus
{
public:
    ModBus(unsigned char *ptrBuff, int nLen);

    ModBus(unsigned char *ptrBuff, int nLen, unsigned char nMeterNumber, unsigned char nFunctionCode);

    enum FunctionCode
    {
        Read_Coil = 0x01,
        Read_DI = 0x02,
        Read_Hold_Reg = 0x03,
        Read_Input_Reg = 0x04,
        Write_Single_Coil = 0x05,
        Write_Single_Reg = 0x06,
        Write_Serval_Coil = 0x0F,
        Write_Serval_Reg = 0x10
    };

    void resetSettings();
    void resetSettings(unsigned char *ptrBuff, int nLen);

    void setMeterNumber(unsigned char nMeterNumber);
    void setFunctionCode(unsigned char nFunctionCode);
    void setStartRegister(unsigned short nRegister);
    void setRegisterCount(unsigned short nCount);

    unsigned char getMeterNumber();
    unsigned char getFunctionCode();
    unsigned short getStartRegister();
    unsigned short getRegisterCount();
    unsigned char getDataLength();
    unsigned char getErrorCode();

    bool setCoilValue(unsigned short nRegister, bool bValue);
    bool setRegisterValue(unsigned short nRegister, unsigned short nValue);
    bool setRawRegistsValue(unsigned short nStartRegister, unsigned char * dataBuff, int nLen);

    bool getCoilValue(int nRegisterIndex, bool &bValue);
    bool getRegisterValue(int nRegisterIndex, unsigned short &nValue);
    bool getRawRegisterValue(int nRegisterIndex, unsigned char* ptrDataBuff, unsigned nBuffLen);

    bool getCmd(int &nLen);
    bool parseCmd(int &nStartIndex, int &nCmdLen);

private:
    void init(unsigned char *ptrBuff, int nLen, unsigned char nMeterNumber, unsigned char nFunctionCode);

private:
    unsigned char*  m_ptrDataBuff;
    int             m_nDataBuffLen;
    unsigned char   m_nMeterNumber;
    unsigned char   m_nFunctionCode;
    unsigned short  m_nStartRegister;
    unsigned short  m_nRegisterCount;
    unsigned char   m_nDataLength;
    unsigned char   m_nErrorCode;
    int             m_nStartIndex;
    int             m_nCmdLength;
};

#endif // MODBUS_H
