/*
 * MC1.c
 *
 *Created on: Dec 17, 2020
 *
 *Description: main  file for the MC1
 *
 * Author: Ahmed Emad
 */


#include "i2c.h"
#include "uart.h"
#include "timers.h"
#include "external_eeprom.h"
#include "buzzer.h"
#include "motor.h"


/*******************************************************************************
 *                      Preprocessor Macros                                    *
 *******************************************************************************/

/*Constant value between the 2 micros to indicate that
 *the micro ready to receive information from UART*/
#define M_READY 0XFF

/*Specific value to check if that first time
 *for the system or  at was  initialized
 *stored in the external EEPROM */
#define PREVIOUS_LOGIN_INDICATOR 0XAA
/*the address where PREVIOUS_LOGIN_INDICATOR is stored in the external EEPROM*/
#define PREVIOUS_LOGIN_INDICATOR_ADDRESS 0X0001

/*address where password will be stored*/
#define PASSWORD_ADDRESS 0X0002

/*specific values to inform if password are correct or not */
#define CORRECT_PASSWORD 0XCC
#define WRONG_PASSWORD   0XBB

/*indicator if the function success or fails to do the task*/
#define SUCCESS 1
#define FAILURE 0


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
}options;

/*ENUM to hold gate state*/
typedef enum {
	CLOSED,GATE_OPENING,OPENED,GATE_CLOSING
}GateStatus;
/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/
/*global variable to hold the system state*/
static  volatile uint8 g_systemState;

/*string to hold the password of the system*/
uint8 g_password[7];

/*indicator for gate state at open gate option*/
static  volatile uint8 g_gateStatus=CLOSED;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 *  Description :1.receives the password from micro2
 *               2. checks if its corrects
 *               3.if correct view options
 *               4.if wrong 3 times it will turn on the buzzer
 */
uint8 checkPassword(void);

/*Description : function to turn on buzzer for 10 seconds
 * 1.turn on timer for 10 seconds
 * 2.turn on buzzer
 * 3.after 10 seconds deinit timer and off the buzzer
 * 4.return to log in */
inline static void alarmOn(void);

/*Description : function to open gate
 * 1. turn on motor clockwise for 2 seconds
 * 2. hold it for  */

inline static void openGate(void);

/*ISR call back function to return system back in log in mode */
void changeSystemState(void);


/*Description :ISR function to count 15 seconds then change gate state*/
void changeGateState(void);






int main (){

	/*variable to hold the history byte to check over if this first time*/
	uint8 logInHistory;
	/*to hold the option entered from user*/
	uint8 option;

	/*configure the BUZZER PIN as an output pin PA0*/
	/*configure the MOTOR PIN as an output pin ,PA0,PA1*/
	DDRA |= 0X07;

	/*defining variable to hold the the configuration of  UART*/
	UartConfigType s_uartConfig ={9600,ASYNCHRONOUS_DOUBLE_SPEED_MODE,8,1,NO_PARITY,0,0,0 };

	/*initialize and configure the UART driver*/
	UART_init(&s_uartConfig);


	/*initialize EEPROM*/
	EEPROM_init();
	/*read the log in history byte to check over if this first time*/
	EEPROM_readByte(PREVIOUS_LOGIN_INDICATOR_ADDRESS, &logInHistory);


	/*if the system was previously initialized */
	if(logInHistory==PREVIOUS_LOGIN_INDICATOR){
		g_systemState = CHECK_PASSWORD_TO_LOG_IN;
		/*load the password from the EEPROM*/
		EEPROM_readString(PASSWORD_ADDRESS, g_password);

	}else{
		/*this first time user should create new password*/
		g_systemState = NEW_PASSWORD;
	}

	/* Enable Global Interrupt I-Bit */
	SREG |= (1<<7);

	while (1){

		/*wait until MC2 ready to receive */
		while (UART_recieveByte()!= M_READY);
		/*informing MC2 the system state*/
		UART_sendByte(g_systemState);


		switch (g_systemState) {
			case NEW_PASSWORD:
				/*informing MC2 that i am ready to receive password*/
				UART_sendByte(M_READY);
				/*get entered password*/
				UART_receiveString(g_password);

				/*store it in EEPROM */
				EEPROM_writeString(PASSWORD_ADDRESS, g_password);
				/*write the indicator flag for initialization*/
				EEPROM_writeByte(PREVIOUS_LOGIN_INDICATOR_ADDRESS, PREVIOUS_LOGIN_INDICATOR);

				g_systemState = VIEW_OPTIONS;
				break;

			case VIEW_OPTIONS :
				/*wait until MC2 Send the option entered by user*/
				UART_sendByte(M_READY);
				option=UART_recieveByte();

				if(option==CREATE_NEW_PASSWORD){
					/*check password*/
					g_systemState = CHECK_PASSWORD_FOR_NEW_PASSWORD;
				}else{
					g_systemState =OPENING_GATE;
				}
				break;

			case CHECK_PASSWORD_FOR_NEW_PASSWORD :

				/*if password right then go and create new password*/
				if (checkPassword()==SUCCESS){
					g_systemState = NEW_PASSWORD;
				}
				break;
			case CHECK_PASSWORD_TO_LOG_IN :
				/*if password right then go and create new password*/
				if (checkPassword()==SUCCESS){
					g_systemState = VIEW_OPTIONS;
				}
				break;
			case OPENING_GATE :
				openGate();
				break;
			case BUZZER_ON:
				alarmOn();
				break;
		}



	}

}


