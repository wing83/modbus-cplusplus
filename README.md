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
    		//todo error
    }



