//------------------------------------------------------------------------------------------------------------//
// Projet: Autoscope
// Autor: Thomas ABGRALL
// Mail: 01thomas.abgrall@gmail.com
// Subject: Driver Linux of steps motors
// Date: 2018-2019
// IDE: Pluma
//------------------------------------------------------------------------------------------------------------//

/*

~/Documents/M1/Projet_telescope/yocto/sources/meta-autoscope/recipes-kernel/a4988-mod


Pour compiler: 
# pour compiler : make ARCH=arm CROSS_COMPILE='/home/thomas/Documents/M1/Linux_embarque/gcc-linaro-7.3.1-2018.05-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-'

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
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>                 //kmalloc()
#include <linux/uaccess.h>              //copy_to/from_user()
#include <linux/ioctl.h>

MODULE_LICENSE("GPL");	// licence général public
MODULE_AUTHOR("Thomas Abgrall");
MODULE_DESCRIPTION("Module Driver moteurs"); 
MODULE_VERSION("Version 1.00"); 

// 	Contrôle moteur rotation
#define Step_MRotation 20
#define Dir_MRotation 21
#define Enable_MRotation 26
// 	Contrôle moteur inclinaison
#define Step_MTilt 13
#define Dir_MTilt 11
#define Enable_MTilt 24
// Contrôle moteur zoom
#define Step_MZoom 12
#define Dir_MZoom 6
#define Enable_MZoom 11
// Capteur de fin de course
#define azimuth 11
#define rising1 18
#define rising2 27
#define zoom1 23
#define zoom2 22
// Configuration mode pas rotation
#define ConfigPasRot1 19 
#define ConfigPasRot2 13
#define ConfigPasRot3 16
// Configuration mode pas inclaison
#define ConfigPasIncl1 25
#define ConfigPasIncl2 8
#define ConfigPasIncl3 10
// Configuration mode pas room
#define ConfigPasZoom1 11
#define ConfigPasZoom2 5
#define ConfigPasZoom3 5

#include <linux/ioctl.h>

//***********************************************************************************************//
//***********************************************************************************************//

#define WR_VALUE 		_IOW('a','1',int32_t*)		
#define RD_VALUE_STATUS	_IOR('a','2',struct Etat*)		
#define ROTATION 		_IOW('a','3', struct Data*)		
#define INCLINAISON 	_IOW('a','4', struct Data*)
#define ZOOM 			_IOW('a','5', struct Data*)
#define STOPONE 		_IOW('a','6', struct Data*)
#define STOPALL 		_IOW('a','7', int32_t*)	
//

int32_t value;
int32_t STATUS = 0; 
dev_t dev = 0;
static struct class *dev_class;
static struct cdev etx_cdev;
static int etx_open(struct inode *inode, struct file *file);
static int etx_release(struct inode *inode, struct file *file);
static ssize_t etx_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t etx_write(struct file *filp, const char *buf, size_t len, loff_t * off);
static long etx_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
//***********************************************************************************************//
//***********************************************************************************************//

//---------------------------------------------Prototype fonctions--------------------------------------------//
//------------------------------------------------------------------------------------------------------------//
void TimerHandler_Rotation(unsigned long unused);	// controle du moteur de rotation
void TimerHandler_Tilt(unsigned long unused); 		// controle des moteurs d'inclinaison
int __init my_init(void);
//------------------------------------------------------------------------------------------------------------//

//------------------------------------------------------------------------------------------------------------//
//--------------------------------------Création variables globales-------------------------------------------//
t_engine MoteurRotation, MoteurTilt, MoteurZoom;	// définition des moteurs en variables globales
struct timer_list timerMotRot, timerMotTilt, timerMotZoom;		// timer des moteurs
int TableauPinOutput[18] = {
	Step_MRotation, Dir_MRotation, Enable_MRotation, 
	Step_MTilt, Dir_MTilt, Enable_MTilt, 
	Step_MZoom, Dir_MZoom, Enable_MZoom, 
	ConfigPasRot1, ConfigPasRot2, ConfigPasRot3, 
	ConfigPasIncl1, ConfigPasIncl2, ConfigPasIncl3, 
	ConfigPasZoom1, ConfigPasZoom2, ConfigPasZoom3}; 
