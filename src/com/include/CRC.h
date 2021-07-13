//
// Created by hoshino on 2021/1/27.
//

#ifndef RADAR_CRC_H
#define RADAR_CRC_H
char Get_CRC8_Check_Sum(unsigned char * pchMessage,unsigned int dwLength,unsigned char ucCRC8);
void Append_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength);
uint16_t Get_CRC16_Check_Sum(uint8_t *pchMessage,uint32_t dwLength,uint16_t wCRC);
void Append_CRC16_Check_Sum(uint8_t * pchMessage,uint32_t dwLength);

extern uint16_t CRC_INIT;
//extern const unsigned char CRC8_INIT = 0xff;
#define CRC8_INIT 0xff
#endif //RADAR_CRC_H
