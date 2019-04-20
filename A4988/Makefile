obj-m += a4988.o

KDIR_OLD = "/home/thomas/Documents/M1/Projet_telescope/linux-rpi-4.14.y"
KDIR = "/lib/modules/$(uname -r)/build"


all:
		make -C $(KDIR_OLD) M=$(PWD) modules


clean:
		make -C $(KDIR_OLD) M=$(PWD) clean
