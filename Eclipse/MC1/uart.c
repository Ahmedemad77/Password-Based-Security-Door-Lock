 /******************************************************************************
 *
 * Module: UART
 *
 * File Name: uart.c
 *
 * Description: Source file for the UART AVR driver
 *
 * Author: Mohamed Tarek
 *
 *******************************************************************************/

#include "uart.h"

/*******************************************************************************
 *                      Preprocessor Macros                                    *
 *******************************************************************************/

#define BAUD_PRESCALE(USART_BAUDRATE) (((F_CPU / ((USART_BAUDRATE) * 8UL))) - 1)


/*******************************************************************************
 *                            GLOBAL VARIABLES                    *
 *******************************************************************************/

static volatile void (*g_callBackPtrUart)(void) = NULL_PTR;

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

void UART_init(UartConfigType * uartConfig_Ptr ){


	/* configure RX as an input pin */
	CLEAR_BIT(DDRD,PD0);
	/* configure TX as an output pin */
	SET_BIT(DDRD,PD1);

	/*setting UCSRA register */
	/* set U2X = 1 for double transmission speed in case of double speed mode */
	if(uartConfig_Ptr->Uart_OperatingMode ==ASYNCHRONOUS_DOUBLE_SPEED_MODE){
		UCSRA = (1<<U2X);
	}else{
		UCSRA = 0;
	}


	/*setting UCSRB register */
	/************************** UCSRB Description **************************
	 * RXCIE  USART RX Complete Interrupt Enable
	 * TXCIE  USART Tx Complete Interrupt Enable
	 * UDRIE  USART Data Register Empty Interrupt Enable
	 * RXEN  = 1 Receiver Enable
	 * RXEN  = 1 Transmitter Enable
	 * UCSZ2 = 1  only For 9-bit data mode
	 * RXB8 & TXB8 only for 9-bit data mode
	 ***********************************************************************/
	UCSRB= (uartConfig_Ptr->UART_RxInterrupt)<<RXCIE |(uartConfig_Ptr->UART_TxInterrupt)<<TXCIE
			| (uartConfig_Ptr->UART_UdrEmptyInterrut)<<UDRIE |(1<<RXEN) | (1<<TXEN);
	/* UCSZ2 = 1  only For 9-bit data mode */
	if(uartConfig_Ptr->UART_dataBitsNum==9){
		UCSRB |=(1<<UCSZ2);
	}



	/*setting UCSRC register */

	/*setting URSEL = 1 to enable writing
	 *set number of stop bits according to configuration */
	if (uartConfig_Ptr->UART_stopBitsNum==2){
		UCSRC  =(1<<URSEL) |(1<<USBS) ;
	}else{
		UCSRC  =(1<<URSEL) ;
	}

	/*adjusting UCSZ1:0 to match the data width */
	switch (uartConfig_Ptr->UART_dataBitsNum) {
		case 5:
		case 6:
		case 7:
		case 8:
			UCSRC |= (((uartConfig_Ptr->UART_dataBitsNum)-5)<<1) &0x06;
			break;
		case 9 :
			UCSRC |= (1<<UCSZ1)|(1<<UCSZ0);
			break;
		default:
			break;
	}

	/*setting parity bits UPM Bits */

	switch (uartConfig_Ptr->UART_ParityBitMode){
	case NO_PARITY :
		break;
	case EVEN_PARITY :
		UCSRC |= (1<<UPM1);
		break;
	case ODD_PARITY :
		UCSRC |= (1<<UPM1) | (1<<UPM0);
		break;
	}


	/*setting UBRR Register for the defined BAUDRATE*/

	if(uartConfig_Ptr->Uart_OperatingMode==ASYNCHRONOUS_NORMAL_MODE){
		/* First 8 bits from the BAUD_PRESCALE inside UBRRL and last 4 bits in UBRRH*/
		UBRRH = BAUD_PRESCALE(2*uartConfig_Ptr->Uart_baudRate)>>8;
		UBRRL = BAUD_PRESCALE(2*uartConfig_Ptr->Uart_baudRate);
	}else{
		/* First 8 bits from the BAUD_PRESCALE inside UBRRL and last 4 bits in UBRRH*/
		UBRRH = BAUD_PRESCALE(uartConfig_Ptr->Uart_baudRate)>>8;
		UBRRL = BAUD_PRESCALE(uartConfig_Ptr->Uart_baudRate);

	}


}

void UART_Deinit(void){
	UCSRB = 0;
	UDR  =0;

}

	
void UART_sendByte(const uint8 data)
{
	/* UDRE flag is set when the Tx buffer (UDR) is empty and ready for 
	 * transmitting a new byte so wait until this flag is set to one */
	while(BIT_IS_CLEAR(UCSRA,UDRE)){}
	/* Put the required data in the UDR register and it also clear the UDRE flag as 
	 * the UDR register is not empty now */	 
	UDR = data;
	/************************* Another Method *************************
	UDR = data;
	while(BIT_IS_CLEAR(UCSRA,TXC)){} // Wait until the transimission is complete TXC = 1
	SET_BIT(UCSRA,TXC); // Clear the TXC flag
	*******************************************************************/	


}

uint8 UART_recieveByte(void)
{
	/* RXC flag is set when the UART receive data so wait until this 
	 * flag is set to one */
	while(BIT_IS_CLEAR(UCSRA,RXC)){}
	/* Read the received data from the Rx buffer (UDR) and the RXC flag 
	   will be cleared after read this data */	 
    return UDR;		
}

void UART_sendString(const uint8 *Str)
{
	uint8 i = 0;
	while(Str[i] != '\0')
	{
		UART_sendByte(Str[i]);
		i++;
	}
	UART_sendByte('#');
	/************************* Another Method *************************
	while(*Str != '\0')
	{
		UART_sendByte(*Str);
		Str++;
	}		
	*******************************************************************/
}

void UART_receiveString(uint8 *Str)
{
	uint8 i = 0;
	Str[i] = UART_recieveByte();
	while(Str[i] != '#')
	{
		i++;
		Str[i] = UART_recieveByte();
	}
	Str[i] = '\0';
}

void UART_setCallBack(void(*a_ptr)(void)){
	g_callBackPtrUart = a_ptr ;
}