/*
 *  Description :1.receives the password from micro2
 *               2. checks if its corrects
 *               3.if correct view options
 *               4.if wrong 3 times it will turn on the buzzer
 */

uint8 checkPassword(void){

	/*count number of failTrials*/
	static uint8 failTrials=0;
	uint8 user_password[7];
	/*wait until micro2 enter the password and send it */
	/*informing MC2 that i am ready to receive password*/
	UART_sendByte(M_READY);

	/*get entered password*/
	UART_receiveString(user_password);

	/*load the password from the EEPROM*/
	EEPROM_readString(PASSWORD_ADDRESS, g_password);

	/*checking for match*/
	for (int var = 0; var < 6; ++var) {

		if(g_password[var] != user_password[var]){
			failTrials++;
			/*wait until MC2 ready to receive */
			while (UART_recieveByte()!= M_READY);
			/*informing MC2 the password is wrong*/
			UART_sendByte(WRONG_PASSWORD);
			/*go to state of the buzzer if the user
			 * enter the password wrong 3 times*/
			if(failTrials==3){
				g_systemState=BUZZER_ON;
				/*clear failTrials for the coming log in*/
				failTrials=0;
			}
			return FAILURE;

		}
	}
	/*wait until MC2 ready to receive */
	while (UART_recieveByte()!= M_READY);
	/*informing MC2 the password is wrong*/
	UART_sendByte(CORRECT_PASSWORD);
	/*the password is  right*/
	/*clear failTrials for the coming log in*/
	failTrials=0;
	return SUCCESS;
}


/*Description : function to turn on buzzer for 8 seconds
 * 1.turn on timer for 10 seconds
 * 2.turn on buzzer
 * 3.after 10 seconds deinit timer and off the buzzer
 * 4.return to log in */
inline static void alarmOn(void){

	/* Create configuration structure for normal mode timer1 */
	TimersConfigType s_timer1Config = {NORMAL,F_CPU_1024,1,TIMER1 };
	/*set the call back to return system back in log in mode*/
	TIMERS_setCallBackTimer1(changeSystemState);

	TIMERS_init(&s_timer1Config);

	/*TURN ON THE BUZZER*/
	buzzerOn();
	
	/* wait until timer ends up counting */
	while(g_systemState==BUZZER_ON);

	/*TURN OFF THE BUZZER*/
	buzzerOff();
	/*deinit the timer*/
	TIMERS_deinit(TIMER1);

}

/*ISR call back function to return system back in log in mode
 * in case of Buzzer on state*/
void changeSystemState(void){
	/*hold the number this function called to calculate one minute*/
	static uint8 entry_counter=0;
	entry_counter++;
	if (entry_counter==8){
		g_systemState = CHECK_PASSWORD_TO_LOG_IN;

		/*Reinitialize the counter for the next time*/
		entry_counter = 0;
	}
}

