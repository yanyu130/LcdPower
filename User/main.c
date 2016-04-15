
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

//short curBattVolume = 0;        //��ǰ����UIֵ
//int curBattVolumePercent = 0;   //��ǰ����UIֵ���ٷֱ�
short curInputVolumeIndex = -1;  //�������
short curOutputVolumeIndex = -1; //�����ѹ

bool fPowerOn_flag = FALSE;

unsigned int  IdleTimeMs = 0;

double LastSystemRunTime = 0;
//double LastCheckEN_OUT_time = 0;
double overCurrentCheckTime = 0;
u8 bPermit_OUT = 0;  //��ǣ��������


uint16_t u16_ADC1_value = 0,u16_ADC1_valueSum = 0;
short sInputPutCurrent = 0;
short sOutputPutCurrent = 0;
int pwmOut = 0;
            
u8 checkOut1A=0,checkOut2A=0;
u8 checkIN = 0;
    
u8 i;
short AD_Voltage = 0;     //�����ѹ
short battVoltage = 0;
short battVoltageMHA = 0;
int battVoltageMHAPercent = 0;
          
BitStatus bit_status;

/* Private defines -----------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/


/********************************************************************************************************
*  Function: Driver_ADCON						     
*  Object: ����ADCģ�� ִ�г�ʼ��      
*  ���룺 ��
*  ����� ��								       	 
*  ��ע�� ��                
********************************************************************************************************/
void Driver_ADCON(ADC1_Channel_TypeDef channel)
{       
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_ADC, ENABLE);
  ADC1_DeInit();
  //����ת����ͨ��3��ϵͳʱ�ӣ�8 ��Ƶ���ر��ⲿ�����¼��������Ҷ��룬�ر�ʩ���ش�����
  ADC1_Init(ADC1_CONVERSIONMODE_SINGLE, channel, ADC1_PRESSEL_FCPU_D8, ADC1_EXTTRIG_TIM, 
    DISABLE, ADC1_ALIGN_RIGHT, ADC1_SCHMITTTRIG_ALL, DISABLE);
  ADC1_ITConfig(ADC1_IT_AWDIE, DISABLE);  //���Ź��жϹر�
  ADC1_ITConfig(ADC1_IT_EOCIE, DISABLE);
  ADC1_Cmd(ENABLE);//����ADC1
}


//��ʱ��2��ʼ��
void Driver_TimeON(void)
{		
        //16Mʱ�� �趨����ʱ�ӵķ�Ƶ,��������Ϊ1US,1000�Σ���1ms�ж�һ��
        //��ʼ��������ֵ,�趨�Զ����ؼĴ���ֵ    
  //CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2, ENABLE);
        TIM2_TimeBaseInit(TIM2_PRESCALER_16,1000);
        TIM2_SetCounter(0x0000);//���������ĳ�ʼֵ����Ϊ0
        TIM2_ITConfig(TIM2_IT_UPDATE, ENABLE);//�����������ж�
        TIM2_Cmd(ENABLE);//ʹ��TMI2
}

