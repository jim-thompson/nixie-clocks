//-----------------------------------------------------------------------
// Dial.h - a simple class to count pulses from a rotary telephone.
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

//-----------------------------------------------------------------------
// To do debouncing of the dial contacts we require Thomas O
// Fredericks's excellent "Bounce" library.
//
// https://github.com/thomasfredericks/Bounce2

#include "Bounce.h"

//-----------------------------------------------------------------------
// Definitions:
//
// Switch - The mechanism inside a rotary dial (from an old telephone)
// that opens and closes as a number is dialed.
//
// Switch states - The two states the dial's internal switch can be in.
//
// 0 - The switch contacts are usually closed, and we refer to this as
// state 0.
//
// 1 - When the contacts open, during dialing, we refer to this as
// state 1.
//
// Pulse - A transition from state 0 to state 1 and back to state 0
// (after debouncing). Typically a pulse is about 100 ms. We do not
// place strict requirements on a pulse except that we reject pulses
// less than 30 ms.
//
// Debounce - When a switch is closed or opened, it does not go
// cleanly from one state to the other; it bounces between states for
// brief instance, before settling into its new state (this is true of
// any switch, not just those in rotary dials). Turning this noisy
// signal into a clean signal is called de-bouncing.
//
// Dial states - The two states a Dial object can be in: Waiting or
// Counting.
//
// In dialing a digit, someone turns the dial and releases it. The
// dial's internal spring returns the dial back to its rest position.
// As the dail turns back to its rest position, it closes the
// contacts, generating a number of pulses. We count these pulse to
// determine the digit that has been dialed.
//
// We are always in one of the two states Counting or Waiting. In the
// Waiting state, we are essentially idle; this corresponds to the
// rotary dial at its rest position, not rotating, or being turned
// forward. We enter the Counting state when someone has released the
// dial, so as to dial a single digit, at the moment we see the first
// closing of the contacts.  We stay in this state, incrementing the
// dial count each time we see a pulse, until a timeout occurs.

enum count_state { counting, waiting };

// Define some macros to make turning debug on and off easier.
// For internal use only.

#define DEBUG 0

#if DEBUG
#define DPRINT(m) { Serial.print(m); }
#define DPRINTLN(m) { Serial.println(m); }
#else
#define DPRINT(m)
#define DPRINTLN(m)
#endif

class RotaryDial
{
  public:
    // Public constructor
    RotaryDial(int pin,		// Specifies the input pin to attach to
	       long t)		// Specifies the length, in
				// milliseconds, of the debounce
				// timeout period
      : count(0),
        last_t1_time(0),
        state(waiting),
        b(pin, t)
    {
    }

    // This function should be called once each time through your main
    // event loop, a minimum of 100 times per secord or more. (Call it
    // at a lower rate, and you risk missing pulses).
    //
    // Returns the NUMBER OF PULSES seen at time-out. So if you dial
    // "1", it returns 1, if you dial "2", it returns 2, and so on,
    // except that if you dial "0", it returns 10 (because dialing "0"
    // generates ten pulses.) If no digit has been dialed, or the Dial
    // is in the Dialing state, it will return 0.

    int cycle()
    {
      int change = b.update();
      int newval = b.read();

      switch(state)
      {
	// We're waiting for something to happen.
        case waiting:

        // Has something happened?
        if (change)
        {
	  // Yes! However, a new value of 0 while waiting is
	  // nonsensical. We don't consider a state transition to be
	  // happening until we see a value of 1 while waiting. Just
	  // ignore it.
          if (newval == 0)
          {
            DPRINTLN("transition to zero while waiting");
          }
          
	  // Yes! If the new value is 1, then we're seeing the
	  // high-to-low transition at the beginning of the FIRST
	  // pulse.
          if (newval > 0)
          {
            DPRINTLN("transition to one while waiting");

	    // Record the time of this 0-to-1 transition. We will
	    // compare to this value later in determining pulse
	    // width.
            last_t1_time = millis();

	    // Transition to the Counting state, and set the initial
	    // count to 1.
            state = counting;
            count = 1;
          }
        }
        break;
        
        case counting:

        // Has anything changed?
        if (change)
        {
	  // Yes! If the new value is 0, then we're seeing the
	  // high-to-low transition at the trailing edge of a pulse.

          if (newval == 0)
          {
            DPRINTLN("transition to zero while counting");

	    // We need to check the pulse width - how long it has been
	    // since the leading edge of this pulse.
	    
            long now = millis();
            long pulse_time = now - last_t1_time;
            
            DPRINT("pulse width = ");
            DPRINTLN(pulse_time);

	    // If the pulse width is less than 30 milliseconds, then
	    // disregard the pulse; this can sometimes happen if one
	    // forces the dial to turn faster than it ordinarily
	    // would. It's reasonable in that circumstance to ignore
	    // the pulse.

            if (pulse_time < 30)
            {
              DPRINTLN("TOO SHORT! Ignoring...");

	      // Transition back to Waiting state without doing
	      // anything (in particular without returning a pulse
	      // count).
              state = waiting;
            }
          }

	  // If the new value is 1, then we're seeing the low-to-high
	  // transition at the leading edge of a pulse.

          if (newval > 0)
          {
            DPRINTLN("transition to one while counting");
    
            // long now = millis();
	    //            long cycle_time = now - last_t1_time;
            
            DPRINT("cycle width = ");
            DPRINTLN(cycle_time);
    
            count += 1;
            last_t1_time = millis();
          }
        }
        else
        {
	  // We're in the Dialing state, but nothing has
	  // changed. Check to see whether we have timed out.

          long now = millis();
          long dwell_time = now - last_t1_time;

	  // In the Dialing state, if nothing has changed in 125 ms,
	  // then we have timed out. Declare dialing done!

          if (dwell_time > 125)
          {
            DPRINT("Dwell time = ");
            DPRINTLN(dwell_time);
            
            DPRINT("new count = ");
            DPRINTLN(count);

	    // We now have the complete dialing of a digit. Transition
	    // to Waiting state, and return the number of pulses seen.

            state = waiting;
            return count;
          }
        }
        break;

	// Always have a default, even if all you do is to break.
        default:
        break;
      }      

      // We have not seen the complete dialing of a digit.
      return 0;
    }

  private:
    int count;			// The number of pulses we have seen
    long last_t1_time;		// The last time we saw the contacts
				// closed.
    count_state state;		// Current state - are we waiting or counting?

    Bounce b;			// The debouncer
};
