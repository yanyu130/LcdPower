
/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include "prohead.h"
#include "lcdControl.h"
#include "delay.h"
#include "key.h"
#include "Led.h"
#include "TimeCtrl.h"

#define EN_OUT1A_LOW   GPIO_WriteLow(GPIOA, GPIO_PIN_1)
#define EN_OUT2A_LOW   GPIO_WriteLow(GPIOA, GPIO_PIN_2)
#define EN_OUT1A_HIGH   GPIO_WriteHigh(GPIOA, GPIO_PIN_1)
#define EN_OUT2A_HIGH   GPIO_WriteHigh(GPIOA, GPIO_PIN_2)

//short curBattVolume = 0;        //当前电量UI值
//int curBattVolumePercent = 0;   //当前电量UI值，百分比
short curInputVolumeIndex = -1;  //输入电流
short curOutputVolumeIndex = -1; //输出电压

bool fPowerOn_flag = FALSE;

unsigned int  IdleTimeMs = 0;

double LastSystemRunTime = 0;
//double LastCheckEN_OUT_time = 0;
double overCurrentCheckTime = 0;
u8 bPermit_OUT = 0;  //标记，允许输出


uint16_t u16_ADC1_value = 0,u16_ADC1_valueSum = 0;
short sInputPutCurrent = 0;
short sOutputPutCurrent = 0;
int pwmOut = 0;
            
u8 checkOut1A=0,checkOut2A=0;
u8 checkIN = 0;
    
u8 i;
short AD_Voltage = 0;     //输入电压
short battVoltage = 0;
short battVoltageMHA = 0;
int battVoltageMHAPercent = 0;
          
BitStatus bit_status;

/* Private defines -----------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/


/********************************************************************************************************
*  Function: Driver_ADCON						     
*  Object: 启动ADC模块 执行初始化      
*  输入： 无
*  输出： 无								       	 
*  备注： 无                
********************************************************************************************************/
void Driver_ADCON(ADC1_Channel_TypeDef channel)
{       
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_ADC, ENABLE);
  ADC1_DeInit();
  //单次转换，通道3，系统时钟，8 分频，关闭外部触发事件，数据右对齐，关闭施密特触发器
  ADC1_Init(ADC1_CONVERSIONMODE_SINGLE, channel, ADC1_PRESSEL_FCPU_D8, ADC1_EXTTRIG_TIM, 
    DISABLE, ADC1_ALIGN_RIGHT, ADC1_SCHMITTTRIG_ALL, DISABLE);
  ADC1_ITConfig(ADC1_IT_AWDIE, DISABLE);  //看门狗中断关闭
  ADC1_ITConfig(ADC1_IT_EOCIE, DISABLE);
  ADC1_Cmd(ENABLE);//启用ADC1
}


//定时器2初始化
void Driver_TimeON(void)
{		
        //16M时钟 设定计数时钟的分频,计数周期为1US,1000次，即1ms中断一次
        //初始化计数器值,设定自动重载寄存器值    
  //CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2, ENABLE);
        TIM2_TimeBaseInit(TIM2_PRESCALER_16,1000);
        TIM2_SetCounter(0x0000);//将计数器的初始值设置为0
        TIM2_ITConfig(TIM2_IT_UPDATE, ENABLE);//允许计数溢出中断
        TIM2_Cmd(ENABLE);//使能TMI2
}

#pragma vector=13+2
__interrupt void TIM2_IRQ()
{
 // static bool flag = TRUE;
 TIM2_ClearITPendingBit(TIM2_IT_UPDATE);

        //---------------- 中断处理 ---------------------
       // count++;
        key_wait_time++;
        IdleTimeMs++;
        systemRunTime ++;
}



void int2str(int n, char *str)
    {
        char buf[10] = "";
        int i = 0;
        int len = 0;
        int temp = n < 0 ? -n: n;  // temp为n的绝对值
   
       if (str == NULL)
       {
           return;
       }
       while(temp)
       {
           buf[i++] = (temp % 10) + '0';  //把temp的每一位上的数存入buf
           temp = temp / 10;
       }
   
       len = n < 0 ? ++i: i;  //如果n是负数，则多需要一位来存储负号
       str[i] = 0;            //末尾是结束符0
       while(1)
       {
           i--;
           if (buf[len-i-1] ==0)
           {
               break;
           }
           str[i] = buf[len-i-1];  //把buf数组里的字符拷到字符串
       }
       if (i == 0 ) 
       {
           str[i] = '-';          //如果是负数，添加一个负号
       }
   }

short accordingADgetBatt(short AD_Voltage)
{
  short battVoltage = 0;
  if(AD_Voltage < 2020)
  {
     battVoltage = 3218;
  }
  else if(AD_Voltage >= 2020 && AD_Voltage < 2073)
  {
     battVoltage = 3218;
  }
  else if(AD_Voltage >= 2073 && AD_Voltage < 2137)
  {
     battVoltage = 3288;
  }
  else if(AD_Voltage >= 2137 && AD_Voltage < 2165)
  {
     battVoltage = 3384;
  }
  else if(AD_Voltage >= 2165 && AD_Voltage < 2204)
  {
     battVoltage = 3412;
  }
  else if(AD_Voltage >= 2204 && AD_Voltage < 2255)
  {
    battVoltage = 3498;
  }
  else if(AD_Voltage >= 2255 && AD_Voltage < 2319)
  {
    battVoltage = 3573;
  }
  else if(AD_Voltage >= 2319 && AD_Voltage < 2328)
  {
    battVoltage = 3670;
  }
  else if(AD_Voltage >= 2328 && AD_Voltage < 2377)
  {
    battVoltage = 3721;
  }
  else if(AD_Voltage >= 2377 && AD_Voltage < 2390)
  {
    battVoltage = 3794;
  }
  else if(AD_Voltage >= 2390 && AD_Voltage < 2520)
  {
    battVoltage = 3828;
  }
  else if(AD_Voltage >= 2520 && AD_Voltage < 2571)
  {
    battVoltage = 4012;
  }
  else if(AD_Voltage >= 2571 && AD_Voltage < 2576)
  {
    battVoltage = 4094;
  }
  else if(AD_Voltage >= 2576 && AD_Voltage < 2586)
  {
    battVoltage = 4110;
  }
  else if(AD_Voltage >= 2586 && AD_Voltage < 2593)
  {
    battVoltage = 4115;
  }
  else if(AD_Voltage >= 2593 && AD_Voltage < 2609)
  {
    battVoltage = 4125;
  }
  else if(AD_Voltage >= 2609)
  {
    battVoltage = 4150;
  } 
  
  return battVoltage;
}



