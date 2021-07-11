/*
 * INTERFACING_MICRO.c
 *
 *  Created on: Dec 15, 2020
 *  Description: main  file for the MC1
 *      Author: Ahmed Emad
 */


#include "timers.h"
#include "uart.h"
#include "lcd.h"
#include "keypad.h"

/*******************************************************************************
 *                      Preprocessor Macros                                    *
 *******************************************************************************/

/*Constant value between the 2 micros to indicate that
 *the micro ready to receive information from UART*/

#define M_READY 0XFF

/*indicator if the function success or fails to do the task*/
#define SUCCESS 1
#define FAILURE 0

#define CORRECT_PASSWORD 0XCC
#define WRONG_PASSWORD   0XBB

/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/
/* declaring  the states of the system*/
typedef enum {
	NEW_PASSWORD,CHECK_PASSWORD_TO_LOG_IN,CHECK_PASSWORD_FOR_NEW_PASSWORD,VIEW_OPTIONS,OPENING_GATE,BUZZER_ON
}SystemState;

/*Options the user can choose from*/
typedef enum{
	OPEN_GATE_OPTION,CREATE_NEW_PASSWORD
}Options;

/*ENUM to hold gate state*/
typedef enum {
	CLOSED,GATE_OPENING,OPENED,GATE_CLOSING
}GateStatus;

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/
/*global variable to hold the system state*/
static uint8 g_systemState;


/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/*
 * Description : Function to get the password from user from keypad
 * 	[in]. string to hold the password
 */

void getPassword (uint8 * a_password_Ptr);

/*
 *  Description : Function to do the process of getting password
 * 	1. enter a password
 * 	2. re-enter the password to confirm it
 * 	3. check that are same
 * 	4. if they matched then send the password to micro1 to store it
 * 	   else repeat the process
 * 	function return flag indicating if process was success or not
 */
static uint8 createNewPassword(void);

/*
 *  Description : Function to view Options and  get option from user and send it to micro 1
 */
inline static void  viewOptions(void);

/*Description : Function to  get the status of the gate and display it on LCD*/
inline static void gateOpeningStatus(void);

/*function to get the password from the user
 * and send it to MICRO2 to check it    */
inline static void getAndSendPassword(void);


int main(){

	/*Initialize the LCD*/
	LCD_init();

	/*defining variable to hold the the configuration of  UART*/
	UartConfigType s_uartConfig ={9600,ASYNCHRONOUS_DOUBLE_SPEED_MODE,8,1,NO_PARITY,0,0,0 };
	/*initialize and configure the UART driver*/
	UART_init(&s_uartConfig);



	while(1){

		/*inform MC1 that micro ready to receive the system state*/
		UART_sendByte(M_READY);

		/*getting the system state*/
		g_systemState = UART_recieveByte();


		switch (g_systemState) {
			case NEW_PASSWORD:
				/*loop until the user enters a correct matched passwords*/
				while( createNewPassword() != SUCCESS){
					createNewPassword();
				}
				break;
			case CHECK_PASSWORD_FOR_NEW_PASSWORD :
			case CHECK_PASSWORD_TO_LOG_IN:
				getAndSendPassword();
				break;
			case VIEW_OPTIONS :
				viewOptions();
				break;
			case OPENING_GATE :
				gateOpeningStatus();
				break;
			case BUZZER_ON:
				LCD_clearScreen();
				LCD_displayString("thief!!!");
				break;
		}
	}


}

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/



/*
 * Description : Function to get the password from user from keypad
 * 	[in]. string to hold the password
 */
void getPassword (uint8 * a_password_Ptr){

	/*counter for current letter*/
	uint8 passCounter = 0;
	/*input letter from the user*/
	uint8 inputLetter;

	/*loop to get the six letters*/
	for (passCounter = 0; passCounter <= 6;) {

		inputLetter = KeyPad_getPressedKey();

		/*if the user  inputs the 6 letters and then enter  then return
		 *13 is ASCI of enter key else continue loop until hits enter button */
		if(passCounter==6){
			if(inputLetter ==13){
				/*terminate the string with null*/
				a_password_Ptr[passCounter]='\0';
				return;
			}
		}else{
			/*store the input letter */
			a_password_Ptr[passCounter]=inputLetter;
			++passCounter;
			LCD_displayCharacter('*');
		}

	}

}

