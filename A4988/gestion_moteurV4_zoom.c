//----------------------------------------------------------------------------------------------------------------//
// Projet: Autoscope
// Autor: Thomas ABGRALL
// Subject: Driver Linux of steps motors
// Date: 2018-2019
// IDE: Pluma
//----------------------------------------------------------------------------------------------------------------//

/*
Il peut être chargé dynamiquement sans avoir besoin de recompiler le noyau (avec la commande insmod ou modprobe) ou de redémarrer le système. 
Chargement/Déchargement d'un module
	avec tous les drivers associés
	Pour ce qui est du chargement et du déchargement d'un module, il faut utiliser la commande modprobe :
	Charger
	Pour charger manuellement le module "3c59x" (il y a de grandes chances qu'il soit chargé si vous en avez 		besoin…) :

	$ sudo modprobe -a 3c59x

Lister

	Voir la liste des modules (on peut aussi utiliser l'utilitaire "modconf" - cf plus bas) :

	lsmod
	

LSMOD : liste de tous les modules présents en mémoire.

INSMOD: insérer un module en mémoire.
        (ex: " insmod /lib/modules/2.0.36/misc/snd-midi.o ") le driver seul

RMMOD: enlever un module de la mémoire. (voir LSMOD)
       (ex: " rmmod snd-midi ") 

DEPMOD: créer une liste des modules utilisables avec le noyau actuel.
        La liste est créée dans /lib/modules/(version noyau)/ et se nomme 
        "modules.dep" . C'est un fichier texte analogue à un fichier Makefile,
        en ce sens qu'il liste les modules disponibles, et les "sous-modules"
        (également eux-même des modules) nécessaires au bon fonctionnement de 
        ces modules. (un peu comme la liste des fichiers objets 
        nécéssaires à la compilation de l'éxecutable dans un Makefile).
        
        charger image dans SD
DD if=Chemin_image_source of=chemin_cible  

nom driver officiel a4988

*/

#include <linux/kernel.h> /* Needed for KERN_INFO */
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/init.h> 
#include <linux/gpio.h>
#include <linux/fs.h>
#include "header.h"

// 	Contrôle moteur rotation
#define Step_MRotation 16
#define Dir_MRotation 18
#define Enable_MRotation 22
// 	Contrôle moteur inclinaison
#define Step_MTilt 11
#define Dir_MTilt 13
#define Enable_MTilt 15
// Contrôle moteur zoom
#define MZoom_Step 3333
#define Dir_MZoom 3333
#define Enable_MZoom 3333
// Capteur de fin de course
#define azimuth 29
#define rising1 31
#define rising2 33
#define zoom1 3333
#define zoom2 3333
// Configuration mode pas rotation
#define ConfigPasRot1 3333
#define ConfigPasRot2 3333
#define ConfigPasRot3 3333
// Configuration mode pas inclaison
#define ConfigPasIncl1 3333
#define ConfigPasIncl2 3333
#define ConfigPasIncl3 3333
// Configuration mode pas room
#define ConfigPasZoom1 3333
#define ConfigPasZoom2 3333
#define ConfigPasZoom3 3333

//----------------------------------------------------------------------------------------------------------------//

//------------------------------------------------Prototype fonctions---------------------------------------------//
//----------------------------------------------------------------------------------------------------------------//
void TimerHandler_Rotation(unsigned long unused);	// controle du moteur de rotation
void TimerHandler_Tilt(unsigned long unused); 		// controle des moteurs d'inclinaison
void posInitial(void);
int __init my_init(void);
//----------------------------------------------------------------------------------------------------------------//

//----------------------------------------------------------------------------------------------------------------//
//----------------------------------------Création variables globales---------------------------------------------//
t_engine MoteurRotation, MoteurTilt, MoteurZoom;	// définition des moteurs en variables globales
struct timer_list timerMotRot, timerMotTilt, timerMotZoom;		// timer des moteurs
int TableauPinOutput[17] = {
	Step_MRotation, Dir_MRotation, Enable_MRotation, 
	Step_MTilt, Dir_MTilt, Enable_MTilt, 
	MZoom_Step, Dir_MZoom, Enable_MZoom, 
	ConfigPasRot1, ConfigPasRot2, ConfigPasRot3, 
	ConfigPasIncl1, ConfigPasIncl2, ConfigPasIncl3, 
	ConfigPasZoom1, ConfigPasZoom2, ConfigPasZoom3
	}; 
