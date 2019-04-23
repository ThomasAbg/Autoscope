/**
 *
 * @file 		mpu_spi_driver.c
 *
 * @brief 		Invensense MPU devices SPI driver
 *
 * @version 	1.0
 *
 * @date 		December 15th, 2018
 *
 * @author 		Clement Ailloud
 *
 **/


#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/spi/spi.h>
#include <linux/types.h>
#include <linux/of.h>
#include <linux/iio/iio.h>
#include <linux/iio/sysfs.h>
#include <linux/iio/types.h>
#include <linux/regmap.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/device.h>

#include "mpu_core.h"

#include "mpu_iio.h"

#define INIT_MSG ((const char *) "initialization...")

static unsigned int major = CHRDEV_MAJOR_DYN_END;

static struct class *class;

static struct regmap *map = NULL;

struct mpu_device {
	struct spi_device *spi;
    struct cdev chdev;
};


/**
 *
 * @struct 	spi_device_id
 *
 * @brief 	Table used for identifying SPI slave devices.
 *
 **/

static const struct spi_device_id mpu_core_spi_id_table[] = {
	{
		.name = "mpu9250-spi"
	},
	{
		.name = "mpu9255-spi",
	},
	{ /* Sentinel */ }
};

MODULE_DEVICE_TABLE(spi, mpu_core_spi_id_table);


/**
 *
 * @struct 	of_device_id
 *
 * @brief 	Bind the driver to Device Tree node
 *
 * 			It informs the Linux kernel about this driver have to be bound to
 * 			the SPI controller.
 *
 * 			The "compatible" field must be the same as compatible property of
 * 			mpu device node in the Device Tree.
 *
 **/

static const struct of_device_id mpu_core_of_match_table[] = {
	{
		.compatible = "invensense,mpu9250-spi"
	},
	{
		.compatible = "invensense,mpu9255-spi"
	},
	{ /* Sentinel */ }
};

MODULE_DEVICE_TABLE(of, mpu_core_of_match_table);


static const struct regmap_config map_config = {
	.reg_bits = 8, // Size of a register
	.val_bits = 8, // Size of a value
	.writeable_reg = NULL,
	.readable_reg = NULL,
//	.disable_locking = true, // Don't use mutex or spinlock
	.fast_io = false, // Use mutex if disable_locking is false
	.read_flag_mask = 0x80,
	.write_flag_mask = 0x00,
};

static int mpu_core_init(struct regmap *map){
	int ret = -1;

	unsigned int data = 0;

	pr_info("%s\n", INIT_MSG);

	if (!map ) {
		pr_err("%s failed\n", INIT_MSG);

		return -1;
	}

	ret = check_device_identity(map, MPU_9255_DEVICE_ID);

	if (ret < 0) {
		pr_err("Invalid device identity\n");
		
		return ret;
	}

	regmap_bulk_read(map, 107, &data, 2);

	pr_info(KERN_INFO "%u\n", data);

	ret = regmap_read(map, MPU_PWR_MGMT_1, &data);

	if (ret < 0)
		return ret;

	// Set reset bit up 
	data |= MPU_RESET_BIT_MASK;

	// Perform internal reset
	ret = regmap_write(map, MPU_PWR_MGMT_1, data);

	if (ret < 0)
		return ret;

	/* Accelerometer configuration */
	ret = set_accelerometer_full_scale(map, ACCEL_FS_SEL_2G);

	if (ret < 0)
		return ret;

	/* Gyroscope configuration */
	ret = set_gyroscope_full_scale(map, GYRO_FS_SEL_250DPS);

	if (ret < 0)
		return ret;

	/* Magnetometer configuration */
#if 0

#endif

	pr_info("mpu9255: configuring power mode...\n");

	// Set power mode for all axis of accelerometer and gyroscope
	ret = set_power_mode(map, ACCEL_AND_GYRO);

	if (ret < 0)
		return ret;

	pr_info("configuring power mode... done\n");

	pr_info("mpu9255: power mode change over to %d\n", ACCEL_AND_GYRO);

	pr_info("%s done\n", INIT_MSG);

	return 0;
}

