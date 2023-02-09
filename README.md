#ShiftX Library#

ShiftX is a library with enhanced functionality of the shiftIn/shiftOut functions compared to what the standard Arduino IDE offers and caters for certain devices such as certain PLLs which require a transfer of a number of bits not being a multiple of 8 in a single transaction.

##Revisions:##
1.0.0 First release

1.0.1 Added recall of shifts required with inEXTCLK_ functions

1.1.0 Add transfer functions for simultaneous reading and writing like the Arduino SPI.transfer() function

1.1.1 Fixed failure to compile on ARM platforms (Due etc.)

##FEATURES:##
Shift 1 to 64 bits at a time whereas the standard shiftIn/shiftOut is fixed at 8 bits.

Support for shifting arrays up to a total of 8191/4095/2047/1023 bytes/words/dwords/qwords respectively.

Programmable delay time in uS.

External shift clock option with programmable polarity for Select and Clock and optional timeout (0 will disable it) in uS - Timeout flag and MSBbitShiftsRequired is reset on a new inEXTCLK_ cycle and are retrievable.

MSBbitShiftsRequired is used for shifting an individual value right up to the bit width of an individual element; with a library such as BitFieldManipulation, array bitshifting is possible.

##HOW TO USE:##

ShiftX.out_(Byte/Word/Dword/Qword)(DataPin, ClockPin, Direction(MSBIFIRST/LSBFIRST), BitsToShift(1-64), DelayTime(0-16383), value) - uses internal clock

ShiftX.out_(Byte/Word/Dword/Qword)Array(DataPin, ClockPin, Direction, BitsToShift, DelayTime, *value) - uses internal clock

ShiftX.in_(Byte/Word/Dword/Qword)(DataPin, ClockPin, Direction(MSBFIRST/LSBFIRST), BitsToShift(1-64), DelayTime(0-16383)) - uses internal clock

ShiftX.in_(Byte/Word/Dword/Qword)Array(DataPin, ClockPin, Direction, BitsToShift, DelayTime, *value) - uses internal clock and returns *value

ShiftX.outEXTCLK_(Byte/Word/Dword/Qword)(SelectPinUsed(true/false), SelectPin, SelectPolarity(0/1), DataPin, ClockPin, ClockPolarity(0/1), Direction(MSBIFIRST/LSBFIRST), BitsToShift(1-64), ClockToggleTimeout, value) - uses external clock with ShiftXtimeout flag

ShiftX.outEXTCLK_(Byte/Word/Dword/Qword)Array(SelectPinUsed, SelectPin, SelectPolarity, DataPin, ClockPin, ClockPolarity, Direction, BitsToShift, ClockToggleTimeout, *value) - uses internal clock

ShiftX.inEXTCLK_(Byte/Word/Dword/Qword)(SelectPinUsed(true/false), SelectPin, SelectPolarity(0/1), DataPin, ClockPin, ClockPolarity(0/1), Direction(MSBIFIRST/LSBFIRST), BitsToShift(1-64), ClockToggleTimeout) - uses external clock with ShiftXtimeout flag

ShiftX.inEXTCLK_(Byte/Word/Dword/Qword)Array(SelectPinUsed, SelectPin, SelectPolarity, DataPin, ClockPin, ClockPolarity, Direction, BitsToShift, ClockToggleTimeout, *value) - uses internal clock and returns *value

ShiftX.transfer_(Byte/Word/Dword/Qword)(DataInPin, DataOutPin, ClockPin, Direction(MSBIFIRST/LSBFIRST), BitsToShift(1-64), DelayTime(0-16383), value, ClockEdge) - uses internal clock - transfers values at the same time - return value is clocked during ClockEdge during transfer

ShiftX.transfer_(Byte/Word/Dword/Qword)Array(DataInPin, DataOutPin, ClockPin, Direction, BitsToShift, DelayTime, *valueOut, *valueIn, ClockEdge) - uses internal clock - return value is valueIn and is clocked during ClockEdge

ShiftX.transferEXTCLK_(Byte/Word/Dword/Qword)(SelectPinUsed(true/false), SelectPin, SelectPolarity(0/1), DataInPin, DataOutPin, ClockPin, ClockPolarity(0/1), Direction(MSBIFIRST/LSBFIRST), BitsToShift(1-64), ClockToggleTimeout, value) - uses external clock with ShiftXtimeout flag - return value is clocked during ClockEdge during transfer

ShiftX.transferEXTCLK_(Byte/Word/Dword/Qword)Array(SelectPinUsed, SelectPin, SelectPolarity, DataInPin, DataOutPin, ClockPin, ClockPolarity, Direction, BitsToShift, ClockToggleTimeout, *valueOut, *valueIn) - uses internal clock  - return value is valueIn and is clocked during ClockEdge

Pin directions and initial idle Clock/Select polarities must be set as appropriate.

BitsToShift is a uint16_t value.

Quad word (uint64_t) support including bit shifts (>>= and <<=) is required for the compiler if using 64 bit numbers.

For shifting arrays, it will shift (BitsToShift / bits_per_element) elements and if there is a remainder, one extra element will be required and all elements are referenced at Element 0 of the array.