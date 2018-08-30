#define DATA_PACKET_SIZE 64

//prototype, declared in annoter file
void PacketReceive(volatile uint8_t* data, uint32_t size);


void DataReceiveInit();
//Receive data from USB
void DataReceive(uint8_t* data, uint32_t size);


//add data to packet
void USBAdd(uint8_t* data, uint32_t size);
void USBAddStr(char* data);
void USBAdd8(uint8_t data);
void USBAdd16(uint16_t data);
void USBAdd32(uint32_t data);
void USBAdd32i(int32_t data);
void USBAddF(float data);
//send command 
void USBSend(void);
