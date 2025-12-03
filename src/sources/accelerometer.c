/**
 * @file accelerometer.c
 * @brief Accelerometer driver for the Micro-Fencing project using MPU-6050.
 * @author Christopher Reed, Micah Baker, Lydia Knierim, Samuel Prusia
 * @date 2025-11
 */

// #include "../includes/accelerometer.h"

#include "accelerometer.h"

static unsigned char accelerometer_initialized = 0;

static unsigned int isqrt(unsigned long n);

/**
 * @brief Initialize the MPU-6050 accelerometer/gyroscope.
 */
acc_error_t accelerometer_init()
{
	MPU6050_initialize();
	if (!MPU6050_testConnection())
	{
		return ACC_INIT_ERROR;
	}
	accelerometer_initialized = 1;
	return ACC_SUCCESS;
}

/**
 * @brief Read raw gyroscope data from all three axes.
 * Performs I2C burst read of 6 bytes starting from GYRO_XOUT_H (0x43).
 */
acc_error_t accelerometer_read_gyro(gyro_data_t* gyro)
{	
	if (!accelerometer_initialized)
	{
		return ACC_NOT_INITIALIZED;
	}
	
	if (gyro == NULL)
	{
		return ACC_INVALID_PARAM;
	}
	
	MPU6050_getRotation(&(gyro_data->gx), &(gyro_data->gy), &(gyro_data->gz));

	// O-scope data
	//gyro->gx = (int16_t) 0b1111110110000001;
	
	return ACC_SUCCESS;
}

/**
 * @brief Calculate the magnitude of angular velocity.
 * Uses integer arithmetic: magnitude = sqrt(gx^2 + gy^2 + gz^2)
 */
acc_error_t accelerometer_calculate_magnitude_with_check(gyro_data_t* gyro, 
														 unsigned int* magnitude)
{
	if (gyro == NULL || magnitude == NULL)
	{
		return ACC_INVALID_PARAM;
	}
	
	// Use unsigned long to prevent overflow during multiplication
	unsigned long sum = 0;
	long gx_long = (long)gyro->gx;
	long gy_long = (long)gyro->gy;
	long gz_long = (long)gyro->gz;
    	
	sum = (gx_long * gx_long) + (gy_long * gy_long) + (gz_long * gz_long);
	
	// Integer square root approximation using Newton's method
	*magnitude = (unsigned int) isqrt(sum);
	
	return ACC_SUCCESS;
}

/**
 * @brief Simple integer square root using Newton's method.
 */
static unsigned int isqrt(unsigned long n)
{
	if (n == 0)
	{
		return 0;
	} 
	
	unsigned int x = n;
	unsigned int x1 = (x + 1) >> 1;
	
	while (x1 < x)
	{
		x = x1;
		x1 = (x + n / x) >> 1;
	}
	
	return x;
}

/**
 * @brief Simplified magnitude calculation (without check wrapper).
 */
unsigned int accelerometer_calculate_magnitude(gyro_data_t* gyro)
{
	unsigned int magnitude = 0;
	accelerometer_calculate_magnitude_with_check(gyro, &magnitude);
	return magnitude;
}

/**
 * @brief Update moving average buffer with new speed value.
 */
void accelerometer_update_moving_avg(moving_avg_t* avg, unsigned int speed)
{
	if (avg == NULL)
	{
		return;
	}
	
	// Add new value to circular buffer
	avg->buffer[avg->index] = speed;
	avg->index++;
	
	// Wrap around if buffer is full
	if (avg->index >= MOVING_AVG_BUFFER_SIZE)
	{
		avg->index = 0;
		avg->is_full = 1;
	}
}

/**
 * @brief Get the current moving average speed.
 */
unsigned int accelerometer_get_moving_avg(moving_avg_t* avg)
{
	unsigned int sum = 0;
	unsigned char i;
	unsigned char count = MOVING_AVG_BUFFER_SIZE;
	
	if (avg == NULL)
	{
		return 0;
	}
	
	// Only calculate average if buffer is full
	if (!avg->is_full)
	{
		return 0;
	}
	
	// Sum all values in the buffer
	for (i = 0; i < count; i++)
	{
		sum += avg->buffer[i];
	}
	
	// Return the average
	return sum / count;
}

/**
 * @brief Reset the moving average buffer.
 */
void accelerometer_reset_moving_avg(moving_avg_t* avg)
{
	unsigned char i;
	
	if (avg == NULL)
	{
		return;
	}
	
	for (i = 0; i < MOVING_AVG_BUFFER_SIZE; i++)
	{
		avg->buffer[i] = 0;
	}
	
	avg->index = 0;
	avg->is_full = 0;
}

/**
 * @brief Map speed value to RGB LED color.
 */
acc_error_t accelerometer_speed_to_color(unsigned int speed,
										 unsigned char* r,
										 unsigned char* g,
										 unsigned char* b)
{
	if (r == NULL || g == NULL || b == NULL)
	{
		return ACC_INVALID_PARAM;
	}
	
	if (speed <= 100)
	{
		// Stationary to slow: Red
		*r = 255;
		*g = 0;
		*b = 0;
	}
	else if (speed <= 300)
	{
		// Slow to medium: Yellow (transitioning from Red to Green)
		*r = 255;
		*g = 255;
		*b = 0;
	}
	else if (speed <= 600)
	{
		// Medium to fast: Green
		*r = 0;
		*g = 255;
		*b = 0;
	}
	else
	{
		// Very fast: Blue
		*r = 0;
		*g = 0;
		*b = 255;
	}
	
	return ACC_SUCCESS;
}
