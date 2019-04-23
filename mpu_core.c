#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/regmap.h>
#include <linux/mutex.h>
#include <linux/iio/iio.h>

#include "mpu_core.h"


int check_device_identity(struct regmap *map, u8 dev_id){
	int ret = -1;
	
	unsigned tmp_dev_id = 0;

	pr_info("mpu9255: checking for device identity...\n");

	if (!map)
		return -1;

	ret = regmap_read(map, MPU_WHO_AM_I, &tmp_dev_id);

	if (ret < 0){
		pr_alert("mpu9255: checking for device identity... failed\n");

		return ret;
	}

	pr_info("mpu9255: checking for device identity... done\n");

	if (tmp_dev_id != dev_id){
		pr_info("mpu9255: invalid chip identity: %d\n", tmp_dev_id);

		return -1;
	} 

	pr_info("mpu9255: chip id: %d\n", tmp_dev_id);	

	return 0;
}

EXPORT_SYMBOL_GPL(check_device_identity);


/**
 *
 * @fn 		int mpu_set_power_mode(struct spi_device *dev, enum power_mode pm);
 *
 * @brief 	Change the power mode of a MPU device.
 *
 **/

int set_power_mode(struct regmap *map, enum power_mode pm){
	int ret = -1;

	unsigned int tx_buf = 0;

	if (!map)
		return -1;

	// Perform read operation on both PWR_MGMT registers for applying bitmask
	// on tx_buf afterwards
	ret = regmap_bulk_read(map, MPU_PWR_MGMT_1, &tx_buf, 2);

	// Invert endianness
	tx_buf = ((tx_buf << 8) & 0xFF00) | ((tx_buf >> 8) & 0x00FF);

	pr_info("PWR_MGMT: 0x%04x\n", (unsigned int) tx_buf);
	pr_info("PWR_MGMT!:%u\n", (unsigned int) tx_buf);

	if (ret < 0)
		return ret;

  	switch(pm){
		case SLEEP:
			tx_buf |= MPU_SLEEP_BIT_MASK; // 0x40

			ret = regmap_write(map, MPU_PWR_MGMT_1, tx_buf);

			break;
		case STANDBY:
#if 0
			tx_buf |= MPU_GYRO_STANDBY_BIT_MASK; // 0x10
#endif

			break;
		case LOW_POWER_ACCELEROMETER:
#if 0
			// Disable sleep mode
			tx_buf &= MPU_SLEEP_BIT_MASK << 8;

			// Enable duty-cycled clock			
			tx_buf |= MPU_CYCLE_BIT_MASK << 8;
#endif

			break;
		case LOW_NOISE_ACCELEROMETER:
			break;
		case GYROSCOPE:
			 /*!
			 * Disable sleep mode:
			 * 	- set SLEEP bit to 0 of PWR_MGMT_1
			 */

			tx_buf &= ~(MPU_SLEEP_BIT_MASK) << 8;

			/*! 
			 * Enable all axis of accelerometer and gyroscope:
			 *  - set [0:5] bits to 0 of PWR_MGMT_2
			 */

			tx_buf &= ~(MPU_DISABLE_XG_MASK | MPU_DISABLE_YG_MASK | MPU_DISABLE_ZG_MASK);

			ret = regmap_bulk_write(map, MPU_PWR_MGMT_1, &tx_buf, 2);

			if (ret < 0)
				return ret;

			break;
		case MAGNETOMETER:
			break;
		case ACCEL_AND_GYRO:

			 /*!
			 * Disable sleep mode:
			 * 	- set SLEEP bit to 0 of PWR_MGMT_1
			 */

			tx_buf &= ~(MPU_SLEEP_BIT_MASK) << 8;

			/*! 
			 * Enable all axis of accelerometer and gyroscope:
			 *  - set [0:5] bits to 0 of PWR_MGMT_2
			 */

			tx_buf &= ~(MPU_DISABLE_XA_MASK | MPU_DISABLE_YA_MASK | MPU_DISABLE_ZA_MASK |
					 	MPU_DISABLE_XG_MASK | MPU_DISABLE_YG_MASK | MPU_DISABLE_ZG_MASK);

			ret = regmap_bulk_write(map, MPU_PWR_MGMT_1, &tx_buf, 2);

			if (ret < 0)
				return ret;

			break;
		case ACCEL_AND_MAGNETOMETER:

			/*!
			 * Disable sleep mode:
			 * 	- set SLEEP bit to 0 of PWR_MGMT_1
			 */

			tx_buf &= ~(0b01000000) << 8;

			/*!
		 * Enable all axis of accelerometer
			 */

			tx_buf &= ~(0b00111000);

			// Do I/O for accelerometer and gyroscope
			ret = regmap_bulk_write(map, MPU_PWR_MGMT_1, &tx_buf, 2);

			if (ret < 0)
				return ret;

			/*!
			 * Enable all axis of magnetometer
			 */



			break;
		case ALL_AXIS:

			 /*!
			 * Disable sleep mode:
			 * 	- set SLEEP bit to 0 of PWR_MGMT_1
			 */

			tx_buf &= ~(0b01000000);

			/*! 
			 * Enable all axis of accelerometer and gyroscope:
			 *  - set [0:5] bits to 0 of PWR_MGMT_2
			 */

			tx_buf &= ~(0b00111111);

			// Do I/O for accelerometer and gyroscope
			ret = regmap_bulk_write(map, MPU_PWR_MGMT_1, &tx_buf, 2);

			if (ret < 0)
				return ret;

			/*!
			 * Enable all axis of magnetometer
			 */


			// Do I/O


			if (ret < 0)
				return ret;

			break;
		default:
			pr_warn("mpu9255: unknown power mode %d\n", (int) pm);

			return -EINVAL;
	}

	return 0;
}