/*Description : function to rotate motor 15 seconds clockwise
 * then hold for 3 seconds the rotate 15 seconds anti-clockwise
 * 1.Initialize timer 1 to create PWM signal to create 125Khzsignal
 * 2.initialize timer 0 to count 15 seconds
 * 3.adjust polarity to rotate clockwise
 * 4.stop the motor
 * 5.adjust polarity to rotate anti-clockwise for 15 seconds
 * 6.stop both timers*/
inline static void openGate(void){

	/*initialize the timer PWM mode without rotating the motor*/
	motor_init();

	/* Create configuration structure for normal mode timer1 to count 15 seconds as gate opening*/
	TimersConfigType s_timer1Config = {NORMAL,F_CPU_256,1,TIMER1 };


	/*Create configuration structure for COMPARE mode timer1 to count 3 seconds*/
	TimersCompareModeConfig s_cmpModeT1Config ={21900,0,OCN_DISCONNECTED,OCN_DISCONNECTED};

	/*set the call back to return system back in log in mode*/
	TIMERS_setCallBackTimer1(changeGateState);

	g_gateStatus = OPENING_GATE;

	/*wait until MC2 ready to receive */
	while (UART_recieveByte()!= M_READY);
	/*informing MC2 CASE OF GATE*/
	UART_sendByte(g_gateStatus);

	// Rotate the motor --> clock wise
	motor_rotateClockwise();


	/*start the timer in normal mode for 15 seconds count*/
	TIMERS_init(&s_timer1Config);

	/*wait until 15 seconds pass*/
	while(g_gateStatus==OPENING_GATE);

	/*stop the timer*/
	TIMERS_deinit(TIMER1);

	/*wait until MC2 ready to receive */
	while (UART_recieveByte()!= M_READY);
	/*informing MC2 CASE OF GATE*/
	UART_sendByte(g_gateStatus);

	// Stop the motor
	motor_stop();

	/*change configuration settings to configure compare mode */
	s_timer1Config.Mode=COMPARE;
	/*start the timer*/
	TIMERS_init(&s_timer1Config,&s_cmpModeT1Config);

	/*wait until 3 seconds pass*/
	while(g_gateStatus==OPENED);

	/*stop the timer*/
	TIMERS_deinit(TIMER1);


	/*wait until MC2 ready to receive */
	while (UART_recieveByte()!= M_READY);
	/*informing MC2 CASE OF GATE*/
	UART_sendByte(g_gateStatus);


	/*change configuration settings to configure Normal mode for 15 seconds count*/
	s_timer1Config.Mode=NORMAL;


	// Rotate the motor --> anti-clock wise to close the door
	motor_rotateAntiClockwise();

	/*start the timer to count 15 seconds */
 	TIMERS_init(&s_timer1Config);

	/*wait until 15 seconds pass*/
	while(g_gateStatus==GATE_CLOSING);

	/*stop the timer*/
	TIMERS_deinit(TIMER1);

	// Stop the motor
	motor_stop();

	g_systemState=VIEW_OPTIONS;

}
/*Description :ISR call back function function to count  the required time
 * for opening and closing motor and time gate held open*/
void changeGateState(void){

	/*variable to count number of overflows happened*/
	static uint8 tickCount = 0;

	tickCount++;

	switch (g_gateStatus) {
		case OPENING_GATE:
			/*number of overflows required for 15 seconds is 7  */
			if(tickCount==7){
				/*reinitialize the counter for the next state*/
				tickCount = 0;
				g_gateStatus =OPENED;
			}
			break;
		case OPENED:
			if(tickCount==2){ /*number of overflows required for 3 seconds*/
				/*reinitialize the counter for the next state*/

				/*reinitialize the counter for the next state*/
				tickCount = 0;
				g_gateStatus =GATE_CLOSING;
			}
			break;
		case GATE_CLOSING :
			if(tickCount==7){ /*number of overflows required for 15 seconds is 7*/
				/*reinitialize the counter for the next TIME*/
				tickCount = 0;
				g_gateStatus =CLOSED;
			}
			break;
	}

}

