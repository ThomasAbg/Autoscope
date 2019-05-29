/**
 *
 * @file 		mpu_i2c_driver.c
 *
 * @brief 		Invensense MPU devices I2C driver
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
#include <linux/i2c.h>
#include <linux/of.h>
#include <linux/iio/iio.h>
#include <linux/iio/sysfs.h>
#include <linux/iio/types.h>
#include <linux/regmap.h>

#include "mpu_core.h"

#include "mpu_iio.h"


#define INIT_MSG ((const char *) "initialization...")


struct mpu_device {
	struct i2c_client *client;
	struct regmap *map;
};


/**
 *
 * @struct 	i2c_device_id
 *
 * @brief 	Table used for identifying SPI slave devices.
 *
 **/

static const struct i2c_device_id mpu_device_i2c_id_table[] = {
	{
		.name = "mpu9250-i2c"
	},
	{
		/*!< Device name>*/
		.name = "mpu9255-i2c",
		/*!< Identity code >*/
		.driver_data = 0
	},
	{ /* Sentinel */ }
};

/*!< Stores MPU-9255 SPI interface specification */
MODULE_DEVICE_TABLE(i2c, mpu_device_i2c_id_table);


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

static const struct of_device_id mpu_device_of_match_table[] = {
	{
		.compatible = "invensense,mpu9250-i2c"
	},
	{
		.compatible = "invensense,mpu9255-i2c"
	},
	{ /* Sentinel */ }
};

/*!< Stores MPU Open Firmware (OF) interface specification */
MODULE_DEVICE_TABLE(of, mpu_device_of_match_table);


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

static int mpu_device_init(struct regmap *map){
	int ret = -1;

	pr_info("%s\n", INIT_MSG);

	if (!map) {
		pr_err("%s failed\n", INIT_MSG);

		return -1;
	}

	ret = check_device_identity(map, MPU_9250_DEVICE_ID);

	if (ret < 0) {
		pr_err("Invalid device identity\n");
		
		return ret;
	}

	// Perform internal reset
	ret = regmap_write(map, MPU_PWR_MGMT_1, 0x80);

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

	/*!< Set power mode of the MPU device for all axis of accelerometer and gyroscope */
	ret = set_power_mode(map, ACCEL_AND_GYRO);

	if (ret < 0) {
		pr_err("mpu9255: configuring power mode... failed\n");

		return ret;
	}

	pr_info("configuring power mode... done\n");

	pr_info("mpu9255: power mode change over to %d\n", ACCEL_AND_GYRO);

	pr_info("%s done\n", INIT_MSG);

#ifdef CONFIG_MPU_9255_IIO
	pr_info("Use sysfs support\n");
#endif

	return 0;
}


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

	/*!< BUG: Needed for avoiding incorrect values ? */
	*val = 0;

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


/**
 *
 * @fn 		static int mpu_device_probe(i2c_client *client, const struct i2c_device_id *device_id)
 *
 * @brief	Initialization function when the driver is loaded.
 *
 * @param	client
 *
 * @param 	device_id
 *
 * @return 	exit status
 *
 * Note:
 *
 * Return negative value on failure, zero on success
 *
 **/

static int mpu_device_probe(struct i2c_client *client, const struct i2c_device_id *device_id){
	int ret = -1;

	/*!< IIO device */
	struct iio_dev *iio = NULL;

	/*!< IIO device data */
	struct mpu_device *st = NULL;

	struct regmap *map = NULL;

	if (!client) {
		pr_err("mpu_device: no client device found\n");

		return -ENODEV;
	}

	if (!i2c_check_functionality(client->adapter,
			I2C_FUNC_SMBUS_BYTE_DATA | I2C_FUNC_SMBUS_WORD_DATA)) {
		dev_err(&client->dev, "SMBUS Byte/Word Data not supported\n");
		
		return -EOPNOTSUPP;
	}

	dev_info(&client->dev, "regmap initialization...\n");

	map = devm_regmap_init_i2c(client, &map_config);

	if (!map)
		return -ENOMEM;

	dev_info(&client->dev, "regmap initialization... done\n");

	/*!< Initialize MPU device */
	ret = mpu_device_init(map);

	if (ret < 0) {
		return ret;
	}

	/*!< Allocate IIO device memory */
	iio = devm_iio_device_alloc(&client->dev, sizeof(struct mpu_device));

	if (!iio) {
		dev_err(&client->dev, "iio device allocation failed\n");

		return -ENOMEM;
	}

	st = iio_priv(iio);

	if (!st)
		return -ENOMEM;

	st->client = client;
	st->map = map;

	/*!< Set iio data from i2c device */
	i2c_set_clientdata(client, iio);

	/** Initialize iio device specific information */
	iio->dev.parent = &client->dev;
	iio->info = &mpu_info;
	iio->name = mpu_device_i2c_id_table->name;
	iio->modes = INDIO_DIRECT_MODE;
	iio->channels = mpu_chan_spec;
	iio->num_channels = ARRAY_SIZE(mpu_chan_spec);

	/*!< Registers IIO device to the IIO core subsystem */
	ret = iio_device_register(iio);

	if (ret < 0) {
		dev_err(&client->dev, "iio device register failed\n");

		return ret;
	}

	return 0;
}


/**
 *
 * @fn 		static int mpu_device_remove(i2c_client *client)
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

static int mpu_device_remove(struct i2c_client *client){
//	int ret;

	struct iio_dev *iio = NULL;

	struct mpu_device *st = NULL;
	
	if (!client) {
		pr_err("mpu_device: no client device found\n");

		return -ENODEV;
	}

	dev_info(&client->dev, "removing...\n");

	// Set sleep mode
//

//!
//! 	--- Interrupt ---
//!

//	free_irq(irq, NULL);


//!
//! 	--- IIO Subsystem ---
//!

	/** Get iio device for unregistration */
	iio = i2c_get_clientdata(client);

	if (!iio) {
		dev_err(&client->dev, "client get driver data failed\n");

		return -1;
	}

	st = iio_priv(iio);

	iio_device_unregister(iio);

	iio_device_free(iio);

	// Shutdown device power or enable sleep mode

	dev_info(&client->dev, "removing... done\n");

	return 0;
}


/**
 *
 * @struct	i2c_driver
 *
 * @brief	I2C driver structure to define the probe function and the remove
 * 			function. It also include the id table of I2C slave devices and
 * 			information related to this driver.
 *
 **/

static struct i2c_driver mpu_device_driver = {
	.id_table = mpu_device_i2c_id_table,
	.probe = mpu_device_probe,
	.remove = mpu_device_remove,
	.driver = {
		.name = THIS_MODULE->name,
		.owner = THIS_MODULE,
		.mod_name = THIS_MODULE->name,
		.of_match_table = of_match_ptr(mpu_device_of_match_table)
	}
};



/**
 *
 * @brief	Replace module_init() and module_exit() by probe and remove
 * 			function registered in i2c_driver.
 *
 * @param	mpu_device_driver from i2c_driver structure
 *
 **/

module_i2c_driver(mpu_device_driver);





/*! Description of the module driver */
MODULE_DESCRIPTION("MPU-9255 Intertial measurement unit driver");

/*! License of the module driver */
MODULE_LICENSE("GPL");

/*! Version of the module driver */
MODULE_VERSION("1.0");

/*! Author of the module driver */
MODULE_AUTHOR("Clement Ailloud");

