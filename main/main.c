#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>

#include "clock.h"
#include "uart.h"

int main(void) {
    clock_init();

    uart_init(9600);

    uint32_t ticks = clock_get_last_ticks();
    
    uint32_t real_f_cpu = ticks * F_CLOCK_REF;
    int32_t  freq_error  = (int32_t)real_f_cpu - (int32_t)F_CPU;
    int32_t  error_permil = (freq_error * 1000L) / (int32_t)F_CPU; // permil (0.1 %)

    printf("\r\n========================================================\r\n");
    printf("        ATMEGA328P CLOCK CALIBRATION DIAGNOSTIC         \r\n");
    printf("========================================================\r\n");
    printf(" CONFIG Target F_CPU          : %lu Hz (%lu MHz)\r\n", (unsigned long)F_CPU, (unsigned long)(F_CPU / 1000000UL));
    printf(" CONFIG Reference Frequency   : %lu Hz\r\n", (unsigned long)F_CLOCK_REF);
    printf("--------------------------------------------------------\r\n");
    printf(" RESULT Measured Ticks/Period : %lu ticks\r\n", ticks);
    printf(" RESULT Real Calibrated F_CPU : %lu Hz\r\n", real_f_cpu);
    printf(" RESULT OSCCAL Register Value : 0x%02X (%u decimal)\r\n", OSCCAL, OSCCAL);
    printf("--------------------------------------------------------\r\n");
    printf(" METRICS Absolute Drift       : %+ld Hz\r\n", (long)freq_error);
    printf(" METRICS Relative Error       : %+ld.%-ld %%\r\n", 
           (long)(error_permil / 10), (long)(error_permil < 0 ? -error_permil % 10 : error_permil % 10));
    printf("========================================================\r\n\n");

    while (1) {
    }
}