int TableauPinInput[4] = {azimuth, rising1, rising2, zoom1, zoom2};
union {		// machine état 
	char CharEtat;			// sous forme de char
	struct {
		unsigned char Rotation :1;		// état du moteur de rotation 1 = en rotation 0 = à l'arret
		unsigned char Tilt :1;			// état du moteur de d'inclinaison 1 = en rotation 0 = à l'arret
		unsigned char Zoom :1;			// éttat du moteur de zoom 1 = en rotation 0 = à l'arret
		unsigned char ErreurTimer :1;	// état si il y a une erreur timer = 1 sinon = 0
		unsigned char ErreurGPIO :1;	// état si il y a une erreur d'écriture/lecture avec les I/O = 1 sinon = 0
		unsigned char unused :4;  		// complete le reste du char avec 4 bits libre
	}Moteur;
}Etat;
	//t_engine *TableOfMotor[2] = {&MoteurRotation, &MoteurTilt};

//----------------------------------------------------------------------------------------------------------------//
//------------------------------------------Fonction initialisation du driver-------------------------------------//
int __init my_init (void)	// initilaisation des timers 
{
	printk(KERN_INFO "DRIVERMOTOR: Initialisation du driver moteur\n"); 
	int i, erreurGPIOIn, erreurGPIOOut, 
		erreurInterruption1, erreurInterruption2, erreurInterruption3, erreurInterruption4, erreurInterruption5;
	// parcours toutes les PINs en Sortie pour liberer les pins pour quelles soient utilisables
	for(i=0; i < sizeof(TableauPinOutput); i++){	
		// associe la pin au module et contrôle qu'il n'y est pas erreur lors de cette action
		if ((erreurGPIOOut = gpio_request(TableauPinOutput[i],THIS_MODULE->name)) != 0) {	
			gpio_free(TableauPinOutput[i]);  // libère la PIN si il y a une erreur dans gpio_request
			printk(KERN_WARNING "DRIVERMOTOR: erreurGPIOOut pin n°%d = %d\n",i ,erreurGPIOOut); 
			return erreurGPIOOut;
		}
		// met la pin en sortie et contrôle qu'il n'y est pas erreur lors de cette action
		if ((erreurGPIOOut = gpio_direction_output(TableauPinOutput[i],1)) != 0) { 
			gpio_free(TableauPinOutput[i]);  // libère la PIN si si il y a une erreur dans gpio_direction_output
			printk(KERN_WARNING "DRIVERMOTOR: erreurGPIOOut pin n°%d = %d\n",i ,erreurGPIOOut); 
			return erreurGPIOOut;
		} 
	} 
	// parcours toutes les PINs en Entrée pour liberer les pins pour quelles soient utilisables
	for(i=0; i < sizeof(TableauPinInput); i++){	
		// associe la pin au module et contrôle qu'il n'y est pas erreur lors de cette action
		if ((erreurGPIOIn = gpio_request(TableauPinInput[i],THIS_MODULE->name)) != 0) {	
			gpio_free(TableauPinInput[i]);  // libère la PIN
			printk(KERN_WARNING "DRIVERMOTOR: erreurGPIOOut pin n°%d = %d\n",i ,erreurGPIOIn);
			return erreurGPIOIn;
		}
		// met la pin en sortie et contrôle qu'il n'y est pas erreur lors de cette action
		if ((erreurGPIOIn = gpio_direction_input(TableauPinInput[i])) != 0) {
			gpio_free(TableauPinInput[i]);
			printk(KERN_WARNING "DRIVERMOTOR: erreurGPIOIn pin n°%d = %d\n",i ,erreurGPIOIn); 
			return erreurGPIOIn;
		}
	} 
    // création de l'interruption bouton de rotation
    if ((erreurInterruption1 = request_irq(gpio_to_irq(azimuth), limit_switch_Azimuth, IRQF_SHARED 
    	| IRQF_TRIGGER_RISING, THIS_MODULE->name, THIS_MODULE->name)) != 0) {  	
            gpio_free(azimuth);
            printk(KERN_WARNING "DRIVERMOTOR: erreurInterruption1 = %d\n",erreurInterruption1);
            return erreurInterruption1;
    }
	// création de l'interruption pour le capteur fin de course inclinaison 1
    if ((erreurInterruption2 = request_irq(gpio_to_irq(rising1), limit_switch_Rising1, IRQF_SHARED 
    	| IRQF_TRIGGER_RISING, THIS_MODULE->name, THIS_MODULE->name)) != 0) {
            gpio_free(rising1);
            printk(KERN_WARNING "DRIVERMOTOR: erreurInterruption2 = %d\n",erreurInterruption2);
            return erreurInterruption2;
    }
    // création de l'interruption pour le capteur fin de course inclinaison 2
    if ((erreurInterruption3 = request_irq(gpio_to_irq(rising2), limit_switch_Rising2, IRQF_SHARED 
    	| IRQF_TRIGGER_RISING, THIS_MODULE->name, THIS_MODULE->name)) != 0) {
            gpio_free(rising2);
            printk(KERN_WARNING "DRIVERMOTOR: erreurInterruption3 = %d\n",erreurInterruption3);
            return erreurInterruption3;
    }
    // création de l'interruption pour le capteur fin de course zoom1
    if ((erreurInterruption4 = request_irq(gpio_to_irq(zoom1), limit_switch_Zoom1, IRQF_SHARED 
    	| IRQF_TRIGGER_RISING, THIS_MODULE->name, THIS_MODULE->name)) != 0) {
            gpio_free(zoom1);
            printk(KERN_WARNING "DRIVERMOTOR: erreurInterruption4 = %d\n",erreurInterruption4);
            return erreurInterruption4;
    }
    // création de l'interruption pour le capteur fin de course zoom2
    if ((erreurInterruption5 = request_irq(gpio_to_irq(zoom2), limit_switch_Zoom2, IRQF_SHARED 
    	| IRQF_TRIGGER_RISING, THIS_MODULE->name, THIS_MODULE->name)) != 0) {
            gpio_free(zoom2);
            printk(KERN_WARNING "DRIVERMOTOR: erreurInterruption5 = %d\n",erreurInterruption5);
            return erreurInterruption5;
    }
	Etat.Moteur.Rotation = 0;
	Etat.Moteur.Tilt = 0;
	Etat.Moteur.Zoom = 0;
	posInitial();
	return 0;
}
//---------------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------------//
//------------------------------------------Initialisation telescope---------------------------------------------//
void posInitial(void)
{
	printk(KERN_INFO "DRIVERMOTOR: Init position telescope\n");
	rotate(100, clockwise);			// le moteur rotation va tourner jusqu'à l'interruption du contacteur ou xième pas
	inclinate(100, clockwise);		// le moteur inclinaison va tourner jusqu'à l'interruption du contacteur ou xième pas
	zoom(100, clockwise);			// le moteur de zoom va tourner jusqu'au ou xième pas ou interruption zoom1 ou zoom2
}
//----------------------------------------------------------------------------------------------------------------//