/*
 *  Description : Function to do the process of getting password
 * 	1. enter a password
 * 	2. re-enter the password to confirm it
 * 	3. check that are same
 * 	4. if they matched then send the password to micro1 to store it
 * 	   else repeat the process
 */
static uint8 createNewPassword(void){

	/*defining two strings to store the user input*/
	uint8 password[7];
	uint8 confirmPassword[7];

	LCD_clearScreen();
	LCD_displayString("EnterNewPASSWORD");
	LCD_goToRowColumn(1, 0);

	/*get  password */
	getPassword(password);

	LCD_clearScreen();
	LCD_displayString("CONFIRM PASSWORD");
	LCD_goToRowColumn(1, 0);


	/*get  password */
	getPassword(confirmPassword);

	/*checking for match*/
	for (int var = 0; var < 6; ++var) {

		if(password[var] != confirmPassword[var]){
			/*case passwords  not matched */
			LCD_clearScreen();
			LCD_displayString("PASSWORDS NOT");
			LCD_goToRowColumn(1, 0);
			LCD_displayString("MATCHED!tryAgain");
			_delay_ms(1000);
			return FAILURE;

		}
	}

	/*case of matching passwords*/
	LCD_clearScreen();
	LCD_displayString("SAVING PASSWORD");

	/*sending password to MC1 to save it as soon as MC1 is ready  */
	while(UART_recieveByte()!=M_READY);
	UART_sendString(password);
	LCD_clearScreen();
	LCD_displayString("PASSWORD SAVED");
	return SUCCESS;
}

/*
 *  Description : Function to get option from user and send it to micro 1
 */
inline static void  viewOptions(void){

	/*hold option from user*/
	uint8 option;

	/*view Options on the screen*/
	LCD_clearScreen();
	LCD_displayString("0-->OPEN GATE");
	LCD_goToRowColumn(1, 0);
	LCD_displayString("1-->NEW PASSWORD");

	/*wait until user enter the option*/
	do {
		option=KeyPad_getPressedKey();
	} while (option!=OPEN_GATE_OPTION&&option!=CREATE_NEW_PASSWORD);

	/*send the option to micro1*/
	while(UART_recieveByte()!=M_READY);
	UART_sendByte(option);
}

/*function to get the password from the user
 * and send it to MICRO2 to check it    */
inline static void getAndSendPassword(void){

	/*to hold user input*/
	uint8 userPassword[7];

	/*asks user for password*/
	LCD_clearScreen();
	LCD_displayString("ENTER PASSWORD");
	LCD_goToRowColumn(1, 0);

	/*get the password entered*/
	getPassword(userPassword);

	/*sending password to MC1 to check it as soon as MC1 is ready  */
	while(UART_recieveByte()!=M_READY);
	UART_sendString(userPassword);


	/*wait until micro check if it is right and send the result*/
	UART_sendByte(M_READY);
	if(UART_recieveByte() == WRONG_PASSWORD){
		LCD_clearScreen();
		LCD_displayString("WRONG PASSWORD!!");
		_delay_ms(1000);
	}
}


/*Description : Function to  get the status of the gate and display it on LCD*/
inline static void gateOpeningStatus(void){

	/*inform MC1 that micro ready to receive the system state*/
	UART_sendByte(M_READY);
	/*getting the gate state*/
	UART_recieveByte();

	LCD_clearScreen();
	LCD_displayString("UNLOCKING");

	/*inform MC1 that micro ready to receive the system state*/
	UART_sendByte(M_READY);
	/*getting the gate state*/
	UART_recieveByte();

	LCD_clearScreen();
	LCD_displayString("GATE OPEN");

	/*inform MC1 that micro ready to receive the system state*/
	UART_sendByte(M_READY);
	/*getting the gate state*/
	UART_recieveByte();

	LCD_clearScreen();
	LCD_displayString("LOCKING");

}



