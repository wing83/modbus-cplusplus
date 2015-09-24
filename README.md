# 基于C++的Modbus协议解析类 

使用示例：<br>

#### 示例1 

生成读取寄存器的报文


    $ cd /path/to/your/project
    $ cordova plugin add cordova-plugin-ble-central


    unsigned char  dataBuff[128] = {0}; 
    ModBus modbus(dataBuff, sizeof(dataBuff)); 



