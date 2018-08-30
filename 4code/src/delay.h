
//use timer 6 for us timer
//use timer 7 for 1 ms timer
void DelayInit();

//countUs < 60000
void DelayUs(uint16_t countUs);
void DelayMs(uint16_t countMs);

uint16_t TimeUs();

//<8192
uint16_t TimeMs();

//tick = 125 us
//Но! Что главное значения изменяются в интервале [0, 0xFFFF]
uint16_t TimeMs8();
