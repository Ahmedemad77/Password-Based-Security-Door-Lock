 /******************************************************************************
 *
 * Module: UART
 *
 * File Name: uart.h
 *
 * Description: Header file for the UART AVR driver
 *
 * Author: Mohamed Tarek
 *
 *******************************************************************************/

#ifndef UART_H_
#define UART_H_

#include "micro_config.h"
#include "std_types.h"
#include "common_macros.h"


/*******************************************************************************
 *                      Preprocessor Macros                                    *
 *******************************************************************************/

/* UART Driver Baud Rate */
//#define USART_BAUDRATE 9600

/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/
typedef enum {
	ASYNCHRONOUS_NORMAL_MODE, ASYNCHRONOUS_DOUBLE_SPEED_MODE
}UartOperatingMode;

typedef enum {
	NO_PARITY,EVEN_PARITY,ODD_PARITY
}UartParityBitMode;


typedef struct{
	uint16 Uart_baudRate;
	UartOperatingMode Uart_OperatingMode;
	uint8 UART_dataBitsNum;
	uint8 UART_stopBitsNum;
	UartParityBitMode UART_ParityBitMode;
	uint8 UART_RxInterrupt;
	uint8 UART_TxInterrupt;
	uint8 UART_UdrEmptyInterrut;
}UartConfigType;


/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

void UART_init(UartConfigType * uartConfig_Ptr );

void UART_Deinit(void);

void UART_setCallBack(void(*a_ptr)(void));

void UART_sendByte(const uint8 data);

uint8 UART_recieveByte(void);

void UART_sendString(const uint8 *Str);

void UART_receiveString(uint8 *Str); // Receive until #

#endif /* UART_H_ */
