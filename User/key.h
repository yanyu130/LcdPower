#ifndef __KEY_H
#define __KEY_H

#include "stm8s.h"

#include "prohead.h"



extern uint key_wait_time;         //������ĵȴ�ʱ��
extern uchar key_state;     //��������

//������״̬
BitStatus ReadKey(void);
uchar key_read();                         //��ȡ����,�����ذ�������
void InitKey(void);

#endif