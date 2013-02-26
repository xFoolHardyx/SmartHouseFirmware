#ifndef POLLED_Q_H
#define POLLED_Q_H

void vStartPolledQueueTasks( unsigned portBASE_TYPE uxPriority );
portBASE_TYPE xArePollingQueuesStillRunning( void );

#endif


