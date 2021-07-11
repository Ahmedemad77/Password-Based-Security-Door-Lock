/******************************************************************************
 *
 * Module:
 *
 * File Name: buzzer.c
 *
 * Description:
 *
 * Author: Ahmed Emad
 *
 *******************************************************************************/
#include "buzzer.h"

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*Description : function to turn on buzzer */
void buzzerOn(void){
	/*TURN ON THE BUZZER*/
	SET_BIT(PORTA,PA2);
}

/*Description : function to turn on buzzer */
void buzzerOff(void){
	/*TURN OFF THE BUZZER*/
	CLEAR_BIT(PORTA,PA2);
}
