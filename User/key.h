#ifndef __KEY_H
#define __KEY_H

#include "stm8s.h"

#include "prohead.h"



extern uint key_wait_time;         //按键后的等待时间
extern uchar key_state;     //按键次数

//读按键状态
BitStatus ReadKey(void);
uchar key_read();                         //读取按键,并返回按键次数
void InitKey(void);

#endif