//-----------------------------------------------------------------------
// nixie-parallel.cpp - Write time values out for the vintage clock.
// with a set of four output bits/pins.

// Copyright (c) 2017 Jim Thompson.

//-----------------------------------------------------------------------
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <Arduino.h>
#include "FourBitDigit.h"

extern unsigned int second;
extern unsigned int minute;
extern unsigned int hour;

// Declare the objects representing each of the six digits and their
// corresponding I/O pins.
FourBitDigit second_ones_digit(46, 50, 52, 48);
FourBitDigit second_tens_digit(47, 49, 51, 53);
FourBitDigit minute_ones_digit(38, 42, 44, 40);
FourBitDigit minute_tens_digit(39, 41, 43, 45);
FourBitDigit hour_ones_digit(30, 34, 36, 32);
FourBitDigit hour_tens_digit(31, 33, 35, 37);

// Write all the values in parallel
extern void nixie_writeall()
{
  // Get the values for the six digits from the time values for hours,
  // minutes, and seconds.
  unsigned int second_ones = second % 10;
  unsigned int second_tens = ((second / 10) % 10);
  unsigned int minute_ones = minute % 10;
  unsigned int minute_tens = ((minute / 10) % 10);
  unsigned int hour_ones = hour % 10;
  unsigned int hour_tens = ((hour / 10) % 10);

  // Set the values in the digit objects.
  second_ones_digit.setValue(second_ones);
  second_tens_digit.setValue(second_tens);
  minute_ones_digit.setValue(minute_ones);
  minute_tens_digit.setValue(minute_tens);
  hour_ones_digit.setValue(hour_ones);
  hour_tens_digit.setValue(hour_tens);

  // Write them all out to their various I/O pins.
  second_ones_digit.writePins();
  second_tens_digit.writePins();
  minute_ones_digit.writePins();
  minute_tens_digit.writePins();
  hour_ones_digit.writePins();
  hour_tens_digit.writePins();
}
