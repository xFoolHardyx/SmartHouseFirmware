#ifndef MPU_WRAPPERS_H
#define MPU_WRAPPERS_H

/* This file redefines API functions to be called through a wrapper macro, but
only for ports that are using the MPU. */
#ifdef portUSING_MPU_WRAPPERS

	/* MPU_WRAPPERS_INCLUDED_FROM_API_FILE will be defined when this file is
	included from queue.c or task.c to prevent it from having an effect within
	those files. */
	#ifndef MPU_WRAPPERS_INCLUDED_FROM_API_FILE

		#define xTaskGenericCreate				MPU_xTaskGenericCreate
		#define vTaskAllocateMPURegions			MPU_vTaskAllocateMPURegions
		#define vTaskDelete						MPU_vTaskDelete
		#define vTaskDelayUntil					MPU_vTaskDelayUntil
		#define vTaskDelay						MPU_vTaskDelay
		#define uxTaskPriorityGet				MPU_uxTaskPriorityGet
		#define vTaskPrioritySet				MPU_vTaskPrioritySet
		#define vTaskSuspend					MPU_vTaskSuspend
		#define xTaskIsTaskSuspended			MPU_xTaskIsTaskSuspended
		#define vTaskResume						MPU_vTaskResume
		#define vTaskSuspendAll					MPU_vTaskSuspendAll
		#define xTaskResumeAll					MPU_xTaskResumeAll
		#define xTaskGetTickCount				MPU_xTaskGetTickCount
		#define uxTaskGetNumberOfTasks			MPU_uxTaskGetNumberOfTasks
		#define vTaskList						MPU_vTaskList
		#define vTaskGetRunTimeStats			MPU_vTaskGetRunTimeStats
		#define vTaskSetApplicationTaskTag		MPU_vTaskSetApplicationTaskTag
		#define xTaskGetApplicationTaskTag		MPU_xTaskGetApplicationTaskTag
		#define xTaskCallApplicationTaskHook	MPU_xTaskCallApplicationTaskHook
		#define uxTaskGetStackHighWaterMark		MPU_uxTaskGetStackHighWaterMark
		#define xTaskGetCurrentTaskHandle		MPU_xTaskGetCurrentTaskHandle
		#define xTaskGetSchedulerState			MPU_xTaskGetSchedulerState

		#define xQueueGenericCreate				MPU_xQueueGenericCreate
		#define xQueueCreateMutex				MPU_xQueueCreateMutex
		#define xQueueGiveMutexRecursive		MPU_xQueueGiveMutexRecursive
		#define xQueueTakeMutexRecursive		MPU_xQueueTakeMutexRecursive
		#define xQueueCreateCountingSemaphore	MPU_xQueueCreateCountingSemaphore
		#define xQueueGenericSend				MPU_xQueueGenericSend
		#define xQueueAltGenericSend			MPU_xQueueAltGenericSend
		#define xQueueAltGenericReceive			MPU_xQueueAltGenericReceive
		#define xQueueGenericReceive			MPU_xQueueGenericReceive
		#define uxQueueMessagesWaiting			MPU_uxQueueMessagesWaiting
		#define vQueueDelete					MPU_vQueueDelete

		#define pvPortMalloc					MPU_pvPortMalloc
		#define vPortFree						MPU_vPortFree
		#define xPortGetFreeHeapSize			MPU_xPortGetFreeHeapSize
		#define vPortInitialiseBlocks			MPU_vPortInitialiseBlocks

		#if configQUEUE_REGISTRY_SIZE > 0
			#define vQueueAddToRegistry				MPU_vQueueAddToRegistry
			#define vQueueUnregisterQueue			MPU_vQueueUnregisterQueue
		#endif

		/* Remove the privileged function macro. */
		#define PRIVILEGED_FUNCTION

	#else /* MPU_WRAPPERS_INCLUDED_FROM_API_FILE */

		/* Ensure API functions go in the privileged execution section. */
		#define PRIVILEGED_FUNCTION __attribute__((section("privileged_functions")))
		#define PRIVILEGED_DATA __attribute__((section("privileged_data")))
        //#define PRIVILEGED_DATA

	#endif /* MPU_WRAPPERS_INCLUDED_FROM_API_FILE */

#else /* portUSING_MPU_WRAPPERS */

	#define PRIVILEGED_FUNCTION
	#define PRIVILEGED_DATA
	#define portUSING_MPU_WRAPPERS 0

#endif /* portUSING_MPU_WRAPPERS */


#endif /* MPU_WRAPPERS_H */

