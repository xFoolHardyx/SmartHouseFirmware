/* Scheduler include files. */
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

/* Demo program include files. */
#include <PollQ.h>

#define pollqSTACK_SIZE			configMINIMAL_STACK_SIZE
#define pollqQUEUE_SIZE			( 10 )
#define pollqPRODUCER_DELAY		( ( portTickType ) 200 / portTICK_RATE_MS )
#define pollqCONSUMER_DELAY		( pollqPRODUCER_DELAY - ( portTickType ) ( 20 / portTICK_RATE_MS ) )
#define pollqNO_DELAY			( ( portTickType ) 0 )
#define pollqVALUES_TO_PRODUCE	( ( signed portBASE_TYPE ) 3 )
#define pollqINITIAL_VALUE		( ( signed portBASE_TYPE ) 0 )

/* The task that posts the incrementing number onto the queue. */
static portTASK_FUNCTION_PROTO( vPolledQueueProducer, pvParameters );

/* The task that empties the queue. */
static portTASK_FUNCTION_PROTO( vPolledQueueConsumer, pvParameters );

/* Variables that are used to check that the tasks are still running with no
errors. */
static volatile signed portBASE_TYPE xPollingConsumerCount = pollqINITIAL_VALUE, xPollingProducerCount = pollqINITIAL_VALUE;

/*-----------------------------------------------------------*/

void vStartPolledQueueTasks( unsigned portBASE_TYPE uxPriority )
{
static xQueueHandle xPolledQueue;

	/* Create the queue used by the producer and consumer. */
	xPolledQueue = xQueueCreate( pollqQUEUE_SIZE, ( unsigned portBASE_TYPE ) sizeof( unsigned short ) );

	/* vQueueAddToRegistry() adds the queue to the queue registry, if one is
	in use.  The queue registry is provided as a means for kernel aware 
	debuggers to locate queues and has no purpose if a kernel aware debugger
	is not being used.  The call to vQueueAddToRegistry() will be removed
	by the pre-processor if configQUEUE_REGISTRY_SIZE is not defined or is 
	defined to be less than 1. */
	vQueueAddToRegistry( xPolledQueue, ( signed char * ) "Poll_Test_Queue" );

	/* Spawn the producer and consumer. */
	xTaskCreate( vPolledQueueConsumer, ( signed char * ) "QConsNB", pollqSTACK_SIZE, ( void * ) &xPolledQueue, uxPriority, ( xTaskHandle * ) NULL );
	xTaskCreate( vPolledQueueProducer, ( signed char * ) "QProdNB", pollqSTACK_SIZE, ( void * ) &xPolledQueue, uxPriority, ( xTaskHandle * ) NULL );
}
/*-----------------------------------------------------------*/

static portTASK_FUNCTION( vPolledQueueProducer, pvParameters )
{
unsigned short usValue = ( unsigned short ) 0;
signed portBASE_TYPE xError = pdFALSE, xLoop;

	for( ;; )
	{		
		for( xLoop = 0; xLoop < pollqVALUES_TO_PRODUCE; xLoop++ )
		{
			/* Send an incrementing number on the queue without blocking. */
			if( xQueueSend( *( ( xQueueHandle * ) pvParameters ), ( void * ) &usValue, pollqNO_DELAY ) != pdPASS )
			{
				/* We should never find the queue full so if we get here there
				has been an error. */
				xError = pdTRUE;
			}
			else
			{
				if( xError == pdFALSE )
				{
					/* If an error has ever been recorded we stop incrementing the
					check variable. */
					portENTER_CRITICAL();
						xPollingProducerCount++;
					portEXIT_CRITICAL();
				}

				/* Update the value we are going to post next time around. */
				usValue++;
			}
		}

		/* Wait before we start posting again to ensure the consumer runs and
		empties the queue. */
		vTaskDelay( pollqPRODUCER_DELAY );
	}
}  /*lint !e818 Function prototype must conform to API. */
/*-----------------------------------------------------------*/

static portTASK_FUNCTION( vPolledQueueConsumer, pvParameters )
{
unsigned short usData, usExpectedValue = ( unsigned short ) 0;
signed portBASE_TYPE xError = pdFALSE;

	for( ;; )
	{		
		/* Loop until the queue is empty. */
		while( uxQueueMessagesWaiting( *( ( xQueueHandle * ) pvParameters ) ) )
		{
			if( xQueueReceive( *( ( xQueueHandle * ) pvParameters ), &usData, pollqNO_DELAY ) == pdPASS )
			{
				if( usData != usExpectedValue )
				{
					/* This is not what we expected to receive so an error has
					occurred. */
					xError = pdTRUE;

					/* Catch-up to the value we received so our next expected
					value should again be correct. */
					usExpectedValue = usData;
				}
				else
				{
					if( xError == pdFALSE )
					{
						/* Only increment the check variable if no errors have
						occurred. */
						portENTER_CRITICAL();
							xPollingConsumerCount++;
						portEXIT_CRITICAL();
					}
				}

				/* Next time round we would expect the number to be one higher. */
				usExpectedValue++;
			}
		}

		/* Now the queue is empty we block, allowing the producer to place more
		items in the queue. */
		vTaskDelay( pollqCONSUMER_DELAY );
	}
} /*lint !e818 Function prototype must conform to API. */
/*-----------------------------------------------------------*/

/* This is called to check that all the created tasks are still running with no errors. */
portBASE_TYPE xArePollingQueuesStillRunning( void )
{
portBASE_TYPE xReturn;

	/* Check both the consumer and producer poll count to check they have both
	been changed since out last trip round.  We do not need a critical section
	around the check variables as this is called from a higher priority than
	the other tasks that access the same variables. */
	if( ( xPollingConsumerCount == pollqINITIAL_VALUE ) ||
		( xPollingProducerCount == pollqINITIAL_VALUE )
	  )
	{
		xReturn = pdFALSE;
	}
	else
	{
		xReturn = pdTRUE;
	}

	/* Set the check variables back down so we know if they have been
	incremented the next time around. */
	xPollingConsumerCount = pollqINITIAL_VALUE;
	xPollingProducerCount = pollqINITIAL_VALUE;

	return xReturn;
}
