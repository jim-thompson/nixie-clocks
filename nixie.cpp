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

// This program is essentially Nixie_02 redesigned to use system timers
// instead of interrupts.

#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>

/* global variables */
unsigned char INDEX = 0;   /* 1 to 6 */
unsigned char HOUR = 0;   /* 1 to 12 */
unsigned char MINUTE = 00;  /* 0 to 59 */
unsigned char SECOND = 00;  /* 0 to 59 */

void nixie_bump_time()
{
  /* increment and check seconds */
  if (++SECOND >= 60)
  {
    SECOND = 0;

    /* increment and check minutes */
    if (++MINUTE >= 60)
    {
      MINUTE = 0;

      /* increment and check hours */
      if (++HOUR >= 24)
      {
        HOUR = 0;
      }
    }
  }
  
  Serial.print(HOUR, DEC);
  Serial.print(":");
  Serial.print(MINUTE, DEC);
  Serial.print(":");
  Serial.print(SECOND, DEC);
  Serial.println();
}

#define UNO 0
#if defined(UNO) && UNO

// Pin-to-port mappings for Uno pins 0 - 13

uint8_t *ports[] = { 
  &PORTD, &PORTD, &PORTD, &PORTD, &PORTD, &PORTD, &PORTD, &PORTD,
  &PORTB, &PORTB, &PORTB, &PORTB, &PORTB, &PORTB
};

// Pin-to-mask mappings for Uno pins 0 - 13

uint8_t masks[] = {
  0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80,
  0x01, 0x02, 0x04, 0x08, 0x10, 0x20,
};

#else
// Mega2560

volatile uint8_t *ports[] = {
  &PORTE, &PORTE, &PORTE, &PORTE, &PORTG, &PORTE, &PORTH, &PORTH,
  &PORTH, &PORTH, &PORTB, &PORTB, &PORTB, &PORTB,
};

uint8_t masks[] = {
  0x01, 0x02, 0x10, 0x20, 0x20, 0x08, 0x08, 0x10,
  0x20, 0x40, 0x10, 0x20, 0x40, 0x80,
};

#endif

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

  switch(INDEX++)
  {
    /* HOUR tens place */
    case 1:
      switchDOff();
      setBLowNibble(HOUR / 10);
      switchPinOn(2);
      break;

    /* HOUR ones place */
    case 2:
      switchDOff();
      setBLowNibble(HOUR % 10);
      switchPinOn(3);
      break;

    /* MINUTE tens place */
    case 3:
      switchDOff();
      setBLowNibble(MINUTE / 10);
      switchPinOn(4);
      break;

    /* MINUTE ones place */
    case 4:
      switchDOff();
      setBLowNibble(MINUTE % 10);
      switchPinOn(5);
      break;

    /* SECOND tens place */
    case 5:
      switchDOff();
      setBLowNibble(SECOND / 10);
      switchPinOn(6);
      break;

    /* SECOND ones place */
    case 6:
      switchDOff();
      setBLowNibble(SECOND % 10);
      switchPinOn(7);

      /* reset index */
      INDEX = 1;
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

void nixie_loop()
{
#define STIME millis()
#define TIME micros()
  unsigned long t = TIME;
  unsigned long st = 0;
  const unsigned long period = 1000;

  while (true)
  {
    if (((TIME) - t) > period)
    {
      nixie_multiplex();
      t = (TIME);
    }

    if (((STIME) - st) > 1000)
    {
      nixie_bump_time();
      st = (STIME);
    }
  }
}