#if 0
static int mpu_read_raw(struct iio_dev *dev, struct iio_chan_spec const *channel, int *val, int *val2, long mask){
	int ret;

	struct mpu_device *st = NULL;

	if (!dev) {
		return -ENODEV;
	}

	st = iio_priv(dev);

	if (!st) {
		return -ENOMEM;
	}

	if (!channel || !val || !val2) {
		return -EINVAL;
	}

	switch (mask) {
		case IIO_CHAN_INFO_PROCESSED:
			switch (channel->type) {
				case IIO_TEMP:
					ret = regmap_bulk_read(st->map, channel->address, (void *) val, 2);

					if (ret < 0)
						return ret;

					*val = sign_extend32(*val, 15);

					*val = (int) ((*val / 333) + 21);

					return IIO_VAL_INT;

				default:
					return -EINVAL;
			}

		case IIO_CHAN_INFO_RAW:
			switch (channel->type) {
				case IIO_ACCEL:
					ret = regmap_bulk_read(st->map, channel->address, (void *) val, 2);

					if (ret < 0)
						return ret;

					*val = sign_extend32(*val, 15);

					return IIO_VAL_INT;

				case IIO_ANGL_VEL:
					ret = regmap_bulk_read(st->map, channel->address, (void *) val, 2);

					if (ret < 0)
						return ret;

					*val = sign_extend32(*val, 15);

					return IIO_VAL_INT;

				default:
					return -EINVAL;
			}

		default:
			return -EINVAL;
	}

	if (IIO_CHAN_INFO_SCALE)
		return -EINVAL;

	return 0;
}


static const struct iio_info mpu_info = {
	.driver_module = THIS_MODULE,
	.read_raw = mpu_read_raw
};
#endif

static int open_operation(struct inode *inode, struct file *file) {
	if (!inode || !file)
		return -EINVAL;

	printk(KERN_DEBUG "%s: character driver has been opened\n", THIS_MODULE->name);

	return 0;
}

static int release_operation(struct inode *inode, struct file *file) {
	if (!inode || !file)
		return -1;
	
	printk(KERN_DEBUG "%s: character driver has been released\n", THIS_MODULE->name);

	return 0;
}

const char str = 0xAA;

static ssize_t read_operation(struct file *file, char __user *user_data, size_t len, loff_t *loff) {
	printk(KERN_DEBUG "%s: character driver has been read\n", THIS_MODULE->name);
    size_t size = 0;

	if (!file || !user_data)
		return -EINVAL;

    // Get private data
//    if (!file->private_data)
//        return -EINVAL;

	size = copy_to_user(user_data, &str, sizeof(str));

    if (size < 0)
        return -EFAULT;

    pr_info("mpu9255: receive %lu bytes from %s\n", len, file->f_path);
    pr_info("mpu9255: offset: %lu\n", *loff);

    return len;
}

static struct file_operations file_ops = {
	.owner = THIS_MODULE,
	.read = read_operation,
	.open = open_operation,
	.release = release_operation
};

/**
 *
 * @fn 		static int mpu_core_probe(struct spi_device *dev)
 *
 * @brief	Initialization function when the driver is loaded.
 *
 * @param	dev spi_device structure
 *
 * @return 	exit status
 *
 * Note:
 *
 * Return negative value on failure, zero on success
 *
 **/

