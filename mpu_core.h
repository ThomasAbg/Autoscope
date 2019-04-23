/**
 *
 * @file 		mpu_core.h
 *
 * @brief 		Here are defined all register, enumeration for imu mode and structure for imu data
 *
 * @version 	1.0
 *
 * @date 		January 27th, 2019
 *
 * @author 		Clement Ailloud
 *
 **/


#ifndef MPU_CORE_H
#define MPU_CORE_H

#include <linux/kernel.h>
#include <linux/regmap.h>

/*! Read register mask */
#define READ_REG_MASK  (0b10000000)

/*! Write register mask */
#define WRITE_REG_MASK (~READ_REG_MASK)


#define MPU_DEFAULT_ADDRESS         /* Unknown */
#define MPU_ALT_DEFAULT_ADDRESS     /* Unknown */



/*!
 * This register is ued to verify the identity of the device. The contents of
 * WHO_AM_I is an 8-bit device ID. The default value of the register is 0x71.
 */

#define WHO_AM_I									(0x75)

/*!
 * Gyroscope X-axis self-test register
 */

#define MPU_SELF_TEST_X_GYRO        			(0x00)

/*!
 * Gyroscope Y-axis self-test register
 */

#define MPU_SELF_TEST_Y_GYRO        			(0x01)

/*!
 * Gyroscope Z-axis self-test register
 */

#define MPU_SELF_TEST_Z_GYRO        			(0x02)

/*!
 * Accelerometer X-axis self-test registers
 */

#define MPU_SELF_TEST_X_ACCEL       0x0D

/*!
 * Accelerometer Y-axis self-test registers address
 */

#define MPU_SELF_TEST_Y_ACCEL       0x0E

/*!
 * Accelerometer Z-axis self-test registers address
 */

#define MPU_SELF_TEST_Z_ACCEL       0x0F

/*!
 * Gyro offset high register address
 */

#define MPU_XG_OFFSET_H             0x13

/*!
 * Gyro offset low register address
 */

#define MPU_XG_OFFSET_L             0x14


/*!
 * Gyro offset high register address
 */

#define MPU_YG_OFFSET_H             0x15
#define MPU_YG_OFFSET_L             0x16
#define MPU_ZG_OFFSET_H             0x17
#define MPU_ZG_OFFSET_L             0x18

/*!
 * Sample rate divider
 */

#define MPU_SMPLRT_DIV              0x19  

/*!
 * MPU-9255 configuration register address
 */

#define MPU_CONFIG                  0x1A

/*! Gyroscope configuration register address */
#define MPU_GYRO_CONFIG             0x1B

/*! Accelerometer configuration register address */
#define MPU_ACCEL_CONFIG            0x1C

/*! Accelerometer configuration 2 register adddress */
#define MPU_ACCEL_CONFIG2           0x1D

#define MPU_LP_ACCEL_ODR            0x1E

#define MPU_WOM_THR                 0x1F

#define MPU_FIFO_EN                 0x23
#define MPU_I2C_MST_CTRL            0x24
#define MPU_I2C_SLV0_ADDR           0x25
#define MPU_I2C_SLV0_REG            0x26
#define MPU_I2C_SLV0_CTRL           0x27
#define MPU_I2C_SLV1_ADDR           0x28
#define MPU_I2C_SLV1_REG            0x29
#define MPU_I2C_SLV1_CTRL           0x2A
#define MPU_I2C_SLV2_ADDR           0x2B
#define MPU_I2C_SLV2_REG            0x2C
#define MPU_I2C_SLV2_CTRL           0x2D
#define MPU_I2C_SLV3_ADDR           0x2E
#define MPU_I2C_SLV3_REG            0x2F
#define MPU_I2C_SLV3_CTRL           0x30
#define MPU_I2C_SLV4_ADDR           0x31
#define MPU_I2C_SLV4_REG            0x32
#define MPU_I2C_SLV4_DO             0x33
#define MPU_I2C_SLV4_CTRL           0x34
#define MPU_I2C_SLV4_DI             0x35
#define MPU_I2C_MST_STATUS          0x36
#define MPU_INT_PIN_CFG             0x37
#define MPU_INT_ENABLE              0x38