int TableauPinInput[5] = {azimuth, rising1, rising2, zoom1, zoom2};
union {		// machine état 
	char CharEtat;			// sous forme de char
	struct {
		unsigned char Rotation :1;		// état du moteur de rotation 1 = en rotation 0 = à l'arret
		unsigned char Tilt :1;			// état du moteur de d'inclinaison 1 = en rotation 0 = à l'arret
		unsigned char Zoom :1;			// état du moteur de zoom 1 = en rotation 0 = à l'arret
		unsigned char ErreurTimer :1;	// etat des timers
		unsigned char ErreurInterruption :1;	// état des interruptions
		unsigned char ErreurGPIO :1;	// état des GPIOs
		unsigned char unused :3;  		// complete le reste du char avec 4 bits libre
	}Moteur;
}Etat;
struct{
	int32_t nbPas;
	int32_t Sens;
	int32_t choixMoteur;
}Data;

//------------------------------------------------------------------------------------------------------------//

//------------------------------------------------------------------------------------------------------------//
//---------------------------------------Fonction lance rotation----------------------------------------------//
unsigned int rotate(unsigned long pas, e_direction direction) // initialiser moteur et lance timer
{
	printk(KERN_INFO "DRIVERMOTOR: Rotation motor will run\n");
	if(Etat.Moteur.Rotation == 0){
		// configuration pin moteur rotation
		MoteurRotation.pin_step = Step_MRotation;	// numéro pin step moteur rotation
		MoteurRotation.pin_dir = direction;	// numéro pin direction 
		gpio_set_value(Dir_MRotation, direction);
		MoteurRotation.number16Step = pas;		// initialisation du nombre de step a parcourir
		MoteurRotation.currentnumber16Step = 0;	// initialisation du nombre de step accompli

		init_timer(& timerMotRot);	// création du timer pour le moteur rotation
		timerMotRot.function = TimerHandler_Rotation;	// assiniation de la fonction au timer
		add_timer(& timerMotRot);	// ajout du timer (mise en route du timer)
	}
	else{
		printk(KERN_INFO "DRIVERMOTOR: Moteur rotation occupé pour le moment\n");
	}
	return 0;
}
//------------------------------------------------------------------------------------------------------------//

