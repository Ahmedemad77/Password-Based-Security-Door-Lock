/******************************************************************************
 *
 * Module:
 *
 * File Name: timers.h
 *
 * Description: Header file for ALL Timers drivers with All modes
 *
 * Author: Ahmed Emad
 *
 *******************************************************************************/




#include "micro_config.h"
#include "std_types.h"
#include "common_macros.h"
#include <stdarg.h>



#ifndef TIMERS_H_
#define TIMERS_H_

/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/


/************************************************************GENERAL DECLERATIONS FOR ALL MODES *********************/


/* enum to hold which timer to operate*/
typedef enum {
	TIMER0,TIMER1,TIMER2
}TimerNum;

typedef enum {
	NORMAL , COMPARE , PWM ,ICU
}TimerMode;

typedef enum {
	NO_CLOCK,F_CPU_CLOCK,F_CPU_8,F_CPU_64,F_CPU_256,F_CPU_1024,F_CPU_T2_32,F_CPU_T2_128
}TimerClock;


/* general variables for all timers */
typedef struct{

	TimerMode Mode;
	TimerClock Clock;
	uint8 IntEnable;
	TimerNum TimerNum;
}TimersConfigType;

/*********************************************************************************************************************/


/*************************************************************Compare Mode Declarations*******************************/

/* enum to hold the mode of OCN PIN  */
typedef enum {
	OCN_DISCONNECTED ,OCN_CONNECTED_TOGGLE,OCN_CONNECTED_SET,OCN_CONNECTED_CLEAR
}TimersCompareModeOcPinMode;


typedef struct {
	uint16 CompareRegValue1; /* to store the compared value for OCRn(for the three TIMERS) */
	uint16 CompareRegValue2; /* only for timer1 for OCR1B */
	TimersCompareModeOcPinMode OCn_Mode; /* to store the mode of OCn(for all timers) and OC1A FOR TIMER 1 */
	TimersCompareModeOcPinMode OC1B_Mode; /* to store the mode of OC1B only for timer 1 */
}TimersCompareModeConfig;
/********************************************************************************************************************/




/**********************************************************PWM MODE Declarations************************************/

typedef enum{
	/* DISCONNECTED case only have meaning in OC1B pin */
	DISCONNECTED,INVERTING,NON_INVERTING
}TimersPwmModeOCnMode;

typedef struct {
	/*to hold OCRn/(OCR1A for TIMER1) value (the value compared to TCNTn to toggle OCn pin) */
	uint16 CompareValue1;

	/* to hold (OCR1B for TIMER1) value (the value compared to TCNTn to toggle OC1B pin)
	 * FOR TIMER1 CASE */
	uint16 Timer1_comparevalue2;

	/* the top value for TIMER1 counter TCNT1
	 * ONLY FOR TIMER1 CASE */
	uint16 Timer1_Top;
	/* to store the mode of OCn PIN INVERTING OR NON_INVERTING */
	TimersPwmModeOCnMode OCn_Mode;

	/* to store the mode of OC1B PIN INVERTING OR NON_INVERTING
	 * ONLY FOR TIMER1 CASE */
	TimersPwmModeOCnMode Timer1_OC1B_mode;

}TimersPwmModeConfig;

/*********************************************************************************************************************/






/************************************************************ICU MODE Declarations************************************/

typedef enum
{
	FALLING,RISING
}Icu_EdgeType;

typedef  TimerClock Icu_Clock;

typedef struct
{
	Icu_Clock clock;
	Icu_EdgeType edge;
}TimersIcuConfigType;


/*********************************************************************************************************************/

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/



/*******GENERAL Functions FOR ALL MODES ,TIMERS ********/

/* Description:  Function to initialize all modes
 * [IN] :TimersConfigType that holds the mode you want to initialize and which timer 0,1,2 and clock and INT enable
 * [IN] :Also additional struct that holds the extra data required for each mode( except NORMAL mode ) which are
 * 		:TimersCompareModeConfig in case of COMPARE MoDE
 * 		:TimersPwmModeConfig in case of PWM MODE
 * 		:TimersIcuConfigType in case of  	ICU MODE
 */
void TIMERS_init(const TimersConfigType* config_ptr,...);

/* Description:  Function to Deinit  the desired TIMER */
void TIMERS_deinit(TimerNum disabledTimer);

/*function to return current value of the timer*/
uint16 TIMERS_getTimerValue(TimerNum timer);

/*function to clear timer register*/
void TIMERS_clearTimerValue(TimerNum timer);

/********************************************************/



/*******CALL BACK FUNCTIONS FOR TIMERS******************/


/*
 * Description: Function to set the Call Back function address for all modes of timer0
 */
void TIMERS_setCallBackTimer0(void(*a_ptr)(void));

/*
 * Description: Function to set the Call Back function address for all modes of timer1 and ICU
 */
void TIMERS_setCallBackTimer1(void(*a_ptr)(void));

/*
 * Description: Function to set the Call Back function address for all modes of timer2
 */
void TIMERS_setCallBackTimer2(void(*a_ptr)(void) );


/********************************************************/


/*****************ICU TIMER1 FUNCTIONS******************/


/*
 * Description: Function to set the required edge detection.
 */
void ICU_setEdgeDetectionType(const Icu_EdgeType edgeType);

/*
 * Description: Function to get the Timer1 Value when the input is captured
 *              The value stored at Input Capture Register ICR1
 */
uint16 ICU_getInputCaptureValue(void);

/*
 * Description: Function to clear the Timer1 Value to start count from ZERO
 */
void ICU_clearTimerValue(void);

/*
 * Description: Function to disable the Timer1 to stop the ICU Driver
 */
void ICU_DeInit(void);


/**************************************************************/

#endif /* TIMERS_H_ */
