//----------------------------------------------------------------------------------------------------------------//
// Projet: Autoscope
// Autor: Thomas ABGRALL
// Subject: Driver Linux of steps motors
// Date: 2018-2019
// IDE: Pluma
//----------------------------------------------------------------------------------------------------------------//

#include <linux/kernel.h> /* Needed for KERN_INFO */
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/init.h> 
#include <linux/gpio.h>
#include <linux/fs.h>
#include "header.h"
//-----------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------Variable par pin-----------------------------------------------//
// 	Contrôle moteur rotation
#define Step_MRotation 16
#define Dir_MRotation 18
#define Sleep_MRotation 22
// 	Contrôle moteur inclinaison
#define Step_MTilt 11
#define Dir_MTilt 13
#define Sleep_MTilt 15
// Capteur de fin de course
#define azimuth 29
#define rising1 31
#define rising2 33
//----------------------------------------------------------------------------------------------------------------//

//------------------------------------------------Prototype fonctions---------------------------------------------//
//---------------------------------------------------------------------------------------------//
void TimerHandler_Rotation(unsigned long unused);	// controle du moteur de rotation
void TimerHandler_Tilt(unsigned long unused); 		// controle des moteurs d'inclinaison
void posInitial(void);
int __init my_init(void);
//----------------------------------------------------------------------------------------------------------------//

//----------------------------------------------------------------------------------------------------------------//
//----------------------------------------Création variables globales---------------------------------------------//
t_engine MoteurRotation, MoteurTilt;	// définition des moteurs en variables globales
 struct timer_list timerMotRot, timerMotTilt;		// timer des moteurs
int TableauPinOutput[6] = {Step_MRotation, Dir_MRotation, Sleep_MRotation, Step_MTilt, Dir_MTilt, Sleep_MTilt}; 
int TableauPinInput[2] = {azimuth, rising1};
union {		// machine état 
	char CharEtat;			// sous forme de char
	struct {
		unsigned char Rotation :1;		// état du moteur de rotation 1 = en rotation 0 = à l'arret
		unsigned char Tilt :1;			// état du moteur de d'inclinaison 1 = en rotation 0 = à l'arret
		unsigned char ErreurTimer :1;	// état si il y a une erreur timer = 1 sinon = 0
		unsigned char ErreurGPIO :1;	// état si il y a une erreur d'écriture/lecture avec les I/O = 1 sinon = 0
		unsigned char unused :4;  		// complete le reste du char avec 4 bits libre
	}Moteur;
}Etat;
int Nbofstep;
	//t_engine *TableOfMotor[2] = {&MoteurRotation, &MoteurTilt};

//----------------------------------------------------------------------------------------------------------------//
//------------------------------------------Fonction initialisation du driver-------------------------------------//
int __init my_init (void)	// initilaisation des timers 
{
	int i, erreurGPIOIn, erreurGPIOOut, erreurInterruption1, erreurInterruption2;
	printk(KERN_INFO "Initailisation du driver moteur\n"); 
	// parcours toutes les PINs en Sortie pour liberer les pins pour quelles soient utilisables
	for(i=1; i < sizeof(TableauPinOutput); i++){	
		// associe la pin au module et contrôle qu'il n'y est pas erreur lors de cette action
		if ((erreurGPIOIn = gpio_request(TableauPinOutput[i],THIS_MODULE->name)) != 0) {	
			gpio_free(TableauPinOutput[i]);  // libère la PIN si il y a une erreur dans gpio_request
			printk(KERN_INFO "erreurGPIOIn pin n°%d = %d\n",i ,erreurGPIOIn); 
			return erreurGPIOIn;
		}
		// met la pin en sortie et contrôle qu'il n'y est pas erreur lors de cette action
		if ((erreurGPIOIn = gpio_direction_output(TableauPinOutput[i],1)) != 0) { 
			gpio_free(TableauPinOutput[i]);  // libère la PIN si si il y a une erreur dans gpio_direction_output
			printk(KERN_INFO "erreurGPIOIn pin n°%d = %d\n",i ,erreurGPIOIn); 
			return erreurGPIOIn;
		} 
	} 
	// parcours toutes les PINs en Entrée pour liberer les pins pour quelles soient utilisables
	for(i=1; i < sizeof(TableauPinInput); i++){	
		// associe la pin au module et contrôle qu'il n'y est pas erreur lors de cette action
		if ((erreurGPIOOut = gpio_request(TableauPinInput[i],THIS_MODULE->name)) != 0) {	
			gpio_free(TableauPinInput[i]);  // libère la PIN
			printk(KERN_INFO "erreurGPIOOut pin n°%d = %d\n",i ,erreurGPIOOut); 
			return erreurGPIOOut;
		}
		// met la pin en sortie et contrôle qu'il n'y est pas erreur lors de cette action
		if ((erreurGPIOOut = gpio_direction_input(TableauPinInput[i])) != 0) {
			gpio_free(TableauPinInput[i]);
			printk(KERN_INFO "erreurGPIOOut pin n°%d = %d\n",i ,erreurGPIOOut); 
			return erreurGPIOOut;
		}
	} 
    // création de l'interruption premier bouton poussoir 1
    if ((erreurInterruption1 = request_irq(gpio_to_irq(azimuth), limit_switch_Azimuth, IRQF_SHARED 
    	| IRQF_TRIGGER_RISING, THIS_MODULE->name, THIS_MODULE->name)) != 0) {
            gpio_free(azimuth);
            printk(KERN_INFO "erreurInterruption1 = %d\n",erreurInterruption1);
            return erreurInterruption1;
    }
	// création de l'interruption premier bouton poussoir 2
    if ((erreurInterruption2 = request_irq(gpio_to_irq(rising1), limit_switch_Rising1, IRQF_SHARED 
    	| IRQF_TRIGGER_RISING, THIS_MODULE->name, THIS_MODULE->name)) != 0) {
            gpio_free(rising1);
            printk(KERN_INFO "erreurInterruption2 = %d\n",erreurInterruption2);
            return erreurInterruption2;
    }
	printk("erreur");
	Etat.Moteur.Rotation = 0;
	Etat.Moteur.Tilt = 0;
	posInitial();
	return 0;
}
//---------------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------------//
//------------------------------------------Initialisation telescope---------------------------------------------//
void posInitial(void)
{
	printk(KERN_INFO "Init position telescope\n");
	rotate(100000, clockwise);			// le moteur rotation va tourner jusqu'à l'interruption du contacteur
	inclinate(100000, clockwise);		// le moteur inclinaison va tourner jusqu'à l'interruption du contacteur
}
//----------------------------------------------------------------------------------------------------------------//

