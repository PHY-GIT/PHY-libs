#include "key.h"
#include "led.h"
#include "hold.h"

/****************************************************************************
* 名    称: InitKey()
* 功    能: 设置按键相应的IO口,采用中断方式
* 入口参数: 无
* 出口参数: 无
****************************************************************************/
void InitKey(void)
{
    P0IEN |= 0x2; // P0.1 设置为中断方式 1：中断使能
    PICTL |= 0x1; //下降沿触发
    IEN1 |= 0x20; //允许 P0 口中断;
    P0IFG &= ~BIT(1); //初始化中断标志位
    EA= 1;        //打开总中断
}

/****************************************************************************
* 名    称: //KeyScan()
* 功    能: 读取按键状态
* 入口参数: 无
* 出口参数: 0为抬起   1为按键按下
****************************************************************************/
bool key_scan(void)
{
    if(KEY == 0)
    {
        DelayMS(10);      //延时10MS去抖
        if(KEY == 0)
        {
            while(!KEY); //松手检测
            return 1;     //有按键按下
        }
    }
    
    return 0;             //无按键按下
}

/****************************************************************************
* 名 称: P0_ISR(void) 中断处理函数
* 描 述: #pragma vector = 中断向量，紧接着是中断处理程序
****************************************************************************/
#pragma vector = P0INT_VECTOR
__interrupt void P0_ISR(void)
{
    P0IFG = 0;    //清中断标志
    P0IF = 0;     //清中断标志
    SysPowerMode(4);         //正常工作模式
}



