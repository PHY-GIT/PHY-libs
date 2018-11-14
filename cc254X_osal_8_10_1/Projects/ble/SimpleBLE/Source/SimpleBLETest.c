/**************************************************************************************************
  Filename:       SimpleBLETest.c
  Revised:        $Date: 2010-08-06 08:56:11 -0700 (Fri, 06 Aug 2010) $
  Revision:       $Revision: 23333 $

  Description:    This file contains the Simple BLE Peripheral sample application
                  for use with the CC2540 Bluetooth Low Energy Protocol Stack.

  Copyright 2010 - 2013 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED �AS IS?WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
**************************************************************************************************/

/*********************************************************************
 * INCLUDES
 */

#include "bcomdef.h"
#include "OSAL.h"
#include "OSAL_PwrMgr.h"

#include "OnBoard.h"
#include "hal_adc.h"
#include "hal_led.h"
#include "hal_key.h"
#include "hal_lcd.h"
#include "simpleBLETest.h"
#include "npi.h"
#include "stdio.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */


/*********************************************************************
 * LOCAL VARIABLES
 */
static uint8 SimpleBLETest_TaskID;   // Task ID for internal task/event processing

/*********************************************************************
 * PUBLIC FUNCTIONS
 */
static void NpiSerialCallback( uint8 port, uint8 events );

/*********************************************************************
 * @fn      SimpleBLETest_Init
 *
 * @brief   Initialization function for the Simple BLE Peripheral App Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notificaiton ... ).
 *
 * @param   task_id - the ID assigned by OSAL.  This ID should be
 *                    used to send messages and set timers.
 *
 * @return  none
 */
void SimpleBLETest_Init( uint8 task_id )
{ //��������id��ȫ�ֱ���   
  SimpleBLETest_TaskID = task_id;       

  // ���ڳ�ʼ�� ������Ĭ����115200, �β��ǻص�����
  NPI_InitTransport(NpiSerialCallback);

  // ���������
  NPI_WriteTransport("��SimpleBLETest_Init\r\n", 20);

  // ���ַ������
  NPI_PrintString("SimpleBLETest_Init2\r\n");  

  // �������һ��ֵ����10���Ʊ�ʾ
  NPI_PrintValue("AmoMcu 1 = ", 168, 10);
  NPI_PrintString("\r\n");  
  
  // �������һ��ֵ����16���Ʊ�ʾ
  NPI_PrintValue("AmoMcu 2 = 0x", 0x88, 16);
  NPI_PrintString("\r\n");  
  
  HalLcdWriteString ( "SimpleBLETest 07", HAL_LCD_LINE_1);
  // Setup a delayed profile startup  
  /*
  ����һ������ ��ô����Ŀ���ǰ��ն��������ķ�������
  SimpleBLETest_ProcessEvent ���Ǵ��� SBP_START_DEVICE_EVT
  */
  osal_set_event( SimpleBLETest_TaskID, SBP_START_DEVICE_EVT );
}

/*********************************************************************
 * @fn      SimpleBLETest_ProcessEvent
 *
 * @brief   Simple BLE Peripheral Application Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id  - The OSAL assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  events not processed
 */
// ��������ǵ�Ӧ�ó�����¼��������� 
uint16 SimpleBLETest_ProcessEvent( uint8 task_id, uint16 events )
{

  VOID task_id; // OSAL required parameter that isn't used in this function

  // SYS_EVENT_MSG ����ϵͳ�¼����簴���¼�������д�¼�����������������¼�
  if ( events & SYS_EVENT_MSG )
  {
    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }

  // ���������Ӧ�ó����Զ�����¼���SBP_START_DEVICE_EVT ��ֵ������Ϊ 0x0001�� 
  // ʵ�������ǿ��Զ��� 16���¼��� ��һ��ʱ������λ�������
  // 
  if ( events & SBP_START_DEVICE_EVT )
  {
    HalLedSet(HAL_LED_1, HAL_LED_MODE_ON);      // ����led1               

    // ��������� ����osal�����ʵ�����Ѿ�������
    return ( events ^ SBP_START_DEVICE_EVT );   
  }

  // Discard unknown events
  return 0;
}

// ���ڻص������� ����Ѹûص�������ʵ�ֵĹ��ܽ���һ��
/*
1, ˼·:  �������յ����ݺ󣬾ͻ����ϵ������»ص���������ʵ�ʲ����з��֣��˻ص�
��������Ƶ���� ����㲻ִ��NPI_ReadTransport�������ж�ȡ�� ��ô����ص������ͻ�
Ƶ���ر�ִ�У����ǣ���ͨ�����ڷ���һ�����ݣ� �㱾�����봦����һ����һ�ε����ݣ����ԣ�
����������������ʱ��Ĵ��������� Ҳ�����յ����ݹ�����߳�ʱ���Ͷ�ȡһ�����ݣ� 
Ȼ����ݵ�ǰ��״̬����ִ�У����û�������ϣ��Ͱ��������ݵ���AT������� �������
���ˣ��Ͱ������͵��Զˡ�  ---------------amomcu   2014.08.17
*/
static void NpiSerialCallback( uint8 port, uint8 events )
{
    (void)port;//�Ӹ� (void)����δ�˱������澯����ȷ���߻��������������������

    if (events & (HAL_UART_RX_TIMEOUT | HAL_UART_RX_FULL))   //����������
    {
        uint8 numBytes = 0;

        numBytes = NPI_RxBufLen();           //�������ڻ������ж����ֽ�
        
        if(numBytes == 0)
        {
            return;
        }
        else
        {
            //���뻺����buffer
            uint8 *buffer = osal_mem_alloc(numBytes);
            if(buffer)
            {
                //��ȡ��ȡ���ڻ��������ݣ��ͷŴ�������   
                NPI_ReadTransport(buffer,numBytes);   

                //���յ������ݷ��͵�����-ʵ�ֻػ� 
                NPI_WriteTransport(buffer, numBytes);  

                //�ͷ�����Ļ�����
                osal_mem_free(buffer);
            }
        }
    }
}


/*********************************************************************
*********************************************************************/