/********************************************************************************************************
*  Function: Driver_FlashUNLock				     
*  Object: 解锁EEPROM
*  输入： 无
*  输出： 无 	 
*  备注:  无
********************************************************************************************************/
void Driver_FlashUNLock(void)
{
                FLASH_Unlock(FLASH_MEMTYPE_DATA);   //解锁EEPROM            
}


/********************************************************************************************************
*  Function: Driver_FlashLock				     
*  Object: 锁定EEPROM
*  输入： 无
*  输出： 无 	 
*  备注:  无
********************************************************************************************************/
void Driver_FlashLock(void)
{
                FLASH_Lock(FLASH_MEMTYPE_DATA);//锁定EEPROM
       
}
/********************************************************************************************************
*  Function: Driver_EPWrite				     
*  Object: 写EEPROM
*  输入： 头地址(0~1535)  数据指针  数据长
*  输出： 无 	 
*  备注: 1.5K EEPROM  不能超过
********************************************************************************************************/
void Driver_EPWrite(uint32_t Adr, u8 *pData, u16 Len)
{
		uint32_t *p;
		//p指针指向EEPROM 对应的单元
		p = (uint32_t*)0x4000 + Adr;   
		//解锁
		Driver_FlashUNLock();
		//写数据
		for( ; Len > ZERO; Len--)
		{              
              FLASH_ProgramByte((uint32_t)p, *pData);
              p++;
              pData++;
		} 
		//加锁
		Driver_FlashLock();
}

/********************************************************************************************************
*  Function: Driver_EPRead				     
*  Object: 读EEPROM
*  输入： 头地址(0~1535) 数据存放指针 数据长
*  输出： 无 	 
*  备注: 1.5K EEPROM
********************************************************************************************************/
void Driver_EPRead(uint32_t Adr, u8 *pData, u16 Len)
{
		uint32_t *p;
		//p指针指向EEPROM 对应的单元
		p = (uint32_t*)0x4000 + Adr; 
		//解锁
		Driver_FlashUNLock();
		//读数据
		for( ; Len > ZERO; Len--)
                {
			*pData++ = FLASH_ReadByte((uint32_t)p);
      p++;
                }
		//加锁
		Driver_FlashLock();
}

//PWM初始化
void Driver_PWMON(void)
{ 
     CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER1, ENABLE);
     //TIM1_DeInit();
     //16 MHz/16 分频= 1 兆赫、向上计数， 1 MHz /1000 =计数 1000
     TIM1_TimeBaseInit( 15, TIM1_COUNTERMODE_UP, 1000, 0 );
     //初始化捕获/比较通道1（PWM模式1，启用输出比较），
     TIM1_OC3Init( TIM1_OCMODE_PWM1, TIM1_OUTPUTSTATE_ENABLE,
    //启用互补输出比较1，脉宽0%，输出比较极性高电平，
    TIM1_OUTPUTNSTATE_ENABLE, 0, TIM1_OCPOLARITY_HIGH,
    //互补输出比较极性高电平，输出比较空闲状态复位，互补输出比较空闲状态复//位）
    TIM1_OCNPOLARITY_HIGH, TIM1_OCIDLESTATE_RESET, TIM1_OCNIDLESTATE_SET );
      //启用定时器
     TIM1_Cmd(ENABLE);
            //允许主输出      移动 TIM1 定时器。选择 PWM 输出。
     TIM1_CtrlPWMOutputs(ENABLE);
}

//检测充电，并做相应处理
int checkBattVoltage(short battVoltage)
{
  int pwmOut = 0;
  BitStatus bit_status;
  //GPIO 方向输入
  //GPIO_Init(GPIOD,GPIO_PIN_5,GPIO_MODE_IN_FL_NO_IT);
  //获取状态
  bit_status = GPIO_ReadInputPin(GPIOD, GPIO_PIN_5);
  if(bit_status != RESET)
  {
      LighScreen();
      
      //充电激活
//      bPermit_OUT = 0;
//      Driver_EPWrite(0, &bPermit_OUT, 1);
//      EN_OUT1A_HIGH;
//      EN_OUT2A_HIGH;
      
      //检测电量，改变充电PWM控制
      if(battVoltage>=4150)
      {
        pwmOut = 0;
      }
      else if(battVoltage<4150 && battVoltage>4012)
      {
        pwmOut = 500;
      }
      else if(battVoltage<=4012)
      {
        pwmOut = 1000;
      }
  }
  return pwmOut;
}

//读取电量AD
uint16_t getBattVolumeAD()
{
    uint16_t u16_ADC1_value = 0, u16_ADC1_valueSum = 0;
    u8 i;
     //启ADC外设,读取电量
     Driver_ADCON(ADC1_CHANNEL_3);
     DIS_Delayms(10);
     // 开始转换
    ADC1_StartConversion();
    DIS_Delayms(10);
    //读ADC值
    for(i=0;i<20;i++)
    {
       u16_ADC1_valueSum += ADC1_GetConversionValue();
    }
     u16_ADC1_value = u16_ADC1_valueSum /20;
     return u16_ADC1_value;
}

