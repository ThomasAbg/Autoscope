/*
 * h.engineControl
 *
 *  Created on: 21 nov. 2018
 *      Author: potato
 */
 
#ifndef ENGINECONTROL_H_
#define ENGINECONTROL_H_

#include <linux/gpio.h>

#define PINNUMBER(port, pinNumber)	(((port) - 1) * 32 + (pinNumber))

typedef struct{
	unsigned int pin_step;		// pin number for step
	unsigned int pin_dir;		// pin number for direction
	unsigned int pin_sleep;		// pin number for sleep
	unsigned int numberStep;	// number of step to do
	unsigned int currentNumberStep; // current number of step did
	
}t_engine;

typedef enum {
	clockwise=0,
	anticlockwise=1
}e_direction;

unsigned int initEngine(void);

unsigned int rotate(t_engine engine, unsigned int pas, e_direction direction);

unsigned int inclinate(t_engine engine1, unsigned int pas, e_direction direction);

unsigned int focus(t_engine *engine, unsigned int pas, e_direction direction);

static void TimerHandler_Rotation(unsigned long unused);	// controle du moteur de rotation

static void  TimerHandler_Tilt(unsigned long unused); // controle des moteurs d'inclinaison

static int __init my_init(void);

void stopOne(t_engine Motor);

void stopall(void);	// Cette fonction permet d'arreter immediatement tous les moteurs

static void __exit my_exit(void);

#endif /* ENGINECONTROL_H_ */