#define MPU_INT_STATUS              0x3A
#define MPU_ACCEL_XOUT_H            0x3B
#define MPU_ACCEL_XOUT_L            0x3C
#define MPU_ACCEL_YOUT_H            0x3D
#define MPU_ACCEL_YOUT_L            0x3E
#define MPU_ACCEL_ZOUT_H            0x3F
#define MPU_ACCEL_ZOUT_L            0x40
#define MPU_TEMP_OUT_H              0x41
#define MPU_TEMP_OUT_L              0x42
#define MPU_GYRO_XOUT_H             0x43
#define MPU_GYRO_XOUT_L             0x44
#define MPU_GYRO_YOUT_H             0x45
#define MPU_GYRO_YOUT_L             0x46
#define MPU_GYRO_ZOUT_H             0x47
#define MPU_GYRO_ZOUT_L             0x48
#define MPU_EXT_SENS_DATA_00        0x49
#define MPU_EXT_SENS_DATA_01        0x4A
#define MPU_EXT_SENS_DATA_02        0x4B
#define MPU_EXT_SENS_DATA_03        0x4C
#define MPU_EXT_SENS_DATA_04        0x4D
#define MPU_EXT_SENS_DATA_05        0x4E
#define MPU_EXT_SENS_DATA_06        0x4F
#define MPU_EXT_SENS_DATA_07        0x50
#define MPU_EXT_SENS_DATA_08        0x51
#define MPU_EXT_SENS_DATA_09        0x52
#define MPU_EXT_SENS_DATA_10        0x53
#define MPU_EXT_SENS_DATA_11        0x54
#define MPU_EXT_SENS_DATA_12        0x55
#define MPU_EXT_SENS_DATA_13        0x56
#define MPU_EXT_SENS_DATA_14        0x57
#define MPU_EXT_SENS_DATA_15        0x58
#define MPU_EXT_SENS_DATA_16        0x59
#define MPU_EXT_SENS_DATA_17        0x5A
#define MPU_EXT_SENS_DATA_18        0x5B
#define MPU_EXT_SENS_DATA_19        0x5C
#define MPU_EXT_SENS_DATA_20        0x5D
#define MPU_EXT_SENS_DATA_21        0x5E
#define MPU_EXT_SENS_DATA_22        0x5F
#define MPU_EXT_SENS_DATA_23        0x60

#define MPU_I2C_SLV0_DO             0x63
#define MPU_I2C_SLV1_DO             0x64
#define MPU_I2C_SLV2_DO             0x65
#define MPU_I2C_SLV3_DO             0x66
#define MPU_I2C_MST_DELAY_CTRL      0x67
#define MPU_SIGNAL_PATH_RESET       0x68
#define MPU_MOT_DETECT_CTRL         0x69
#define MPU_USER_CTRL               0x6A
#define MPU_PWR_MGMT_1              0x6B
#define MPU_PWR_MGMT_2              0x6C

#define MPU_FIFO_COUNTH             0x72
#define MPU_FIFO_COUNTL             0x73
#define MPU_FIFO_R_W                0x74
#define MPU_WHO_AM_I                0x75
#define MPU_XA_OFFSET_H             0x77
#define MPU_XA_OFFSET_L             0x78

#define MPU_YA_OFFSET_H             0x7A
#define MPU_YA_OFFSET_L             0x7B

#define MPU_ZA_OFFSET_H             0x7D
#define MPU_ZA_OFFSET_L             0x7E

/* End of MPU_9255 register map */

//CONFIG reister masks
#define MPU_FIFO_MODE_MASK          0x40
#define MPU_EXT_SYNC_SET_MASK       0x38
#define MPU_DLPF_CFG_MASK           0x07

