# 基于C++的Modbus协议解析类 

使用示例：<br>

#### 示例1 

生成读取寄存器的报文


    unsigned char  dataBuff[128] = {0}; 
    ModBus modbus(dataBuff, sizeof(dataBuff)); 
    modbus.setMeterNumber(0x01);
    modbus.setFunctionCode(ModBus::Read_Coil);
    modbus.setStartRegister(0x13);
    modbus.setRegisterCount(0x13);

    int nCmdLen = 0;
    if(!modbus.getCmd(nCmdLen))
    {
    	//todo process error
    }


#### 示例2

生成写入寄存器的报文

    unsigned char  dataBuff[128] = {0}; 
    ModBus modbus(dataBuff, sizeof(dataBuff)); 
    modbus.setMeterNumber(0x01);
    modbus.setFunctionCode(ModBus::Write_Serval_Reg);
    modbus.setStartRegister(0x01);
    modbus.setRegisterCount(0x02);
    modbus.setRegisterValue(0x01, 0x3412);
    modbus.setRegisterValue(0x01, 0x102);

    int nCmdLen = 0;
    if(!modbus.getCmd(nLen)
    {
    	//todo process error
    }

#### 示例3

解析读取寄存器的报文

    unsigned char dataBuff[] = {0xFF, 0x04, 0x03, 0x06, 0x02, 0x2B, 0x00, 0x00, 0x00, 0x64, 0x3A, 0x2A};
    ModBus modbus(dataBuff, sizeof(dataBuff)); 
    int nStartIndex, nCmdLen;
    if(modbus.parseCmd(nStartIndex, nCmdLen))
    {
        //nStartIndex应该等1，nCmdLen应该等于11
        unsigned char nMeterNumber = modbus.getMeterNumber(); //等于0x04
        unsigned char nFunctionCode = modbus.getFunctionCode(); //等于0x03
        unsigned char nDataLen = modbus.getDataLength(); //等于6

        unsigned short nValue;
        if(modbus.getRegisterValue(0, nValue))
        {
            //nValue 应该等于0x22B
        }
    }
