/******************************************************************************
 *
 * Module:
 *
 * File Name: timers.c
 *
 * Description: Source file for ALL Timers drivers with All modes
 *
 * Author: Ahmed Emad
 *
 *******************************************************************************/
#include "timers.h"

/*******************************************************************************
 *                            GLOBAL VARIABLES                    *
 *******************************************************************************/

static volatile void (*g_callBackPtrTimer0)(void) = NULL_PTR;
static volatile void (*g_callBackPtrTimer1)(void) = NULL_PTR;
static volatile void (*g_callBackPtrTimer2)(void) = NULL_PTR;
/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/

/*set up call back functions for overflow mode interrupt service routine*/
ISR(TIMER0_OVF_vect){
	if(g_callBackPtrTimer0 != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackPtrTimer0)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}
ISR(TIMER1_OVF_vect){
	if(g_callBackPtrTimer1 != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackPtrTimer1)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}
ISR(TIMER2_OVF_vect){
	if(g_callBackPtrTimer1 != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackPtrTimer1)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}


/*set up call back functions for compare  mode interrupt service routine*/

ISR(TIMER0_COMP_vect){
	if(g_callBackPtrTimer0 != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackPtrTimer0)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}
ISR(TIMER1_COMPA_vect){
	if(g_callBackPtrTimer1 != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackPtrTimer1)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}
ISR(TIMER1_COMPB_vect){
	if(g_callBackPtrTimer1 != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackPtrTimer1)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}
ISR(TIMER2_COMP_vect){
	if(g_callBackPtrTimer2 != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackPtrTimer2)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}

/*set up call back functions for ICU mode interrupt service routine*/


ISR(TIMER1_CAPT_vect)
{
	if(g_callBackPtrTimer1 != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackPtrTimer1)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}

/*******************************************************************************
 *                      Functions Prototypes(Private)                          *
 *******************************************************************************/

/*Description : Init functions for normal mode for timers 0,1,2*/
inline static void TIMER0_InitNormal (const TimersConfigType * config_ptr);
inline static void TIMER1_InitNormal (const TimersConfigType * config_ptr);
inline static void TIMER2_InitNormal (const TimersConfigType * config_ptr);


/*Description : Init functions for compare mode for timers 0,1,2*/
inline static void TIMER0_InitCompare (const TimersConfigType * config_ptr,const TimersCompareModeConfig* compareConfigPtr);
inline static void TIMER1_InitCompare (const TimersConfigType * config_ptr,const TimersCompareModeConfig* compareConfigPtr);
inline static void TIMER2_InitCompare (const TimersConfigType * config_ptr,const TimersCompareModeConfig* compareConfigPtr);



/*Description : Init functions for Pwm mode for timers 0,1,2*/
inline static void TIMER0_InitPwm (const TimersConfigType * config_ptr ,const TimersPwmModeConfig * pwmConfig_Ptr);
inline static void TIMER1_Initpwm (const TimersConfigType * config_ptr ,const TimersPwmModeConfig * pwmConfig_Ptr);
inline static void TIMER2_InitPwm (const TimersConfigType * config_ptr ,const TimersPwmModeConfig * pwmConfig_Ptr);


/*Description : Init functions for ICU mode for timers 0,1,2*/
inline static void Icu_init(const TimersIcuConfigType * Icu_Config_Ptr);

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*******GENERAL Functions FOR ALL MODES ,All TIMERS ********/

/* Description:  Function to initialize all modes that can take more than one input in case of
 *				 modes other than 	normal mode
 * [IN] :TimersConfigType that holds the mode you want to initialize and which timer 0,1,2 and clock and INT enable
 * [IN] :Also additional struct that holds the extra data required for each mode( except NORMAL mode ) which are
 * 		:TimersCompareModeConfig in case of COMPARE MoDE
 * 		:TimersPwmModeConfig in case of PWM MODE
 * 		:TimersIcuConfigType in case of  	ICU MODE
 */