//GYRO_CONFIG reister masks
#define MPU_XGYRO_CTEN_MASK         0x80
#define MPU_YGYRO_CTEN_MASK         0x40
#define MPU_ZGYRO_CTEN_MASK         0x20
#define MPU_GYRO_FS_SEL_MASK        0x18
#define MPU_FCHOICE_B_MASK          0x03

#define MPU_GYRO_FULL_SCALE_250DPS  0
#define MPU_GYRO_FULL_SCALE_500DPS  1
#define MPU_GYRO_FULL_SCALE_1000DPS 2
#define MPU_GYRO_FULL_SCALE_2000DPS 3

//ACCEL_CONFIG reister masks
#define MPU_AX_ST_EN_MASK           0x80
#define MPU_AY_ST_EN_MASK           0x40
#define MPU_AZ_ST_EN_MASK           0x20
#define MPU_ACCEL_FS_SEL_MASK       0x18

#define MPU_FULL_SCALE_2G           0
#define MPU_FULL_SCALE_4G           1
#define MPU_FULL_SCALE_8G           2
#define MPU_FULL_SCALE_16G          3

//ACCEL_CONFIG_2 reister masks
#define MPU_ACCEL_FCHOICE_B_MASK    0xC0
#define MPU_A_DLPF_CFG_MASK         0x03

//LP_ACCEL_ODR reister masks
#define MPU_LPOSC_CLKSEL_MASK       0x0F

//FIFO_EN reister masks
#define MPU_TEMP_FIFO_EN_MASK       0x80
#define MPU_GYRO_XOUT_MASK          0x40
#define MPU_GYRO_YOUT_MASK          0x20
#define MPU_GYRO_ZOUT_MASK          0x10
#define MPU_ACCEL_MASK              0x08
#define MPU_SLV2_MASK               0x04
#define MPU_SLV1_MASK               0x02
#define MPU_SLV0_MASK               0x01

//I2C_MST_CTRL reister masks
#define MPU_MULT_MST_EN_MASK        0x80
#define MPU_WAIT_FOR_ES_MASK        0x40
#define MPU_SLV_3_FIFO_EN_MASK      0x20
#define MPU_I2C_MST_P_NSR_MASK      0x10
#define MPU_I2C_MST_CLK_MASK        0x0F

//I2C_SLV0_ADDR reister masks
#define MPU_I2C_SLV0_RNW_MASK       0x80
#define MPU_I2C_ID_0_MASK           0x7F

//I2C_SLV0_CTRL reister masks
#define MPU_I2C_SLV0_EN_MASK        0x80
#define MPU_I2C_SLV0_BYTE_SW_MASK   0x40
#define MPU_I2C_SLV0_REG_DIS_MASK   0x20
#define MPU_I2C_SLV0_GRP_MASK       0x10
#define MPU_I2C_SLV0_LENG_MASK      0x0F

//I2C_SLV1_ADDR reister masks
#define MPU_I2C_SLV1_RNW_MASK       0x80
#define MPU_I2C_ID_1_MASK           0x7F

//I2C_SLV1_CTRL reister masks
#define MPU_I2C_SLV1_EN_MASK        0x80
#define MPU_I2C_SLV1_BYTE_SW_MASK   0x40
#define MPU_I2C_SLV1_REG_DIS_MASK   0x20
#define MPU_I2C_SLV1_GRP_MASK       0x10
#define MPU_I2C_SLV1_LENG_MASK      0x0F

//I2C_SLV2_ADDR reister masks
#define MPU_I2C_SLV2_RNW_MASK       0x80
#define MPU_I2C_ID_2_MASK           0x7F

//I2C_SLV2_CTRL reister masks
#define MPU_I2C_SLV2_EN_MASK        0x80
#define MPU_I2C_SLV2_BYTE_SW_MASK   0x40
#define MPU_I2C_SLV2_REG_DIS_MASK   0x20
#define MPU_I2C_SLV2_GRP_MASK       0x10
#define MPU_I2C_SLV2_LENG_MASK      0x0F

