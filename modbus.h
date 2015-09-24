/***********************************
 * @file modbus.h
 * @author Xiao Zhi
 * @version 1.0
 * @date 2015-09-23
 * @brief Modbus协议的解析类
 * @details
 *************************************/

#ifndef MODBUS_H
#define MODBUS_H

class ModBus
{
public:
    /**
     * @brief 构造函数
     * @param ptrBuff 数据缓冲区
     * @param nLen 缓冲区的长度
     */
    ModBus(unsigned char *ptrBuff, int nLen);

    /**
     * @brief 构造函数
     * @param ptrBuff 数据缓冲区
     * @param nLen 缓冲区的长度
     * @param nMeterNumber 设备地址
     * @param nFunctionCode 本次协议的功能码
     */
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

    /**
     * @brief 复位成员变量，该函数在当Modbus对象使用过后，
     * 又要继续解析协议，但不想重新构建一个新的对象时，可以
     * 调用本函数后，继续使用旧的对象。该函数不会修改缓冲区的指向
     * 及缓冲区的长度。
     */
    void resetSettings();

    /**
     * @brief 复位成员变量，该函数在当Modbus对象使用过后，
     * 又要继续解析协议，但不想重新构建一个新的对象时，可以
     * 调用本函数后，继续使用旧的对象。该函数会重新设置缓冲区
     * 及缓冲区的长度。
     * @param ptrBuff 数据缓冲区
     * @param nLen 缓冲区的长度
     */
    void resetSettings(unsigned char *ptrBuff, int nLen);

    /**
     * @brief 设置Modbus表具的表号
     * @param nMeterNumber 表号
     */
    void setMeterNumber(unsigned char nMeterNumber);

    /**
     * @brief 设置Modbus协议的功能码，目前只支持以下类型的功能码：
     * 读线圈(0x01)、读输入离散量(0x02)、读多个寄存器(0x03)、
     * 读输入寄存器(0x04)、写单个线圈(0x05)、写单个寄存器(0x06)、
     * 写多个线圈(0x0F)、写多个寄存器(0x10).
     * @param nFunctionCode 功能码
     */
    void setFunctionCode(unsigned char nFunctionCode);

    /**
     * @brief 设置起始寄存器地址
     * @param nRegister 起始寄存器的地址
     */
    void setStartRegister(unsigned short nRegister);

    /**
     * @brief 设置起始寄存器长度
     * @param nCount 寄存器长度
     */
    void setRegisterCount(unsigned short nCount);

    /**
     * @brief 获取Modbus表具的表号
     * @return 表号
     */
    unsigned char getMeterNumber();

    /**
     * @brief 获取Modbus协议的功能码
     * @return 功能码
     */
    unsigned char getFunctionCode();

    /**
     * @brief 获取起始寄存器地址
     * @return 起始寄存器地址
     */
    unsigned short getStartRegister();

    /**
     * @brief 获取起始寄存器个数
     * @return 寄存器个数
     */
    unsigned short getRegisterCount();

    /**
     * @brief 获取数据长度,在解析读取命令(0x01, 0x02, 0x03, 0x04)时，
     * 会根据报文的内容返回相应的数据内容长度，用户可根据该返回值判断该报文
     * 是否是自己想要的报文
     * @return 数据内容的长度
     */
    unsigned char getDataLength();

    /**
     * @brief 获取异常码，在解析异常报文时，会返回报文中的异常码。
     * @return 异常码
     */
    unsigned char getErrorCode();

    /**
     * @brief 设置需要写入的线圈值，在生成写单个线圈(0x05)、写多外线圈(0x0F)命令时，
     * 需要调用该函数来设置需要写入线圈的值。线圈默认值为false.在调用该函数
     * 前,必须先设置功能码及线圈的起始地址,如果是写入多个线圈,则还需要设置线
     * 圈的个数.
     * @param nRegister 线圈的地址,如果功能码为写单个线圈,则该地址必须
     * 与之前设置的起始地址一致,如果功能码为写多个线圈,则该地址必须在之前
     * 设置的起始地址与线圈长度的范围之内.
     * @param bValue 线圈值
     * @return 写入的结果
     * @sa true 写入成功
     * @sa false 写入失败
     */
    bool setCoilValue(unsigned short nRegister, bool bValue);

