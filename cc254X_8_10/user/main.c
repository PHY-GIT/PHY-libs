#define  OTP_VAR_GLOBALS
#include "user.h"

#define DISABLE_ALL_INTERRUPTS() (IEN0 = IEN1 = IEN2 = 0x00)       //三个



/****************************************************************************
* 变量初始化函数
****************************************************************************/
void sys_var_init(void)
{


}


/****************************************************************************
* 初始化系统时钟
****************************************************************************/
void InitClock(void)
{   
    DISABLE_ALL_INTERRUPTS();

    CLKCONCMD &= ~0x40;              //设置系统时钟源为 32MHZ晶振
    while(CLKCONSTA & 0x40);         //等待晶振稳定 
    CLKCONCMD &= ~0x47;              //设置系统主时钟频率为 32MHZ
}

/****************************************************************************
* 系统初始化函数
****************************************************************************/
void sys_init(void)
{   
    //InitClock();             //设置系统时钟源为 32MHZ晶振
	InitLed();               //设置LED灯相关IO口
	InitKey();               //设置按键相关IO口
	//Timer1_Init();           //定时器1
	//Timer3_Init();           //定时器1
	//Uart0_Init();            //串口初始化
    //InitSensor();            //传感器初始化
    //sys_var_init();
}

/****************************************************************************
* 主程序入口函数
****************************************************************************/
void main(void)
{
    sys_init();              //系统初始化 
    
    while(1){
        for (uchar i=0; i<6; i++)  //LED1闪烁3次提醒用户将进入睡眠模式
        {
            LED1 = ~LED1;
            DelayMS(500);
        }

        SysPowerMode(3);     //进入睡眠模式PM3,按下按键S1中断唤醒系统
    }
}




