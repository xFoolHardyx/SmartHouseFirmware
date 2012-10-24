#include <stdlib.h>
#include <stdio.h>
#include "FreeRTOS.h"

void vGreenBlinkTask( void *pvParametrs ) {
    for( ;; ) {
       // P8OUT ^= BIT7;

        // Выполнить задержку в 700 FreeRTOS тиков. Величина одного тика задана в FreeRTOSConfig.h и как правило составляет 1мс.
        vTaskDelay( 700 );
    }
}

void vRedBlinkTask( void *pvParametrs ) {
    for( ;; ) {
      //  P8OUT ^= BIT6;

        // Выполнить задержку в 1000 FreeRTOS тиков. Величина одного тика задана в FreeRTOSConfig.h и как правило составляет 1мс.
        vTaskDelay( 1000 );
    }
}

void main(void) {
    // Инициализация микроконтроллера. Данный код будет у каждого свой.
    vInitSystem();

    // Создание тасков. Я не включил код проверки ошибок, но не стоит забывать об этом!
    xTaskCreate( &vGreenBlinkTask, (signed char *)"GreenBlink", configMINIMAL_STACK_SIZE, NULL, 1, NULL );
    xTaskCreate( &vRedBlinkTask, (signed char *)"RedBlink", configMINIMAL_STACK_SIZE, NULL, 1, NULL );

    // Запуск планировщика т.е начало работы тасков.
    vTaskStartScheduler();

    // Сюда стоит поместить код обработки ошибок, в случае если планировщик не заработал.
    // Для примера я использую просто бесконечный цикл.
    for( ;; ) { }
}