void TIMERS_init(const TimersConfigType* config_ptr,...){


	switch (config_ptr->Mode) {
	/*********************************************************NORMAL MODE********************************/
	case NORMAL:{
		switch (config_ptr->TimerNum) {
		case TIMER0:
			TIMER0_InitNormal(config_ptr);
			break;
		case TIMER1:
			TIMER1_InitNormal(config_ptr);
			break;
		case TIMER2:
			TIMER2_InitNormal(config_ptr);
			break;
		}
		break;
	}
	/*****************************************************************************************************/

	/*********************************************************COMPARE MODE********************************/
	case COMPARE :{
		/******** getting the second argument compareConfig ******************/
		TimersCompareModeConfig * compareConfigPtr=NULL_PTR;
		va_list ap;
		va_start( ap,1);
		compareConfigPtr =va_arg(ap,TimersCompareModeConfig *);
		va_end(ap);
		/******************************************************************/
		switch (config_ptr->TimerNum) {
		case TIMER0:
			TIMER0_InitCompare (config_ptr,compareConfigPtr);
			break;
		case TIMER1:
			TIMER1_InitCompare (config_ptr,compareConfigPtr);
			break;
		case TIMER2:
			TIMER2_InitCompare (config_ptr,compareConfigPtr);
			break;
		}
		break;
	}

	/*******************************************************************************************************/

	/*********************************************************PWM  MODE*************************************/
	case PWM :{
		/***************** getting the second argument compareConfig ******************/
		TimersPwmModeConfig * pwmConfig_Ptr =NULL_PTR;
		va_list ap;
		va_start( ap,1);
		pwmConfig_Ptr =va_arg(ap,TimersPwmModeConfig *);
		va_end(ap);
		/*****************************************************************************/
		switch (config_ptr->TimerNum) {
		case TIMER0:
			TIMER0_InitPwm(config_ptr, pwmConfig_Ptr);
			break;
		case TIMER1:
			TIMER1_Initpwm(config_ptr, pwmConfig_Ptr);
			break;
		case TIMER2:
			TIMER2_InitPwm(config_ptr, pwmConfig_Ptr);
			break;
		}

		break;
	}
	/*******************************************************************************************************/


	/*********************************************************ICU MODE*************************************/
	case ICU :{
		/***************** getting the second argument compareConfig ******************/
		TimersIcuConfigType * Icu_Config_Ptr =NULL_PTR;
		va_list ap;
		va_start( ap,1);
		Icu_Config_Ptr =va_arg(ap,TimersIcuConfigType *);
		va_end(ap);
		/*****************************************************************************/
		Icu_init(Icu_Config_Ptr);
		break;
	}

	}


}



/* Description:  Function to Deinit  the desired TIMER */
void TIMERS_deinit(TimerNum disabledTimer){

	switch (disabledTimer) {
			case TIMER0:

				/* Clear All Timer1 Registers */
				TCNT0 = 0;
				TCCR0 = 0;
				OCR0  = 0;
				/* Disable the  interrupts
				 * OCIE0 INTERUPT
				 * TOIE0 INTERUPT */
				TIMSK &=0xFC;

				break;
			case TIMER1:


				/* Clear All Timer1 Registers */
				TCCR1B = 0;
				TCCR1A = 0;
				TCNT1 = 0;
				ICR1 = 0;
				OCR1A = 0;
				OCR1B = 0;

				/* Disable the  interrupts
				 * OCIE1B INTERUPT
				 * OCIE1A INTERUPT
				 * TICIE1 INTERUPT*/
				TIMSK &= 0XC3;
				break;
			case TIMER2:
				/* Clear All Timer1 Registers */
				TCNT2 = 0;
				TCCR2 = 0;
				OCR2  = 0;
				/* Disable the  interrupts
				 * OCIE2 INTERUPT
				 * TOIE2 INTERUPT */
				TIMSK &=0x3F;

				break;
			}
}

/*function to return the current value of the timer */
uint16 TIMERS_getTimerValue(TimerNum timer){

	switch (timer) {
	case TIMER0:
		return TCNT0;
		break;
	case TIMER1:
		return TCNT1;
		break;
	case TIMER2:
		return TCNT2;
		break;
	}

	return 0;
}

