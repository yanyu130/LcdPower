#include "delay.h"

//����ʱ����
void DIS_Delayms(u16 Num)
{	             
   unsigned int i;
   while(Num--)
   { 
     for(i=0;i<1228;i++);
   }
}

/*��ʱ*/
//void delay(int i)               
//{
//  int j,k;
//  for(j=0;j<i;j++)
//  for(k=0;k<110;k++);
//}

