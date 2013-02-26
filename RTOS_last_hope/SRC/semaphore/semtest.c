/* Scheduler include files. */
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

/* Demo app include files. */
#include <semtest.h>

/* The value to which the shared variables are counted. */
#define semtstBLOCKING_EXPECTED_VALUE		( ( unsigned long ) 0xfff )
#define semtstNON_BLOCKING_EXPECTED_VALUE	( ( unsigned long ) 0xff  )

#define semtstSTACK_SIZE			configMINIMAL_STACK_SIZE

#define semtstNUM_TASKS				( 4 )

#define semtstDELAY_FACTOR			( ( portTickType ) 10 )

/* The task function as described at the top of the file. */
static portTASK_FUNCTION_PROTO( prvSemaphoreTest, pvParameters );

/* Structure used to pass parameters to each task. */
typedef struct SEMAPHORE_PARAMETERS
{
	xSemaphoreHandle xSemaphore;
	volatile unsigned long *pulSharedVariable;
	portTickType xBlockTime;
} xSemaphoreParameters;

/* Variables used to check that all the tasks are still running without errors. */
static volatile short sCheckVariables[ semtstNUM_TASKS ] = { 0 };
static volatile short sNextCheckVariable = 0;

/*-----------------------------------------------------------*/

void vStartSemaphoreTasks( unsigned portBASE_TYPE uxPriority )
{
xSemaphoreParameters *pxFirstSemaphoreParameters, *pxSecondSemaphoreParameters;
const portTickType xBlockTime = ( portTickType ) 100;

	/* Create the structure used to pass parameters to the first two tasks. */
	pxFirstSemaphoreParameters = ( xSemaphoreParameters * ) pvPortMalloc( sizeof( xSemaphoreParameters ) );

	if( pxFirstSemaphoreParameters != NULL )
	{
		/* Create the semaphore used by the first two tasks. */
		vSemaphoreCreateBinary( pxFirstSemaphoreParameters->xSemaphore );

		if( pxFirstSemaphoreParameters->xSemaphore != NULL )
		{
			/* Create the variable which is to be shared by the first two tasks. */
			pxFirstSemaphoreParameters->pulSharedVariable = ( unsigned long * ) pvPortMalloc( sizeof( unsigned long ) );

			/* Initialise the share variable to the value the tasks expect. */
			*( pxFirstSemaphoreParameters->pulSharedVariable ) = semtstNON_BLOCKING_EXPECTED_VALUE;

			/* The first two tasks do not block on semaphore calls. */
			pxFirstSemaphoreParameters->xBlockTime = ( portTickType ) 0;

			/* Spawn the first two tasks.  As they poll they operate at the idle priority. */
			xTaskCreate( prvSemaphoreTest, ( signed char * ) "PolSEM1", semtstSTACK_SIZE, ( void * ) pxFirstSemaphoreParameters, tskIDLE_PRIORITY, ( xTaskHandle * ) NULL );
			xTaskCreate( prvSemaphoreTest, ( signed char * ) "PolSEM2", semtstSTACK_SIZE, ( void * ) pxFirstSemaphoreParameters, tskIDLE_PRIORITY, ( xTaskHandle * ) NULL );
		}
	}

	/* Do exactly the same to create the second set of tasks, only this time 
	provide a block time for the semaphore calls. */
	pxSecondSemaphoreParameters = ( xSemaphoreParameters * ) pvPortMalloc( sizeof( xSemaphoreParameters ) );
	if( pxSecondSemaphoreParameters != NULL )
	{
		vSemaphoreCreateBinary( pxSecondSemaphoreParameters->xSemaphore );

		if( pxSecondSemaphoreParameters->xSemaphore != NULL )
		{
			pxSecondSemaphoreParameters->pulSharedVariable = ( unsigned long * ) pvPortMalloc( sizeof( unsigned long ) );
			*( pxSecondSemaphoreParameters->pulSharedVariable ) = semtstBLOCKING_EXPECTED_VALUE;
			pxSecondSemaphoreParameters->xBlockTime = xBlockTime / portTICK_RATE_MS;

			xTaskCreate( prvSemaphoreTest, ( signed char * ) "BlkSEM1", semtstSTACK_SIZE, ( void * ) pxSecondSemaphoreParameters, uxPriority, ( xTaskHandle * ) NULL );
			xTaskCreate( prvSemaphoreTest, ( signed char * ) "BlkSEM2", semtstSTACK_SIZE, ( void * ) pxSecondSemaphoreParameters, uxPriority, ( xTaskHandle * ) NULL );
		}
	}

	/* vQueueAddToRegistry() adds the semaphore to the registry, if one is
	in use.  The registry is provided as a means for kernel aware 
	debuggers to locate semaphores and has no purpose if a kernel aware debugger
	is not being used.  The call to vQueueAddToRegistry() will be removed
	by the pre-processor if configQUEUE_REGISTRY_SIZE is not defined or is 
	defined to be less than 1. */
	vQueueAddToRegistry( ( xQueueHandle ) pxFirstSemaphoreParameters->xSemaphore, ( signed char * ) "Counting_Sem_1" );
	vQueueAddToRegistry( ( xQueueHandle ) pxSecondSemaphoreParameters->xSemaphore, ( signed char * ) "Counting_Sem_2" );
}
/*-----------------------------------------------------------*/