//获得电量图形字符下标
void getVolumePatternIndex(int battVoltageMHAPercent, char* strPatternIndex)
{
  
  if(battVoltageMHAPercent == 0)  //0%
  {
    strPatternIndex[0] = '#';
  }
  else if(battVoltageMHAPercent > 0 && battVoltageMHAPercent <= 30) //25%
  {
    strPatternIndex[0] = '&';
  }
  else if(battVoltageMHAPercent > 30 && battVoltageMHAPercent <= 70)  //50%
  {
    strPatternIndex[0] = '<';
  }
  else if(battVoltageMHAPercent > 70 && battVoltageMHAPercent <= 95)  //75%
  {
    strPatternIndex[0] = '>';
  }
  else if(battVoltageMHAPercent > 95 && battVoltageMHAPercent <= 100) //100%
  {
    strPatternIndex[0] = '!';
  }
  //return strPatternIndex;
}

//更新电量值UI
void updateUIBattVolume(short battVoltageMHA)
{
  char strAD[10];
  static short curBattVolume = -1;
  if(curBattVolume == battVoltageMHA)
  {
    return;
  }
  curBattVolume = battVoltageMHA;
  
  if(battVoltageMHA > 0)
  {
      int2str(battVoltageMHA,strAD);
      strcat(strAD,"mah");
      display_string_16x24(0,0,strAD,custome,black);
  }
  else
  {
      display_string_16x24(0,2,"0mah",custome,black);
  }
}

//更新电量值百分比UI
void updateUIBattVolumePercent(int battVoltageMHAPercent)
{
  static int curBattVolumePercent = -1;
  char strADPercent[10];
  char strPatternIndex[2];  //存放电量图案的索引号
  strPatternIndex[1] = '\0';
  if(curBattVolumePercent == battVoltageMHAPercent)
  {
    return;
  }
  curBattVolumePercent = battVoltageMHAPercent;
  
  //显示百分比   
  if(battVoltageMHAPercent > 0)
  {
    int2str(battVoltageMHAPercent,strADPercent);
    strcat(strADPercent,"% ");
    display_string_16x24(1,1,strADPercent,custome,black);
              
    //显示电量的图案
    getVolumePatternIndex(battVoltageMHAPercent, strPatternIndex);
    display_string_16x24(1,5,strPatternIndex,custome,black);
  }
  else
  {
      display_string_16x24(1,1,"0% ",custome,black);
      display_string_16x24(1,5,"#",custome,black);
  }
}

//更新输入电量值
void updateUIInputVolume(short InputVolumeIndex)
{
  curOutputVolumeIndex = -1;  //初始化输出电压的index
  
  if(curInputVolumeIndex == InputVolumeIndex)
  {
    return;
  }
  curInputVolumeIndex = InputVolumeIndex;

  if(curInputVolumeIndex == 100)
  {
    display_string_16x24(2,0,"IN100ma",custome,black);
  }
  if(curInputVolumeIndex == 300)
  {
    display_string_16x24(2,0,"IN300ma",custome,black);
  }
  if(curInputVolumeIndex == 500)
  {
    display_string_16x24(2,0,"IN500ma",custome,black);
  }
  else if(curInputVolumeIndex == 600)
  {
    display_string_16x24(2,0,"IN600ma",custome,black);
  }
  else if(curInputVolumeIndex == 800)
  {
    display_string_16x24(2,0,"IN800ma",custome,black);
  }
  else if(curInputVolumeIndex == 1000)
  {
    display_string_16x24(2,0,"IN1000ma",custome,black);
  }
  else if(curInputVolumeIndex == 1200)
  {
    display_string_16x24(2,0,"IN1200ma",custome,black);
  }
  else if(curInputVolumeIndex == 1500)
  {
    display_string_16x24(2,0,"IN1500ma",custome,black);
  }
  else if(curInputVolumeIndex == 0)
  {
    display_string_16x24(2,0,"IN   0ma",custome,black);
  }
  
}

//更新输出电压值
void updateUIOutputVolume(short OutputVolumeIndex)
{
  curInputVolumeIndex = -1;   //初始化输入电流的index
  
  if(curOutputVolumeIndex == OutputVolumeIndex)
  {
    return;
  }
  curOutputVolumeIndex = OutputVolumeIndex;
  
  if(curOutputVolumeIndex == 100)
  {
    display_string_16x24(2,0,"$",custome,black);
    display_string_16x24(2,2,"100ma",custome,black);
  }
  else if(curOutputVolumeIndex == 300)
  {
    display_string_16x24(2,0,"$",custome,black);
    display_string_16x24(2,2,"300ma",custome,black);
  }
  else if(curOutputVolumeIndex == 500)
  {
    display_string_16x24(2,0,"$",custome,black);
    display_string_16x24(2,2,"500ma",custome,black);
  }
  else if(curOutputVolumeIndex == 600)
  {
    display_string_16x24(2,0,"$",custome,black);
    display_string_16x24(2,2,"600ma",custome,black);
  }
  else if(curOutputVolumeIndex == 800)
  {
    display_string_16x24(2,0,"$",custome,black);
    display_string_16x24(2,2,"800ma",custome,black);
  }
  
}

  //处理按键
void DealPressKey()
{
  uchar key_state_temp;

            key_state_temp = key_read();
            if(key_state_temp == 1)
            {
               LED_Low;        //按键1次，灯灭
               key_wait_time = 0;
               IdleTimeMs = 0;
               LighScreen();
            }
            else if(key_state_temp >= 2)
            {
                LED_High;        //按键大于1次，灯亮    
                key_wait_time = 0;
                IdleTimeMs = 0;
                LighScreen();
            }
}