/*function to clear timer register*/
void TIMERS_clearTimerValue(TimerNum timer){

	switch (timer) {
	case TIMER0:
		TCNT0 = 0;
		break;
	case TIMER1:
		TCNT1 = 0;
		break;
	case TIMER2:
		TCNT2  = 0;
		break;
	}

}

void TIMERS_setCallBackTimer0(void(*a_ptr)(void)){
	g_callBackPtrTimer0 = a_ptr ;
}
void TIMERS_setCallBackTimer1(void(*a_ptr)(void)){
	g_callBackPtrTimer1 = a_ptr ;
}
void TIMERS_setCallBackTimer2(void(*a_ptr)(void)){
	g_callBackPtrTimer2 = a_ptr ;
}




/*
 * Description: Function to set the required edge detection.
 */
void ICU_setEdgeDetectionType(const Icu_EdgeType a_edgeType)
{
	/*
	 * insert the required edge type in ICES1 bit in TCCR1B Register
	 */
	TCCR1B = (TCCR1B & 0xBF) | (a_edgeType<<6);
}


/*
 * Description: Function to get the Timer1 Value when the input is captured
 *              The value stored at Input Capture Register ICR1
 */
uint16 ICU_getInputCaptureValue(void)
{
	return ICR1;
}

/*
 * Description: Function to clear the Timer1 Value to start count from ZERO
 */
void ICU_clearTimerValue(void)
{
	TCNT1 = 0;
}

/*
 * Description: Function to disable the Timer1 to stop the ICU Driver
 */
void ICU_DeInit(void)
{
	/* Clear All Timer1 Registers */
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1 = 0;
	ICR1 = 0;

	/* Disable the Input Capture interrupt */
	TIMSK &= ~(1<<TICIE1);
}

/*******************************************************************************
 *                      Functions Definitions(Private)                          *
 *******************************************************************************/

/***************************************Normal MODE****************************/

inline static void TIMER0_InitNormal (const TimersConfigType * config_ptr){

	TCNT0 = 0; //Set Timer initial value to 0
	/*  Enable Timer0 Overflow Interrupt  if interrupt allowed*/
	if(config_ptr->IntEnable){
		TIMSK |= (1<<TOIE0);
	}
	/* Configure the timer control register
	 * 1. Non PWM mode FOC0=1
	 * 2. Normal Mode WGM01=0 & WGM00=0
	 * 3. Normal Mode COM00=0 & COM01=0
	 * 4. setting clock
	 */
	TCCR0 = (1<<FOC0) |( (config_ptr->Clock)&0x07) ;

}
inline static void TIMER1_InitNormal (const TimersConfigType * config_ptr){
	TCNT1 = 0; //Set Timer initial value to 0
	/*  Enable Timer0 Overflow Interrupt  if interrupt allowed*/
	if(config_ptr->IntEnable){
		TIMSK |= (1<<TOIE1);
	}
	/* Configure the timer control register
	 * 1. Non PWM mode FOC0=1
	 * 2. Normal Mode WGM11=0 & WGM10=0
	 * 3. Normal Mode COM10=0 & COM11=0
	 */
	TCCR1A = (1<<FOC1A) |(1<<FOC1B);
	/* 4. setting clock*/
	TCCR1B = ((config_ptr->Clock)&0xF7);
}
inline static void TIMER2_InitNormal (const TimersConfigType * config_ptr){


	TCNT2 = 0; //Set Timer initial value to 0
	/*  Enable Timer0 Overflow Interrupt  if interrupt allowed*/
	if(config_ptr->IntEnable){
		TIMSK |= (1<<TOIE2);
	}
	/* Configure the timer control register
	 * 1. Non PWM mode FOC2=1
	 * 2. Normal Mode WGM21=0 & WGM20=0
	 * 3. Normal Mode COM20=0 & COM21=0
	 * 4. set clock
	 */
	switch (config_ptr->Clock) {
	case NO_CLOCK:
	case F_CPU_CLOCK:
	case F_CPU_8 :
		TCCR2 = (1<<FOC2) |( (config_ptr->Clock)&0x07) ;
		break;
	case F_CPU_T2_32 :
		/*  clock = F_CPU/32 CS20=0 CS21=1 CS22=1 (0x03)*/
		TCCR2 = (1<<FOC2) |(0x03) ;
		break;
	case F_CPU_64 :
		/*  clock = F_CPU/64 CS20=1 CS21=0 CS22=0 (0x04)*/
		TCCR2 = (1<<FOC2) |(0x04) ;
		break;
	case F_CPU_T2_128 :
		/*  clock = F_CPU/64 CS20=1 CS21=0 CS22=1 (0x05)*/
		TCCR2 = (1<<FOC2) |(0x05) ;
		break;
	case F_CPU_256 :
		/*  clock = F_CPU/64 CS20=1 CS21=1 CS22=0 (0x06)*/
		TCCR2 = (1<<FOC2) |(0x06) ;
		break;
	case F_CPU_1024 :
		/*  clock = F_CPU/64 CS20=1 CS21=1 CS22=1 (0x07)*/
		TCCR2 = (1<<FOC2) |(0x07) ;
		break;
	}
}

