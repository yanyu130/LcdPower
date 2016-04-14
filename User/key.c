#include "key.h"
#include "delay.h"
#include "TimeCtrl.h"
#include "lcdControl.h"
#include "Led.h"
uint key_wait_time = 0;         //按键后的等待时间
uchar key_state = 0;     //按键次数
#define MAX_WAIT_TIME 600  //按键后等待另一次按键的时间，单位：ms

void InitKey(void)
{
  //GPIO 方向输入, 上拉
   GPIO_Init(GPIOD,GPIO_PIN_4,GPIO_MODE_IN_PU_IT);
   //下降沿触发
   EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOD, EXTI_SENSITIVITY_FALL_ONLY);
}

//读按键状态
BitStatus ReadKey(void)
{
	  BitStatus bit_status;	  
          
	  //获取状态
          bit_status = GPIO_ReadInputPin(GPIOD, GPIO_PIN_4);
         
          return bit_status;
}

void Halt_OffDevice(void)
{
  //关闭设备前，设置系统主时钟，和中断 
  CLK_DeInit();  
  
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
  
  ADC1_DeInit();
  TIM1_DeInit();

  //停机前关闭不需要的功能模块的时钟  
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
  //关闭设备前，设置系统主时钟，和中断 
 // CLK_DeInit();  
  
  //CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV4);
  
  //ADC1_DeInit();
  //TIM1_DeInit();

  //停机前关闭不需要的功能模块的时钟  
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
  //关闭设备前，设置系统主时钟，和中断 
 // Clk_Init();  
  
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
   //开启外部中断
	InitKey();
	//开总中断
        enableInterrupts(); 	/* 开启总中断 */
  
  //ADC1_Init();
  //TIM1_Init();

  //停机前关闭不需要的功能模块的时钟  
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER3,ENABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER1,ENABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_ADC,ENABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2,ENABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER4,ENABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_AWU,ENABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_I2C,ENABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART2,ENABLE);
}



uchar key_read()                         //读取按键,并返回按键次数
{    
    uchar key_state_temp = 0;
    static double currentTime;
    if(ReadKey() == RESET)
    {
        if(ReadKey() == RESET);
        {
          currentTime = systemRunTime;
            while(ReadKey() == RESET);
            //如果超过三秒，进入halt
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
            key_wait_time = 0;                 //按键后重新计时
            
        }
    }

    //超过一定时间没按键，函数将返回按键次数
    if((key_wait_time >= MAX_WAIT_TIME) || (key_state >= 2))     
    {
        key_wait_time = 0;
        key_state_temp = key_state;
        key_state = 0;
    }

    return key_state_temp;
}

