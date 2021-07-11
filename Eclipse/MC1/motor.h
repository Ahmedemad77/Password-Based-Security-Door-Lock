/******************************************************************************
 *
 * Module:
 *
 * File Name: dc_motor.h
 *
 * Description: providing API's to control functionality  of the motor
 * 				1- rotate clock-wise
 * 				2- rotate anti clock-wise
 * 				3- stop
 *
 * Author: Ahmed Emad
 *
 *******************************************************************************/



#ifndef DC_MOTOR_H_
#define DC_MOTOR_H_

#include "micro_config.h"
#include "std_types.h"
#include "common_macros.h"




/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/*
 * Description : initialize the timer PWM mode without rotating the motor
 */
void motor_init(void);
/*
 * Description : disable the timer PWM mode also stop the motor
 */
void motor_deInit(void);

/*
 *Description :Rotate the motor  clock wise
 */

void motor_rotateClockwise(void);

/*
 *Description :Rotate the motor  clock wise
 */

void motor_rotateAntiClockwise(void);

/*
 *Description :stop the motor
 */

void motor_stop(void);
#endif /* DC_MOTOR_H_ */