//----------------------------------------------------------------------------------------------------------------//
//------------------------------------------Fonction lance rotation-----------------------------------------------//
unsigned int rotate(unsigned long pas, e_direction direction) // initialiser moteur et lance timer
{
	printk(KERN_INFO "Rotation motor will run\n");
	if(Etat.Moteur.Rotation == 0){
		// configuration pin moteur rotation
		MoteurRotation.pin_step = Step_MRotation;	// numéro pin step moteur rotation
		MoteurRotation.pin_dir	= Dir_MRotation;	// numéro pin direction 
		MoteurRotation.pin_sleep= Sleep_MRotation;	// numéro pin sleep
		MoteurRotation.numberStep=pas;		// initialisation du nombre de step a parcourir
		MoteurRotation.currentNumberStep=0;	// initialisation du nombre de step accompli

		init_timer(& timerMotRot);	// création du timer pour le moteur rotation
		timerMotRot.function = TimerHandler_Rotation;	// assiniation de la fonction au timer
		add_timer(& timerMotRot);	// ajout du timer (mise en route du timer)
	}
	else{
		printk(KERN_INFO "Moteur rotation occupé pour le moment\n");
	}
	return 0;
}
//----------------------------------------------------------------------------------------------------------------//

//----------------------------------------------------------------------------------------------------------------//
//------------------------------------------Fonction lance inclinaison--------------------------------------------//
unsigned int inclinate(unsigned long pas, e_direction direction)//init moteur et lance timer
{
	printk(KERN_INFO "Inclinate motor will run\n");
	if(Etat.Moteur.Tilt == 0){
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
	}
	else{
		printk(KERN_INFO "Moteur inclinaison occupé pour le moment\n");
	}
	return 0;
}	
//----------------------------------------------------------------------------------------------------------------//

//################################################################################################################//
//------------------------------------------Fonctions Appeler a la fin de leur timer------------------------------//
void TimerHandler_Rotation(unsigned long unused)	// controle du moteur de rotation
{
	int erreur_set_value;
	printk(KERN_INFO "Handler rotation\n");
	Etat.Moteur.Rotation = 1;
	if(MoteurRotation.currentNumberStep < MoteurRotation.numberStep){	// controle distance parcouru
		if(gpio_get_value(Step_MRotation) == 0){	// controle de l'état de la clk (haut ou bas)
			gpio_set_value(Step_MRotation, 1);	// passe a 1 l'état de la step
			if((erreur_set_value = gpio_get_value(Step_MRotation)) != 1)	 
				printk(KERN_INFO "erreur_set_value: La valeur 1 n'a pas pu etre écrite dans le pin step rotation");
			MoteurRotation.currentNumberStep += 1;		// un step effectuer, incrementation
		}
		else{				// la clk est a état haut mise a l'état bas
			gpio_set_value(Step_MRotation, 0);	// mise en bas de la step
			if((erreur_set_value = gpio_get_value(Step_MRotation)) != 0)  
				printk(KERN_INFO "erreur_set_value: La valeur 0 n'a pas pu etre écrite dans le pin step rotation");
		}
		mod_timer(& timerMotRot, jiffies + msecs_to_jiffies(500)); // 1ms
	}
	else{					// le nombre de step moteur a été atteind
		del_timer(& timerMotRot);	// destruction du timer pour ce moteur
		Etat.Moteur.Rotation = 0;
	}
}