/****************************************************************************/

/***************************************COMPARE MODE****************************/
inline static void TIMER0_InitCompare (const TimersConfigType * config_ptr,const TimersCompareModeConfig* compareConfigPtr){
	TCNT0 = 0;    // Set Timer initial value to 0
	OCR0  =  (uint8)compareConfigPtr->CompareRegValue1; // Set Compare Value
	if(config_ptr->IntEnable){
		TIMSK |= (1<<OCIE0); // Enable Timer0 Compare Interrupt
	}
	/* Configure timer0 control register
	 * 1. Non PWM mode FOC0=1
	 * 2. CTC Mode WGM01=1 & WGM00=0
	 * 4. set the clock
	 * 5.configure OC0 (PB3) as output pin if connected
	 */
	switch (compareConfigPtr->OCn_Mode) {
	case OCN_DISCONNECTED:
		/* COM00=0 & COM01=0  */
		TCCR0 = (1<<FOC0) | (1<<WGM01) | ( (config_ptr->Clock) &0x07);
		break;
	case OCN_CONNECTED_TOGGLE:
		/* COM00=1 & COM01=0  */
		TCCR0 = (1<<FOC0) | (1<<WGM01) | ( (config_ptr->Clock) &0x07) |(1<<COM00);
		DDRB |=(1<<PB3);
		break;
	case OCN_CONNECTED_CLEAR :
		/* COM00=0 & COM01=1  */
		TCCR0 = (1<<FOC0) | (1<<WGM01) | ( (config_ptr->Clock) &0x07) | (1<<COM01);
		DDRB |=(1<<PB3);
		break;
	case OCN_CONNECTED_SET :
		/* COM00=1 & COM01=1  */
		TCCR0 = (1<<FOC0) | (1<<WGM01) | ( (config_ptr->Clock) &0x07) |(1<<COM00)|(1<<COM01);
		DDRB |=(1<<PB3);
		break;

	}


}
inline static void TIMER1_InitCompare (const TimersConfigType * config_ptr,const TimersCompareModeConfig* compareConfigPtr){



	// Set Timer initial value to 0
	TCNT1 = 0;

	if(compareConfigPtr->OC1B_Mode != OCN_DISCONNECTED){
		/* set the compare value for OCR1B if OC1B connected  */
		OCR1B = compareConfigPtr->CompareRegValue2;

		/* set the OC1B pin as output */
		DDRD |=(1<<PD6);
	}

	/* Configure timer1 control register TCCR1A
	 * 1. Non PWM mode FOC1A=1,FOC1B=1
	 * 2. CTC Mode WGM11=0 & WGM10=0
	 * 3. OC1A,OC1B adjust
	 * 4.configure the OC1A as output if CONNECTED
	 */
	switch (compareConfigPtr->OCn_Mode) {
	case OCN_DISCONNECTED:
		/* COM1A1=0 & COM1A0=0  */
		TCCR1A = (1<<FOC1A) | (1<<FOC1B) ;
		break;
	case OCN_CONNECTED_TOGGLE:
		/* COM1A1=0 & COM1A0=1  */
		TCCR1A = (1<<FOC1A) | (1<<FOC1B) |(1<<COM1A0) ;
		DDRD |=(1<<PD5);
		break;
	case OCN_CONNECTED_CLEAR :
		/* COM1A1=1 & COM1A0=0  */
		TCCR1A = (1<<FOC1A) | (1<<FOC1B) |(1<<COM1A1) ;
		DDRD |=(1<<PD5);
		break;
	case OCN_CONNECTED_SET :
		/* COM1A1=1 & COM1A0=1  */
		TCCR1A = (1<<FOC1A) | (1<<FOC1B)  |(1<<COM1A0) | (1<<COM1A1);
		DDRD |=(1<<PD5);
		break;
	}

	/* for OC1B if connected */
	if(compareConfigPtr->OC1B_Mode != OCN_DISCONNECTED){
		switch (compareConfigPtr->OC1B_Mode) {
		case OCN_DISCONNECTED:
			break;
		case OCN_CONNECTED_TOGGLE:
			/* COM1A1=0 & COM1A0=1  */
			TCCR1A |= (1<<COM1B0) ;
			break;
		case OCN_CONNECTED_CLEAR :
			/* COM1A1=1 & COM1A0=0  */
			TCCR1A  |= (1<<COM1B1) ;
			break;
		case OCN_CONNECTED_SET :
			/* COM1A1=1 & COM1A0=1  */
			TCCR1A |=(1<<COM1B0) | (1<<COM1B1);
			break;

		}
	}

	/* Set Compare Value of OCR1A */
	OCR1A  =  compareConfigPtr->CompareRegValue1;

	/*clear out compare flag as if not cleared the interrupt will be
	 * executed before timer1 works */
	SET_BIT(TIFR,OCF1A);
	/* Configure timer control register TCCR1B
	 * 1. CTC mode WGM12=1 WGM13=0 (Mode Number 4)
	 * 2. set Prescaler
     */
	TCCR1B = (1<<WGM12) | ((config_ptr->Clock)&0xF7);

	if(config_ptr->IntEnable){
		TIMSK |= (1<<OCIE1A); // Enable Timer1 OUTCompareA Interrupt

		/* Enable Timer1 OUTCompareB Interrupt if interrupts allowed */
		if(compareConfigPtr->OC1B_Mode != OCN_DISCONNECTED){
			TIMSK |= (1<<OCIE1B);
		}
	}


}
inline static void TIMER2_InitCompare (const TimersConfigType * config_ptr,const TimersCompareModeConfig* compareConfigPtr){
	TCNT2 = 0;    // Set Timer initial value to 0
		OCR2  =  (uint8)compareConfigPtr->CompareRegValue1; // Set Compare Value
		if(config_ptr->IntEnable){
			TIMSK |= (1<<OCIE2); // Enable Timer0 Compare Interrupt
		}
		/* Configure timer0 control register
		 * 1. Non PWM mode FOC2=1
		 * 2. CTC Mode WGM21=1 & WGM20=0
		 * 3.configure OC0 (PB3) as output pin if connected
		 */
		switch (compareConfigPtr->OCn_Mode) {
			case OCN_DISCONNECTED:
				/* COM21=0 & COM20=0  */
				TCCR2 = (1<<FOC2) | (1<<WGM21) ;
				break;
			case OCN_CONNECTED_TOGGLE:
				/* COM21=0 & COM20=1  */
				TCCR2 = (1<<FOC2) | (1<<WGM21) | (1<<COM20) ;
				DDRD |=(1<<PD7);
				break;
			case OCN_CONNECTED_CLEAR :
				/* COM21=1 & COM20=0  */
				TCCR2 = (1<<FOC2) | (1<<WGM21) | (1<<COM21) ;
				DDRD |=(1<<PD7);
				break;
			case OCN_CONNECTED_SET :
				/* COM21=1 & COM20=1  */
				TCCR2 = (1<<FOC2) | (1<<WGM21) | (1<<COM20) |(1<<COM21) ;
				DDRD |=(1<<PD7);
				break;
			}
		/* set the clock  */

		switch (config_ptr->Clock) {
			case NO_CLOCK:
			case F_CPU_CLOCK:
			case F_CPU_8 :
				TCCR2 |= ( (config_ptr->Clock)&0x07) ;
				break;
			case F_CPU_T2_32 :
				/*  clock = F_CPU/32 CS20=0 CS21=1 CS22=1 (0x03)*/
				TCCR2 |=(0x03) ;
				break;
			case F_CPU_64 :
				/*  clock = F_CPU/64 CS20=1 CS21=0 CS22=0 (0x04)*/
				TCCR2  |= (0x04) ;
				break;
			case F_CPU_T2_128 :
				/*  clock = F_CPU/64 CS20=1 CS21=0 CS22=1 (0x05)*/
				TCCR2  |= (0x05) ;
				break;
			case F_CPU_256 :
				/*  clock = F_CPU/64 CS20=1 CS21=1 CS22=0 (0x06)*/
				TCCR2 |= (0x06) ;
				break;
			case F_CPU_1024 :
				/*  clock = F_CPU/64 CS20=1 CS21=1 CS22=1 (0x07)*/
				TCCR2 |= (0x07) ;
				break;
			}

}