//----------------------------------------------------------------------------------------------------------------//
//------------------------------------------Fonction lance rotation-----------------------------------------------//
unsigned int rotate(unsigned long pas, e_direction direction) // initialiser moteur et lance timer
{
	printk(KERN_INFO "DRIVERMOTOR: Rotation motor will run\n");
	if(Etat.Moteur.Rotation == 0){
		// configuration pin moteur rotation
		MoteurRotation.pin_step = Step_MRotation;	// numéro pin step moteur rotation
		Dir_MRotation = MoteurRotation.pin_dir = direction;	// numéro pin direction 
		MoteurRotation.pin_sleep= Enable_MRotation;	// numéro pin sleep
		MoteurRotation.number16Step=pas;		// initialisation du nombre de step a parcourir
		MoteurRotation.currentnumber16Step=0;	// initialisation du nombre de step accompli

		init_timer(& timerMotRot);	// création du timer pour le moteur rotation
		timerMotRot.function = TimerHandler_Rotation;	// assiniation de la fonction au timer
		add_timer(& timerMotRot);	// ajout du timer (mise en route du timer)
	}
	else{
		printk(KERN_INFO "DRIVERMOTOR: Moteur rotation occupé pour le moment\n");
	}
	return 0;
}
//----------------------------------------------------------------------------------------------------------------//

