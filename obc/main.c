#define F_CPU 16000000L // Specify oscillator frequency

#include <tasks.h>

// FreeRTOS
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>

extern TaskHandle_t I2C_task;

int main(void)
{
	
	// Debugging task to see that the FreeRTOS works
	extern void task_led_blinky(void *pvParameters);
	xTaskCreate(task_led_blinky, "Task to blink led pin 7", 100, NULL, 2, NULL);
	
	// Start Scheduler
	vTaskStartScheduler();
	
	/*--------------------------------*/

	/* Execution will only reach here if there was insufficient heap to start the scheduler. */
	for ( ;; );
	
	return 0;
}