static portTASK_FUNCTION( prvSemaphoreTest, pvParameters )
{
xSemaphoreParameters *pxParameters;
volatile unsigned long *pulSharedVariable, ulExpectedValue;
unsigned long ulCounter;
short sError = pdFALSE, sCheckVariableToUse;

	/* See which check variable to use.  sNextCheckVariable is not semaphore 
	protected! */
	portENTER_CRITICAL();
		sCheckVariableToUse = sNextCheckVariable;
		sNextCheckVariable++;
	portEXIT_CRITICAL();

	/* A structure is passed in as the parameter.  This contains the shared 
	variable being guarded. */
	pxParameters = ( xSemaphoreParameters * ) pvParameters;
	pulSharedVariable = pxParameters->pulSharedVariable;

	/* If we are blocking we use a much higher count to ensure loads of context
	switches occur during the count. */
	if( pxParameters->xBlockTime > ( portTickType ) 0 )
	{
		ulExpectedValue = semtstBLOCKING_EXPECTED_VALUE;
	}
	else
	{
		ulExpectedValue = semtstNON_BLOCKING_EXPECTED_VALUE;
	}

	for( ;; )
	{
		/* Try to obtain the semaphore. */
		if( xSemaphoreTake( pxParameters->xSemaphore, pxParameters->xBlockTime ) == pdPASS )
		{
			/* We have the semaphore and so expect any other tasks using the
			shared variable to have left it in the state we expect to find
			it. */
			if( *pulSharedVariable != ulExpectedValue )
			{
				sError = pdTRUE;
			}
			
			/* Clear the variable, then count it back up to the expected value
			before releasing the semaphore.  Would expect a context switch or
			two during this time. */
			for( ulCounter = ( unsigned long ) 0; ulCounter <= ulExpectedValue; ulCounter++ )
			{
				*pulSharedVariable = ulCounter;
				if( *pulSharedVariable != ulCounter )
				{
					sError = pdTRUE;
				}
			}

			/* Release the semaphore, and if no errors have occurred increment the check
			variable. */
			if(	xSemaphoreGive( pxParameters->xSemaphore ) == pdFALSE )
			{
				sError = pdTRUE;
			}

			if( sError == pdFALSE )
			{
				if( sCheckVariableToUse < semtstNUM_TASKS )
				{
					( sCheckVariables[ sCheckVariableToUse ] )++;
				}
			}

			/* If we have a block time then we are running at a priority higher
			than the idle priority.  This task takes a long time to complete
			a cycle	(deliberately so to test the guarding) so will be starving
			out lower priority tasks.  Block for some time to allow give lower
			priority tasks some processor time. */
			vTaskDelay( pxParameters->xBlockTime * semtstDELAY_FACTOR );
		}
		else
		{
			if( pxParameters->xBlockTime == ( portTickType ) 0 )
			{
				/* We have not got the semaphore yet, so no point using the
				processor.  We are not blocking when attempting to obtain the
				semaphore. */
				taskYIELD();
			}
		}
	}
}
/*-----------------------------------------------------------*/

/* This is called to check that all the created tasks are still running. */
portBASE_TYPE xAreSemaphoreTasksStillRunning( void )
{
static short sLastCheckVariables[ semtstNUM_TASKS ] = { 0 };
portBASE_TYPE xTask, xReturn = pdTRUE;

	for( xTask = 0; xTask < semtstNUM_TASKS; xTask++ )
	{
		if( sLastCheckVariables[ xTask ] == sCheckVariables[ xTask ] )
		{
			xReturn = pdFALSE;
		}

		sLastCheckVariables[ xTask ] = sCheckVariables[ xTask ];
	}

	return xReturn;
}