//----------------------------------------------------------------------------------------------------------------//
//------------------------------------------Fonction lance inclinaison--------------------------------------------//
unsigned int inclinate(unsigned long pas, e_direction direction)//init moteur et lance timer
{
	printk(KERN_INFO "DRIVERMOTOR: Inclinate motor will run\n");
	if(Etat.Moteur.Tilt == 0){
		// configuration pin moteur inclinaison
		MoteurTilt.pin_step = Step_MTilt;	// numéro pin step moteur rotation
		Dir_MTilt = MoteurTilt.pin_dir = direction;	// numéro pin direction 
		MoteurTilt.pin_sleep= Enable_MTilt;	// numéro pin sleep
		MoteurTilt.number16Step=pas;	// initialisation du nombre de step a parcourir
		MoteurTilt.currentnumber16Step=0;	// initialisation du nombre de step accompli
		
		init_timer(& timerMotTilt);	// création du timer pour le moteur rotation
		//timer_setup(&timerMotTilt, TimerHandler_Tilt, 0);
		timerMotTilt.function = TimerHandler_Tilt;	// assiniation de la fonction au timer
		add_timer(& timerMotTilt);	// ajout du timer (mise en route du timer)
	}
	else{
		printk(KERN_INFO "DRIVERMOTOR: Moteur inclinaison occupé pour le moment\n");
	}
	return 0;
}	
//----------------------------------------------------------------------------------------------------------------//


//----------------------------------------------------------------------------------------------------------------//
//------------------------------------------Fonction zoom-----------------------------------------------//
unsigned int zoom(unsigned long pas, e_direction direction) // initialiser moteur et lance timer
{
	printk(KERN_INFO "DRIVERMOTOR: Rotation motor zoom will run\n");
	if(Etat.Moteur.Zoom == 0){
		// configuration pin moteur rotation
		MoteurZoom.pin_step = Step_MZoom;	// numéro pin step moteur rotation
		Dir_MZoom = MoteurZoom.pin_dir = direction;	// numéro pin direction 
		MoteurZoom.pin_sleep= Enable_MZoom;	// numéro pin sleep
		MoteurZoom.number16Step=pas;		// initialisation du nombre de step a parcourir
		MoteurZoom.currentnumber16Step=0;	// initialisation du nombre de step accompli

		init_timer(& timerMotZoom);	// création du timer pour le moteur rotation
		timerMotZoom.function = TimerHandler_Zoom;	// assiniation de la fonction au timer
		add_timer(& timerMotRot);	// ajout du timer (mise en route du timer)
	}
	else{
		printk(KERN_INFO "DRIVERMOTOR: Moteur rotation occupé pour le moment\n");
	}
	return 0;
}
//----------------------------------------------------------------------------------------------------------------//

//################################################################################################################//
//------------------------------------------Fonctions Appeler a la fin de leur timer------------------------------//
void TimerHandler_Rotation(unsigned long unused)	// controle du moteur de rotation
{
	int erreur_set_value, nbPascomplet;
	printk(KERN_INFO "DRIVERMOTOR: Handler rotation\n");
	Etat.Moteur.Rotation = 1;
	nbPascomplet = MoteurRotation.number16Step / 16;
	if(MoteurRotation.currentnumber16Step < MoteurRotation.number16Step){
		if(MoteurRotation.currentnumber16Step < nbPascomplet){ // tant que l'on peu faire des pas, mode full pas
			ConfigPasRot1 = 0;
			ConfigPasRot2 = 0;
			ConfigPasRot3 = 0;
		}
		else if(MoteurRotation.currentnumber16Step < (MoteurRotation.number16Step % 16)){ // si full pas impossible 16ème
			ConfigPasRot1 = 1;	// configuration 16ème de pas
			ConfigPasRot2 = 1;
			ConfigPasRot3 = 1;
		}
		if(gpio_get_value(Step_MRotation) == 0){	// controle de l'état de la clk (haut ou bas)
			gpio_set_value(Step_MRotation, 1);	// passe a 1 l'état de la step
			if((erreur_set_value = gpio_get_value(Step_MRotation)) != 1)	 
				printk(KERN_WARNING "DRIVERMOTOR: erreur_set_value: La valeur 1 n'a pas pu etre écrite dans le pin step rotation");
			MoteurRotation.currentnumber16Step += 1;		// un step effectuer, incrementation
		}
		else{				// la clk est a état haut mise a l'état bas
			gpio_set_value(Step_MRotation, 0);	// mise en bas de la step
			if((erreur_set_value = gpio_get_value(Step_MRotation)) != 0)  
				printk(KERN_WARNING "DRIVERMOTOR: erreur_set_value: La valeur 0 n'a pas pu etre écrite dans le pin step rotation");
		}
		if(activeTimer = mod_timer(& timerMotRot, jiffies + msecs_to_jiffies(1)) != 1){
			printk(KERN_WARNING "DRIVERMOTOR: timerMotRot n'a pas pu être activé");
		}
	}
	else{					// le nombre de step moteur a été atteind
		del_timer(& timerMotRot);	// destruction du timer pour ce moteur
		Etat.Moteur.Rotation = 0;
	}
}

