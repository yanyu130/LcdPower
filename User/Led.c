#include "Led.h"


void LedGPIO_init()
{
    GPIO_Init(GPIOC,GPIO_PIN_4,GPIO_MODE_OUT_PP_HIGH_FAST);
}

//LED���� ����1 ����; 0 Ϩ��;
void Driver_LEDCont(bool flag)
{
		static bool startflag = TRUE;
		if(startflag)
		{
                                
                                LedGPIO_init();
				startflag = (bool)0;
		}
		if(flag)
		{
                                LED_Low;
                                //GPIO_WriteLow(GPIOA,GPIO_PIN_3);//PC4����͵�ƽ������LED
                                                          
		}
		else
		{
                                LED_High;
                                //GPIO_WriteHigh(GPIOA,GPIO_PIN_3);//PC4����ߵ�ƽ���ر�LED��
                                
		}
}