void TimerHandler_Tilt(unsigned long unused) // controle des moteurs d'inclinaison
{
	int erreur_set_value;
	printk(KERN_INFO "Handler tilt\n");
	Etat.Moteur.Tilt = 1;
	if(MoteurTilt.currentNumberStep < MoteurTilt.numberStep){	
		if(gpio_get_value(Step_MTilt) == 0){
			gpio_set_value(Step_MTilt, 1);	// mise en haut de la step
			if((erreur_set_value = gpio_get_value(Step_MTilt)) != 1)
				printk(KERN_INFO "erreur_set_value: La valeur 1 n'a pas pu etre écrite dans le pin step inclinaison");
			MoteurTilt.currentNumberStep += 1;
		}
		else{
			gpio_set_value(Step_MTilt, 0);	// mise en bas de la step
			if((erreur_set_value = gpio_get_value(Step_MTilt)) != 0)
				printk(KERN_INFO "erreur_set_value: La valeur 0 n'a pas pu etre écrite dans le pin step inclinaison");
		}
		mod_timer(& timerMotTilt, jiffies + msecs_to_jiffies(500)); // 1ms
	}
	else{
		del_timer(& timerMotTilt);
		Etat.Moteur.Tilt = 0;
	}
}
//#################################################################################################################//

//#################################################################################################################//
//########################################Interruption capteurs fin de courses#####################################// 
irqreturn_t limit_switch_Azimuth(int irq, void * ident)	//action lors de l'interruption contacteur rotation
{
        printk(KERN_INFO "%s: %s()\n", THIS_MODULE->name, __FUNCTION__);
        printk(KERN_INFO "Nombre de pas effectuer par le moteur rotation = %d" ,MoteurRotation.currentNumberStep); // £££££££££££££££CETTE LIGNE SERA A SUPPRIMER APPORT AVOIR FAIT LE RATIO PAS/ANGLE£££££££££££££££££££
		del_timer(& timerMotRot);	// destruction du timer pour ce moteur
		Etat.Moteur.Rotation = 0;	// mise a 0 de l'état du moteur de rotation
        return IRQ_HANDLED;
}

irqreturn_t limit_switch_Rising1(int irq, void * ident)	//action lors de l'interruption contacteur rotation
{
        printk(KERN_INFO "%s: %s()\n", THIS_MODULE->name, __FUNCTION__);
		printk(KERN_INFO "Nombre de pas effectuer par le moteur inclinaison = %d" ,MoteurTilt.currentNumberStep); // £££££££££££££££CETTE LIGNE SERA A SUPPRIMER APPORT AVOIR FAIT LE RATIO PAS/ANGLE£££££££££££££££££££
		del_timer(& timerMotTilt);	// destruction du timer pour ce moteur
		Etat.Moteur.Tilt = 0;		// mise a 0 de l'état du moteur d'inclinaison
        return IRQ_HANDLED;
}
//###############################################################################################################//

//---------------------------------------------------------------------------------------------------------------//
//------------------------------------------Fonctions Arret------------------------------------------------------//
void stopOne(t_engine Motor)	// permet d'arrter un moteur au choix
{
	printk(KERN_INFO "Stop_One\n");
	if(Motor.pin_step == MoteurRotation.pin_step){	// vérifie si le moteur rotation a été choisi
		MoteurRotation.pin_sleep = 1;	// endort le moteur
		del_timer(& timerMotRot);	// destruction du timer moteur rotation
		gpio_free(Step_MRotation);	// libération de la PIN du step MRotation
	}
	else if(Motor.pin_step == MoteurTilt.pin_step){	// vérifie si le moteur inclinaison a été choisi
		MoteurTilt.pin_sleep = 1;		// endort le moteur
	}
}

void stopall()	// Cette fonction permet d'arreter immediatement tous les moteurs
{
	printk(KERN_INFO "stop all\n");
	MoteurRotation.pin_sleep = 1;	// endort le moteur rotation
	MoteurTilt.pin_sleep = 1;		// endort le moteur inclinaison
}
//---------------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------------//
//------------------------------------------Fonctions Exit-------------------------------------------------------//
void __exit my_exit (void)	// destructeur de timer
{
	printk(KERN_INFO "EXIT\n");
	del_timer(& timerMotRot);	// destruction du timer moteur rotation
	del_timer(& timerMotTilt);	// destruction du timer moteur inclinaison
	gpio_free(Step_MRotation);	// libération de la PIN du step MRotation
	gpio_free(Step_MTilt);		// libération de la PIN du step MRotation
	free_irq(gpio_to_irq(azimuth), THIS_MODULE->name);	// désactive interruption azimute
	free_irq(gpio_to_irq(rising1), THIS_MODULE->name);	// désactive interruption inclinaison
    gpio_free(azimuth);	// libération pin interrupter azimute
    gpio_free(rising1);	// libération pin interrupter inclinaison
}
//---------------------------------------------------------------------------------------------------------------//
//------------------------------------------Fonctions lancer au démarrage du driver------------------------------//

module_init(my_init);	// definition de l'initialiser du timer
module_exit(my_exit);	// definition du destructeur du timer
MODULE_LICENSE("GPL");	// licence général public
//---------------------------------------------------------------------------------------------------------------//

// ajouter la fonction de qualibration avec interrupteur
// récupperer les valeurs de retour de chacune des fonctions principales