//I2C_SLV3_ADDR reister masks
#define MPU_I2C_SLV3_RNW_MASK       0x80
#define MPU_I2C_ID_3_MASK           0x7F

//I2C_SLV3_CTRL reister masks
#define MPU_I2C_SLV3_EN_MASK        0x80
#define MPU_I2C_SLV3_BYTE_SW_MASK   0x40
#define MPU_I2C_SLV3_REG_DIS_MASK   0x20
#define MPU_I2C_SLV3_GRP_MASK       0x10
#define MPU_I2C_SLV3_LENG_MASK      0x0F

//I2C_SLV4_ADDR reister masks
#define MPU_I2C_SLV4_RNW_MASK       0x80
#define MPU_I2C_ID_4_MASK           0x7F

//I2C_SLV4_CTRL reister masks
#define MPU_I2C_SLV4_EN_MASK        0x80
#define MPU_SLV4_DONE_INT_EN_MASK   0x40
#define MPU_I2C_SLV4_REG_DIS_MASK   0x20
#define MPU_I2C_MST_DLY_MASK        0x1F

//I2C_MST_STATUS reister masks
#define MPU_PASS_THROUGH_MASK       0x80
#define MPU_I2C_SLV4_DONE_MASK      0x40
#define MPU_I2C_LOST_ARB_MASK       0x20
#define MPU_I2C_SLV4_NACK_MASK      0x10
#define MPU_I2C_SLV3_NACK_MASK      0x08
#define MPU_I2C_SLV2_NACK_MASK      0x04
#define MPU_I2C_SLV1_NACK_MASK      0x02
#define MPU_I2C_SLV0_NACK_MASK      0x01

//INT_PIN_CFG reister masks
#define MPU_ACTL_MASK               0x80
#define MPU_OPEN_MASK               0x40
#define MPU_LATCH_INT_EN_MASK       0x20
#define MPU_INT_ANYRD_2CLEAR_MASK   0x10
#define MPU_ACTL_FSYNC_MASK         0x08
#define MPU_FSYNC_INT_MODE_EN_MASK  0x04
#define MPU_BYPASS_EN_MASK          0x02

//INT_ENABLE reister masks
#define MPU_WOM_EN_MASK             0x40
#define MPU_FIFO_OFLOW_EN_MASK      0x10
#define MPU_FSYNC_INT_EN_MASK       0x08
#define MPU_RAW_RDY_EN_MASK         0x01

//INT_STATUS reister masks
#define MPU_WOM_INT_MASK            0x40
#define MPU_FIFO_OFLOW_INT_MASK     0x10
#define MPU_FSYNC_INT_MASK          0x08
#define MPU_RAW_DATA_RDY_INT_MASK   0x01

//I2C_MST_DELAY_CTRL reister masks
#define MPU_DELAY_ES_SHADOW_MASK    0x80
#define MPU_I2C_SLV4_DLY_EN_MASK    0x10
#define MPU_I2C_SLV3_DLY_EN_MASK    0x08
#define MPU_I2C_SLV2_DLY_EN_MASK    0x04
#define MPU_I2C_SLV1_DLY_EN_MASK    0x02
#define MPU_I2C_SLV0_DLY_EN_MASK    0x01

//SIGNAL_PATH_RESET reister masks
#define MPU_GYRO_RST_MASK           0x04
#define MPU_ACCEL_RST_MASK          0x02
#define MPU_TEMP_RST_MASK           0x01

//MOT_DETECT_CTRL reister masks
#define MPU_ACCEL_INTEL_EN_MASK     0x80
#define MPU_ACCEL_INTEL_MODE_MASK   0x40