static int mpu_core_probe(struct spi_device *spi){
	int ret = -1;

	struct iio_dev *iio = NULL;

	struct mpu_device *st = NULL;

	if (!spi) {
		pr_err("mpu_core: no spi device found\n");

		return -ENODEV;
	} else {
		// Print SPI controller configuration
		pr_info("max_speed_hz: %lu\n", (unsigned long) spi->max_speed_hz);
		pr_info("bits_per_word: %u\n", (unsigned int) spi->bits_per_word);
		pr_info("mode: %hu\n", (unsigned short) spi->mode);
		pr_info("chip_select: %u\n", (unsigned int) spi->chip_select);
		pr_info("cs_gpio: %d\n", spi->cs_gpio);
		pr_info("irq: %d\n", spi->irq);
	}

	dev_info(&spi->dev, "regmap initialization...\n");

	// Automatically freed
	map = devm_regmap_init_spi(spi, &map_config);

	if (!map)
		return -ENOMEM;

	dev_info(&spi->dev, "regmap initialization... done\n");

	// Initialize MPU device
	ret = mpu_core_init(map);

	if (ret < 0) {
		return ret;
	}
#if 0
	// Allocate IIO device memory
	iio = devm_iio_device_alloc(&spi->dev, sizeof(struct mpu_device));

	if (!iio) {
		dev_err(&spi->dev, "iio device allocation failed\n");

		return -ENOMEM;
	}

	st = iio_priv(iio);
	st->spi = spi;
	st->map = map;

	// Set iio data to spi device
	spi_set_drvdata(spi, iio);

	// Initialize iio device specific information
	iio->dev.parent = &spi->dev;
	iio->info = &mpu_info;
	iio->name = mpu_core_spi_id_table->name;
	iio->modes = INDIO_DIRECT_MODE;
	iio->channels = mpu_chan_spec;
	iio->num_channels = ARRAY_SIZE(mpu_chan_spec);

	// Registers IIO device to the IIO core subsystem
	ret = iio_device_register(iio);

	if (ret < 0) {
		dev_err(&spi->dev, "iio device register failed\n");

		return ret;
	}
#endif

#if 0
	ret = request_irq(dev->irq, NULL, 0, dev->modalias, NULL);

	if (ret < 0){
		pr_err("mpu9255: request_irq: failed\n");

		return ret;
	}
#endif
	
	ret = register_chrdev(major, THIS_MODULE->name, &file_ops);

	if (ret < 0)
		return ret;

	class = class_create(THIS_MODULE, THIS_MODULE->name);

	if (!class) {
		unregister_chrdev(major, THIS_MODULE->name);

		return PTR_ERR(class);
	}

 	if (!device_create(class, &spi->dev, MKDEV(major, 0), NULL, "mpu9255")) {
		dev_err(&spi->dev, "%s: device_create failed\n", __func__);

		class_destroy(class);

		unregister_chrdev(major, THIS_MODULE->name);

		return -ENODEV;
	}

	return 0;
}


/**
 *
 * @fn 		static int mpu_core_remove(struct spi_device *dev)
 *
 * @brief 	Cleanup function when the driver is removed.
 *
 * @param 	dev
 *
 * @return 	exit status
 *
 * Note:
 *
 * Return negative value on failure, zero on success
 *
 **/

static int mpu_core_remove(struct spi_device *spi){
//	int ret;

#if 0
	struct iio_dev *iio = NULL;
#endif

	struct mpu_device *st = NULL;

	if (!spi) {
		pr_err("mpu_core: no spi device found\n");

		return -ENODEV;
	}

	dev_info(&spi->dev, "removing...\n");

	// Set sleep mode
// 	ret = mpu_set_power_mode(spi, SLEEP);
//
// 	if (ret < 0){
// 		dev_err(spi->dev, "mpu_set_power_mode failed\n");
// 		
// 		return ret;
// 	}

//!
//! 	--- Interrupt ---
//!

//	free_irq(spi->irq, NULL);


//!
//! 	--- IIO Subsystem ---
//!

#if 0
	/** Get iio device for unregistration */
	iio = spi_get_drvdata(spi);

	if (!iio) {
		dev_err(&spi->dev, "spi get driver data failed\n");

		return -1;
	}

	st = iio_priv(iio);

	/*!< Unregister device from IIO core */
	iio_device_unregister(iio);

	/*!< Clean up IIO device */
	iio_device_free(iio);
#endif
	// Shutdown device power or enable sleep mode

	// Remove device from /dev
	device_destroy(class, MKDEV(major, 0));

	class_destroy(class);

	unregister_chrdev(major, THIS_MODULE->name);

	dev_info(&spi->dev, "removing... done\n");

	return 0;
}


/**
 *
 * @struct	spi_driver
 *
 * @brief	SPI driver structure to define the probe function and the remove
 * 			function. It also include the id table of SPI slave devices and
 * 			information related to this driver.
 *
 **/

static struct spi_driver mpu_core_driver = {
	.id_table = mpu_core_spi_id_table,
	.probe = mpu_core_probe,
	.remove = mpu_core_remove,
	.driver = {
		.name = THIS_MODULE->name,
		.owner = THIS_MODULE,
		.mod_name = THIS_MODULE->name,
		.of_match_table = of_match_ptr(mpu_core_of_match_table)
	}
};



/**
 *
 * @brief	Replace module_init() and module_exit() by probe and remove
 * 			function registered in spi_driver.
 *
 * @param	mpu_core_driver from spi_driver structure
 *
 **/

module_spi_driver(mpu_core_driver);





/*! Description of the module driver */
MODULE_DESCRIPTION("MPU-9255 Intertial measurement unit driver");

/*! License of the module driver */
MODULE_LICENSE("GPL");

/*! Version of the module driver */
MODULE_VERSION("1.0");

/*! Author of the module driver */
MODULE_AUTHOR("Clement Ailloud");

