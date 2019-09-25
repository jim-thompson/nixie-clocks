/*********************************************************************
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*********************************************************************/

#include <Arduino.h>

// This variable tracks which digit we're currently writing out to the
// display. We cycle through the six digits in order.
unsigned int index = 0;

// Variables to track the current time.
unsigned int hour = 0;
unsigned int minute = 0;
unsigned int second = 0;

#define UNO 0
#if defined(UNO) && UNO

// Port and pin mappings for the Arduino Uno. We use these so that we
// can write values directly to the pins without having to use
// digitalWrite, which is many times slower.

uint8_t *ports[] = { 
    &PORTD, &PORTD, &PORTD, &PORTD, &PORTD, &PORTD, &PORTD, &PORTD,
    &PORTB, &PORTB, &PORTB, &PORTB, &PORTB, &PORTB
};

uint8_t masks[] = {
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80,
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20,
};

#else

// Port and pin mappings for the Arduino Mega 2560.

volatile uint8_t *ports[] = {
    &PORTE, &PORTE, &PORTE, &PORTE, &PORTG, &PORTE, &PORTH, &PORTH,
    &PORTH, &PORTH, &PORTB, &PORTB, &PORTB, &PORTB,
};

uint8_t masks[] = {
    0x01, 0x02, 0x10, 0x20, 0x20, 0x08, 0x08, 0x10,
    0x20, 0x40, 0x10, 0x20, 0x40, 0x80,
};

#endif

// Inline functions that use the above port and pin declarations to
// turn individual pins on or off.

__attribute__((always_inline)) inline void switchPinOn(uint8_t pin) { *(ports[pin]) |= masks[pin]; }
__attribute__((always_inline)) inline void switchPinOff(uint8_t pin) { *(ports[pin]) &= ~masks[pin]; }

void switchDOff()
{
    switchPinOff(2);
    switchPinOff(3);
    switchPinOff(4);
    switchPinOff(5);
    switchPinOff(6);
    switchPinOff(7);
}

void setBLowNibble(int value)
{
    if (value & 0x01) switchPinOn(8); else switchPinOff(8);
    if (value & 0x02) switchPinOn(9); else switchPinOff(9);
    if (value & 0x04) switchPinOn(10); else switchPinOff(10);
    if (value & 0x08) switchPinOn(11); else switchPinOff(11);
}

void nixie_multiplex()
{
    // Turn on the hour tens LED
    switchPinOn(12);
  
    switch(index++)
    {
    case 1:
	// Tens of hours
	switchDOff();
	setBLowNibble(hour / 10);
	switchPinOn(2);
	break;
      
    case 2:
	// Ones of hours
	switchDOff();
	setBLowNibble(hour % 10);
	switchPinOn(3);
	break;

    case 3:
	// Tens of minutes
	switchDOff();
	setBLowNibble(minute / 10);
	switchPinOn(4);
	break;

    case 4:
	// Ones of minutes
	switchDOff();
	setBLowNibble(minute % 10);
	switchPinOn(5);
	break;

    case 5:
	// Tens of seconds
	switchDOff();
	setBLowNibble(second / 10);
	switchPinOn(6);
	break;

    case 6:
	// Ones of seconds
	switchDOff();
	setBLowNibble(second % 10);
	switchPinOn(7);

	/* reset index */
	index = 1;
	break;
    }
}

void nixie_setup()
{
    /* configure pins */
    // Digit position anodes, 1 per digit
    // Port D 0x04 - 0x80
    pinMode(2, OUTPUT);
    pinMode(3, OUTPUT);
    pinMode(4, OUTPUT);
    pinMode(5, OUTPUT);
    pinMode(6, OUTPUT);
    pinMode(7, OUTPUT);

    // Digit value cathodes, 4 -> 10
    // Port B 0x0f
    pinMode(8, OUTPUT);
    pinMode(9, OUTPUT);
    pinMode(10, OUTPUT);
    pinMode(11, OUTPUT);

    // Hour tens LED
    // Port B 0x10
    pinMode(12, OUTPUT);

    for (int i = 2; i <= 12; i++)
    {
	digitalWrite(i, LOW);
    }
}
