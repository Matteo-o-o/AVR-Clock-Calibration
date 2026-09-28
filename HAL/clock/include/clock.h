#ifndef CLOCK_H
#define CLOCK_H

#include <stdbool.h>
#include <stdint.h>

// Default reference frequency definition if not provided by Makefile
#ifndef F_CLOCK_REF
#define F_CLOCK_REF 1000UL
#endif

// Initializes clocks: runs dichotomy calibration once using F_CLOCK_REF, sets OSCCAL, and configures Timer 0 for system millis.
void clock_init(void);

// Returns the elapsed time since startup in milliseconds.
uint32_t clock_millis(void);


// Checks if a given time interval has elapsed.
bool clock_has_elapsed(uint32_t start_time, uint32_t interval_ms);

// Returns the last measured ticks recorded during the startup calibration pass.
uint16_t clock_get_last_ticks(void);

#endif // CLOCK_H