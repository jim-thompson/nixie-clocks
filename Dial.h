#include "Bounce.h"

/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
 
// Pin 13 has an LED connected on most Arduino boards.
// give it a name:

enum count_state { counting, waiting };
// static count_state state = waiting;

#define DEBUG 1
#if DEBUG
#define DPRINT(m) { Serial.print(m); }
#define DPRINTLN(m) { Serial.println(m); }
#define DUMP() dump()
#else
#define DPRINT(m)
#define DPRINTLN(m)
#define DUMP()
#endif

#if defined(DEBUG)
unsigned long total_t = 0;
unsigned long t_count = 0;
#endif

class RotaryDial
{
  public:
    RotaryDial(int pin, long t)
      : count(0),
        zero_time(0),
        last_t1_time(0),
        state(waiting),
        b(pin, t)
    {
    }
    
    int cycle()
    {
#if defined(DEBUG)
      unsigned long t0, t1;

      t0 = micros();
      int change = b.update();
      int newval = b.read();
      t1 = micros();

      total_t += t1 - t0;
      t_count += 1;
#endif

      switch(state)
      {
        case waiting:
        if (change)
        {
          if (newval == 0)
          {
            DPRINTLN("transition to zero while waiting");
          }
          
          if (newval > 0)
          {
            DPRINTLN("transition to one while waiting");
            last_t1_time = millis();
            state = counting;
            count = 1;
          }
        }
        break;
        
        case counting:
        if (change)
        {
          if (newval == 0)
          {
            DPRINTLN("transition to zero while counting");
            
            long now = millis();
            long pulse_time = now - last_t1_time;
            
            DPRINT("pulse width = ");
            DPRINTLN(pulse_time);

            if (pulse_time < 30)
            {
              DPRINTLN("TOO SHORT! Ignoring...");
              state = waiting;
              DUMP();
            }
          }
          
          if (newval > 0)
          {
            DPRINTLN("transition to one while counting");
    
            long now = millis();
            long cycle_time = now - last_t1_time;
            
            DPRINT("cycle width = ");
            DPRINTLN(cycle_time);
    
            count += 1;
            last_t1_time = millis();
          }
        }
        else
        {
          long now = millis();
          long z_dwell = now - last_t1_time;
          
          if (z_dwell > 125)
          {
            DPRINT("Dwell time = ");
            DPRINTLN(z_dwell);
            
            state = waiting;
            
            DPRINT("new count = ");
            DPRINTLN(count);

            DUMP();
            
            return count;
          }
        }
        break;
        
        default:
        break;
      }      
      
      return 0;
    }

#if defined(DEBUG)
    void dump()
    {
      double t = total_t;
      double c = t_count;

      double avg = t / c;

      Serial.print("Total t: ");
      Serial.print(t);
      Serial.print(", count: ");
      Serial.print(c);
      Serial.print(", Avg: ");
      Serial.print(avg);
      Serial.println("");
    }
#endif
    
  private:
    int count;
    long zero_time;
    long last_t1_time;
    count_state state;
    Bounce b;
};

