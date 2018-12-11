/*
 * mpu9250.c
 *
 *  Created on: Apr 19, 2018
 *      Author: Adrestia
 */

#ifndef __MPU9250_H__
#define __MPU9250_H__

typedef struct imu_data{
    int16_t x_accel;
    int16_t y_accel;
    int16_t z_accel;
    int16_t x_gyro;
    int16_t y_gyro;
    int16_t z_gyro;
    int16_t x_mag;
    int16_t y_mag;
    int16_t z_mag;
    int16_t tempc;
} imu_raw_t;

uint8_t get_sensors(uint32_t module,uint8_t address, imu_raw_t* imu_ptr);

uint8_t initMPU9250(uint32_t module, uint8_t address);

uint8_t assert_MPU(uint32_t module, uint8_t address);

uint8_t assert_mag(uint32_t module, uint8_t address);

uint8_t get_accel(uint32_t module, uint8_t address, imu_raw_t* imu_ptr);

uint8_t get_gyro(uint32_t module, uint8_t address, imu_raw_t* imu_ptr);

uint8_t get_mag(uint32_t module, uint8_t address, imu_raw_t* imu_ptr);

uint8_t get_tempc(uint32_t module, uint8_t address, imu_raw_t* imu_ptr);

#endif