EXPORT_SYMBOL_GPL(set_power_mode);


int set_accelerometer_full_scale(struct regmap *map, enum Accel_fs fs){
	int ret = -1;

	unsigned reg = 0;

	if (!map)
		return -1;

	ret = regmap_read(map, MPU_ACCEL_CONFIG, &reg);

	if (ret < 0)
		return ret;

	// Reset all bit of ACCEL_FS_SEL
	reg &= ~(ACCEL_FS_SEL_MASK);

	switch (fs) {
		case ACCEL_FS_SEL_2G:
			// Do nothing (ACCEL_FS_SEL is already set to 0)

			break;
		case ACCEL_FS_SEL_4G:
			// Select accelerometer full scale to 4G
			reg |= ACCEL_FS_SEL_4G << 3;

			break;
		case ACCEL_FS_SEL_8G:
			// Select accelerometer full scale to 4G
			reg |= ACCEL_FS_SEL_8G << 3;

			break;
		case ACCEL_FS_SEL_16G:
			// Select accelerometer full scale to 4G
			reg |= ACCEL_FS_SEL_16G << 3;

			break;
		default:
			break;
	}

	ret = regmap_write(map, MPU_ACCEL_CONFIG, reg);

	if (ret < 0)
		return ret;

	return 0;
}

EXPORT_SYMBOL_GPL(set_accelerometer_full_scale);


int set_gyroscope_full_scale(struct regmap *map, enum Gyro_fs fs){
	int ret = -1;

	unsigned reg = 0;

	if (!map)
		return -1;

	// Get current register content
	ret = regmap_read(map, MPU_GYRO_CONFIG, &reg);

	if (ret < 0)
		return ret;

	// Reset all bit of GYRO_FS_SEL
	reg &= ~(GYRO_FS_SEL_MASK);

	// Update register content
	switch (fs) {
		case GYRO_FS_SEL_250DPS:
			// Do nothing (GYRO_FS_SEL is already set to 0)

			break;
		case GYRO_FS_SEL_500DPS:
			reg |= GYRO_FS_SEL_500DPS << GYRO_FS_SEL_LSB;

			break;
		case GYRO_FS_SEL_1000DPS:
			reg |= GYRO_FS_SEL_1000DPS << GYRO_FS_SEL_LSB;

			break;
		case GYRO_FS_SEL_2000DPS:
			reg |= GYRO_FS_SEL_2000DPS << GYRO_FS_SEL_LSB;

			break;
		default:
			pr_err("Unknown gyroscope full scale\n");

			break;
	}

	ret = regmap_write(map, MPU_GYRO_CONFIG, reg);

	if (ret < 0)
		return ret;

	return 0;
}

EXPORT_SYMBOL_GPL(set_gyroscope_full_scale);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Clement Ailloud");