void initLCD()
{
  //初始化LCD
        LCD_GPIO_Config();
        
        LCD_init(); 

        Lcd_clear();
        BACK_LIGHT_ON;
}

//获取输入状态
BitStatus getInputStatus()
{
  BitStatus bit_status;	  
  //GPIO 方向输入
  //GPIO_Init(GPIOD,GPIO_PIN_5,GPIO_MODE_IN_FL_NO_IT);
  //获取状态
  bit_status = GPIO_ReadInputPin(GPIOD, GPIO_PIN_5);
  return bit_status;
}

short calInputVol(short InputVoltageAD)
{
  short ret = 0;
   ret =  (short)((float)InputVoltageAD*1024/3297);
   return ret;
}

short calOutputVol(short OutputVoltageAD)
{
  short ret = 0;
   ret =  (short)((float)OutputVoltageAD*1024/3297);
   return ret;
}

//返回放电电量
//通过电池电压，和放电的电压值，得到放电电流
short getOutputCurrent(short battVolume)
{
  uint16_t u16_ADC1_valueSum = 0, u16_ADC1_value = 0;
  short OutputCurrent = 0;
  u8 i;
  //读取charge状态,PB1
  Driver_ADCON(ADC1_CHANNEL_1);
  DIS_Delayms(10);
  // 开始转换
  ADC1_StartConversion();
  DIS_Delayms(10);
  u16_ADC1_valueSum = 0;
  
  //读ADC值
  for(i=0;i<20;i++)
  {
    u16_ADC1_valueSum += ADC1_GetConversionValue();
  }
  u16_ADC1_value = u16_ADC1_valueSum /20;
  
  //3.3V

  if(battVolume >= 3300 && battVolume < 3400)
  {
    if(u16_ADC1_value >= calOutputVol(20) && u16_ADC1_value < calOutputVol(56)) 
    {
      OutputCurrent = (100);
    }
    else if(u16_ADC1_value >= calOutputVol(56) && u16_ADC1_value < calOutputVol(95))
    {
      OutputCurrent = (300);
    }
    else if(u16_ADC1_value >= calOutputVol(95) && u16_ADC1_value < calOutputVol(116))
    {
      OutputCurrent = (500);
    }
    else if(u16_ADC1_value >= calOutputVol(116) && u16_ADC1_value < calOutputVol(160))
    {
      OutputCurrent = (600);
    }
    else if(u16_ADC1_value >= calOutputVol(160))
    {
      OutputCurrent = (800);
    }
  }
  
  //3.4v
  else if(battVolume >= 3400 && battVolume < 3500)
  {
    if(u16_ADC1_value >= calOutputVol(19) && u16_ADC1_value < calOutputVol(55)) 
    {
      OutputCurrent = (100);
    }
    else if(u16_ADC1_value >= calOutputVol(55) && u16_ADC1_value < calOutputVol(92))
    {
      OutputCurrent = (300);
    }
    else if(u16_ADC1_value >= calOutputVol(92) && u16_ADC1_value < calOutputVol(113))
    {
      OutputCurrent = (500);
    }
    else if(u16_ADC1_value >= calOutputVol(113) && u16_ADC1_value < calOutputVol(154))
    {
      OutputCurrent = (600);
    }
    else if(u16_ADC1_value >= calOutputVol(154))
    {
      OutputCurrent = (800);
    }
  }
  
  //3.5v
  if(battVolume >= 3500 && battVolume < 3600)
 {
    if(u16_ADC1_value >= calOutputVol(19) && u16_ADC1_value < calOutputVol(54)) 
    {
      OutputCurrent = (100);
    }
    else if(u16_ADC1_value >= calOutputVol(54) && u16_ADC1_value < calOutputVol(90))
    {
      OutputCurrent = (300);
    }
    else if(u16_ADC1_value >= calOutputVol(90) && u16_ADC1_value < calOutputVol(110))
    {
      OutputCurrent = (500);
    }
    else if(u16_ADC1_value >= calOutputVol(110) && u16_ADC1_value < calOutputVol(150))
    {
      OutputCurrent = (600);
    }
    else if(u16_ADC1_value >= calOutputVol(150))
    {
      OutputCurrent = (800);
    }
  }
  
  //3.6V
  if(battVolume >= 3600 && battVolume < 3700)
  {
     if(u16_ADC1_value >= calOutputVol(19) && u16_ADC1_value < calOutputVol(54)) 
    {
      OutputCurrent = (100);
    }
    else if(u16_ADC1_value >= calOutputVol(54) && u16_ADC1_value < calOutputVol(90))
    {
      OutputCurrent = (300);
    }
    else if(u16_ADC1_value >= calOutputVol(90) && u16_ADC1_value < calOutputVol(110))
    {
      OutputCurrent = (500);
    }
    else if(u16_ADC1_value >= calOutputVol(110) && u16_ADC1_value < calOutputVol(150))
    {
      OutputCurrent = (600);
    }
    else if(u16_ADC1_value >= calOutputVol(150))
    {
      OutputCurrent = (800);
    }
  }
  
  //3.7V
  if(battVolume >= 3700 && battVolume < 3800)
  {
    if(u16_ADC1_value >= calOutputVol(18) && u16_ADC1_value < calOutputVol(51)) 
    {
      OutputCurrent = (100);
    }
    else if(u16_ADC1_value >= calOutputVol(51) && u16_ADC1_value < calOutputVol(86))
    {
      OutputCurrent = (300);
    }
    else if(u16_ADC1_value >= calOutputVol(86) && u16_ADC1_value < calOutputVol(105))
    {
      OutputCurrent = (500);
    }
    else if(u16_ADC1_value >= calOutputVol(105) && u16_ADC1_value < calOutputVol(142))
    {
      OutputCurrent = (600);
    }
    else if(u16_ADC1_value >= calOutputVol(142))
    {
      OutputCurrent = (800);
    }
  }
  
  //3.8V
  if(battVolume >= 3800 && battVolume < 3900)
  {
    if(u16_ADC1_value >= calOutputVol(17) && u16_ADC1_value < calOutputVol(51)) 
    {
      OutputCurrent = (100);
    }
    else if(u16_ADC1_value >= calOutputVol(51) && u16_ADC1_value < calOutputVol(84))
    {
      OutputCurrent = (300);
    }
    else if(u16_ADC1_value >= calOutputVol(84) && u16_ADC1_value < calOutputVol(102))
    {
      OutputCurrent = (500);
    }
    else if(u16_ADC1_value >= calOutputVol(102) && u16_ADC1_value < calOutputVol(138))
    {
      OutputCurrent = (600);
    }
    else if(u16_ADC1_value >= calOutputVol(138))
    {
      OutputCurrent = (800);
    }
  }
  
  //3.9V

  if(battVolume >= 3900 && battVolume < 4000)
  {
    if(u16_ADC1_value >= calOutputVol(17) && u16_ADC1_value < calOutputVol(50)) 
    {
      OutputCurrent = (100);
    }
    else if(u16_ADC1_value >= calOutputVol(50) && u16_ADC1_value < calOutputVol(82))
    {
      OutputCurrent = (300);
    }
    else if(u16_ADC1_value >= calOutputVol(82) && u16_ADC1_value < calOutputVol(100))
    {
      OutputCurrent = (500);
    }
    else if(u16_ADC1_value >= calOutputVol(100) && u16_ADC1_value < calOutputVol(135))
    {
      OutputCurrent = (600);
    }
    else if(u16_ADC1_value >= calOutputVol(135))
    {
      OutputCurrent = (800);
    }
  }
  
  //4.0V	
  if(battVolume >= 4000 && battVolume < 4100)
  {
    if(u16_ADC1_value >= calOutputVol(17) && u16_ADC1_value < calOutputVol(49)) 
    {
      OutputCurrent = (100);
    }
    else if(u16_ADC1_value >= calOutputVol(49) && u16_ADC1_value < calOutputVol(80))
    {
      OutputCurrent = (300);
    }
    else if(u16_ADC1_value >= calOutputVol(80) && u16_ADC1_value < calOutputVol(98))
    {
      OutputCurrent = (500);
    }
    else if(u16_ADC1_value >= calOutputVol(98) && u16_ADC1_value < calOutputVol(131))
    {
      OutputCurrent = (600);
    }
    else if(u16_ADC1_value >= calOutputVol(131))
    {
      OutputCurrent = (800);
    }
  }
  
  //4.1V
  if(battVolume >= 4000 && battVolume < 4100)
  {
    if(u16_ADC1_value >= calOutputVol(17) && u16_ADC1_value < calOutputVol(48)) 
    {
      OutputCurrent = (100);
    }
    else if(u16_ADC1_value >= calOutputVol(48) && u16_ADC1_value < calOutputVol(79))
    {
      OutputCurrent = (300);
    }
    else if(u16_ADC1_value >= calOutputVol(79) && u16_ADC1_value < calOutputVol(97))
    {
      OutputCurrent = (500);
    }
    else if(u16_ADC1_value >= calOutputVol(97) && u16_ADC1_value < calOutputVol(129))
    {
      OutputCurrent = (600);
    }
    else if(u16_ADC1_value >= calOutputVol(129))
    {
      OutputCurrent = (800);
    }
  }
  
  //4.2V

  if(battVolume >= 4000 && battVolume < 4100)
  {
    if(u16_ADC1_value >= calOutputVol(16) && u16_ADC1_value < calOutputVol(47)) 
    {
      OutputCurrent = (100);
    }
    else if(u16_ADC1_value >= calOutputVol(47) && u16_ADC1_value < calOutputVol(78))
    {
      OutputCurrent = (300);
    }
    else if(u16_ADC1_value >= calOutputVol(78) && u16_ADC1_value < calOutputVol(93))
    {
      OutputCurrent = (500);
    }
    else if(u16_ADC1_value >= calOutputVol(93) && u16_ADC1_value < calOutputVol(126))
    {
      OutputCurrent = (600);
    }
    else if(u16_ADC1_value >= calOutputVol(126))
    {
      OutputCurrent = (800);
    }
  }
  return OutputCurrent;
}

