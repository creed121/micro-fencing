/**
 * @file accelerometer.h
 * @brief Accelerometer driver for the Micro-Fencing project using MPU-6050.
 * @author Christopher Reed, Micah Baker, Lydia Knierim, Samuel Prusia
 * @date 2025-11
 */

#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include <pic18f25k22.h>

/* ============================================================================
 * MPU-6050 Register Definitions
 * ============================================================================ */
#define MPU6050_PWR_MGMT_1      0x6B  // Power management register
#define MPU6050_GYRO_CONFIG     0x1B  // Gyroscope configuration register
#define MPU6050_ACCEL_CONFIG    0x1C  // Accelerometer configuration register
#define MPU6050_ACCEL_XOUT_H    0x3B  // Accelerometer X-axis high byte
#define MPU6050_GYRO_XOUT_H     0x43  // Gyroscope X-axis high byte
#define MPU6050_WHO_AM_I        0x75  // Device ID register

/* ============================================================================
 * Error Codes
 * ============================================================================ */
typedef enum
{
    ACC_SUCCESS          = 0x00,  // Operation successful
    ACC_I2C_ERROR        = 0x01,  // I2C communication error
    ACC_INIT_ERROR       = 0x02,  // Initialization error
    ACC_NOT_INITIALIZED  = 0x03,  // Accelerometer not initialized
    ACC_INVALID_PARAM    = 0x04   // Invalid parameter
} acc_error_t;

/* ============================================================================
 * Data Structure for Gyroscope Data
 * ============================================================================ */
typedef struct
{
    int gx;    // Gyroscope X-axis raw value
    int gy;    // Gyroscope Y-axis raw value
    int gz;    // Gyroscope Z-axis raw value
} gyro_data_t;

/* ============================================================================
 * Data Structure for Moving Average Buffer
 * ============================================================================ */
#define MOVING_AVG_BUFFER_SIZE 8  // Size of moving average buffer

typedef struct
{
    unsigned int buffer[MOVING_AVG_BUFFER_SIZE];
    unsigned char index;
    unsigned char is_full;
} moving_avg_t;

/* ============================================================================
 * Function Prototypes
 * ============================================================================ */

/**
 * @brief Initialize the MPU-6050 accelerometer/gyroscope.
 * 
 * Configures the MPU-6050 for gyroscope measurement with:
 * - ±250°/s sensitivity (GYRO_CONFIG = 0x00)
 * - Internal clock as timing source
 * 
 * @return acc_error_t Error code (ACC_SUCCESS or error)
 */
acc_error_t accelerometer_init(void);

/**
 * @brief Read raw gyroscope data from all three axes.
 * 
 * Performs I2C burst read of 6 bytes starting from GYRO_XOUT_H.
 * Combines high and low bytes into 16-bit signed values.
 * 
 * @param gyro Pointer to gyro_data_t structure to store results
 * @return acc_error_t Error code (ACC_SUCCESS or error)
 */
acc_error_t accelerometer_read_gyro(gyro_data_t* gyro);

/**
 * @brief Calculate the magnitude of angular velocity from gyroscope data.
 * 
 * Computes: magnitude = sqrt(gx^2 + gy^2 + gz^2)
 * Result is in raw gyroscope units (not °/s).
 * 
 * @param gyro Pointer to gyro_data_t structure with gyroscope values
 * @return unsigned int Magnitude of angular velocity
 */
unsigned int accelerometer_calculate_magnitude(gyro_data_t* gyro);

/**
 * @brief Update moving average buffer with new speed value.
 * 
 * Adds speed value to circular buffer and updates average.
 * Used for smoothing instantaneous speed measurements.
 * 
 * @param avg Pointer to moving_avg_t buffer structure
 * @param speed New speed value to add
 * @return void
 */
void accelerometer_update_moving_avg(moving_avg_t* avg, unsigned int speed);

/**
 * @brief Get the current moving average speed.
 * 
 * @param avg Pointer to moving_avg_t buffer structure
 * @return unsigned int Current moving average value (0 if buffer not full)
 */
unsigned int accelerometer_get_moving_avg(moving_avg_t* avg);

/**
 * @brief Map speed value to RGB LED color.
 * 
 * Speed Mapping (in raw gyro units):
 * - 0-100:        Red (255, 0, 0)
 * - 100-300:      Yellow (255, 255, 0)
 * - 300-600:      Green (0, 255, 0)
 * - 600+:         Blue (0, 0, 255)
 * 
 * @param speed Moving average speed value
 * @param r Pointer to red component (0-255)
 * @param g Pointer to green component (0-255)
 * @param b Pointer to blue component (0-255)
 * @return acc_error_t Error code (ACC_SUCCESS or error)
 */
acc_error_t accelerometer_speed_to_color(unsigned int speed, 
                                         unsigned char* r,
                                         unsigned char* g,
                                         unsigned char* b);

/**
 * @brief Reset the moving average buffer.
 * 
 * Clears the buffer and resets index/full flag.
 * 
 * @param avg Pointer to moving_avg_t buffer structure
 * @return void
 */
void accelerometer_reset_moving_avg(moving_avg_t* avg);

#endif  // ACCELEROMETER_H
