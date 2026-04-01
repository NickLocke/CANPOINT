# Point Control Logic (CANPOINT)
A Raspberry Pi Pico based module specifically intended to handle the operation of points and the corresponding control panel on Roxeter.

## Configuration
This will need to be set up in the configuration data in the code:
- Layout point number – integer 
- Internal point number – integer 

## Consumed Events
- Roxeter panel point switch normal – on / off
- Roxeter panel point switch reverse – on / off
- Route requiring points normal called (on) / cleared (off)
- Route requiring points reverse called (on) / cleared (off)
- Points detected normal – on / off
- Points detected reverse – on / off
- Track over points occupied (on) / clear (off) – Do we need multiples?

## Produced Events
- Point motor movement request reverse (on) / normal (off)
- Points normal indication – on / off
- Points reverse indication – on / off
- Points locked indication – on / off
- Points out-of-correspondence indication – on (flashing) / off
- Points set and locked normal – on 
- Points set and locked reverse - on

## Internal State
### Point state:
- Unknown
- Normal
- Locked normal
- Reverse
- Locked reverse

### Availability:
- Free to be locked normal
- Free to be locked reverse


