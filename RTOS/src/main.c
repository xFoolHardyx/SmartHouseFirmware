#include <stdlib.h>
#include <stdio.h>
#include "FreeRTOS.h"

void vGreenBlinkTask( void *pvParametrs ) {
    for( ;; ) {
       // P8OUT ^= BIT7;

        // ��������� �������� � 700 FreeRTOS �����. �������� ������ ���� ������ � FreeRTOSConfig.h � ��� ������� ���������� 1��.
        vTaskDelay( 700 );
    }
}

void vRedBlinkTask( void *pvParametrs ) {
    for( ;; ) {
      //  P8OUT ^= BIT6;

        // ��������� �������� � 1000 FreeRTOS �����. �������� ������ ���� ������ � FreeRTOSConfig.h � ��� ������� ���������� 1��.
        vTaskDelay( 1000 );
    }
}

void main(void) {
    // ������������� ����������������. ������ ��� ����� � ������� ����.
    vInitSystem();

    // �������� ������. � �� ������� ��� �������� ������, �� �� ����� �������� �� ����!
    xTaskCreate( &vGreenBlinkTask, (signed char *)"GreenBlink", configMINIMAL_STACK_SIZE, NULL, 1, NULL );
    xTaskCreate( &vRedBlinkTask, (signed char *)"RedBlink", configMINIMAL_STACK_SIZE, NULL, 1, NULL );

    // ������ ������������ �.� ������ ������ ������.
    vTaskStartScheduler();

    // ���� ����� ��������� ��� ��������� ������, � ������ ���� ����������� �� ���������.
    // ��� ������� � ��������� ������ ����������� ����.
    for( ;; ) { }
}