#pragma vector=13+2
__interrupt void TIM2_IRQ()
{
 // static bool flag = TRUE;
 TIM2_ClearITPendingBit(TIM2_IT_UPDATE);

        //---------------- �жϴ��� ---------------------
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
        int temp = n < 0 ? -n: n;  // tempΪn�ľ���ֵ
   
       if (str == NULL)
       {
           return;
       }
       while(temp)
       {
           buf[i++] = (temp % 10) + '0';  //��temp��ÿһλ�ϵ�������buf
           temp = temp / 10;
       }
   
       len = n < 0 ? ++i: i;  //���n�Ǹ����������Ҫһλ���洢����
       str[i] = 0;            //ĩβ�ǽ�����0
       while(1)
       {
           i--;
           if (buf[len-i-1] ==0)
           {
               break;
           }
           str[i] = buf[len-i-1];  //��buf��������ַ������ַ���
       }
       if (i == 0 ) 
       {
           str[i] = '-';          //����Ǹ��������һ������
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
*  Object: ����EEPROM
*  ���룺 ��
*  ����� �� 	 
*  ��ע:  ��
********************************************************************************************************/
void Driver_FlashUNLock(void)
{
                FLASH_Unlock(FLASH_MEMTYPE_DATA);   //����EEPROM            
}


/********************************************************************************************************
*  Function: Driver_FlashLock				     
*  Object: ����EEPROM
*  ���룺 ��
*  ����� �� 	 
*  ��ע:  ��
********************************************************************************************************/
void Driver_FlashLock(void)
{
                FLASH_Lock(FLASH_MEMTYPE_DATA);//����EEPROM
       
}
/********************************************************************************************************
*  Function: Driver_EPWrite				     
*  Object: дEEPROM
*  ���룺 ͷ��ַ(0~1535)  ����ָ��  ���ݳ�
*  ����� �� 	 
*  ��ע: 1.5K EEPROM  ���ܳ���
********************************************************************************************************/
void Driver_EPWrite(uint32_t Adr, u8 *pData, u16 Len)
{
		uint32_t *p;
		//pָ��ָ��EEPROM ��Ӧ�ĵ�Ԫ
		p = (uint32_t*)0x4000 + Adr;   
		//����
		Driver_FlashUNLock();
		//д����
		for( ; Len > ZERO; Len--)
		{              
              FLASH_ProgramByte((uint32_t)p, *pData);
              p++;
              pData++;
		} 
		//����
		Driver_FlashLock();
}

/********************************************************************************************************
*  Function: Driver_EPRead				     
*  Object: ��EEPROM
*  ���룺 ͷ��ַ(0~1535) ���ݴ��ָ�� ���ݳ�
*  ����� �� 	 
*  ��ע: 1.5K EEPROM
********************************************************************************************************/
void Driver_EPRead(uint32_t Adr, u8 *pData, u16 Len)
{
		uint32_t *p;
		//pָ��ָ��EEPROM ��Ӧ�ĵ�Ԫ
		p = (uint32_t*)0x4000 + Adr; 
		//����
		Driver_FlashUNLock();
		//������
		for( ; Len > ZERO; Len--)
                {
			*pData++ = FLASH_ReadByte((uint32_t)p);
      p++;
                }
		//����
		Driver_FlashLock();
}

//PWM��ʼ��
void Driver_PWMON(void)
{ 
     CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER1, ENABLE);
     //TIM1_DeInit();
     //16 MHz/16 ��Ƶ= 1 �׺ա����ϼ����� 1 MHz /1000 =���� 1000
     TIM1_TimeBaseInit( 15, TIM1_COUNTERMODE_UP, 1000, 0 );
     //��ʼ������/�Ƚ�ͨ��1��PWMģʽ1����������Ƚϣ���
     TIM1_OC3Init( TIM1_OCMODE_PWM1, TIM1_OUTPUTSTATE_ENABLE,
    //���û�������Ƚ�1������0%������Ƚϼ��Ըߵ�ƽ��
    TIM1_OUTPUTNSTATE_ENABLE, 0, TIM1_OCPOLARITY_HIGH,
    //��������Ƚϼ��Ըߵ�ƽ������ȽϿ���״̬��λ����������ȽϿ���״̬��//λ��
    TIM1_OCNPOLARITY_HIGH, TIM1_OCIDLESTATE_RESET, TIM1_OCNIDLESTATE_SET );
      //���ö�ʱ��
     TIM1_Cmd(ENABLE);
            //���������      �ƶ� TIM1 ��ʱ����ѡ�� PWM �����
     TIM1_CtrlPWMOutputs(ENABLE);
}

//����磬������Ӧ����
int checkBattVoltage(short battVoltage)
{
  int pwmOut = 0;
  BitStatus bit_status;
  //GPIO ��������
  //GPIO_Init(GPIOD,GPIO_PIN_5,GPIO_MODE_IN_FL_NO_IT);
  //��ȡ״̬
  bit_status = GPIO_ReadInputPin(GPIOD, GPIO_PIN_5);
  if(bit_status != RESET)
  {
      LighScreen();
      
      //��缤��
//      bPermit_OUT = 0;
//      Driver_EPWrite(0, &bPermit_OUT, 1);
//      EN_OUT1A_HIGH;
//      EN_OUT2A_HIGH;
      
      //���������ı���PWM����
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

//��ȡ����AD
uint16_t getBattVolumeAD()
{
    uint16_t u16_ADC1_value = 0, u16_ADC1_valueSum = 0;
    u8 i;
     //��ADC����,��ȡ����
     Driver_ADCON(ADC1_CHANNEL_3);
     DIS_Delayms(10);
     // ��ʼת��
    ADC1_StartConversion();
    DIS_Delayms(10);
    //��ADCֵ
    for(i=0;i<20;i++)
    {
       u16_ADC1_valueSum += ADC1_GetConversionValue();
    }
     u16_ADC1_value = u16_ADC1_valueSum /20;
     return u16_ADC1_value;
}

//��õ���ͼ���ַ��±�
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

//���µ���ֵUI
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

//���µ���ֵ�ٷֱ�UI
void updateUIBattVolumePercent(int battVoltageMHAPercent)
{
  static int curBattVolumePercent = -1;
  char strADPercent[10];
  char strPatternIndex[2];  //��ŵ���ͼ����������
  strPatternIndex[1] = '\0';
  if(curBattVolumePercent == battVoltageMHAPercent)
  {
    return;
  }
  curBattVolumePercent = battVoltageMHAPercent;
  
  //��ʾ�ٷֱ�   
  if(battVoltageMHAPercent > 0)
  {
    int2str(battVoltageMHAPercent,strADPercent);
    strcat(strADPercent,"% ");
    display_string_16x24(1,1,strADPercent,custome,black);
              
    //��ʾ������ͼ��
    getVolumePatternIndex(battVoltageMHAPercent, strPatternIndex);
    display_string_16x24(1,5,strPatternIndex,custome,black);
  }
  else
  {
      display_string_16x24(1,1,"0% ",custome,black);
      display_string_16x24(1,5,"#",custome,black);
  }
}

//�����������ֵ
void updateUIInputVolume(short InputVolumeIndex)
{
  curOutputVolumeIndex = -1;  //��ʼ�������ѹ��index
  
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

//���������ѹֵ
void updateUIOutputVolume(short OutputVolumeIndex)
{
  curInputVolumeIndex = -1;   //��ʼ�����������index
  
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

  //������
void DealPressKey()
{
  uchar key_state_temp;

            key_state_temp = key_read();
            if(key_state_temp == 1)
            {
               LED_Low;        //����1�Σ�����
               key_wait_time = 0;
               IdleTimeMs = 0;
               LighScreen();
            }
            else if(key_state_temp >= 2)
            {
                LED_High;        //��������1�Σ�����    
                key_wait_time = 0;
                IdleTimeMs = 0;
                LighScreen();
            }
}

void initLCD()
{
  //��ʼ��LCD
        LCD_GPIO_Config();
        
        LCD_init(); 

        Lcd_clear();
        BACK_LIGHT_ON;
}

//��ȡ����״̬
BitStatus getInputStatus()
{
  BitStatus bit_status;	  
  //GPIO ��������
  //GPIO_Init(GPIOD,GPIO_PIN_5,GPIO_MODE_IN_FL_NO_IT);
  //��ȡ״̬
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

//���طŵ����
//ͨ����ص�ѹ���ͷŵ�ĵ�ѹֵ���õ��ŵ����
short getOutputCurrent(short battVolume)
{
  uint16_t u16_ADC1_valueSum = 0, u16_ADC1_value = 0;
  short OutputCurrent = 0;
  u8 i;
  //��ȡcharge״̬,PB1
  Driver_ADCON(ADC1_CHANNEL_1);
  DIS_Delayms(10);
  // ��ʼת��
  ADC1_StartConversion();
  DIS_Delayms(10);
  u16_ADC1_valueSum = 0;
  
  //��ADCֵ
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

//���طŵ����
//ͨ����ص�ѹ���ͷŵ�ĵ�ѹֵ���õ��ŵ����
short getOutputCurrent2(short battVolume)
{
  uint16_t u16_ADC1_valueSum = 0, u16_ADC1_value = 0;
  short OutputCurrent = 0;
  u8 i;
  //��ȡcharge״̬,PB0
  Driver_ADCON(ADC1_CHANNEL_0);
  DIS_Delayms(10);
  // ��ʼת��
  ADC1_StartConversion();
  DIS_Delayms(10);
  u16_ADC1_valueSum = 0;
  
  //��ADCֵ
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

//���س�����
//ͨ����ص�ѹ���ͳ��ĵ�ѹֵ���õ�������
short getInputCurrent(short battVolume)
{
  uint16_t u16_ADC1_valueSum = 0, u16_ADC1_value = 0;
  short InputCurrent = 0;
  u8 i;
  //��ȡcharge״̬,PB2
  Driver_ADCON(ADC1_CHANNEL_2);
  DIS_Delayms(10);
  // ��ʼת��
  ADC1_StartConversion();
  DIS_Delayms(10);
  u16_ADC1_valueSum = 0;
  
  //��ADCֵ
  for(i=0;i<20;i++)
  {
    u16_ADC1_valueSum += ADC1_GetConversionValue();
  }
  u16_ADC1_value = u16_ADC1_valueSum /20;
  
  //battVolume = 3333;
  //u16_ADC1_value = 25;
  
//  if(u16_ADC1_value < calInputVol(64))  //Ҫ��64MV����
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
        //����ŵ�IO��
        GPIO_Init(GPIOA,GPIO_PIN_1,GPIO_MODE_OUT_PP_HIGH_FAST); 
        GPIO_Init(GPIOA,GPIO_PIN_2,GPIO_MODE_OUT_PP_HIGH_FAST);
        
        //��ʼ��������
        GPIO_Init(GPIOD,GPIO_PIN_5,GPIO_MODE_IN_FL_NO_IT);
        //����ŵ�
        EN_OUT1A_HIGH;
        EN_OUT2A_HIGH;
 
        //��ʼ��LCD
        initLCD();
        
	//������ʱ��1ms�ж�
	Driver_TimeON();
        
        //�ŵ�ռ�ձ�
        Driver_PWMON();
        TIM1_SetCompare3(0);
        
        enableInterrupts(); 	//* �������ж� */
        
        //��ʼ��LED
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
//����ֵ��
//EXTI_SENSITIVITY_FALL_LOW �½��غ͵͵�ƽ����
//EXTI_SENSITIVITY_RISE_ONLY �������ش���
//EXTI_SENSITIVITY_FALL_ONLY ���½��ش���
//EXTI_SENSITIVITY_RISE_FALL �����غ��½��ش���  
  switch(sensitivity)
  {
  case EXTI_SENSITIVITY_FALL_LOW:
    break;
  case EXTI_SENSITIVITY_RISE_ONLY:
    break;
  case EXTI_SENSITIVITY_FALL_ONLY:
      //--- �ⲿ�жϴ��� ----
      //���жϱ�־
      //ExiFlag = True;    
      break;
  case EXTI_SENSITIVITY_RISE_FALL:
    break;
    default:break;
  } 
  
}

void loop()
{
      //���ʱ��ˢ����Ļ
      if((systemRunTime - LastSystemRunTime > 3000) || (LastSystemRunTime == 0))
          {
            LastSystemRunTime = systemRunTime;
            
            //��ʱ�رճ��
            TIM1_SetCompare3(0);
            //��ʱֹͣ�ŵ�
            EN_OUT1A_LOW;
            EN_OUT2A_LOW;
            DIS_Delayms(10);
            u16_ADC1_value = getBattVolumeAD(); //��ȡ����AD
            AD_Voltage = 3.298f*u16_ADC1_value/1024*1000;
            battVoltage = accordingADgetBatt(AD_Voltage);
            //�ָ�PWM���
            TIM1_SetCompare3(pwmOut);
             //�ָ��ŵ�
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
            
            //���µ�����ʾ
            battVoltageMHA = 156.0f*battVoltageMHAPercent;
            updateUIBattVolume(battVoltageMHA);

            //���µ����ٷֱȺ͵���ͼ��
            updateUIBattVolumePercent(battVoltageMHAPercent);
            
          }
          
          //����磬�ŵ�
          if(systemRunTime - overCurrentCheckTime > 500)
          {
              overCurrentCheckTime = systemRunTime;
              
                //��ȡOVERCURRENT��PB1,USB1
                Driver_ADCON(ADC1_CHANNEL_1);
                DIS_Delayms(10);
                // ��ʼת��
                ADC1_StartConversion();
                DIS_Delayms(10);
                u16_ADC1_valueSum = 0;
                checkOut1A = 0;
                //��ADCֵ
                for(i=0;i<20;i++)
                {
                  u16_ADC1_valueSum += ADC1_GetConversionValue();
                }
                u16_ADC1_value = u16_ADC1_valueSum /20;
                if(u16_ADC1_value > 0)  //��־�����
                {
                    checkOut1A = 1;
                }
                else
                {
                    checkOut1A = 0;
                }
                
                
                //��ȡOVERCURRENT1,PB0��USB3
                Driver_ADCON(ADC1_CHANNEL_0);
                DIS_Delayms(10);
                // ��ʼת��
                ADC1_StartConversion();
                DIS_Delayms(10);
                u16_ADC1_valueSum = 0;
                checkOut2A = 0;
                //��ADCֵ
                for(i=0;i<20;i++)
                {
                  u16_ADC1_valueSum += ADC1_GetConversionValue();
                }
                u16_ADC1_value = u16_ADC1_valueSum /20;
                if(u16_ADC1_value > 0)  //��־�����
                {
                  checkOut2A = 1;
                }
                else
                {
                  checkOut2A = 0;
                }
                
                //��ȡcharge״̬,PB2
                Driver_ADCON(ADC1_CHANNEL_2);
                DIS_Delayms(10);
                // ��ʼת��
                ADC1_StartConversion();
                DIS_Delayms(10);
                u16_ADC1_valueSum = 0;
                
                //��ADCֵ
                for(i=0;i<20;i++)
                {
                  u16_ADC1_valueSum += ADC1_GetConversionValue();
                }
                u16_ADC1_value = u16_ADC1_valueSum /20;
                if(u16_ADC1_value > 0)  //��־�����
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
                     //ͨ����ص�ѹ����ȡ�ŵ����
                     sOutputPutCurrent = getOutputCurrent2(battVoltage);
                     updateUIOutputVolume(sOutputPutCurrent);
                  }
                  else if(checkOut1A == 1)
                  {
                     //ͨ����ص�ѹ����ȡ�ŵ����
                     sOutputPutCurrent = getOutputCurrent(battVoltage);
                     updateUIOutputVolume(sOutputPutCurrent);
                  }
              }
              else //if(checkIN == 1)             //��ʾ�������
              {
                  display_string_16x24(3,0,"IN ",custome,black);
                
                  //ͨ����ص�ѹ����ȡ������
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
             
                   
              //���PWM����
              pwmOut = checkBattVoltage(battVoltage);
              //pwmOut = 1000;
              TIM1_SetCompare3(pwmOut);
              //TIM1_SetCompare3(1000);
          }
 
            //������
            DealPressKey();
          
          //һ��ʱ��󣬹�������͹���
            if(IdleTimeMs >= 60000)
            {
              IdleTimeMs = 0;
              CloseScreen();
            }
}



void main(void)
{
        //ȫ��ִ��      
        CLK_SYSCLKConfig(CLK_PRESCALER_HSIDIV1);
        
        //�����ⲿ�ж�
	InitKey();
	//�����ж�
        enableInterrupts(); 	/* �������ж� */
        
        // Halt_OffDevice();//ͣ��ǰ�رղ���Ҫ������
        halt();//����ͣ��ģʽ
        //System_Init();//ϵͳ��ʼ������
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