void TimerHandler_Tilt(unsigned long unused) // controle des moteurs d'inclinaison
{
	int erreur_set_value;
	printk(KERN_INFO "DRIVERMOTOR: Handler tilt\n");
	Etat.Moteur.Tilt = 1;
	nbPascomplet = MoteurTilt.number16Step / 16;
	if(MoteurTilt.currentnumber16Step < MoteurTilt.number16Step){
		if(MoteurTilt.currentnumber16Step < nbPascomplet){ // si full pas impossible 16ème
			ConfigPasRot1 = 0;
			ConfigPasRot2 = 0;
			ConfigPasRot3 = 0;
		}
		else if(MoteurTilt.currentnumber16Step < (MoteurTilt.number16Step % 16)){	// si full pas impossible 16ème
			ConfigPasRot1 = 1;
			ConfigPasRot2 = 1;
			ConfigPasRot3 = 1;
		}
		if(gpio_get_value(Step_MTilt) == 0){
			gpio_set_value(Step_MTilt, 1);	// mise en haut de la step
			if((erreur_set_value = gpio_get_value(Step_MTilt)) != 1)
				printk(KERN_INFO "DRIVERMOTOR: erreur_set_value: La valeur 1 n'a pas pu etre écrite dans le pin step inclinaison");
			MoteurTilt.currentnumber16Step += 1;
		}
		else{
			gpio_set_value(Step_MTilt, 0);	// mise en bas de la step
			if((erreur_set_value = gpio_get_value(Step_MTilt)) != 0)
				printk(KERN_WARNING "DRIVERMOTOR: erreur_set_value: La valeur 0 n'a pas pu etre écrite dans le pin step inclinaison");
		}
		if(activeTimer = mod_timer(& timerMotTilt, jiffies + msecs_to_jiffies(1)) != 1){
			printk(KERN_WARNING "DRIVERMOTOR: timerMotTilt n'a pas pu être activé");
		}	
	}
	else{
		del_timer(& timerMotTilt);
		Etat.Moteur.Tilt = 0;
	}
}

void TimerHandler_Zoom(unsigned long unused) // controle des moteurs d'inclinaison
{
	int erreur_set_value;
	printk(KERN_INFO "DRIVERMOTOR: Handler tilt\n");
	Etat.Moteur.Zoom = 1;
	nbPascomplet = MoteurZoom.number16Step / 16;
	if(MoteurZoom.currentnumber16Step < MoteurZoom.number16Step){
		if(MoteurZoom.currentnumber16Step < nbPascomplet){ // si full pas impossible 16ème
			ConfigPasZoom1 = 0;
			ConfigPasZoom2 = 0;
			ConfigPasZoom3 = 0;
		}
		else if(MoteurZoom.currentnumber16Step < (MoteurZoom.number16Step % 16)){	// si full pas impossible 16ème
			ConfigPasZoom1 = 1;
			ConfigPasZoom2 = 1;
			ConfigPasZoom3 = 1;
		}
		if(gpio_get_value(Step_MZoom) == 0){
			gpio_set_value(Step_MZoom, 1);	// mise en haut de la step
			if((erreur_set_value = gpio_get_value(Step_MZoom)) != 1)
				printk(KERN_INFO "DRIVERMOTOR: erreur_set_value: La valeur 1 n'a pas pu etre écrite dans le pin step Zoom");
			MoteurZoom.currentnumber16Step += 1;
		}
		else{
			gpio_set_value(Step_MZoom, 0);	// mise en bas de la step
			if((erreur_set_value = gpio_get_value(Step_MZoom)) != 0)
				printk(KERN_WARNING "DRIVERMOTOR: erreur_set_value: La valeur 0 n'a pas pu etre écrite dans le pin step zoom");
		}
		if(activeTimer = mod_timer(& timerMotZoom, jiffies + msecs_to_jiffies(50)) != 1){
			printk(KERN_WARNING "DRIVERMOTOR: timerMotZoom n'a pas pu être activé");
		}	
	}
	else{
		del_timer(& timerMotZoom);
		Etat.Moteur.Zoom = 0;
	}
}

