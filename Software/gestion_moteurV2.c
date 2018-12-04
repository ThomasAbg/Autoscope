/************************************************************************\

\************************************************************************/
#include <linux/kernel.h> /* Needed for KERN_INFO */
#include <linux/init.h> /* Needed for the macros */
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/gpio.h>
#include <linux/fs.h>
#include "header.h"

// Variable par pin
// 	Contrôle moteur rotation
#define Step_MRotation 8
#define Dir_MRotation 10
#define Sleep_MRotation 12
// 	Contrôle moteur inclinaison
#define Step_MTilt 11
#define Dir_MTilt 13
#define Sleep_MTilt 15

static void TimerHandler_Rotation(unsigned long unused);	// controle du moteur de rotation
static void TimerHandler_Tilt(unsigned long unused); // controle des moteurs d'inclinaison

t_engine MoteurRotation, MoteurTilt;	// définition des moteurs en variables globales
static struct timer_list timerMotRot, timerMotTilt;		// timer des moteurs
int TableauPin[6] = {Step_MRotation, Dir_MRotation, Sleep_MRotation, Step_MTilt, Dir_MTilt, Sleep_MTilt}; 
union Etat {		// machine état 
char CharEtat;			// sous forme de char
struct {unsigned int Rotation :1;		// état du moteur de rotation 1 = en rotation 0 = à l'arret
		unsigned int Tilt :1;			// état du moteur de d'inclinaison 1 = en rotation 0 = à l'arret
		unsigned int ErreurTimer :1;	// état si il y a une erreur timer = 1 sinon = 0
		unsigned int ErreurGPIO :1;		// état si il y a une erreur d'écriture/lecture avec les I/O = 1 sinon = 0
		unsigned int unused :28;  // pour un int de 32 bit, emplacement libre
		}Moteur;
};
	//t_engine *TableOfMotor[2] = {&MoteurRotation, &MoteurTilt};
//int time_interval = 1;

static int __init my_init (void)	// initilaisation des timers  //**** j'ai enlever static ****
{
	int err, i;
	for(i=1; i < sizeof(TableauPin); i++){	// parcours toutees les PINs
		if ((err = gpio_request(TableauPin[i],THIS_MODULE->name)) != 0) {	
			gpio_free(TableauPin[i]);  // libère la PIN
			return err;
		}
		if ((err = gpio_direction_output(TableauPin[i],1)) != 0) {
			gpio_free(TableauPin[i]);
			return err;
		}
		return 0;
	} 
	return 0;
}

unsigned int rotate(t_engine engine, unsigned int pas, e_direction direction) // initialiser moteur et lance timer
{
	// configuration pin moteur rotation
	MoteurRotation.pin_step = Step_MRotation;	// numéro pin step moteur rotation
	MoteurRotation.pin_dir	= Dir_MRotation;	// numéro pin direction 
	MoteurRotation.pin_sleep= Sleep_MRotation;	// numéro pin sleep
	MoteurRotation.numberStep=pas;	// initialisation du nombre de step a parcourir
	MoteurRotation.currentNumberStep=0;	// initialisation du nombre de step accompli

	init_timer(& timerMotRot);	// création du timer pour le moteur rotation
	timerMotRot.function = TimerHandler_Rotation;	// assiniation de la fonction au timer
	add_timer(& timerMotRot);	// ajout du timer (mise en route du timer)
	return 1;
}

unsigned int inclinate(t_engine engine1, unsigned int pas, e_direction direction)//init moteur et lance timer
{
	// configuration pin moteur inclinaison
	MoteurTilt.pin_step = Step_MTilt;	// numéro pin step moteur rotation
	MoteurTilt.pin_dir	= Dir_MTilt;	// numéro pin direction 
	MoteurTilt.pin_sleep= Sleep_MTilt;	// numéro pin sleep
	MoteurTilt.numberStep=pas;	// initialisation du nombre de step a parcourir
	MoteurTilt.currentNumberStep=0;	// initialisation du nombre de step accompli
	
	init_timer(& timerMotTilt);	// création du timer pour le moteur rotation
	//timer_setup(&timerMotTilt, TimerHandler_Tilt, 0);
	timerMotTilt.function = TimerHandler_Tilt;	// assiniation de la fonction au timer
	add_timer(& timerMotTilt);	// ajout du timer (mise en route du timer)
	return 0;
}	

static void TimerHandler_Rotation(unsigned long unused)	// controle du moteur de rotation
{
	if(MoteurRotation.currentNumberStep < MoteurRotation.numberStep){	// controle distance parcouru
		if(gpio_get_value(Step_MRotation) == 0){		// controle de l'état de la clk (haut ou bas)
			gpio_set_value(Step_MRotation, 1);	// passe a 1 l'état de la clk 
			MoteurRotation.currentNumberStep += 1;	// un step effectuer, incrementation
		}
		else{	// la clk est a état haut mise a l'état bas
			gpio_set_value(Step_MRotation, 0);  // mise en bas de la clk
		}
		mod_timer(& timerMotRot, jiffies + msecs_to_jiffies(500)); // 1ms
	}
	else{	// le nombre de step moteur a été atteind
		del_timer(& timerMotRot);	// destruction du timer pour ce moteur
		}
}

static void TimerHandler_Tilt(unsigned long unused) // controle des moteurs d'inclinaison
{
	if(MoteurTilt.currentNumberStep < MoteurTilt.numberStep){	
		if(gpio_get_value(Step_MTilt) == 0){
			gpio_set_value(Step_MTilt, 1);
			MoteurTilt.currentNumberStep += 1;
		}
		else{
			gpio_set_value(Step_MTilt, 0);  // Off
		}
		mod_timer(& timerMotTilt, jiffies + msecs_to_jiffies(500)); // 1ms
	}
	else{
		del_timer(& timerMotTilt);
	}
}

void stopOne(t_engine Motor)	// permet d'arrter un moteur au choix
{
	if(Motor.pin_step == MoteurRotation.pin_step){	// vérifie si le moteur rotation a été choisi
		MoteurRotation.pin_sleep = 1;	// endort le moteur
	}
	else if(Motor.pin_step == MoteurTilt.pin_step){	// vérifie si le moteur inclinaison a été choisi
		MoteurTilt.pin_sleep = 1;		// endort le moteur
	}
}

void stopall()	// Cette fonction permet d'arreter immediatement tous les moteurs
{
	MoteurRotation.pin_sleep = 1;	// endort le moteur rotation
	MoteurTilt.pin_sleep = 1;		// endort le moteur inclinaison
}

static void __exit my_exit (void)	// destructeur de timer
{
	del_timer(& timerMotRot);	// destruction du timer moteur rotation
	gpio_free(Step_MRotation);	// libération de la PIN du step MRotation
}

module_init(my_init);	// initialisation
module_exit(my_exit);
MODULE_LICENSE("GPL");

// ajouter la fonction de qualibration avec interrupteur
// récupperer les valeurs de retour de chacune des fonctions principales


