/*
 * mpu9250.c
 *
 *  Created on: Apr 19, 2018
 *      Author: Adrestia
 */
#include <stdint.h>
#include <stdbool.h>
#include "drivers/pinout.h"
#include "utils/i2c_nointerrupt.h"
#include "utils/mpu9250_regs.h"
#include "drivers/mpu9250.h"


uint8_t get_sensors(uint32_t module,uint8_t address, imu_raw_t* imu_ptr)
{
    if(get_accel(module, address, imu_ptr)) return 1;
    if(get_mag(module, AK8963_ADDRESS, imu_ptr)) return 2;
    if(get_gyro(module, address, imu_ptr)) return 3;
    if(get_tempc(module, address, imu_ptr)) return 4;

    return 0;
}

uint8_t initMPU9250(uint32_t module, uint8_t address)
{
    //this assumes you have already called init_i2c
    //for the respective i2c module this sensor is plugged into
    //pull the device out of sleep mode
    //(zero everything in PWR_MGMT_1 register)
    //sendi2cbyte(module,address,PWR_MGMT_1, 0x00);

    //enable magnetometer bypass
    sendi2cbyte(module,address, INT_PIN_CFG, 0x22);
    initAK8963(module,AK8963_ADDRESS);

    return 0;
}

uint8_t initAK8963(uint32_t module, uint8_t address)
{
    sendi2cbyte(module,address, AK8963_CNTL, MFS_14BITS << 4 | M_100HZ);
    //delay  5000us
    return 0;
}

uint8_t assert_MPU(uint32_t module, uint8_t address)
{
    //unit testing, returns 73
    ASSERT(geti2cbyte(module,address,WHO_AM_I_MPU9250) == I_AM_HERE_MPU9250);
    return 0;
}

uint8_t assert_mag(uint32_t module, uint8_t address)
{
    //unit testing
    ASSERT(geti2cbyte(module,address,WHO_AM_I_AK8963) == I_AM_HERE_AK8963);

    return 0;
}

uint8_t get_accel(uint32_t module, uint8_t address, imu_raw_t* imu_ptr)
{

    uint8_t xout[2];
    uint8_t yout[2];
    uint8_t zout[2];

    geti2cbytes(module,address,ACCEL_XOUT_H,2,xout);
    geti2cbytes(module,address,ACCEL_YOUT_H,2,yout);
    geti2cbytes(module,address,ACCEL_ZOUT_H,2,zout);

    imu_ptr->x_accel = ((int16_t )(xout[0] << 8))|(int16_t )(xout[1]);
    imu_ptr->y_accel = ((int16_t )(yout[0] << 8))|(int16_t )(yout[1]);
    imu_ptr->z_accel = ((int16_t )(zout[0] << 8))|(int16_t )(zout[1]);

    return 0;
}

uint8_t get_gyro(uint32_t module, uint8_t address, imu_raw_t* imu_ptr)
{

    uint8_t xout[2];
    uint8_t yout[2];
    uint8_t zout[2];

    geti2cbytes(module,address,GYRO_XOUT_H,2,xout);
    geti2cbytes(module,address,GYRO_YOUT_H,2,yout);
    geti2cbytes(module,address,GYRO_ZOUT_H,2,zout);

    imu_ptr->x_gyro = ((int16_t )(xout[0] << 8))|(int16_t )(xout[1]);
    imu_ptr->y_gyro = ((int16_t )(yout[0] << 8))|(int16_t )(yout[1]);
    imu_ptr->z_gyro = ((int16_t )(zout[0] << 8))|(int16_t )(zout[1]);

    return 0;
}

uint8_t get_mag2(uint32_t module, uint8_t address, imu_raw_t* imu_ptr)
{
    //make sure to switch the accelerometer on
    //by writing 0x00 to the PWR_MGMT_1 register first

    uint8_t xout[2];
    uint8_t yout[2];
    uint8_t zout[2];

    geti2cbytes(module,AK8963_ADDRESS,AK8963_XOUT_L ,2,xout);
    geti2cbytes(module,AK8963_ADDRESS,AK8963_YOUT_L ,2,yout);
    geti2cbytes(module,AK8963_ADDRESS,AK8963_ZOUT_L ,2,zout);

    imu_ptr->x_mag = ((int16_t )(xout[1] << 8))|(int16_t )(xout[0]);
    imu_ptr->y_mag = ((int16_t )(yout[1] << 8))|(int16_t )(yout[0]);
    imu_ptr->z_mag = ((int16_t )(zout[1] << 8))|(int16_t )(zout[0]);

    return 0;
}

uint8_t get_mag(uint32_t module, uint8_t address, imu_raw_t* imu_ptr)
{



    uint8_t out[7];

    //mag is auxiliary sensor, have to go to slave registers
    // (read the i2c slave of our i2c slave)
    //sendi2cbyte(module, address, I2C_SLV0_REG, AK8963_XOUT_L);
    // Read 7 bytes from I2C slave 0 (magnetometer)
    //sendi2cbyte(module, address, I2C_SLV0_CTRL, 0x87);
    // Move 7 registers from MPU reg to here
    geti2cbytes(module, address, AK8963_XOUT_L, 7, out);

    uint8_t c = 0 & out[6]; // End data read by reading ST2 register

    // Check if magnetic sensor overflow is set, if not then report data
    if(!(c & 0x08))
    {
        // Turn the MSB and LSB into a signed 16-bit value
        imu_ptr->x_mag = ((int16_t )(out[1] << 8))|(int16_t )(out[0]);
        imu_ptr->y_mag = ((int16_t )(out[3] << 8))|(int16_t )(out[2]);
        imu_ptr->z_mag = ((int16_t )(out[4] << 8))|(int16_t )(out[4]);

        return 0;
    }
    return 1; //error
}

uint8_t get_tempc(uint32_t module, uint8_t address, imu_raw_t* imu_ptr)
{
    //make sure to switch the accelerometer on
    //by writing 0x00 to the PWR_MGMT_1 register first

    uint8_t out[2];

    geti2cbytes(module,address,TEMP_OUT_H,2,out);

    imu_ptr->tempc = ((int16_t )(out[0] << 8))|(int16_t )(out[1]);

    return 0;
}