//返回放电电量
//通过电池电压，和放电的电压值，得到放电电流
short getOutputCurrent2(short battVolume)
{
  uint16_t u16_ADC1_valueSum = 0, u16_ADC1_value = 0;
  short OutputCurrent = 0;
  u8 i;
  //读取charge状态,PB0
  Driver_ADCON(ADC1_CHANNEL_0);
  DIS_Delayms(10);
  // 开始转换
  ADC1_StartConversion();
  DIS_Delayms(10);
  u16_ADC1_valueSum = 0;
  
  //读ADC值
  for(i=0;i<20;i++)
  {
    u16_ADC1_valueSum += ADC1_GetConversionValue();
  }
  u16_ADC1_value = u16_ADC1_valueSum /20;
  
  //3.3V

  if(battVolume >= 3300 && battVolume < 3400)
  {
    if(u16_ADC1_value >= calOutputVol(20) && u16_ADC1_value < calOutputVol(56)) 
    {
      OutputCurrent = (100);
    }
    else if(u16_ADC1_value >= calOutputVol(56) && u16_ADC1_value < calOutputVol(95))
    {
      OutputCurrent = (300);
    }
    else if(u16_ADC1_value >= calOutputVol(95) && u16_ADC1_value < calOutputVol(116))
    {
      OutputCurrent = (500);
    }
    else if(u16_ADC1_value >= calOutputVol(116) && u16_ADC1_value < calOutputVol(160))
    {
      OutputCurrent = (600);
    }
    else if(u16_ADC1_value >= calOutputVol(160))
    {
      OutputCurrent = (800);
    }
  }
  
  //3.4v
  else if(battVolume >= 3400 && battVolume < 3500)
  {
    if(u16_ADC1_value >= calOutputVol(19) && u16_ADC1_value < calOutputVol(55)) 
    {
      OutputCurrent = (100);
    }
    else if(u16_ADC1_value >= calOutputVol(55) && u16_ADC1_value < calOutputVol(92))
    {
      OutputCurrent = (300);
    }
    else if(u16_ADC1_value >= calOutputVol(92) && u16_ADC1_value < calOutputVol(113))
    {
      OutputCurrent = (500);
    }
    else if(u16_ADC1_value >= calOutputVol(113) && u16_ADC1_value < calOutputVol(154))
    {
      OutputCurrent = (600);
    }
    else if(u16_ADC1_value >= calOutputVol(154))
    {
      OutputCurrent = (800);
    }
  }
  
  //3.5v
  if(battVolume >= 3500 && battVolume < 3600)
 {
    if(u16_ADC1_value >= calOutputVol(19) && u16_ADC1_value < calOutputVol(54)) 
    {
      OutputCurrent = (100);
    }
    else if(u16_ADC1_value >= calOutputVol(54) && u16_ADC1_value < calOutputVol(90))
    {
      OutputCurrent = (300);
    }
    else if(u16_ADC1_value >= calOutputVol(90) && u16_ADC1_value < calOutputVol(110))
    {
      OutputCurrent = (500);
    }
    else if(u16_ADC1_value >= calOutputVol(110) && u16_ADC1_value < calOutputVol(150))
    {
      OutputCurrent = (600);
    }
    else if(u16_ADC1_value >= calOutputVol(150))
    {
      OutputCurrent = (800);
    }
  }
  
  //3.6V
  if(battVolume >= 3600 && battVolume < 3700)
  {
     if(u16_ADC1_value >= calOutputVol(19) && u16_ADC1_value < calOutputVol(54)) 
    {
      OutputCurrent = (100);
    }
    else if(u16_ADC1_value >= calOutputVol(54) && u16_ADC1_value < calOutputVol(90))
    {
      OutputCurrent = (300);
    }
    else if(u16_ADC1_value >= calOutputVol(90) && u16_ADC1_value < calOutputVol(110))
    {
      OutputCurrent = (500);
    }
    else if(u16_ADC1_value >= calOutputVol(110) && u16_ADC1_value < calOutputVol(150))
    {
      OutputCurrent = (600);
    }
    else if(u16_ADC1_value >= calOutputVol(150))
    {
      OutputCurrent = (800);
    }
  }
  
  //3.7V
  if(battVolume >= 3700 && battVolume < 3800)
  {
    if(u16_ADC1_value >= calOutputVol(18) && u16_ADC1_value < calOutputVol(51)) 
    {
      OutputCurrent = (100);
    }
    else if(u16_ADC1_value >= calOutputVol(51) && u16_ADC1_value < calOutputVol(86))
    {
      OutputCurrent = (300);
    }
    else if(u16_ADC1_value >= calOutputVol(86) && u16_ADC1_value < calOutputVol(105))
    {
      OutputCurrent = (500);
    }
    else if(u16_ADC1_value >= calOutputVol(105) && u16_ADC1_value < calOutputVol(142))
    {
      OutputCurrent = (600);
    }
    else if(u16_ADC1_value >= calOutputVol(142))
    {
      OutputCurrent = (800);
    }
  }
  
  //3.8V
  if(battVolume >= 3800 && battVolume < 3900)
  {
    if(u16_ADC1_value >= calOutputVol(17) && u16_ADC1_value < calOutputVol(51)) 
    {
      OutputCurrent = (100);
    }
    else if(u16_ADC1_value >= calOutputVol(51) && u16_ADC1_value < calOutputVol(84))
    {
      OutputCurrent = (300);
    }
    else if(u16_ADC1_value >= calOutputVol(84) && u16_ADC1_value < calOutputVol(102))
    {
      OutputCurrent = (500);
    }
    else if(u16_ADC1_value >= calOutputVol(102) && u16_ADC1_value < calOutputVol(138))
    {
      OutputCurrent = (600);
    }
    else if(u16_ADC1_value >= calOutputVol(138))
    {
      OutputCurrent = (800);
    }
  }
  
  //3.9V

  if(battVolume >= 3900 && battVolume < 4000)
  {
    if(u16_ADC1_value >= calOutputVol(17) && u16_ADC1_value < calOutputVol(50)) 
    {
      OutputCurrent = (100);
    }
    else if(u16_ADC1_value >= calOutputVol(50) && u16_ADC1_value < calOutputVol(82))
    {
      OutputCurrent = (300);
    }
    else if(u16_ADC1_value >= calOutputVol(82) && u16_ADC1_value < calOutputVol(100))
    {
      OutputCurrent = (500);
    }
    else if(u16_ADC1_value >= calOutputVol(100) && u16_ADC1_value < calOutputVol(135))
    {
      OutputCurrent = (600);
    }
    else if(u16_ADC1_value >= calOutputVol(135))
    {
      OutputCurrent = (800);
    }
  }
  
  //4.0V	
  if(battVolume >= 4000 && battVolume < 4100)
  {
    if(u16_ADC1_value >= calOutputVol(17) && u16_ADC1_value < calOutputVol(49)) 
    {
      OutputCurrent = (100);
    }
    else if(u16_ADC1_value >= calOutputVol(49) && u16_ADC1_value < calOutputVol(80))
    {
      OutputCurrent = (300);
    }
    else if(u16_ADC1_value >= calOutputVol(80) && u16_ADC1_value < calOutputVol(98))
    {
      OutputCurrent = (500);
    }
    else if(u16_ADC1_value >= calOutputVol(98) && u16_ADC1_value < calOutputVol(131))
    {
      OutputCurrent = (600);
    }
    else if(u16_ADC1_value >= calOutputVol(131))
    {
      OutputCurrent = (800);
    }
  }
  
  //4.1V
  if(battVolume >= 4000 && battVolume < 4100)
  {
    if(u16_ADC1_value >= calOutputVol(17) && u16_ADC1_value < calOutputVol(48)) 
    {
      OutputCurrent = (100);
    }
    else if(u16_ADC1_value >= calOutputVol(48) && u16_ADC1_value < calOutputVol(79))
    {
      OutputCurrent = (300);
    }
    else if(u16_ADC1_value >= calOutputVol(79) && u16_ADC1_value < calOutputVol(97))
    {
      OutputCurrent = (500);
    }
    else if(u16_ADC1_value >= calOutputVol(97) && u16_ADC1_value < calOutputVol(129))
    {
      OutputCurrent = (600);
    }
    else if(u16_ADC1_value >= calOutputVol(129))
    {
      OutputCurrent = (800);
    }
  }
  
  //4.2V

  if(battVolume >= 4000 && battVolume < 4100)
  {
    if(u16_ADC1_value >= calOutputVol(16) && u16_ADC1_value < calOutputVol(47)) 
    {
      OutputCurrent = (100);
    }
    else if(u16_ADC1_value >= calOutputVol(47) && u16_ADC1_value < calOutputVol(78))
    {
      OutputCurrent = (300);
    }
    else if(u16_ADC1_value >= calOutputVol(78) && u16_ADC1_value < calOutputVol(93))
    {
      OutputCurrent = (500);
    }
    else if(u16_ADC1_value >= calOutputVol(93) && u16_ADC1_value < calOutputVol(126))
    {
      OutputCurrent = (600);
    }
    else if(u16_ADC1_value >= calOutputVol(126))
    {
      OutputCurrent = (800);
    }
  }
  return OutputCurrent;
}

