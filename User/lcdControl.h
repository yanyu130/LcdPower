#ifndef __LCDCONTROL_H
#define __LCDCONTROL_H

#include "prohead.h"
#include "stm8s.h"
#include "delay.h"

#define COLOR_TO_MTK_COLOR_SIMUL(color) ((((color) >> 19) & 0x1f) << 11) \
                                            |((((color) >> 10) & 0x3f) << 5) \
                                            |(((color) >> 3) & 0x1f)



#define _DEBUG

#define CS_High   GPIO_WriteHigh(GPIOC, GPIO_PIN_5)
#define CS_Low    GPIO_WriteLow(GPIOC, GPIO_PIN_5)

#define RST_High  GPIO_WriteHigh(GPIOC, GPIO_PIN_7)
#define RST_Low   GPIO_WriteLow(GPIOC, GPIO_PIN_7)

#define RS_High	  GPIO_WriteHigh(GPIOC, GPIO_PIN_6)
#define RS_Low    GPIO_WriteLow(GPIOC, GPIO_PIN_6)

#define WR_High  GPIO_WriteHigh(GPIOE, GPIO_PIN_5)
#define WR_Low   GPIO_WriteLow(GPIOE, GPIO_PIN_5)

#define RD_High  GPIO_WriteHigh(GPIOC, GPIO_PIN_1)
#define RD_Low   GPIO_WriteLow(GPIOC, GPIO_PIN_1)

#define BACK_LIGHT_ON GPIO_WriteLow(GPIOC, GPIO_PIN_2)
#define BACK_LIGHT_OFF GPIO_WriteHigh(GPIOC, GPIO_PIN_2)



#ifdef _DEBUG
  #define D1_Low  GPIO_WriteLow(GPIOD, GPIO_PIN_7)
  #define D1_High  GPIO_WriteHigh(GPIOD, GPIO_PIN_7)

  #define D2_High  GPIO_WriteHigh(GPIOD, GPIO_PIN_6)
  #define D2_Low  GPIO_WriteLow(GPIOD, GPIO_PIN_6)
 
#else

  #define D1_Low  GPIO_WriteLow(GPIOB, GPIO_PIN_7)
  #define D1_High  GPIO_WriteHigh(GPIOB, GPIO_PIN_7)

  #define D2_High  GPIO_WriteHigh(GPIOB, GPIO_PIN_6)
  #define D2_Low  GPIO_WriteLow(GPIOB, GPIO_PIN_6)
  
#endif

#define D0_High  GPIO_WriteHigh(GPIOF, GPIO_PIN_4)

#define D3_High  GPIO_WriteHigh(GPIOB, GPIO_PIN_5)
#define D4_High  GPIO_WriteHigh(GPIOB, GPIO_PIN_4)
#define D5_High  GPIO_WriteHigh(GPIOD, GPIO_PIN_0)
#define D6_High  GPIO_WriteHigh(GPIOD, GPIO_PIN_2)
#define D7_High  GPIO_WriteHigh(GPIOD, GPIO_PIN_3)

#define D0_Low  GPIO_WriteLow(GPIOF, GPIO_PIN_4)

#define D3_Low  GPIO_WriteLow(GPIOB, GPIO_PIN_5)
#define D4_Low  GPIO_WriteLow(GPIOB, GPIO_PIN_4)
#define D5_Low  GPIO_WriteLow(GPIOD, GPIO_PIN_0)
#define D6_Low  GPIO_WriteLow(GPIOD, GPIO_PIN_2)
#define D7_Low  GPIO_WriteLow(GPIOD, GPIO_PIN_3)

#define red    0xf800	//�����ɫ
#define blue   0x001f	//������ɫ
#define green  0x07e0 	//������ɫ
#define deep_green  0x0600 	//��������ɫ
#define white  0xffff 	//�����ɫ	
#define black  0x0000 	//�����ɫ	
#define orange 0xfc08 	//�����ɫ
#define yellow 0xffe0	//�����ɫ
#define pink   0xf3f3 	//����ۺ�ɫ
#define purple 0xa1d6	 //������ɫ
#define brown  0x8200	 //������ɫ
#define gray   0xc618 //�����ɫ
#define custome COLOR_TO_MTK_COLOR_SIMUL(0x19ffff)





extern  void LCD_init(void);

extern void disp_32x16(int XS,int YS,uint x,uint y,char *dp,uint font_color,uint back_color);

//extern void address_set(u8 x, u8 y, u8 ColumnLength, u8 RowLength);

//extern void main_Pattern(u8 x, u8 y, u8 ColumnLength, u8 RowLength,unsigned int jk,unsigned int jm);

extern void Lcd_clear();

extern void BIT8_Set_Bit(uchar index, uchar value);

extern void BIT8_Set_All(uchar data1);

extern void WriteData(uchar data1);
extern void WriteCom(uchar data1);
//��ʾȫ����һɫ��
extern void display_color(int XS,int YS,int x_total,int y_total,int color);
//���崰�����꣺��ʼ���꣨XS,YS)�Լ����ڴ�С��x_total,y_total)
extern void lcd_address(int XS,int YS,int x_total,int y_total);
//����8λ������
extern void data_out(uchar data1);

//��16λ���ݣ�16λ����һ��ֵ
extern void data_out_16(int data_16bit);


/******************************************************************
�������ܣ���ʾASCII�ַ���
******************************************************************/
extern void display_string_16x32(u16 page,u16 column,char *text,int font_color,int back_color);
extern void display_string_16x24(u16 page,u16 column,char *text,int font_color,int back_color);
extern void display_string_8x24(u16 page,u16 column,char *text,int font_color,int back_color);

//����ɫ��8λ�����ݣ�����8�����ص㣩ת���ɲ�ɫ�����ݴ����Һ����
extern void mono_data_out(u8 mono_data,int font_color,int back_color);

extern void LCD_GPIO_Config(void);

extern void CloseScreen();

extern void LighScreen();

#endif