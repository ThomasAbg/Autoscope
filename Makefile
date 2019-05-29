obj-m := mpu9250.o

#mpu9250-y := mpu_spi_driver.o mpu_core.o
mpu9250-y := mpu_i2c_driver.o mpu_core.o

SRC := $(shell pwd)

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC)

modules_install:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) modules_install

clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) clean
