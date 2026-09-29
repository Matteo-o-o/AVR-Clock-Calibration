#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdint.h>
#include "clock.h"

#define TARGET_TICKS      (F_CPU / F_CLOCK_REF)
#define TOLERANCE         2U
#define CAPTURE_TIMEOUT   50000UL

static volatile uint32_t g_system_millis = 0;
static uint16_t g_last_measured_ticks = 0; // Stocke la dernière mesure retenue

// Single-point measurement function exclusively for calibration
static uint16_t measure_calibration_ticks(void) {
    uint32_t timeout = 0;

    TCCR1A = 0;
    TCCR1B = (1 << CS10) | (1 << ICES1); // Rising edge, Prescaler 1
    TIFR1  = (1 << ICF1);

    while (!(TIFR1 & (1 << ICF1))) {
        if (++timeout > CAPTURE_TIMEOUT) return 0;
    }
    uint16_t t1 = ICR1;
    TIFR1 = (1 << ICF1);

    timeout = 0;
    while (!(TIFR1 & (1 << ICF1))) {
        if (++timeout > CAPTURE_TIMEOUT) return 0;
    }
    uint16_t t2 = ICR1;

    return (t2 - t1);
}

// Calibration algorithm via dichotomy
void clock_calibrate_osccal(void) {
    int16_t min = 0;
    int16_t max = 255;
    uint8_t best_osccal = OSCCAL;
    uint16_t min_error = 0xFFFF;

    while (min <= max) {
        int16_t mid = min + ((max - min) / 2);
        OSCCAL = (uint8_t)mid;
        _delay_ms(3);

        uint16_t measured_ticks = measure_calibration_ticks();
        if (measured_ticks == 0) {
            break; 
        }

        uint16_t error = (measured_ticks > TARGET_TICKS) ? 
                         (measured_ticks - TARGET_TICKS) : 
                         (TARGET_TICKS - measured_ticks);

        if (error < min_error) {
            min_error = error;
            best_osccal = (uint8_t)mid;
            g_last_measured_ticks = measured_ticks; // Sauvegarde la mesure associée au meilleur OSCCAL
        }

        if (error <= TOLERANCE) {
            break;
        }

        if (measured_ticks > TARGET_TICKS) {
            max = mid - 1;
        } else {
            min = mid + 1;
        }
    }

    OSCCAL = best_osccal;
}

void clock_init(void) {
    cli();
    DDRB &= ~(1 << DDB0);
    PORTB &= ~(1 << PORTB0);
    
    // Run calibration once
    clock_calibrate_osccal();

    // Configure Timer 0 for system millis (optional, but useful for delays)
    TCNT0 = 0;
    OCR0A = (uint8_t)(((F_CPU / 64UL) / 1000UL) - 1UL);
    TCCR0A = (1 << WGM01);
    TCCR0B = (1 << CS01) | (1 << CS00);
    TIMSK0 |= (1 << OCIE0A);

    sei();
}

uint32_t clock_millis(void) {
    uint32_t millis_copy;
    uint8_t sreg = SREG;
    cli();
    millis_copy = g_system_millis;
    SREG = sreg;
    return millis_copy;
}

// Getter function required by main.c
uint16_t clock_get_last_ticks(void) {
    return g_last_measured_ticks;
}

ISR(TIMER0_COMPA_vect) {
    g_system_millis++;
}