//返回充电电量
//通过电池电压，和充电的电压值，得到充电电流
short getInputCurrent(short battVolume)
{
  uint16_t u16_ADC1_valueSum = 0, u16_ADC1_value = 0;
  short InputCurrent = 0;
  u8 i;
  //读取charge状态,PB2
  Driver_ADCON(ADC1_CHANNEL_2);
  DIS_Delayms(10);
  // 开始转换
  ADC1_StartConversion();
  DIS_Delayms(10);
  u16_ADC1_valueSum = 0;
  
  //读ADC值
  for(i=0;i<20;i++)
  {
    u16_ADC1_valueSum += ADC1_GetConversionValue();
  }
  u16_ADC1_value = u16_ADC1_valueSum /20;
  
  //battVolume = 3333;
  //u16_ADC1_value = 25;
  
//  if(u16_ADC1_value < calInputVol(64))  //要求64MV以上
//  {
//    return -1;
//  }
  
  
  if(battVolume < 3700)
  {
      InputCurrent = (1190);
  }
  else if(battVolume >= 3700 && battVolume < 3900)
  {
      InputCurrent = (799);
  }
  else if(battVolume >= 3900)
  {
    InputCurrent = (599);
  }
  
  return InputCurrent;
}

void Setup()
{
        //允许放电IO口
        GPIO_Init(GPIOA,GPIO_PIN_1,GPIO_MODE_OUT_PP_HIGH_FAST); 
        GPIO_Init(GPIOA,GPIO_PIN_2,GPIO_MODE_OUT_PP_HIGH_FAST);
        
        //初始化检测充电口
        GPIO_Init(GPIOD,GPIO_PIN_5,GPIO_MODE_IN_FL_NO_IT);
        //允许放电
        EN_OUT1A_HIGH;
        EN_OUT2A_HIGH;
 
        //初始化LCD
        initLCD();
        
	//开启定时器1ms中断
	Driver_TimeON();
        
        //放电占空比
        Driver_PWMON();
        TIM1_SetCompare3(0);
        
        enableInterrupts(); 	//* 开启总中断 */
        
        //初始化LED
        LedGPIO_init();
        LED_Low;

}

