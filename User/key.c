#include "key.h"
#include "delay.h"
#include "TimeCtrl.h"
#include "lcdControl.h"
#include "Led.h"
uint key_wait_time = 0;         //������ĵȴ�ʱ��
uchar key_state = 0;     //��������
#define MAX_WAIT_TIME 600  //������ȴ���һ�ΰ�����ʱ�䣬��λ��ms

void InitKey(void)
{
  //GPIO ��������, ����
   GPIO_Init(GPIOD,GPIO_PIN_4,GPIO_MODE_IN_PU_IT);
   //�½��ش���
   EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOD, EXTI_SENSITIVITY_FALL_ONLY);
}

//������״̬
BitStatus ReadKey(void)
{
	  BitStatus bit_status;	  
          
	  //��ȡ״̬
          bit_status = GPIO_ReadInputPin(GPIOD, GPIO_PIN_4);
         
          return bit_status;
}

void Halt_OffDevice(void)
{
  //�ر��豸ǰ������ϵͳ��ʱ�ӣ����ж� 
  CLK_DeInit();  
  
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
  
  ADC1_DeInit();
  TIM1_DeInit();

  //ͣ��ǰ�رղ���Ҫ�Ĺ���ģ���ʱ��  
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER3,DISABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER1,DISABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_ADC,DISABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2,DISABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER4,DISABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_AWU,DISABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_I2C,DISABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART2,DISABLE);
}

void Halt_OFFDevice(void)
{
  //�ر��豸ǰ������ϵͳ��ʱ�ӣ����ж� 
 // CLK_DeInit();  
  
  //CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV4);
  
  //ADC1_DeInit();
  //TIM1_DeInit();

  //ͣ��ǰ�رղ���Ҫ�Ĺ���ģ���ʱ��  
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER3,DISABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER1,DISABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_ADC,DISABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2,DISABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER4,DISABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_AWU,DISABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_I2C,DISABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART2,DISABLE);
  enableInterrupts();
}

void Halt_OnDevice(void)
{
  //�ر��豸ǰ������ϵͳ��ʱ�ӣ����ж� 
 // Clk_Init();  
  
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
   //�����ⲿ�ж�
	InitKey();
	//�����ж�
        enableInterrupts(); 	/* �������ж� */
  
  //ADC1_Init();
  //TIM1_Init();

  //ͣ��ǰ�رղ���Ҫ�Ĺ���ģ���ʱ��  
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER3,ENABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER1,ENABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_ADC,ENABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2,ENABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER4,ENABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_AWU,ENABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_I2C,ENABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART2,ENABLE);
}



uchar key_read()                         //��ȡ����,�����ذ�������
{    
    uchar key_state_temp = 0;
    static double currentTime;
    if(ReadKey() == RESET)
    {
        if(ReadKey() == RESET);
        {
          currentTime = systemRunTime;
            while(ReadKey() == RESET);
            //����������룬����halt
            if((systemRunTime - currentTime) > 3000)
            {
              CloseScreen();
              LED_Low;
              Halt_OFFDevice();
              halt();
              Halt_OnDevice();
              LighScreen();
            }
            key_state++;
            key_wait_time = 0;                 //���������¼�ʱ
            
        }
    }

    //����һ��ʱ��û���������������ذ�������
    if((key_wait_time >= MAX_WAIT_TIME) || (key_state >= 2))     
    {
        key_wait_time = 0;
        key_state_temp = key_state;
        key_state = 0;
    }

    return key_state_temp;
}

