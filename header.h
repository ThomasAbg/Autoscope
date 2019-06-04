/*
 * h.engineControl
 *
 *  Created on: 21 nov. 2018
 *      Author: potato
 */

#ifndef ENGINECONTROL_H_
#define ENGINECONTROL_H_

#include <linux/kernel.h> 
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/ioctl.h>
#include <linux/timer.h>
#include <linux/init.h> 
#include <linux/gpio.h>
#include <linux/fs.h>

#define PINNUMBER(port, pinNumber)	(((port) - 1) * 32 + (pinNumber))

#define WR_VALUE _IOW('a','1',int32_t*)
#define RD_VALUE _IOR('a','2',int32_t*)


typedef struct{
	unsigned int pin_step;		// pin number for step
	unsigned int pin_dir;		// pin number for direction
	unsigned int number16Step;	// number of step to do
	unsigned int currentnumber16Step; // current number of step did
}t_engine;

typedef enum {
	clockwise=0,
	anticlockwise=1
}e_direction;

typedef struct
{
    int status, dignity, ego;
} query_arg_t;
 

//**************************************************************************

irqreturn_t limit_switch_Azimuth(int irq, void * ident); 

irqreturn_t limit_switch_Rising1(int irq, void * ident);

irqreturn_t limit_switch_Rising2(int irq, void * ident);

irqreturn_t limit_switch_Zoom1(int irq, void * ident);

irqreturn_t limit_switch_Zoom2(int irq, void * ident);

//**************************************************************************

unsigned int rotate(unsigned long pas, e_direction direction);

unsigned int inclinate(unsigned long pas, e_direction direction);

unsigned int zoom(unsigned long pas, e_direction direction); // initialiser moteur et lance timer

//**************************************************************************

void TimerHandler_Rotation(unsigned long unused);	// controle du moteur de rotation

void TimerHandler_Tilt(unsigned long unused); // controle des moteurs d'inclinaison

void TimerHandler_Zoom(unsigned long unused); // controle des moteurs d'inclinaison

//**************************************************************************

int __init my_init (void);

//**************************************************************************

void stopOne(t_engine Motor);

void stopall(void);	// Cette fonction permet d'arreter immediatement tous les moteurs

//**************************************************************************

void __exit my_exit (void);

#endif /* ENGINECONTROL_H_ */