//------------------------------------------------------------------------------------------------------------//
//----------------------------------------Fonction lance inclinaison------------------------------------------//
unsigned int inclinate(unsigned long pas, e_direction direction)//init moteur et lance timer
{
	printk(KERN_INFO "DRIVERMOTOR: Inclinate motor will run\n");
	if(Etat.Moteur.Tilt == 0){
		// configuration pin moteur inclinaison
		MoteurTilt.pin_step = Step_MTilt;		// numéro pin step moteur rotation
		MoteurTilt.pin_dir = direction;			// numéro pin direction 
		gpio_set_value(Dir_MTilt, direction);
		MoteurTilt.number16Step=pas;			// initialisation du nombre de step a parcourir
		MoteurTilt.currentnumber16Step=0;		// initialisation du nombre de step accompli
		
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
//------------------------------------------------------------------------------------------------------------//


//------------------------------------------------------------------------------------------------------------//
//------------------------------------------Fonction zoom-----------------------------------------------//
unsigned int zoom(unsigned long pas, e_direction direction) // initialiser moteur et lance timer
{
	printk(KERN_INFO "DRIVERMOTOR: Rotation motor zoom will run\n");
	if(Etat.Moteur.Zoom == 0){
		// configuration pin moteur rotation
		MoteurZoom.pin_step = Step_MZoom;	// numéro pin step moteur rotation
		MoteurZoom.pin_dir = direction;	// numéro pin direction 
		gpio_set_value(Dir_MZoom, direction);
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
//-----------------------------------------------------------------------------------------------------------//

//##############################################################################################################//
//-----------------------------------------Fonctions Appeler a la fin de leur timer----------------------------//
void TimerHandler_Rotation(unsigned long unused)	// controle du moteur de rotation
{
	int erreur_set_value, nbPascomplet, activeTimer;
	printk(KERN_INFO "DRIVERMOTOR: Handler rotation\n");
	Etat.Moteur.Rotation = 1;
	nbPascomplet = MoteurRotation.number16Step / 16;
	if(MoteurRotation.currentnumber16Step < MoteurRotation.number16Step){
		if(MoteurRotation.currentnumber16Step < nbPascomplet){ // tant que l'on peu faire des pas, mode full pas
			gpio_set_value(ConfigPasRot1, 0);	// configuration pas complet
			gpio_set_value(ConfigPasRot2, 0);
			gpio_set_value(ConfigPasRot3, 0);
		}
		else if(MoteurRotation.currentnumber16Step < (MoteurRotation.number16Step % 16)){ // si full pas impossible 16ème
			gpio_set_value(ConfigPasRot1, 1);	// configuration 16ème de pas
			gpio_set_value(ConfigPasRot2, 1);
			gpio_set_value(ConfigPasRot3, 1);
		}
		if(gpio_get_value(Step_MRotation) == 0){	// controle de l'état de la clk (haut ou bas)
			gpio_set_value(Step_MRotation, 1);	// passe a 1 l'état de la step
			if((erreur_set_value = gpio_get_value(Step_MRotation)) != 1)	 
				printk(KERN_WARNING "DRIVERMOTOR: erreur_set_value: La valeur 1 n'a pas pu etre écrite dans le pin step rotation\n");
			if(MoteurRotation.currentnumber16Step < nbPascomplet)
				MoteurRotation.currentnumber16Step += 16;	// incrémentation du nb de 16ème de pas effectué avec +1 pas complet qui vient d'être fait
			else
				MoteurRotation.currentnumber16Step += 1;	// incrémentation du nb de 16ème de pas effectué avec +1 16ème de pas qui vient d'être fait
		}
		else{				// la clk est a état haut mise a l'état bas
			gpio_set_value(Step_MRotation, 0);	// mise en bas de la step
			if((erreur_set_value = gpio_get_value(Step_MRotation)) != 0)  
				printk(KERN_WARNING "DRIVERMOTOR: erreur_set_value: La valeur 0 n'a pas pu etre écrite dans le pin step rotation\n");
		}
		if((activeTimer = mod_timer(& timerMotRot, jiffies + msecs_to_jiffies(1))) != 1){
			printk(KERN_WARNING "DRIVERMOTOR: timerMotRot n'a pas pu être activé\n");
		}
	}
	else{					// le nombre de step moteur a été atteind
		del_timer(& timerMotRot);	// destruction du timer pour ce moteur
		Etat.Moteur.Rotation = 0;
	}
}

void TimerHandler_Tilt(unsigned long unused) // controle des moteurs d'inclinaison
{
	int erreur_set_value, activeTimer, nbPascomplet;
	printk(KERN_INFO "DRIVERMOTOR: Handler tilt\n");
	Etat.Moteur.Tilt = 1;
	nbPascomplet = MoteurTilt.number16Step / 16;
	if(MoteurTilt.currentnumber16Step < MoteurTilt.number16Step){
		if(MoteurTilt.currentnumber16Step < nbPascomplet){ // si full pas impossible 16ème
			gpio_set_value(ConfigPasIncl1, 0);	// configuration pas complet
			gpio_set_value(ConfigPasIncl2, 0);
			gpio_set_value(ConfigPasIncl3, 0);
		}
		else if(MoteurTilt.currentnumber16Step < (MoteurTilt.number16Step % 16)){	// si full pas impossible 16ème
			gpio_set_value(ConfigPasIncl1, 1);	// configuration 16ème de pas
			gpio_set_value(ConfigPasIncl2, 1);
			gpio_set_value(ConfigPasIncl3, 1);
		}
		if(gpio_get_value(Step_MTilt) == 0){
			gpio_set_value(Step_MTilt, 1);	// mise en haut de la step
			if((erreur_set_value = gpio_get_value(Step_MTilt)) != 1)
				printk(KERN_INFO "DRIVERMOTOR: erreur_set_value: La valeur 1 n'a pas pu etre écrite dans le pin step inclinaison\n");
			if(MoteurTilt.currentnumber16Step < nbPascomplet)
				MoteurTilt.currentnumber16Step += 16;	// incrémentation du nb de 16ème de pas effectué avec +1 pas complet qui vient d'être fait
			else
				MoteurTilt.currentnumber16Step += 1;	// incrémentation du nb de 16ème de pas effectué avec +1 16ème de pas qui vient d'être fait
		}
		else{
			gpio_set_value(Step_MTilt, 0);	// mise en bas de la step
			if((erreur_set_value = gpio_get_value(Step_MTilt)) != 0)
				printk(KERN_WARNING "DRIVERMOTOR: erreur_set_value: La valeur 0 n'a pas pu etre écrite dans le pin step inclinaison\n");
		}
		if((activeTimer = mod_timer(& timerMotTilt, jiffies + msecs_to_jiffies(1))) != 1){
			printk(KERN_WARNING "DRIVERMOTOR: timerMotTilt n'a pas pu être activé\n");
		}	
	}
	else{
		del_timer(& timerMotTilt);
		Etat.Moteur.Tilt = 0;
	}
}

void TimerHandler_Zoom(unsigned long unused) // controle des moteurs d'inclinaison
{
	int erreur_set_value, activeTimer, nbPascomplet;
	printk(KERN_INFO "DRIVERMOTOR: Handler tilt\n");
	Etat.Moteur.Zoom = 1;
	nbPascomplet = MoteurZoom.number16Step / 16;
	if(MoteurZoom.currentnumber16Step < MoteurZoom.number16Step){
		if(MoteurZoom.currentnumber16Step < nbPascomplet){ // si full pas impossible 16ème
			gpio_set_value(ConfigPasZoom1, 0);	// configuration 16ème de pas
			gpio_set_value(ConfigPasZoom2, 0);
			gpio_set_value(ConfigPasZoom3, 0);
		}
		else if(MoteurZoom.currentnumber16Step < (MoteurZoom.number16Step % 16)){	// si full pas impossible 16ème
			gpio_set_value(ConfigPasZoom1, 1);	// configuration 16ème de pas
			gpio_set_value(ConfigPasZoom2, 1);
			gpio_set_value(ConfigPasZoom3, 1);
		}
		if(gpio_get_value(Step_MZoom) == 0){
			gpio_set_value(Step_MZoom, 1);	// mise en haut de la step
			if((erreur_set_value = gpio_get_value(Step_MZoom)) != 1)
				printk(KERN_INFO "DRIVERMOTOR: erreur_set_value: La valeur 1 n'a pas pu etre écrite dans le pin step Zoom\n");

			if(MoteurZoom.currentnumber16Step < nbPascomplet)
				MoteurZoom.currentnumber16Step += 16;	// incrémentation du nb de 16ème de pas effectué avec +1 pas complet qui vient d'être fait
			else
				MoteurZoom.currentnumber16Step += 1;	// incrémentation du nb de 16ème de pas effectué avec +1 16ème de pas qui vient d'être fait
		}
		else{
			gpio_set_value(Step_MZoom, 0);	// mise en bas de la step
			if((erreur_set_value = gpio_get_value(Step_MZoom)) != 0)
				printk(KERN_WARNING "DRIVERMOTOR: erreur_set_value: La valeur 0 n'a pas pu etre écrite dans le pin step zoom\n");
		}
		if((activeTimer = mod_timer(& timerMotZoom, jiffies + msecs_to_jiffies(50))) != 1){
			printk(KERN_WARNING "DRIVERMOTOR: timerMotZoom n'a pas pu être activé\n");
		}	
	}
	else{
		del_timer(& timerMotZoom);
		Etat.Moteur.Zoom = 0;
	}
}

//##############################################################################################################//

//##############################################################################################################//
//#######################################Interruption capteurs fin de courses################################// 
irqreturn_t limit_switch_Azimuth(int irq, void * ident)	//action lors de l'interruption contacteur rotation
{
        printk(KERN_INFO "DRIVERMOTOR: %s: %s()\n", THIS_MODULE->name, __FUNCTION__);
        printk(KERN_INFO "DRIVERMOTOR: Nombre de pas effectuer par le moteur rotation = %d\n", MoteurRotation.currentnumber16Step); // CETTE LIGNE SERA A SUPPRIMER APPORT AVOIR FAIT LE RATIO PAS/ANGLE
		del_timer(& timerMotRot);	// destruction du timer pour ce moteur
		Etat.Moteur.Rotation = 0;	// mise a 0 de l'état du moteur de rotation
		MoteurTilt.number16Step = 0;
        return IRQ_HANDLED;
}

irqreturn_t limit_switch_Rising1(int irq, void * ident)	//action lors de l'interruption contacteur rotation
{
        printk(KERN_INFO "DRIVERMOTOR: %s: %s()\n", THIS_MODULE->name, __FUNCTION__);
		printk(KERN_INFO "DRIVERMOTOR: Nombre de pas effectuer par le moteur inclinaison = %d\n" , MoteurTilt.currentnumber16Step); // CETTE LIGNE SERA A SUPPRIMER APPORT AVOIR FAIT LE RATIO PAS/ANGLE
		del_timer(& timerMotTilt);	// destruction du timer pour ce moteur
		Etat.Moteur.Tilt = 0;		// mise a 0 de l'état du moteur d'inclinaison
		MoteurTilt.number16Step = 0;
		
        return IRQ_HANDLED;
}

irqreturn_t limit_switch_Rising2(int irq, void * ident)	//action lors de l'interruption contacteur rotation
{
        printk(KERN_INFO "DRIVERMOTOR: %s: %s()\n", THIS_MODULE->name, __FUNCTION__);
		printk(KERN_INFO "DRIVERMOTOR: Nombre de pas effectuer par le moteur inclinaison = %d\n" , MoteurTilt.currentnumber16Step); // CETTE LIGNE SERA A SUPPRIMER APPORT AVOIR FAIT LE RATIO PAS/ANGLE
		del_timer(& timerMotTilt);	// destruction du timer pour ce moteur
		Etat.Moteur.Tilt = 0;		// mise a 0 de l'état du moteur d'inclinaison
		MoteurTilt.number16Step = 0;
		
        return IRQ_HANDLED;
}

irqreturn_t limit_switch_Zoom1(int irq, void * ident)	//action lors de l'interruption contacteur rotation
{
        printk(KERN_INFO "DRIVERMOTOR: %s: %s()\n", THIS_MODULE->name, __FUNCTION__);
		printk(KERN_INFO "DRIVERMOTOR: Nombre de pas effectuer par le moteur zoom1 = %d\n" , MoteurZoom.currentnumber16Step);// CETTE LIGNE SERA A SUPPRIMER APPORT AVOIR FAIT LE RATIO PAS/ANGLE
		del_timer(& timerMotZoom);	// destruction du timer pour ce moteur
		Etat.Moteur.Zoom = 0;		// mise a 0 de l'état du moteur d'inclinaison
		MoteurZoom.number16Step = 0;
		
        return IRQ_HANDLED;
}

irqreturn_t limit_switch_Zoom2(int irq, void * ident)	//action lors de l'interruption contacteur rotation
{
        printk(KERN_INFO "DRIVERMOTOR: %s: %s()\n", THIS_MODULE->name, __FUNCTION__);
		printk(KERN_INFO "DRIVERMOTOR: Nombre de pas effectuer par le moteur zoom2 = %d\n" , MoteurZoom.currentnumber16Step);// CETTE LIGNE SERA A SUPPRIMER APPORT AVOIR FAIT LE RATIO PAS/ANGLE
		del_timer(& timerMotZoom);	// destruction du timer pour ce moteur
		Etat.Moteur.Zoom = 0;		// mise a 0 de l'état du moteur d'inclinaison
		MoteurZoom.number16Step = 0;
		
        return IRQ_HANDLED;
}

//###############################################################################################################//

//-------------------------------------------------------------------------------------------------------------//
//------------------------------------------Fonctions Arret----------------------------------------------------//
void stopOne(t_engine Motor)	// permet d'arrter un moteur au choix
{
	printk(KERN_INFO "DRIVERMOTOR: MoteurRotation stop\n");
	if(Motor.pin_step == MoteurRotation.pin_step){	// vérifie si le moteur rotation a été choisi
		printk(KERN_INFO "DRIVERMOTOR: MoteurRotation stop\n");
		MoteurRotation.number16Step = 0;	// l'objectif de pas à effuectuer est mis à zero
		del_timer(& timerMotRot);	// destruction du timer moteur rotation
		gpio_free(Step_MRotation);	// libération de la PIN du step MRotation
	}
	else if(Motor.pin_step == MoteurTilt.pin_step){	// vérifie si le moteur inclinaison a été choisi
		printk(KERN_INFO "DRIVERMOTOR: MoteurTilt stop\n");
		MoteurTilt.number16Step = 0;	// l'objectif de pas à effuectuer est mis à zero
		del_timer(& timerMotTilt);	// destruction du timer moteur rotation
		gpio_free(Step_MTilt);	// libération de la PIN du step MRotation
	}
	else if(Motor.pin_step == MoteurZoom.pin_step){	// vérifie si le moteur inclinaison a été choisi
		printk(KERN_INFO "DRIVERMOTOR: MoteurZoom stop\n");
		MoteurZoom.number16Step = 0;	// l'objectif de pas à effuectuer est mis à zero
		del_timer(& timerMotZoom);	// destruction du timer moteur rotation
		gpio_free(Step_MTilt);	// libération de la PIN du step MRotation
	}
}

void stopall()	// Cette fonction permet d'arreter immediatement tous les moteurs
{
	printk(KERN_INFO "DRIVERMOTOR: Stop all\n");
	stopOne(MoteurRotation);	// arret du moteur rotation
	stopOne(MoteurTilt);		// arret du moteur inclinaison
	stopOne(MoteurZoom);		// arret du moteur zoom
}
//------------------------------------------------------------------------------------------------------------//

//**************************************************************************************************************//WR_VALUE
//**************************************************************************************************************//
static struct file_operations fops =
{
        .owner          = THIS_MODULE,
        .open           = etx_open,
        .unlocked_ioctl = etx_ioctl,
        .release        = etx_release,
};
 
static int etx_open(struct inode *inode, struct file *file)
{
        printk(KERN_INFO "DRIVERMOTOR: Device File Opened...!!!\n");
        return 0;
}
 
static int etx_release(struct inode *inode, struct file *file)
{
        printk(KERN_INFO "DRIVERMOTOR: Device File Closed...!!!\n");
        return 0;
}

static ssize_t etx_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
        printk(KERN_INFO "Read Function\n");
        return 0;
}

static ssize_t etx_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
        printk(KERN_INFO "Write function\n");
        return 0;
}
 
static long etx_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
         switch(cmd) {
                case WR_VALUE:
						printk(KERN_INFO "DRIVERMOTOR: receve order write value\n");						
                        copy_from_user(&value ,(int32_t*) arg, sizeof(value));
                        printk(KERN_INFO "DRIVERMOTOR: Value = %d\n", value);
                        break;

                case RD_VALUE_STATUS:
						printk(KERN_INFO "DRIVERMOTOR: receve order read value Status\n");						
                        copy_to_user((struct Etat*) arg, &Etat, sizeof(Etat));
                        printk(KERN_INFO "DRIVERMOTOR: Status value send = %c\n", Etat.CharEtat);
                        break;

				case ROTATION:
						printk(KERN_INFO "DRIVERMOTOR: receve order Rotation\n");						
						copy_from_user(&Data, (int32_t*) arg, 12);
						printk(KERN_INFO "DRIVERMOTOR: order to do %d step in sens of rotation ", Data.nbPas);
						// appel de la fonction pour faire tourner le moteur de rotation
						if(Data.Sens == 0){
							printk("clockwise\n");
							rotate(Data.nbPas, clockwise);}	// application la rotation en sens horaire
						else if(Data.Sens == 1){
							printk("anclockwise\n");
							rotate(Data.nbPas, anticlockwise);// application la rotation en sens non horaire
						}
						else
							printk(KERN_WARNING "DRIVERMOTOR: the chose of sens is not between 0-1\n");						
						break;

				case INCLINAISON:
						printk(KERN_INFO "DRIVERMOTOR: receve order inclinaison\n");						
						copy_from_user((struct Data*) arg, &Data , 3*sizeof(int32_t));

						printk(KERN_INFO "DRIVERMOTOR: order to do tild %d step in", Data.nbPas);
						// appel de la fonction pour faire tourner le moteur de rotation
						if(Data.Sens == 0){
							printk("clockwise\n");
							inclinate(Data.nbPas, clockwise);}	// application la rotation en sens horaire
						else if(Data.Sens == 1){
							printk("anticlockwise\n");
							inclinate(Data.nbPas, anticlockwise);}// application la rotation en sens non horaire
						else
							printk(KERN_WARNING "DRIVERMOTOR: the chose of sens is not between 0-1\n");						
						break;

				case ZOOM:
						printk(KERN_INFO "DRIVERMOTOR: receve order Zoom\n");						
						copy_from_user((struct Data*) arg, &Data , 3*sizeof(int32_t));

						printk(KERN_INFO "DRIVERMOTOR: order to do %d step in sens of rotation zoom ", Data.nbPas);
						// appel de la fonction pour faire tourner le moteur de rotation
						if(Data.Sens == 0){
							printk("clockwise\n");
							zoom(Data.nbPas, clockwise);}	// application la rotation en sens horaire
						else if(Data.Sens == 1){
							printk("anticlockwise\n");
							zoom(Data.nbPas, anticlockwise);}// application la rotation en sens non horaire
						else
							printk(KERN_WARNING "DRIVERMOTOR: the chose of sens is not between 0-1\n");												
						break;

				case STOPONE:
						printk(KERN_INFO "DRIVERMOTOR: receve order StopOne\n");						
						copy_from_user((struct Data*) arg, &Data , 3*sizeof(int32_t));
						if(Data.choixMoteur == 1)	// controle si choix moteur rotation
							stopOne(MoteurRotation);
						else if(Data.choixMoteur == 2)	// controle si choix moteur inclinaison
							stopOne(MoteurTilt);
						else if(Data.choixMoteur == 3)	// controle si choix moteur zoom
							stopOne(MoteurZoom);	
						break;

				case STOPALL:
						printk(KERN_INFO "DRIVERMOTOR: receve order StopAll\n");
						copy_from_user((struct Data*) arg, &Data , 3*sizeof(int32_t));
						stopall();
        }
        return 0;
}
//**************************************************************************************************************//
//************************************************************************************************************//

//------------------------------------------------------------------------------------------------------------//
//----------------------------------------Fonction initialisation du driver------------------------------------//
int __init my_init (void)	// initilaisation des timers 
{
	printk(KERN_INFO "DRIVERMOTOR: Initialisation du driver moteur\n"); 
	int i, erreurGPIOIn, erreurGPIOOut, erreurInterruption1, erreurInterruption2, erreurInterruption3, erreurInterruption4, erreurInterruption5;
	// parcours toutes les PINs en Sortie pour liberer les pins pour quelles soient utilisables
	for(i=0; i < (sizeof(TableauPinOutput)/sizeof(int)); i++){	
		gpio_free(TableauPinOutput[i]);  // libère la PIN si il y a une erreur dans gpio_request
		// associe la pin au module et contrôle qu'il n'y est pas erreur lors de cette action
		if ((erreurGPIOOut = gpio_request(TableauPinOutput[i],THIS_MODULE->name)) != 0) {	
			printk(KERN_WARNING "DRIVERMOTOR: erreurGPIOOut liberation pinTableau n°%d = %d\n",i ,erreurGPIOOut); 
			return erreurGPIOOut;
		}
		// met la pin en sortie et contrôle qu'il n'y est pas erreur lors de cette action
		gpio_free(TableauPinOutput[i]);  // libère la PIN si si il y a une erreur dans gpio_direction_output
		if ((erreurGPIOOut = gpio_direction_output(TableauPinOutput[i],1)) != 0) { 
			printk(KERN_WARNING "DRIVERMOTOR: erreurGPIOOut direction pinTableau n°%d = %d\n",i ,erreurGPIOOut); 
			return erreurGPIOOut;
		} 
	} 
	// parcours toutes les PINs en Entrée pour liberer les pins pour quelles soient utilisables
	for(i=0; i < (sizeof(TableauPinInput)/sizeof(int)); i++){	
		gpio_free(TableauPinInput[i]);  // libère la PIN
		// associe la pin au module et contrôle qu'il n'y est pas erreur lors de cette action
		if ((erreurGPIOIn = gpio_request(TableauPinInput[i],THIS_MODULE->name)) != 0) {	
			printk(KERN_WARNING "DRIVERMOTOR: erreurGPIOIn liberation pinTableau n°%d = %d\n",i ,erreurGPIOIn);
			return erreurGPIOIn;
		}
		gpio_free(TableauPinInput[i]);
		// met la pin en sortie et contrôle qu'il n'y est pas erreur lors de cette action
		if ((erreurGPIOIn = gpio_direction_input(TableauPinInput[i])) != 0) {
			printk(KERN_WARNING "DRIVERMOTOR: erreurGPIOIn direction pinTableau n°%d = %d\n",i ,erreurGPIOIn); 
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
    // Allocationn du nombre Major du driver (identification)
    if((alloc_chrdev_region(&dev, 0, 1, "etx_Dev")) <0){
            printk(KERN_INFO "Cannot allocate major number\n");
            return -1;
    }
    printk(KERN_INFO "DRIVERMOTOR: Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));

	Etat.Moteur.Rotation = 0;
	Etat.Moteur.Tilt = 0;
	Etat.Moteur.Zoom = 0;
	
    /*Creating cdev structure*/
    cdev_init(&etx_cdev,&fops);

    /*Adding character device to the system*/
    if((cdev_add(&etx_cdev,dev,1)) < 0){
        printk(KERN_WARNING "DRIVERMOTOR: Cannot add the device to the system\n");
        goto r_class;
    }

    /*Creating struct class*/
    if((dev_class = class_create(THIS_MODULE,"etx_class")) == NULL){
        printk(KERN_WARNING "DRIVERMOTOR: Cannot create the struct class\n");
        goto r_class;
    }

    /*Creating device*/
    if((device_create(dev_class,NULL,dev,NULL,"etx_device")) == NULL){
        printk(KERN_WARNING "DRIVERMOTOR: Cannot create the Device 1\n");
        goto r_device;
    }
    printk(KERN_INFO "DRIVERMOTOR: Device Driver Insert...Done!!!\n");
	return 0;

	r_device:
    	class_destroy(dev_class);
	r_class:
    	unregister_chrdev_region(dev,1);
    	return -1;
}
//------------------------------------------------------------------------------------------------------------//

//------------------------------------------------------------------------------------------------------------//
//---------------------------------------------Fonctions Exit-------------------------------------------------//
void __exit my_exit (void)	// destructeur
{
	int i;
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
    device_destroy(dev_class,dev);
    class_destroy(dev_class);
    cdev_del(&etx_cdev);
    unregister_chrdev_region(dev, 1);
    printk(KERN_INFO "DRIVERMOTOR: Device Driver Remove...Done!!!\n");
}
//------------------------------------------------------------------------------------------------------------//
//----------------------------------------Fonctions lancer au démarrage du driver-----------------------------//

module_init(my_init);	// definition de l'initialiser du timer
module_exit(my_exit);	// definition du destructeur du timer

//------------------------------------------------------------------------------------------------------------//