//USER_CTRL reister masks
#define MPU_FIFO_EN_MASK            0x40
#define MPU_I2C_MST_EN_MASK         0x20
#define MPU_I2C_IF_DIS_MASK         0x10
#define MPU_FIFO_RST_MASK           0x04
#define MPU_I2C_MST_RST_MASK        0x02
#define MPU_SIG_COND_RST_MASK       0x01

//PWR_MGMT_1 reister masks
#define MPU_H_RESET_MASK            0x80
#define MPU_SLEEP_MASK              0x40
#define MPU_CYCLE_MASK              0x20
#define MPU_GYRO_STANDBY_CYCLE_MASK 0x10
#define MPU_PD_PTAT_MASK            0x08
#define MPU_CLKSEL_MASK             0x07

//PWR_MGMT_2 reister masks
#define MPU_DISABLE_XA_MASK         0x20
#define MPU_DISABLE_YA_MASK         0x10
#define MPU_DISABLE_ZA_MASK         0x08
#define MPU_DISABLE_XG_MASK         0x04
#define MPU_DISABLE_YG_MASK         0x02
#define MPU_DISABLE_ZG_MASK         0x01

#define MPU_DISABLE_XYZA_MASK       0x38
#define MPU_DISABLE_XYZG_MASK       0x07

//Manetometer reister maps
#define MPU_MAG_ADDRESS             0x0C

#define MPU_MAG_WIA                 0x00
#define MPU_MAG_INFO                0x01
#define MPU_MAG_ST1                 0x02
#define MPU_MAG_XOUT_L              0x03
#define MPU_MAG_XOUT_H              0x04
#define MPU_MAG_YOUT_L              0x05
#define MPU_MAG_YOUT_H             0x06
#define MPU_MAG_ZOUT_L             0x07
#define MPU_MAG_ZOUT_H             0x08
#define MPU_MAG_ST2                0x09
#define MPU_MAG_CNTL               0x0A
#define MPU_MAG_RSV                0x0B /* Warning: Do not be used */
#define MPU_MAG_ASTC               0x0C
#define MPU_MAG_TS1                0x0D
#define MPU_MAG_TS2                0x0E
#define MPU_MAG_I2CDIS             0x0F
#define MPU_MAG_ASAX               0x10
#define MPU_MAG_ASAY               0x11
#define MPU_MAG_ASAZ 				0x12

#define MPU_RESET_BIT_MASK 				0x80
#define MPU_SLEEP_BIT_MASK 				0x40
#define MPU_CYCLE_BIT_MASK 				0x20
#define MPU_GYRO_STANDBY_BIT_MASK 		0x10
#define MPU_PD_PTAT_BIT_MASK 			0x08
#define MPU_CLKSETL_BITS_MASK 			0x07

#define MPU_DISABLE_XA_BIT_MASK 		(0x20)
#define MPU_DISABLE_YA_BIT_MASK 		(0x10)
#define MPU_DISABLE_ZA_BIT_MASK 		(0x08)
#define MPU_DISABLE_XG_BIT_MASK 		(0x04)
#define MPU_DISABLE_YG_BIT_MASK 		(0x02)
#define MPU_DISABLE_ZG_BIT_MASK 		(0x01)

#define ACCEL_FS_SEL_MASK 				(0x18)
#define GYRO_FS_SEL_MASK 				(0x18)

#define ACCEL_FS_SEL_LSB				(0x03)
#define GYRO_FS_SEL_LSB					(0x03)

/*!< MPU-9250 device ID */
#define MPU_9250_DEVICE_ID 				(0x71)

/*!< MPU-9255 device ID */
#define MPU_9255_DEVICE_ID 				(0x73)


/*!
 *
 * @struct 	Accelerometer_output mpu_core.h
 *
 * @brief  	Structure which contains the accelerometer output data for all axis.
 *
 * @var   	x 
 *
 * @var 	y
 *
 * @var 	z
 *
 **/

struct Accelerometer_output {
	s16 x;
	s16 y;
	s16 z;
};

