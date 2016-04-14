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

#define red    0xf800	//定义红色
#define blue   0x001f	//定义蓝色
#define green  0x07e0 	//定义绿色
#define deep_green  0x0600 	//定义深绿色
#define white  0xffff 	//定义白色	
#define black  0x0000 	//定义黑色	
#define orange 0xfc08 	//定义橙色
#define yellow 0xffe0	//定义黄色
#define pink   0xf3f3 	//定义粉红色
#define purple 0xa1d6	 //定义紫色
#define brown  0x8200	 //定义棕色
#define gray   0xc618 //定义灰色
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
//显示全屏单一色彩
extern void display_color(int XS,int YS,int x_total,int y_total,int color);
//定义窗口坐标：开始坐标（XS,YS)以及窗口大小（x_total,y_total)
extern void lcd_address(int XS,int YS,int x_total,int y_total);
//传送8位的数据
extern void data_out(uchar data1);

//传16位数据，16位数据一起赋值
extern void data_out_16(int data_16bit);


/******************************************************************
函数功能：显示ASCII字符串
******************************************************************/
extern void display_string_16x32(u16 page,u16 column,char *text,int font_color,int back_color);
extern void display_string_16x24(u16 page,u16 column,char *text,int font_color,int back_color);
extern void display_string_8x24(u16 page,u16 column,char *text,int font_color,int back_color);

//将单色的8位的数据（代表8个像素点）转换成彩色的数据传输给液晶屏
extern void mono_data_out(u8 mono_data,int font_color,int back_color);

extern void LCD_GPIO_Config(void);

extern void CloseScreen();

extern void LighScreen();

#endif