/****************************************************************************/


/***************************************PWM MODE****************************/


inline static void TIMER0_InitPwm (const TimersConfigType * config_ptr ,const TimersPwmModeConfig * pwmConfig_Ptr){

	TCNT0 = 0; //Set Timer Initial value

	OCR0  = (uint8)(pwmConfig_Ptr->CompareValue1); // Set Compare Value

	DDRB  = DDRB | (1<<PB3); //set PB3/OC0 as output pin --> pin where the PWM signal is generated from MC.

	/* Configure timer control register
	 * 1. Fast PWM mode FOC0=0
	 * 2. Fast PWM Mode WGM01=1 & WGM00=1
	 * 3. adjust OC0 inverting (COM01=1)| (COM00=1)or  NON inverting COM00=0 & COM01=1
	 * 4. adjust clock
	 */
	if(pwmConfig_Ptr->OCn_Mode == NON_INVERTING)
		TCCR0 = (1<<WGM00) | (1<<WGM01) | (1<<COM01) | ( (config_ptr->Clock)&0x07 );
	else
		TCCR0 = (1<<WGM00) | (1<<WGM01) | (1<<COM01)| (1<<COM00) | ( (config_ptr->Clock)&0x07 );

}
inline static void TIMER1_Initpwm (const TimersConfigType * config_ptr ,const TimersPwmModeConfig * pwmConfig_Ptr){

	TCNT1 = 0;		/* Set timer1 initial count to zero */
	ICR1 = pwmConfig_Ptr->Timer1_Top;	/* Set TOP count for timer1 in ICR1 register */

	OCR1A = pwmConfig_Ptr->CompareValue1; /* Set the compare value */

	DDRD |= (1<<PD5); //set PD5/OC1A as output pin --> pin where the PWM signal is generated from MC.

	if (pwmConfig_Ptr->Timer1_OC1B_mode!=DISCONNECTED){
		OCR1B =pwmConfig_Ptr->Timer1_comparevalue2;
		DDRD |= (1<<PD4); //set PD5/OC1B as output pin --> pin where the PWM signal is generated from MC.
	}


	/* Configure timer control register TCCR1A
     * 1. set the mode of OC1A
     * 2. FOC1A=0 FOC1B=0 because these bits are only active in case non-pwm mode
     * 3. Fast Pwm Mode with the TOP in ICR1 WGM10=0 WGM11=1 (Mode Number 14)
	 */
	switch (pwmConfig_Ptr->OCn_Mode) {
		case DISCONNECTED:
			break;
		case INVERTING:
			/* COM1A1=1 & COM1A0=1  */
			TCCR1A = (1<<WGM11) | (1<<COM1A1)| (1<<COM1A0) ;
			break;
		case NON_INVERTING :
			/* COM1A1=1 & COM1A0=0  */
			TCCR1A = (1<<WGM11) |  (1<<COM1A1) ;
			break;
		}

	/* set the mode of OC1B */
	switch (pwmConfig_Ptr->Timer1_OC1B_mode) {
			case DISCONNECTED :
				break;
			case INVERTING:
				/* COM1A1=1 & COM1A0=1  */
				TCCR1A |=  (1<<COM1B1)| (1<<COM1B0) ;
				break;
			case NON_INVERTING :
				/* COM1A1=1 & COM1A0=0  */
				TCCR1A |=  (1<<COM1B1) ;
				break;
			}


	/* Configure timer control register TCCR1B
	 * 1. Fast Pwm Mode with the TOP in ICR1 WGM12=1 WGM13=1 (Mode Number 14)
	 * 2. set Prescaler
     */
	TCCR1B = (1<<WGM12) | (1<<WGM13) | ((config_ptr->Clock)&0xF7);
}
inline static void TIMER2_InitPwm (const TimersConfigType * config_ptr ,const TimersPwmModeConfig * pwmConfig_Ptr){

	TCNT2 = 0; //Set Timer Initial value

	OCR2  = (uint8)(pwmConfig_Ptr->CompareValue1); // Set Compare Value

	DDRD  = DDRD | (1<<PD7); //set PB3/OC0 as output pin --> pin where the PWM signal is generated from MC.

	/* Configure timer control register
	 * 1. Fast PWM mode FOC2=0
	 * 2. Fast PWM Mode WGM21=1 & WGM20=1
	 * 3. adjust OC0 inverting (COM21=1)| (COM20=1) or  NON inverting COM20=0 & COM21=1
	 * 4. adjust clock
	 */
	if(pwmConfig_Ptr->OCn_Mode == NON_INVERTING)
		TCCR2 = (1<<WGM20) | (1<<WGM21) | (1<<COM21) ;
	else
		TCCR2 = (1<<WGM20) | (1<<WGM21) | (1<<COM21)| (1<<COM20);


	/* set the clock  */

	switch (config_ptr->Clock) {
	case NO_CLOCK:
	case F_CPU_CLOCK:
	case F_CPU_8 :
		TCCR2 |= ( (config_ptr->Clock)&0x07) ;
		break;
	case F_CPU_T2_32 :
		/*  clock = F_CPU/32 CS20=0 CS21=1 CS22=1 (0x03)*/
		TCCR2 |=(0x03) ;
		break;
	case F_CPU_64 :
		/*  clock = F_CPU/64 CS20=1 CS21=0 CS22=0 (0x04)*/
		TCCR2  |= (0x04) ;
		break;
	case F_CPU_T2_128 :
		/*  clock = F_CPU/64 CS20=1 CS21=0 CS22=1 (0x05)*/
		TCCR2  |= (0x05) ;
		break;
	case F_CPU_256 :
		/*  clock = F_CPU/64 CS20=1 CS21=1 CS22=0 (0x06)*/
		TCCR2 |= (0x06) ;
		break;
	case F_CPU_1024 :
		/*  clock = F_CPU/64 CS20=1 CS21=1 CS22=1 (0x07)*/
		TCCR2 |= (0x07) ;
		break;
	}
}

/***************************************************************************/

/*********************************ICU MODE*************************************/

inline static void Icu_init(const TimersIcuConfigType * Config_Ptr){
	/* Configure ICP1/PD6 as i/p pin */
		DDRD &= ~(1<<PD6);

		/* Timer1 always operates in Normal Mode */
		TCCR1A = (1<<FOC1A) | (1<<FOC1B);

		/*
		 * insert the required clock value in the first three bits (CS10, CS11 and CS12)
		 * of TCCR1B Register
		 */
		TCCR1B = (TCCR1B & 0xF8) | (Config_Ptr->clock);
		/*
	     * insert the required edge type in ICES1 bit in TCCR1B Register
		 */
		TCCR1B = (TCCR1B & 0xBF) | ((Config_Ptr->edge)<<6);

		/* Initial Value for Timer1 */
		TCNT1 = 0;

		/* Initial Value for the input capture register */
		ICR1 = 0;

		/* Enable the Input Capture interrupt to generate an interrupt when edge is detected on ICP1/PD6 pin */
		TIMSK |= (1<<TICIE1);

}


/******************************************************************************************************/