/*!
 *
 * @struct 	Gyroscope_output mpu_core.h
 *
 * @brief  	Structure which contains the gyroscope output data for all axis.
 *
 * @var   	x 
 *
 * @var 	y
 *
 * @var 	z
 *
 **/

struct Gyroscope_output {
	s16 x;
	s16 y;
	s16 z;
};

/*!
 *
 * @struct 	Magnetometer_output mpu_core.h
 *
 * @brief  	Structure which contains the magnetometer output data for all axis.
 *
 * @var   	x 
 *
 * @var 	y
 *
 * @var 	z
 *
 **/

struct Magnetometer_output {
	unsigned x : 14;
	unsigned y : 14;
	unsigned z : 14;
};


/**
 *
 * 	Standard power mode
 * 	-------------------
 *
 * 	+-------------------------------------------------------+
 * 	| MODE | GYROSCOPE | ACCELEROMETER | MAGNETOMETER | DPM |
 * 	+------+-----------+---------------+--------------+-----+
 * 	| 1    |    Off    |      Off      |      Off     | Off |
 * 	+------+-----------+---------------+--------------+-----+
 * 	| 2
 * 	+
 * 	|
 * 	+
 * 	|
 * 	+
 *
 **/


/*!
 *
 * @enum 	power_mode
 *
 * @brief 	Standard power mode of MPU-9255. Manage power and used sensors.
 *
 * @var 	SLEEP
 *
 * @var 	STANDBY
 *
 * @var 	LOW_POWER_ACCELEROMETER
 *
 * @var 	LOW_NOISE_ACCELEROMETER
 *
 * @var 	GYROSCOPE
 *
 * @var 	MAGNETOMETER
 *
 * @var  	ACCEL_AND_GYRO
 *
 * @var 	ACCEL_AND_MAGNETOMETER
 *
 * @var 	ALL_AXIS
 *
 */

enum power_mode {
	SLEEP = 1,
	STANDBY,
	LOW_POWER_ACCELEROMETER,
	LOW_NOISE_ACCELEROMETER, 
	GYROSCOPE,
	MAGNETOMETER,
	ACCEL_AND_GYRO,
	ACCEL_AND_MAGNETOMETER,
	ALL_AXIS
};


/**
 *
 * @enum 	accel_fs
 *
 * @brief 	Accelerometer full scale selection
 *
 * 			Modify the accelerometer sensitivity from 2G to 16G
 *
 * @var 	ACCEL_FS_SEL_2G
 *
 * @var 	ACCEL_FS_SEL_4G
 *
 * @var 	ACCEL_FS_SEL_8G
 *
 * @var 	ACCEL_FS_SEL_16G
 *
 */

enum Accel_fs {
	ACCEL_FS_SEL_2G,
	ACCEL_FS_SEL_4G,
	ACCEL_FS_SEL_8G,
	ACCEL_FS_SEL_16G
};


/**
 *
 * @enum 	gyro_fs
 *
 * @brief 	Gyroscope full scale selection
 *
 * 			Modify the gyroscope sensitivity from 250DPS to 2000DPS
 *
 * @var 	GYRO_FS_SEL_250DPS
 *
 * @var 	GYRO_FS_SEL_500DPS
 *
 * @var 	GYRO_FS_SEL_1000DPS
 *
 * @var 	GYRO_FS_SEL_2000DPS
 *
 */

enum Gyro_fs {
	GYRO_FS_SEL_250DPS,
	GYRO_FS_SEL_500DPS,
	GYRO_FS_SEL_1000DPS,
	GYRO_FS_SEL_2000DPS
};


int check_device_identity(struct regmap *map, u8 dev_id);

int set_power_mode(struct regmap *map, enum power_mode pm);

int set_accelerometer_full_scale(struct regmap *map, enum Accel_fs fs);

int set_gyroscope_full_scale(struct regmap *map, enum Gyro_fs fs);

#endif
