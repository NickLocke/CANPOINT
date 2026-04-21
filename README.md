# Point Control Logic (CANPOINT)
A Raspberry Pi Pico based module specifically intended to handle the operation of points and the corresponding control panel on Roxeter.

## Configuration
This will need to be set up in the configuration data in the code:
- Layout point number – integer 
- Internal point number – integer 

## Consumed Events
- Roxeter panel point switch normal – on / off
- Roxeter panel point switch reverse – on / off
- Route requiring points normal called (on) / cleared (off) - High Byte TBC
- Route requiring points reverse called (on) / cleared (off) - High Byte TBC
- Points detected normal – on / off
- Points detected reverse – on / off
- Track(s) over points occupied (on) / clear (off) 

## Produced Events
- Point motor movement request reverse (on) / normal (off) - High byte 0
- Points normal indication – on / off - High byte 1
- Points reverse indication – on / off - High byte 2
- Points locked indication – on / off - High byte 3
- Points out-of-correspondence indication – on (flashing) / off - High byte 4
- Points set and locked normal – on - High byte 5
- Points set and locked reverse - on - High byte 6

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

Some events are predefined and cannot be changed. All have the CANPOINT's node number with the event number set as follows:

- 1 - the CANPOINT's node number with the event number set to the internal point number plus 1000,
- 2 - the CANPOINT's node number with the event number set to the internal point number plus 2000.
