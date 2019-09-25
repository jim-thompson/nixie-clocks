#include <Arduino.h>
#include "nixie-parallel.h"

extern unsigned char SECOND;
extern unsigned char MINUTE;
extern unsigned char HOUR;

// FourBitDigit hour_ones_digit(34, 36, 38, 40);
// FourBitDigit hour_tens_digit(35, 37, 39, 41);

// FourBitDigit minute_ones_digit(42, 44, 46, 48);
// FourBitDigit minute_tens_digit(43, 45, 47, 49);

// FourBitDigit second_ones_digit(46, 48, 50, 52);
// FourBitDigit second_tens_digit(47, 49, 51, 53);


FourBitDigit second_ones_digit(46, 50, 52, 48);
FourBitDigit second_tens_digit(47, 49, 51, 53);
FourBitDigit minute_ones_digit(38, 42, 44, 40);
FourBitDigit minute_tens_digit(39, 41, 43, 45);
FourBitDigit hour_ones_digit(30, 34, 36, 32);
FourBitDigit hour_tens_digit(31, 33, 35, 37);


void setDigit(FourBitDigit &digit, unsigned int value)
{
  digit.setValue(value);
  digit.writePins();
}

void setAllDigits(int value)
{
  setDigit(hour_tens_digit, value);
  setDigit(hour_ones_digit, value);
  setDigit(minute_tens_digit, value);
  setDigit(minute_ones_digit, value);
  setDigit(second_tens_digit, value);
  setDigit(second_ones_digit, value);
}

void setTestPattern1()
{
  setDigit(hour_tens_digit, 1);
  setDigit(hour_ones_digit, 2);
  setDigit(minute_tens_digit, 3);
  setDigit(minute_ones_digit, 4);
  setDigit(second_tens_digit, 5);
  setDigit(second_ones_digit, 6);
}

void setTestPattern2()
{
  setDigit(hour_tens_digit, 6);
  setDigit(hour_ones_digit, 5);
  setDigit(minute_tens_digit, 4);
  setDigit(minute_ones_digit, 3);
  setDigit(second_tens_digit, 2);
  setDigit(second_ones_digit, 1);
}

extern void test()
{
  Serial.println("In test routine");
  while (true)
    {
      for (int i = 0; i < 10; i++)
	{
	  setAllDigits(i);
	  delay(1000);
	}

      setTestPattern1();
      delay(1000);

      setTestPattern2();
      delay(1000);
    }
}

extern void nixie_writeall()
{
  unsigned int second_ones = SECOND % 10;
  unsigned int second_tens = ((SECOND / 10) % 10);
  unsigned int minute_ones = MINUTE % 10;
  unsigned int minute_tens = ((MINUTE / 10) % 10);
  unsigned int hour_ones = HOUR % 10;
  unsigned int hour_tens = ((HOUR / 10) % 10);

  second_ones_digit.setValue(second_ones);
  second_tens_digit.setValue(second_tens);
  minute_ones_digit.setValue(minute_ones);
  minute_tens_digit.setValue(minute_tens);
  hour_ones_digit.setValue(hour_ones);
  hour_tens_digit.setValue(hour_tens);

  second_ones_digit.writePins();
  second_tens_digit.writePins();
  minute_ones_digit.writePins();
  minute_tens_digit.writePins();
  hour_ones_digit.writePins();
  hour_tens_digit.writePins();
}
