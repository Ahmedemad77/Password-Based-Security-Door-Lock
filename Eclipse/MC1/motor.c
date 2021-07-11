/******************************************************************************
 *
 * Module:
 *
 * File Name: motor.c
 *
 * Description:
 *
 * Author: Ahmed Emad
 *
 *******************************************************************************/
#include "motor.h"
#include "timers.h"



/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description : initialize the timer PWM mode without rotating the motor
 */
void motor_init(void){

	/* define configuration structure for  general settings for timer0 */
	TimersConfigType s_timer0Config = {PWM,F_CPU_64,0 /*interrupt disable*/ ,TIMER0 };

	/*define configuration structure for PWM mode timer0
	 * to operate 490 kHZ the top value is 6120 and disconnect OCB*/
	TimersPwmModeConfig s_pwmTimer0Config ={128,0 /*compare value OCR1B*/ ,0 ,NON_INVERTING,DISCONNECTED};
	TIMERS_init(&s_timer0Config,&s_pwmTimer0Config);

	/*initially stop   the motor */
	PORTA &= (~(1<<PA0));
	PORTA &= (~(1<<PA1));
}
/*
 * Description : disable the timer PWM mode also stop the motor
 */
void motor_deInit(void){

	/*disable the PWM signal*/
	TIMERS_deinit(TIMER0);

	// Stop the motor
		PORTA &= (~(1<<PA0));
		PORTA &= (~(1<<PA1));

}

/*
 *Description :Rotate the motor  clock wise
 */

void motor_rotateClockwise(void){
	// Rotate the motor --> clock wise
	PORTA &= (~(1<<PA0));
	PORTA |= (1<<PA1);


}

/*
 *Description :Rotate the motor anti clock wise
 */

void motor_rotateAntiClockwise(void){
	// Rotate the motor --> anti-clock wise
		PORTA |= (1<<PA0);
		PORTA &= (~(1<<PA1));
}

/*
 *Description :stop the motor
 */

void motor_stop(void){
	// Stop the motor
	PORTA &= (~(1<<PA0));
	PORTA &= (~(1<<PA1));

}