    /**
     * @brief 设置需要写入的寄存器值，在生成写单个寄存器(0x06)、写多个寄存器(0x10)命令时，
     * 需要调用该函数来设置需要写入寄存器的值。在调用该函数前,必须先设置功能码及寄存器的起始
     * 地址,如果是写入多个寄存器,则还需要设置寄存器的个数.
     * @param nRegister 寄存器的地址,如果功能码为写单个寄存器,则该地址必须
     * 与之前设置的起始地址一致,如果功能码为写多个寄存器,则该地址必须在之前
     * 设置的起始地址与寄存器长度的范围之内.
     * @param nValue 寄存器值
     * @return 写入的结果
     * @sa true 写入成功
     * @sa false 写入失败
     */
    bool setRegisterValue(unsigned short nRegister, unsigned short nValue);

    /**
     * @brief 设置多个寄存器的值,在生成写多个寄存器(0x10)命令时，需要调用该函数来设置
     * 需要写入寄存器的值。在调用该函数前,必须先设置功能码,寄存器的起始地址及寄存器的个数.
     * 这些值是按modbus协议的字节序进行组织的.
     * @param nRegister 寄存器的地址,该地址必须在之前设置的起始地址与寄存器长度的范围之内.
     * @param dataBuff 寄存器值的数据缓冲区
     * @param nLen 数据缓冲区的长度
     * @return 写入的结果
     * @sa true 写入成功
     * @sa false 写入失败
     */
    bool setRawRegistersValue(unsigned short nStartRegister, unsigned char * dataBuff, int nLen);

    /**
     * @brief 获取线圈的值,在解析读取命令(0x01, 0x02)时，调用该函数可以返回线圈的值,
     * 在解析写单个线圈(0x05)时,可调用该函数来验证所返回的报文是否是所需要的报文.
     * @param nRegisterIndex 线圈地址的偏移
     * @param bValue 返回线圈的值, 该参数为引用参数,用于返回线圈值.
     * @return 获取成功与否
     * @sa true 函数调用成功
     * @sa false 函数调用失败
     */
    bool getCoilValue(int nRegisterIndex, bool &bValue);

    /**
     * @brief 获取寄存器的值,在解析读取命令(0x03, 0x04)时，调用该函数可以返回寄存器的值,
     * 在解析写单个寄存器(0x06)时,可调用该函数来验证所返回的报文是否是所需要的报文.
     * @param nRegisterIndex 寄存器地址的偏移
     * @param nValue 返回寄存器的值, 该参数为引用参数,用于返回寄存器值.
     * @return 获取成功与否
     * @sa true 函数调用成功
     * @sa false 函数调用失败
     */
    bool getRegisterValue(int nRegisterIndex, unsigned short &nValue);

    /**
     * @brief 获取多个寄存器的值,在解析读取多个寄存器命令0x04时，调用该函数可以返回寄存器的值,
     * 与setRawRegistersValue一个样,返回的值是按modbus协议的字节序进行组织的,需要使用者进行字节序转换.
     * @param nRegisterIndex 寄存器地址的偏移
     * @param ptrDataBuff 寄存器值的数据缓冲区
     * @param nBuffLen 数据缓冲区的长度
     * @return 获取成功与否
     * @sa true 函数调用成功
     * @sa false 函数调用失败
     */
    bool getRawRegistersValue(int nRegisterIndex, unsigned char* ptrDataBuff, unsigned nBuffLen);

    /**
     * @brief 生成Modbus报文, 生成的报文存储在构造函数中所设置的缓冲区中.
     * @param nLen 引用变量,返回所生成的报文的实际长度.
     * @return 报文生成成功与否
     * @sa true 函数调用成功
     * @sa false 函数调用失败
     */
    bool getCmd(int &nLen);

    /**
     * @brief 解析modbus报文
     * @param nStartIndex 引用变量,如果报文解析成功,则返回实际命令的起始位置.
     * @param nCmdLen 引用变量,如果报文解析成功,则返回实际命令的长度.
     * @return 解析报文成功与否
     * @sa true 解析成功
     * @sa false 解析失败
     */
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