INTERRUPT_HANDLER(EXTI_PORTD_IRQHandler, 6)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  
  EXTI_Sensitivity_TypeDef sensitivity;
  sensitivity = EXTI_GetExtIntSensitivity(EXTI_PORT_GPIOD);
//返回值：
//EXTI_SENSITIVITY_FALL_LOW 下降沿和低电平触发
//EXTI_SENSITIVITY_RISE_ONLY 仅上升沿触发
//EXTI_SENSITIVITY_FALL_ONLY 仅下降沿触发
//EXTI_SENSITIVITY_RISE_FALL 上升沿和下降沿触发  
  switch(sensitivity)
  {
  case EXTI_SENSITIVITY_FALL_LOW:
    break;
  case EXTI_SENSITIVITY_RISE_ONLY:
    break;
  case EXTI_SENSITIVITY_FALL_ONLY:
      //--- 外部中断处理 ----
      //置中断标志
      //ExiFlag = True;    
      break;
  case EXTI_SENSITIVITY_RISE_FALL:
    break;
    default:break;
  } 
  
}

void loop()
{
      //间隔时间刷新屏幕
      if((systemRunTime - LastSystemRunTime > 3000) || (LastSystemRunTime == 0))
          {
            LastSystemRunTime = systemRunTime;
            
            //暂时关闭充电
            TIM1_SetCompare3(0);
            //暂时停止放电
            EN_OUT1A_LOW;
            EN_OUT2A_LOW;
            DIS_Delayms(10);
            u16_ADC1_value = getBattVolumeAD(); //读取电量AD
            AD_Voltage = 3.298f*u16_ADC1_value/1024*1000;
            battVoltage = accordingADgetBatt(AD_Voltage);
            //恢复PWM充电
            TIM1_SetCompare3(pwmOut);
             //恢复放电
            EN_OUT1A_HIGH;
            EN_OUT2A_HIGH;
            
            if(battVoltage>=3440)
            {
              battVoltageMHAPercent = 100.0f*(battVoltage-3440)/(4150-3440);
            }
            else
            {
              battVoltageMHAPercent = 0;
            }
            
            //更新电量显示
            battVoltageMHA = 156.0f*battVoltageMHAPercent;
            updateUIBattVolume(battVoltageMHA);

            //更新电量百分比和电量图形
            updateUIBattVolumePercent(battVoltageMHAPercent);
            
          }
          
          //检测充电，放电
          if(systemRunTime - overCurrentCheckTime > 500)
          {
              overCurrentCheckTime = systemRunTime;
              
                //读取OVERCURRENT，PB1,USB1
                Driver_ADCON(ADC1_CHANNEL_1);
                DIS_Delayms(10);
                // 开始转换
                ADC1_StartConversion();
                DIS_Delayms(10);
                u16_ADC1_valueSum = 0;
                checkOut1A = 0;
                //读ADC值
                for(i=0;i<20;i++)
                {
                  u16_ADC1_valueSum += ADC1_GetConversionValue();
                }
                u16_ADC1_value = u16_ADC1_valueSum /20;
                if(u16_ADC1_value > 0)  //标志有输出
                {
                    checkOut1A = 1;
                }
                else
                {
                    checkOut1A = 0;
                }
                
                
                //读取OVERCURRENT1,PB0，USB3
                Driver_ADCON(ADC1_CHANNEL_0);
                DIS_Delayms(10);
                // 开始转换
                ADC1_StartConversion();
                DIS_Delayms(10);
                u16_ADC1_valueSum = 0;
                checkOut2A = 0;
                //读ADC值
                for(i=0;i<20;i++)
                {
                  u16_ADC1_valueSum += ADC1_GetConversionValue();
                }
                u16_ADC1_value = u16_ADC1_valueSum /20;
                if(u16_ADC1_value > 0)  //标志有输出
                {
                  checkOut2A = 1;
                }
                else
                {
                  checkOut2A = 0;
                }
                
                //读取charge状态,PB2
                Driver_ADCON(ADC1_CHANNEL_2);
                DIS_Delayms(10);
                // 开始转换
                ADC1_StartConversion();
                DIS_Delayms(10);
                u16_ADC1_valueSum = 0;
                
                //读ADC值
                for(i=0;i<20;i++)
                {
                  u16_ADC1_valueSum += ADC1_GetConversionValue();
                }
                u16_ADC1_value = u16_ADC1_valueSum /20;
                if(u16_ADC1_value > 0)  //标志有输出
                {
                  checkIN = 1;
                }
                else
                {
                  checkIN = 0;
                }
                
                
             // bit_status = getInputStatus();
                 
              if((checkOut2A == 1)||(checkOut1A == 1))
              {
                  //display_string_16x24(3,0,"$",custome,black);
//                  if((checkOut2A == 1)&&(checkOut1A == 1))
//                  {
//                     updateUIOutputVolume(4950);
//                  }
//                  else 
                  if(checkOut2A == 1)
                  {
                     //通过电池电压，获取放电电流
                     sOutputPutCurrent = getOutputCurrent2(battVoltage);
                     updateUIOutputVolume(sOutputPutCurrent);
                  }
                  else if(checkOut1A == 1)
                  {
                     //通过电池电压，获取放电电流
                     sOutputPutCurrent = getOutputCurrent(battVoltage);
                     updateUIOutputVolume(sOutputPutCurrent);
                  }
              }
              else //if(checkIN == 1)             //显示输入电流
              {
                  display_string_16x24(3,0,"IN ",custome,black);
                
                  //通过电池电压，获取充电电流
                  sInputPutCurrent = getInputCurrent(battVoltage);
                  //if(sInputPutCurrent != -1)
                  { 
                      updateUIInputVolume(sInputPutCurrent);
                  }
                  
              }
               
//              else if(((checkOut2A == 0)&&(checkOut1A == 0))&&(bit_status == RESET))
//              {
//                display_string_16x24(2,0,"        ",custome,black);
//              }
             
                   
              //充电PWM控制
              pwmOut = checkBattVoltage(battVoltage);
              //pwmOut = 1000;
              TIM1_SetCompare3(pwmOut);
              //TIM1_SetCompare3(1000);
          }
 
            //处理按键
            DealPressKey();
          
          //一段时间后，关屏背光和关屏
            if(IdleTimeMs >= 60000)
            {
              IdleTimeMs = 0;
              CloseScreen();
            }
}



void main(void)
{
        //全速执行      
        CLK_SYSCLKConfig(CLK_PRESCALER_HSIDIV1);
        
        //开启外部中断
	InitKey();
	//开总中断
        enableInterrupts(); 	/* 开启总中断 */
        
        // Halt_OffDevice();//停机前关闭不需要的外设
        halt();//进入停机模式
        //System_Init();//系统初始化函数
        Setup();
	while(1)
	{
            loop();
	}
}
		


#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