//#################################################################################################################//

//#################################################################################################################//
//########################################Interruption capteurs fin de courses#####################################// 
irqreturn_t limit_switch_Azimuth(int irq, void * ident)	//action lors de l'interruption contacteur rotation
{
        printk(KERN_INFO "DRIVERMOTOR: %s: %s()\n", THIS_MODULE->name, __FUNCTION__);
        printk(KERN_INFO "DRIVERMOTOR: Nombre de pas effectuer par le moteur rotation = %d", MoteurRotation.currentnumber16Step); // £££££££££££££££CETTE LIGNE SERA A SUPPRIMER APPORT AVOIR FAIT LE RATIO PAS/ANGLE£££££££££££££££££££
		del_timer(& timerMotRot);	// destruction du timer pour ce moteur
		Etat.Moteur.Rotation = 0;	// mise a 0 de l'état du moteur de rotation
		MoteurTilt.number16Step = 0;
        return IRQ_HANDLED;
}

irqreturn_t limit_switch_Rising1(int irq, void * ident)	//action lors de l'interruption contacteur rotation
{
        printk(KERN_INFO "DRIVERMOTOR: %s: %s()\n", THIS_MODULE->name, __FUNCTION__);
		printk(KERN_INFO "DRIVERMOTOR: Nombre de pas effectuer par le moteur inclinaison = %d" , MoteurTilt.currentnumber16Step); // £££££££££££££££CETTE LIGNE SERA A SUPPRIMER APPORT AVOIR FAIT LE RATIO PAS/ANGLE£££££££££££££££££££
		del_timer(& timerMotTilt);	// destruction du timer pour ce moteur
		Etat.Moteur.Tilt = 0;		// mise a 0 de l'état du moteur d'inclinaison
		MoteurTilt.number16Step = 0;
		
        return IRQ_HANDLED;
}

irqreturn_t limit_switch_Rising2(int irq, void * ident)	//action lors de l'interruption contacteur rotation
{
        printk(KERN_INFO "DRIVERMOTOR: %s: %s()\n", THIS_MODULE->name, __FUNCTION__);
		printk(KERN_INFO "DRIVERMOTOR: Nombre de pas effectuer par le moteur inclinaison = %d" , MoteurTilt.currentnumber16Step); // £££££££££££££££CETTE LIGNE SERA A SUPPRIMER APPORT AVOIR FAIT LE RATIO PAS/ANGLE£££££££££££££££££££
		del_timer(& timerMotTilt);	// destruction du timer pour ce moteur
		Etat.Moteur.Tilt = 0;		// mise a 0 de l'état du moteur d'inclinaison
		MoteurTilt.number16Step = 0;
		
        return IRQ_HANDLED;
}

irqreturn_t limit_switch_Zoom1(int irq, void * ident)	//action lors de l'interruption contacteur rotation
{
        printk(KERN_INFO "DRIVERMOTOR: %s: %s()\n", THIS_MODULE->name, __FUNCTION__);
		printk(KERN_INFO "DRIVERMOTOR: Nombre de pas effectuer par le moteur zoom1 = %d" , MoteurZoom.currentnumber16Step);// £££££££££££££££CETTE LIGNE SERA A SUPPRIMER APPORT AVOIR FAIT LE RATIO PAS/ANGLE£££££££££££££££££££
		del_timer(& timerMotZoom);	// destruction du timer pour ce moteur
		Etat.Moteur.Zoom = 0;		// mise a 0 de l'état du moteur d'inclinaison
		MoteurZoom.number16Step = 0;
		
        return IRQ_HANDLED;
}

