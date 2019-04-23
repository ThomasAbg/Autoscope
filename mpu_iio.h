/**
 *
 * @file 	mpu_iio.h
 *
 * @brief 	
 *
 * @version 1.0
 *
 * @date 	January 27th, 2019
 *
 * @author 	Clement Ailloud
 *
 **/

#ifndef MPU_IIO_H
#define MPU_IIO_H

#include <linux/iio/iio.h>
#include <linux/iio/sysfs.h>

#include "mpu_core.h"


const struct iio_chan_spec mpu_chan_spec[] = {
	/** Sensor temperature channel specification */
	{
		.type = IIO_TEMP,
		.address = MPU_TEMP_OUT_H,
		.info_mask_separate = BIT(IIO_CHAN_INFO_PROCESSED),
	},

	/** Accelerometer X channel specification */
	{
    	.type = IIO_ACCEL,
	    .channel2 = IIO_MOD_X,
	    .address = MPU_ACCEL_XOUT_H,
	    .info_mask_separate = BIT(IIO_CHAN_INFO_RAW),
	    .info_mask_shared_by_type = BIT(IIO_CHAN_INFO_SCALE),
	    .modified = 1
	},

	/** Accelerometer Y channel specification */
	{
    	.type = IIO_ACCEL,
	    .channel2 = IIO_MOD_Y,
	    .address = MPU_ACCEL_YOUT_H,
	    .info_mask_separate = BIT(IIO_CHAN_INFO_RAW),
	    .info_mask_shared_by_type = BIT(IIO_CHAN_INFO_SCALE),
	    .modified = 1
	},

	/** Accelerometer Z channel specification */
	{
    	.type = IIO_ACCEL,
	    .channel2 = IIO_MOD_Z,
	    .address = MPU_ACCEL_ZOUT_H,
	    .info_mask_separate = BIT(IIO_CHAN_INFO_RAW),
	    .info_mask_shared_by_type = BIT(IIO_CHAN_INFO_SCALE),
	    .modified = 1
	},

	/** Gyroscope X channel specification */
	{
    	.type = IIO_ANGL_VEL,
	    .channel2 = IIO_MOD_X,
	    .address = MPU_GYRO_XOUT_H,
	    .info_mask_separate = BIT(IIO_CHAN_INFO_RAW),
	    .info_mask_shared_by_type = BIT(IIO_CHAN_INFO_SCALE),
	    .modified = 1
	},

	/** Gyroscope Y channel specification */
	{
    	.type = IIO_ANGL_VEL,
	    .channel2 = IIO_MOD_Y,
	    .address = MPU_GYRO_YOUT_H,
	    .info_mask_separate = BIT(IIO_CHAN_INFO_RAW),
	    .info_mask_shared_by_type = BIT(IIO_CHAN_INFO_SCALE),
	    .modified = 1
	},

	/** Gyroscope Z channel specification */
	{
    	.type = IIO_ANGL_VEL,
	    .channel2 = IIO_MOD_Z,
	    .address = MPU_GYRO_ZOUT_H,
	    .info_mask_separate = BIT(IIO_CHAN_INFO_RAW),
	    .info_mask_shared_by_type = BIT(IIO_CHAN_INFO_SCALE),
	    .modified = 1
	}
};


#endif