irqreturn_t limit_switch_Zoom2(int irq, void * ident)	//action lors de l'interruption contacteur rotation
{
        printk(KERN_INFO "DRIVERMOTOR: %s: %s()\n", THIS_MODULE->name, __FUNCTION__);
		printk(KERN_INFO "DRIVERMOTOR: Nombre de pas effectuer par le moteur zoom2 = %d" , MoteurZoom.currentnumber16Step);// £££££££££££££££CETTE LIGNE SERA A SUPPRIMER APPORT AVOIR FAIT LE RATIO PAS/ANGLE£££££££££££££££££££
		del_timer(& timerMotZoom);	// destruction du timer pour ce moteur
		Etat.Moteur.Zoom = 0;		// mise a 0 de l'état du moteur d'inclinaison
		MoteurZoom.number16Step = 0;
		
        return IRQ_HANDLED;
}

//###############################################################################################################//

//---------------------------------------------------------------------------------------------------------------//
//------------------------------------------Fonctions Arret------------------------------------------------------//
void stopOne(t_engine Motor)	// permet d'arrter un moteur au choix
{
	printk(KERN_INFO "DRIVERMOTOR: Stop_One\n");
	if(Motor.pin_step == MoteurRotation.pin_step){	// vérifie si le moteur rotation a été choisi
		MoteurRotation.number16Step = 0;	// l'objectif de pas à effuectuer est mis à zero
		del_timer(& timerMotRot);	// destruction du timer moteur rotation
		gpio_free(Step_MRotation);	// libération de la PIN du step MRotation
	}
	else if(Motor.pin_step == MoteurTilt.pin_step){	// vérifie si le moteur inclinaison a été choisi
		MoteurTilt.number16Step = 0;	// l'objectif de pas à effuectuer est mis à zero
		del_timer(& timerMotTilt);	// destruction du timer moteur rotation
		gpio_free(Step_MTilt);	// libération de la PIN du step MRotation
	}
}

void stopall()	// Cette fonction permet d'arreter immediatement tous les moteurs
{
	printk(KERN_INFO "DRIVERMOTOR: Stop all\n");
	stopOne(MoteurRotation);	// arret du moteur rotation
	stopOne(MoteurTilt);		// arret du moteur inclinaison
}
//---------------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------------//
//----------------------------------------------Fonctions Exit---------------------------------------------------//
void __exit my_exit (void)	// destructeur
{
	printk(KERN_INFO "DRIVERMOTOR: EXIT\n");
	del_timer(& timerMotRot);	// destruction du timer moteur rotation
	del_timer(& timerMotTilt);	// destruction du timer moteur inclinaison
	del_timer(& timerMotZoom);  // destruction du timer moteur zoom
	for(i=0;i<5;i++){
		gpio_free(TableauPinOutput[i]);
	}
	for(i=0;i<1;i++){
		gpio_free(TableauPinInput[i]);
	}
	free_irq(gpio_to_irq(azimuth), THIS_MODULE->name);	// désactive interruption azimute
	free_irq(gpio_to_irq(rising1), THIS_MODULE->name);	// désactive interruption inclinaison
    gpio_free(azimuth);	// libération pin interrupter azimute
    gpio_free(rising1);	// libération pin interrupter inclinaison
	gpio_free(rising2);	// libération pin interrupter inclinaison 2
	gpio_free(zoom1);	// libération pin interrupter zoom 
	gpio_free(zoom2);	// libération pin interrupter zoom 2
}
//---------------------------------------------------------------------------------------------------------------//
//------------------------------------------Fonctions lancer au démarrage du driver------------------------------//

module_init(my_init);	// definition de l'initialiser du timer
module_exit(my_exit);	// definition du destructeur du timer
MODULE_LICENSE("GPL");	// licence général public
//---------------------------------------------------------------------------------------------------------------//

// ajouter la fonction de qualibration avec interrupteur
// récupperer les valeurs de retour de chacune des fonctions principales


