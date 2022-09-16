#include "ShiftX.h"

bool ShiftXclass::WaitForPulse(bool SelectPinUsed, uint8_t SelectPin, uint8_t SelectPolarity, uint8_t ClockPin, uint8_t ClockPolarity, uint32_t ClockToggleTimeout) {
  bool PulseReceived = true;
  Timeout = false;
  uint32_t StartTime = micros();
  while (true) { // wait for clock pulse, negation of Select or timeout
    if (digitalRead(ClockPin) == ClockPolarity) {
      break;
    }
    uint32_t StopTime = micros();
    StopTime -= StartTime;
    if ((SelectPinUsed == true && digitalRead(SelectPin) != SelectPolarity) || (ClockToggleTimeout != 0 && StopTime > ClockToggleTimeout)) {
      PulseReceived = false;
      Timeout = true;
      break;
    }
  }
  return PulseReceived;
}

void ShiftXclass::out_Byte(uint8_t DataPin, uint8_t ClockPin, uint8_t Direction, uint8_t BitsToShift, uint16_t DelayTime, uint8_t value) {
  uint8_t ClockPinIdleState = digitalRead(ClockPin);
  uint8_t mask = 1;
  if (Direction == LSBFIRST) {
    for (int i = 0; i < BitsToShift; i++) {
      if ((mask & value) == 0) {
        digitalWrite(DataPin, LOW);
      }
      else {
        digitalWrite(DataPin, HIGH);
      }
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      digitalWrite(ClockPin, !ClockPinIdleState);
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      digitalWrite(ClockPin, ClockPinIdleState);
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      mask <<= 1;
    }
  }
  else if (Direction == MSBFIRST) {
    mask <<= (BitsToShift - 1);
    for (int i = 0; i < BitsToShift; i++) {
      if ((mask & value) == 0) {
        digitalWrite(DataPin, LOW);
      }
      else {
        digitalWrite(DataPin, HIGH);
      }
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      digitalWrite(ClockPin, !ClockPinIdleState);
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      digitalWrite(ClockPin, ClockPinIdleState);
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      if (mask == 0x80) { // deal with unwanted sign bit
        mask = 0x40;
      }
      else {
        mask >>= 1;
      }
    }
  }
}

uint8_t ShiftXclass::in_Byte(uint8_t DataPin, uint8_t ClockPin, uint8_t Direction, uint8_t BitsToShift, uint16_t DelayTime) {
  uint8_t ClockPinIdleState = digitalRead(ClockPin);
  uint8_t data = 0;
  uint8_t mask = 1;
  if (Direction == LSBFIRST) {
    for (int i = 0; i < BitsToShift; i++) {
      digitalWrite(ClockPin, !ClockPinIdleState);
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      if (digitalRead(DataPin) == HIGH) {
        data |= mask;
      }
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      digitalWrite(ClockPin, ClockPinIdleState);
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      mask <<= 1;
    }
  }
  else if (Direction == MSBFIRST) {
    mask <<= (BitsToShift - 1);
    for (int i = 0; i < BitsToShift; i++) {
      digitalWrite(ClockPin, !ClockPinIdleState);
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      if (digitalRead(DataPin) == HIGH) {
        data |= mask;
      }
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      digitalWrite(ClockPin, ClockPinIdleState);
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      if (mask == 0x80) { // deal with unwanted sign bit
        mask = 0x40;
      }
      else {
        mask >>= 1;
      }
    }
  }
  return data;
}

void ShiftXclass::outEXTCLK_Byte(bool SelectPinUsed, uint8_t SelectPin, uint8_t SelectPolarity, uint8_t DataPin, uint8_t ClockPin, uint8_t ClockPolarity, uint8_t Direction, uint8_t BitsToShift, uint32_t ClockToggleTimeout, uint8_t value) {
  uint8_t mask = 1;
  if (Direction == LSBFIRST) {
    for (int i = 0; i < BitsToShift; i++) {
      if (WaitForPulse(SelectPinUsed, SelectPin, SelectPolarity, ClockPin, !ClockPolarity, ClockToggleTimeout) == false) {
        break;
      }
      if ((mask & value) == 0) {
        digitalWrite(DataPin, LOW);
      }
      else {
        digitalWrite(DataPin, HIGH);
      }
      mask <<= 1;
      if (WaitForPulse(SelectPinUsed, SelectPin, SelectPolarity, ClockPin, ClockPolarity, ClockToggleTimeout) == false) {
        break;
      }
    }
  }
  else if (Direction == MSBFIRST) {
    mask <<= (BitsToShift - 1);
    for (int i = 0; i < BitsToShift; i++) {
      if (WaitForPulse(SelectPinUsed, SelectPin, SelectPolarity, ClockPin, !ClockPolarity, ClockToggleTimeout) == false) {
        break;
      }
      if ((mask & value) == 0) {
        digitalWrite(DataPin, LOW);
      }
      else {
        digitalWrite(DataPin, HIGH);
      }
      if (mask == 0x80) { // deal with unwanted sign bit
        mask = 0x40;
      }
      else {
        mask >>= 1;
      }
      if (WaitForPulse(SelectPinUsed, SelectPin, SelectPolarity, ClockPin, ClockPolarity, ClockToggleTimeout) == false) {
        break;
      }
    }
  }
}

uint8_t ShiftXclass::inEXTCLK_Byte(bool SelectPinUsed, uint8_t SelectPin, uint8_t SelectPolarity, uint8_t DataPin, uint8_t ClockPin, uint8_t ClockPolarity, uint8_t Direction, uint8_t BitsToShift, uint32_t ClockToggleTimeout) {
  MSBbitShiftsRequired = 0;
  uint8_t data = 0;
  uint8_t mask = 1;
  if (Direction == LSBFIRST) {
    for (int i = 0; i < BitsToShift; i++) {
      if (WaitForPulse(SelectPinUsed, SelectPin, SelectPolarity, ClockPin, ClockPolarity, ClockToggleTimeout) == false) {
        break;
      }
      if (digitalRead(DataPin) == HIGH) {
        data |= mask;
      }
      mask <<= 1;
      if (WaitForPulse(SelectPinUsed, SelectPin, SelectPolarity, ClockPin, !ClockPolarity, ClockToggleTimeout) == false) {
        break;
      }
    }
  }
  else if (Direction == MSBFIRST) {
    mask <<= (BitsToShift - 1);
    for (int i = 0; i < BitsToShift; i++) {
      if (WaitForPulse(SelectPinUsed, SelectPin, SelectPolarity, ClockPin, ClockPolarity, ClockToggleTimeout) == false) {
        MSBbitShiftsRequired = (7 - i);
        break;
      }
      if (digitalRead(DataPin) == HIGH) {
        data |= mask;
      }
      if (mask == 0x80) { // deal with unwanted sign bit
        mask = 0x40;
      }
      else {
        mask >>= 1;
      }
      if (WaitForPulse(SelectPinUsed, SelectPin, SelectPolarity, ClockPin, !ClockPolarity, ClockToggleTimeout) == false) {
        break;
      }
    }
  }
  return data;
}

void ShiftXclass::out_ByteArray(uint8_t DataPin, uint8_t ClockPin, uint8_t Direction, uint16_t BitsToShift, uint16_t DelayTime, const uint8_t *value) {
  if (Direction == LSBFIRST) {
    uint16_t ElementsRequired = BitsToShift;
    ElementsRequired /= 8;
    uint8_t BitRemainder = BitsToShift;
    BitRemainder %= 8;
    if (BitRemainder == 0) {
      ElementsRequired--;
    }
    for (int i = ElementsRequired; i >= 0; i--) {
      if (i == 0 && BitRemainder != 0) {
        out_Byte(DataPin, ClockPin, Direction, BitRemainder, DelayTime, value[i]);
      }
      else {
        out_Byte(DataPin, ClockPin, Direction, 8, DelayTime, value[i]);
      }
    }
  }
  else if (Direction == MSBFIRST) {
    uint16_t ElementsRequired = BitsToShift;
    ElementsRequired /= 8;
    uint8_t BitRemainder = BitsToShift;
    BitRemainder %= 8;
    if (BitRemainder != 0) {
      ElementsRequired++;
    }
    for (int i = 0; i < ElementsRequired; i++) {
      if (i == 0 && BitRemainder != 0) {
        out_Byte(DataPin, ClockPin, Direction, BitRemainder, DelayTime, value[i]);
      }
      else {
        out_Byte(DataPin, ClockPin, Direction, 8, DelayTime, value[i]);
      }
    }
  }
}

void ShiftXclass::in_ByteArray(uint8_t DataPin, uint8_t ClockPin, uint8_t Direction, uint16_t BitsToShift, uint16_t DelayTime, uint8_t *value) {
  if (Direction == LSBFIRST) {
    uint16_t ElementsRequired = BitsToShift;
    ElementsRequired /= 8;
    uint8_t BitRemainder = BitsToShift;
    BitRemainder %= 8;
    if (BitRemainder == 0) {
      ElementsRequired--;
    }
    for (int i = ElementsRequired; i >= 0; i--) {
      if (i == 0 && BitRemainder != 0) {
        value[i] = in_Byte(DataPin, ClockPin, Direction, BitRemainder, DelayTime);
      }
      else {
        value[i] = in_Byte(DataPin, ClockPin, Direction, 8, DelayTime);
      }
    }
  }
  else if (Direction == MSBFIRST) {
    uint16_t ElementsRequired = BitsToShift;
    ElementsRequired /= 8;
    uint8_t BitRemainder = BitsToShift;
    BitRemainder %= 8;
    if (BitRemainder != 0) {
      ElementsRequired++;
    }
    for (int i = 0; i < ElementsRequired; i++) {
      if (i == 0 && BitRemainder != 0) {
        value[i] = in_Byte(DataPin, ClockPin, Direction, BitRemainder, DelayTime);
      }
      else {
        value[i] = in_Byte(DataPin, ClockPin, Direction, 8, DelayTime);
      }
    }
  }
}

void ShiftXclass::outEXTCLK_ByteArray(bool SelectPinUsed, uint8_t SelectPin, uint8_t SelectPolarity, uint8_t DataPin, uint8_t ClockPin, uint8_t ClockPolarity, uint8_t Direction, uint16_t BitsToShift, uint32_t ClockToggleTimeout, const uint8_t *value) {
  if (Direction == LSBFIRST) {
    uint16_t ElementsRequired = BitsToShift;
    ElementsRequired /= 8;
    uint8_t BitRemainder = BitsToShift;
    BitRemainder %= 8;
    if (BitRemainder == 0) {
      ElementsRequired--;
    }
    for (int i = ElementsRequired; i >= 0; i--) {
      if (i == 0 && BitRemainder != 0) {
        outEXTCLK_Byte(SelectPinUsed, SelectPin, SelectPolarity, DataPin, ClockPin, ClockPolarity, Direction, BitRemainder, ClockToggleTimeout, value[i]);
      }
      else {
        outEXTCLK_Byte(SelectPinUsed, SelectPin, SelectPolarity, DataPin, ClockPin, ClockPolarity, Direction, 8, ClockToggleTimeout, value[i]);
      }
    }
  }
  else if (Direction == MSBFIRST) {
    uint16_t ElementsRequired = BitsToShift;
    ElementsRequired /= 8;
    uint8_t BitRemainder = BitsToShift;
    BitRemainder %= 8;
    if (BitRemainder != 0) {
      ElementsRequired++;
    }
    for (int i = 0; i < ElementsRequired; i++) {
      if (i == 0 && BitRemainder != 0) {
        outEXTCLK_Byte(SelectPinUsed, SelectPin, SelectPolarity, DataPin, ClockPin, ClockPolarity, Direction, BitRemainder, ClockToggleTimeout, value[i]);
      }
      else {
        outEXTCLK_Byte(SelectPinUsed, SelectPin, SelectPolarity, DataPin, ClockPin, ClockPolarity, Direction, 8, ClockToggleTimeout, value[i]);
      }
    }
  }
}

void ShiftXclass::inEXTCLK_ByteArray(bool SelectPinUsed, uint8_t SelectPin, uint8_t SelectPolarity, uint8_t DataPin, uint8_t ClockPin, uint8_t ClockPolarity, uint8_t Direction, uint16_t BitsToShift, uint32_t ClockToggleTimeout, uint8_t *value){
  if (Direction == LSBFIRST) {
    uint16_t ElementsRequired = BitsToShift;
    ElementsRequired /= 8;
    uint8_t BitRemainder = BitsToShift;
    BitRemainder %= 8;
    if (BitRemainder == 0) {
      ElementsRequired--;
    }
    for (int i = ElementsRequired; i >= 0; i--) {
      if (i == 0 && BitRemainder != 0) {
        value[i] = inEXTCLK_Byte(SelectPinUsed, SelectPin, SelectPolarity, DataPin, ClockPin, ClockPolarity, Direction, BitRemainder, ClockToggleTimeout);
      }
      else {
        value[i] = inEXTCLK_Byte(SelectPinUsed, SelectPin, SelectPolarity, DataPin, ClockPin, ClockPolarity, Direction, 8, ClockToggleTimeout);
      }
    }
  }
  else if (Direction == MSBFIRST) {
    uint16_t ElementsRequired = BitsToShift;
    ElementsRequired /= 8;
    uint8_t BitRemainder = BitsToShift;
    BitRemainder %= 8;
    if (BitRemainder != 0) {
      ElementsRequired++;
    }
    for (int i = 0; i < ElementsRequired; i++) {
      if (Timeout == true) {
        uint16_t temp = MSBbitShiftsRequired;
        MSBbitShiftsRequired = (7 - i);
        MSBbitShiftsRequired *= 8;
        MSBbitShiftsRequired += temp;
        break;
      }
      if (i == 0 && BitRemainder != 0) {
        value[i] = inEXTCLK_Byte(SelectPinUsed, SelectPin, SelectPolarity, DataPin, ClockPin, ClockPolarity, Direction, BitRemainder, ClockToggleTimeout);
      }
      else {
        value[i] = inEXTCLK_Byte(SelectPinUsed, SelectPin, SelectPolarity, DataPin, ClockPin, ClockPolarity, Direction, 8, ClockToggleTimeout);
      }
    }
  }
}

void ShiftXclass::out_Word(uint8_t DataPin, uint8_t ClockPin, uint8_t Direction, uint8_t BitsToShift, uint16_t DelayTime, uint16_t value) {
  uint8_t ClockPinIdleState = digitalRead(ClockPin);
  uint16_t mask = 1;
  if (Direction == LSBFIRST) {
    for (int i = 0; i < BitsToShift; i++) {
      if ((mask & value) == 0) {
        digitalWrite(DataPin, LOW);
      }
      else {
        digitalWrite(DataPin, HIGH);
      }
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      digitalWrite(ClockPin, !ClockPinIdleState);
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      digitalWrite(ClockPin, ClockPinIdleState);
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      mask <<= 1;
    }
  }
  else if (Direction == MSBFIRST) {
    mask <<= (BitsToShift - 1);
    for (int i = 0; i < BitsToShift; i++) {
      if ((mask & value) == 0) {
        digitalWrite(DataPin, LOW);
      }
      else {
        digitalWrite(DataPin, HIGH);
      }
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      digitalWrite(ClockPin, !ClockPinIdleState);
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      digitalWrite(ClockPin, ClockPinIdleState);
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      if (mask == 0x8000) { // deal with unwanted sign bit
        mask = 0x4000;
      }
      else {
        mask >>= 1;
      }
    }
  }
}

uint16_t ShiftXclass::in_Word(uint8_t DataPin, uint8_t ClockPin, uint8_t Direction, uint8_t BitsToShift, uint16_t DelayTime) {
  uint8_t ClockPinIdleState = digitalRead(ClockPin);
  uint16_t data = 0;
  uint16_t mask = 1;
  if (Direction == LSBFIRST) {
    for (int i = 0; i < BitsToShift; i++) {
      digitalWrite(ClockPin, !ClockPinIdleState);
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      if (digitalRead(DataPin) == HIGH) {
        data |= mask;
      }
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      digitalWrite(ClockPin, ClockPinIdleState);
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      mask <<= 1;
    }
  }
  else if (Direction == MSBFIRST) {
    mask <<= (BitsToShift - 1);
    for (int i = 0; i < BitsToShift; i++) {
      digitalWrite(ClockPin, !ClockPinIdleState);
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      if (digitalRead(DataPin) == HIGH) {
        data |= mask;
      }
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      digitalWrite(ClockPin, ClockPinIdleState);
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      if (mask == 0x8000) { // deal with unwanted sign bit
        mask = 0x4000;
      }
      else {
        mask >>= 1;
      }
    }
  }
  return data;
}

void ShiftXclass::outEXTCLK_Word(bool SelectPinUsed, uint8_t SelectPin, uint8_t SelectPolarity, uint8_t DataPin, uint8_t ClockPin, uint8_t ClockPolarity, uint8_t Direction, uint8_t BitsToShift, uint32_t ClockToggleTimeout, uint16_t value) {
  uint16_t mask = 1;
  if (Direction == LSBFIRST) {
    for (int i = 0; i < BitsToShift; i++) {
      if (WaitForPulse(SelectPinUsed, SelectPin, SelectPolarity, ClockPin, ClockPolarity, ClockToggleTimeout) == false) {
        break;
      }
      if ((mask & value) == 0) {
        digitalWrite(DataPin, LOW);
      }
      else {
        digitalWrite(DataPin, HIGH);
      }
      mask <<= 1;
      if (WaitForPulse(SelectPinUsed, SelectPin, SelectPolarity, ClockPin, ClockPolarity, ClockToggleTimeout) == false) {
        break;
      }
    }
  }
  else if (Direction == MSBFIRST) {
    mask <<= (BitsToShift - 1);
    for (int i = 0; i < BitsToShift; i++) {
      if (WaitForPulse(SelectPinUsed, SelectPin, SelectPolarity, ClockPin, !ClockPolarity, ClockToggleTimeout) == false) {
        break;
      }
      if ((mask & value) == 0) {
        digitalWrite(DataPin, LOW);
      }
      else {
        digitalWrite(DataPin, HIGH);
      }
      if (mask == 0x8000) { // deal with unwanted sign bit
        mask = 0x4000;
      }
      else {
        mask >>= 1;
      }
      if (WaitForPulse(SelectPinUsed, SelectPin, SelectPolarity, ClockPin, ClockPolarity, ClockToggleTimeout) == false) {
        break;
      }
    }
  }
}

uint16_t ShiftXclass::inEXTCLK_Word(bool SelectPinUsed, uint8_t SelectPin, uint8_t SelectPolarity, uint8_t DataPin, uint8_t ClockPin, uint8_t ClockPolarity, uint8_t Direction, uint8_t BitsToShift, uint32_t ClockToggleTimeout) {
  uint16_t data = 0;
  uint16_t mask = 1;
  MSBbitShiftsRequired = 0;
  if (Direction == LSBFIRST) {
    for (int i = 0; i < BitsToShift; i++) {
      if (WaitForPulse(SelectPinUsed, SelectPin, SelectPolarity, ClockPin, ClockPolarity, ClockToggleTimeout) == false) {
        break;
      }
      if (digitalRead(DataPin) == HIGH) {
        data |= mask;
      }
      mask <<= 1;
      if (WaitForPulse(SelectPinUsed, SelectPin, SelectPolarity, ClockPin, !ClockPolarity, ClockToggleTimeout) == false) {
        break;
      }
    }
  }
  else if (Direction == MSBFIRST) {
    mask <<= (BitsToShift - 1);
    for (int i = 0; i < BitsToShift; i++) {
      if (WaitForPulse(SelectPinUsed, SelectPin, SelectPolarity, ClockPin, ClockPolarity, ClockToggleTimeout) == false) {
        MSBbitShiftsRequired = (15 - i);
        break;
      }
      if (digitalRead(DataPin) == HIGH) {
        data |= mask;
      }
      if (mask == 0x8000) { // deal with unwanted sign bit
        mask = 0x4000;
      }
      else {
        mask >>= 1;
      }
      if (WaitForPulse(SelectPinUsed, SelectPin, SelectPolarity, ClockPin, !ClockPolarity, ClockToggleTimeout) == false) {
        break;
      }
    }
  }
  return data;
}

void ShiftXclass::out_WordArray(uint8_t DataPin, uint8_t ClockPin, uint8_t Direction, uint16_t BitsToShift, uint16_t DelayTime, const uint16_t *value) {
  if (Direction == LSBFIRST) {
    uint16_t ElementsRequired = BitsToShift;
    ElementsRequired /= 16;
    uint8_t BitRemainder = BitsToShift;
    BitRemainder %= 16;
    if (BitRemainder == 0) {
      ElementsRequired--;
    }
    for (int i = ElementsRequired; i >= 0; i--) {
      if (i == 0 && BitRemainder != 0) {
        out_Word(DataPin, ClockPin, Direction, BitRemainder, DelayTime, value[i]);
      }
      else {
        out_Word(DataPin, ClockPin, Direction, 16, DelayTime, value[i]);
      }
    }
  }
  else if (Direction == MSBFIRST) {
    uint16_t ElementsRequired = BitsToShift;
    ElementsRequired /= 16;
    uint8_t BitRemainder = BitsToShift;
    BitRemainder %= 16;
    if (BitRemainder != 0) {
      ElementsRequired++;
    }
    for (int i = 0; i < ElementsRequired; i++) {
      if (i == 0 && BitRemainder != 0) {
        out_Word(DataPin, ClockPin, Direction, BitRemainder, DelayTime, value[i]);
      }
      else {
        out_Word(DataPin, ClockPin, Direction, 16, DelayTime, value[i]);
      }
    }
  }
}

void ShiftXclass::in_WordArray(uint8_t DataPin, uint8_t ClockPin, uint8_t Direction, uint16_t BitsToShift, uint16_t DelayTime, uint16_t *value) {
  if (Direction == LSBFIRST) {
    uint16_t ElementsRequired = BitsToShift;
    ElementsRequired /= 16;
    uint8_t BitRemainder = BitsToShift;
    BitRemainder %= 16;
    if (BitRemainder == 0) {
      ElementsRequired--;
    }
    for (int i = ElementsRequired; i >= 0; i--) {
      if (i == 0 && BitRemainder != 0) {
        value[i] = in_Word(DataPin, ClockPin, Direction, BitRemainder, DelayTime);
      }
      else {
        value[i] = in_Word(DataPin, ClockPin, Direction, 16, DelayTime);
      }
    }
  }
  else if (Direction == MSBFIRST) {
    uint16_t ElementsRequired = BitsToShift;
    ElementsRequired /= 16;
    uint8_t BitRemainder = BitsToShift;
    BitRemainder %= 16;
    if (BitRemainder != 0) {
      ElementsRequired++;
    }
    for (int i = 0; i < ElementsRequired; i++) {
      if (i == 0 && BitRemainder != 0) {
        value[i] = in_Word(DataPin, ClockPin, Direction, BitRemainder, DelayTime);
      }
      else {
        value[i] = in_Word(DataPin, ClockPin, Direction, 16, DelayTime);
      }
    }
  }
}

void ShiftXclass::outEXTCLK_WordArray(bool SelectPinUsed, uint8_t SelectPin, uint8_t SelectPolarity, uint8_t DataPin, uint8_t ClockPin, uint8_t ClockPolarity, uint8_t Direction, uint16_t BitsToShift, uint32_t ClockToggleTimeout, const uint16_t *value) {
  if (Direction == LSBFIRST) {
    uint16_t ElementsRequired = BitsToShift;
    ElementsRequired /= 16;
    uint8_t BitRemainder = BitsToShift;
    BitRemainder %= 16;
    if (BitRemainder == 0) {
      ElementsRequired--;
    }
    for (int i = ElementsRequired; i >= 0; i--) {
      if (i == 0 && BitRemainder != 0) {
        outEXTCLK_Byte(SelectPinUsed, SelectPin, SelectPolarity, DataPin, ClockPin, ClockPolarity, Direction, BitRemainder, ClockToggleTimeout, value[i]);
      }
      else {
        outEXTCLK_Byte(SelectPinUsed, SelectPin, SelectPolarity, DataPin, ClockPin, ClockPolarity, Direction, 16, ClockToggleTimeout, value[i]);
      }
    }
  }
  else if (Direction == MSBFIRST) {
    uint16_t ElementsRequired = BitsToShift;
    ElementsRequired /= 16;
    uint8_t BitRemainder = BitsToShift;
    BitRemainder %= 16;
    if (BitRemainder != 0) {
      ElementsRequired++;
    }
    for (int i = 0; i < ElementsRequired; i++) {
      if (i == 0 && BitRemainder != 0) {
        outEXTCLK_Byte(SelectPinUsed, SelectPin, SelectPolarity, DataPin, ClockPin, ClockPolarity, Direction, BitRemainder, ClockToggleTimeout, value[i]);
      }
      else {
        outEXTCLK_Byte(SelectPinUsed, SelectPin, SelectPolarity, DataPin, ClockPin, ClockPolarity, Direction, 16, ClockToggleTimeout, value[i]);
      }
    }
  }
}

void ShiftXclass::inEXTCLK_WordArray(bool SelectPinUsed, uint8_t SelectPin, uint8_t SelectPolarity, uint8_t DataPin, uint8_t ClockPin, uint8_t ClockPolarity, uint8_t Direction, uint16_t BitsToShift, uint32_t ClockToggleTimeout, uint16_t *value){
  if (Direction == LSBFIRST) {
    uint16_t ElementsRequired = BitsToShift;
    ElementsRequired /= 16;
    uint8_t BitRemainder = BitsToShift;
    BitRemainder %= 16;
    if (BitRemainder == 0) {
      ElementsRequired--;
    }
    for (int i = ElementsRequired; i >= 0; i--) {
      if (i == 0 && BitRemainder != 0) {
        value[i] = inEXTCLK_Byte(SelectPinUsed, SelectPin, SelectPolarity, DataPin, ClockPin, ClockPolarity, Direction, BitRemainder, ClockToggleTimeout);
      }
      else {
        value[i] = inEXTCLK_Byte(SelectPinUsed, SelectPin, SelectPolarity, DataPin, ClockPin, ClockPolarity, Direction, 16, ClockToggleTimeout);
      }
    }
  }
  else if (Direction == MSBFIRST) {
    uint16_t ElementsRequired = BitsToShift;
    ElementsRequired /= 16;
    uint8_t BitRemainder = BitsToShift;
    BitRemainder %= 16;
    if (BitRemainder != 0) {
      ElementsRequired++;
    }
    for (int i = 0; i < ElementsRequired; i++) {
      if (Timeout == true) {
        uint16_t temp = MSBbitShiftsRequired;
        MSBbitShiftsRequired = (15 - i);
        MSBbitShiftsRequired *= 16;
        MSBbitShiftsRequired += temp;
        break;
      }
      if (i == 0 && BitRemainder != 0) {
        value[i] = inEXTCLK_Byte(SelectPinUsed, SelectPin, SelectPolarity, DataPin, ClockPin, ClockPolarity, Direction, BitRemainder, ClockToggleTimeout);
      }
      else {
        value[i] = inEXTCLK_Byte(SelectPinUsed, SelectPin, SelectPolarity, DataPin, ClockPin, ClockPolarity, Direction, 16, ClockToggleTimeout);
      }
    }
  }
}

void ShiftXclass::out_Dword(uint8_t DataPin, uint8_t ClockPin, uint8_t Direction, uint8_t BitsToShift, uint16_t DelayTime, uint32_t value) {
  uint8_t ClockPinIdleState = digitalRead(ClockPin);
  uint32_t mask = 1;
  if (Direction == LSBFIRST) {
    for (int i = 0; i < BitsToShift; i++) {
      if ((mask & value) == 0) {
        digitalWrite(DataPin, LOW);
      }
      else {
        digitalWrite(DataPin, HIGH);
      }
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      digitalWrite(ClockPin, !ClockPinIdleState);
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      digitalWrite(ClockPin, ClockPinIdleState);
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      mask <<= 1;
    }
  }
  else if (Direction == MSBFIRST) {
    mask <<= (BitsToShift - 1);
    for (int i = 0; i < BitsToShift; i++) {
      if ((mask & value) == 0) {
        digitalWrite(DataPin, LOW);
      }
      else {
        digitalWrite(DataPin, HIGH);
      }
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      digitalWrite(ClockPin, !ClockPinIdleState);
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      digitalWrite(ClockPin, ClockPinIdleState);
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      if (mask == 0x80000000) { // deal with unwanted sign bit
        mask = 0x40000000;
      }
      else {
        mask >>= 1;
      }
    }
  }
}

uint32_t ShiftXclass::in_Dword(uint8_t DataPin, uint8_t ClockPin, uint8_t Direction, uint8_t BitsToShift, uint16_t DelayTime) {
  uint8_t ClockPinIdleState = digitalRead(ClockPin);
  uint32_t data = 0;
  uint32_t mask = 1;
  if (Direction == LSBFIRST) {
    for (int i = 0; i < BitsToShift; i++) {
      digitalWrite(ClockPin, !ClockPinIdleState);
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      if (digitalRead(DataPin) == HIGH) {
        data |= mask;
      }
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      digitalWrite(ClockPin, ClockPinIdleState);
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      mask <<= 1;
    }
  }
  else if (Direction == MSBFIRST) {
    mask <<= (BitsToShift - 1);
    for (int i = 0; i < BitsToShift; i++) {
      digitalWrite(ClockPin, !ClockPinIdleState);
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      if (digitalRead(DataPin) == HIGH) {
        data |= mask;
      }
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      digitalWrite(ClockPin, ClockPinIdleState);
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      if (mask == 0x80000000) { // deal with unwanted sign bit
        mask = 0x40000000;
      }
      else {
        mask >>= 1;
      }
    }
  }
  return data;
}

void ShiftXclass::outEXTCLK_Dword(bool SelectPinUsed, uint8_t SelectPin, uint8_t SelectPolarity, uint8_t DataPin, uint8_t ClockPin, uint8_t ClockPolarity, uint8_t Direction, uint8_t BitsToShift, uint32_t ClockToggleTimeout, uint32_t value) {
  uint32_t mask = 1;
  if (Direction == LSBFIRST) {
    for (int i = 0; i < BitsToShift; i++) {
      if (WaitForPulse(SelectPinUsed, SelectPin, SelectPolarity, ClockPin, ClockPolarity, ClockToggleTimeout) == false) {
        break;
      }
      if ((mask & value) == 0) {
        digitalWrite(DataPin, LOW);
      }
      else {
        digitalWrite(DataPin, HIGH);
      }
      mask <<= 1;
      if (WaitForPulse(SelectPinUsed, SelectPin, SelectPolarity, ClockPin, ClockPolarity, ClockToggleTimeout) == false) {
        break;
      }
    }
  }
  else if (Direction == MSBFIRST) {
    mask <<= (BitsToShift - 1);
    for (int i = 0; i < BitsToShift; i++) {
      if (WaitForPulse(SelectPinUsed, SelectPin, SelectPolarity, ClockPin, !ClockPolarity, ClockToggleTimeout) == false) {
        break;
      }
      if ((mask & value) == 0) {
        digitalWrite(DataPin, LOW);
      }
      else {
        digitalWrite(DataPin, HIGH);
      }
      if (mask == 0x80000000) { // deal with unwanted sign bit
        mask = 0x40000000;
      }
      else {
        mask >>= 1;
      }
      if (WaitForPulse(SelectPinUsed, SelectPin, SelectPolarity, ClockPin, ClockPolarity, ClockToggleTimeout) == false) {
        break;
      }
    }
  }
}

uint32_t ShiftXclass::inEXTCLK_Dword(bool SelectPinUsed, uint8_t SelectPin, uint8_t SelectPolarity, uint8_t DataPin, uint8_t ClockPin, uint8_t ClockPolarity, uint8_t Direction, uint8_t BitsToShift, uint32_t ClockToggleTimeout) {
  uint32_t data = 0;
  uint32_t mask = 1;
  MSBbitShiftsRequired = 0;
  if (Direction == LSBFIRST) {
    for (int i = 0; i < BitsToShift; i++) {
      if (WaitForPulse(SelectPinUsed, SelectPin, SelectPolarity, ClockPin, ClockPolarity, ClockToggleTimeout) == false) {
        MSBbitShiftsRequired = (31 - i);
        break;
      }
      if (digitalRead(DataPin) == HIGH) {
        data |= mask;
      }
      mask <<= 1;
      if (WaitForPulse(SelectPinUsed, SelectPin, SelectPolarity, ClockPin, !ClockPolarity, ClockToggleTimeout) == false) {
        break;
      }
    }
  }
  else if (Direction == MSBFIRST) {
    mask <<= (BitsToShift - 1);
    for (int i = 0; i < BitsToShift; i++) {
      if (WaitForPulse(SelectPinUsed, SelectPin, SelectPolarity, ClockPin, ClockPolarity, ClockToggleTimeout) == false) {
        break;
      }
      if (digitalRead(DataPin) == HIGH) {
        data |= mask;
      }
      if (mask == 0x80000000) { // deal with unwanted sign bit
        mask = 0x40000000;
      }
      else {
        mask >>= 1;
      }
      if (WaitForPulse(SelectPinUsed, SelectPin, SelectPolarity, ClockPin, !ClockPolarity, ClockToggleTimeout) == false) {
        break;
      }
    }
  }
  return data;
}

void ShiftXclass::out_DwordArray(uint8_t DataPin, uint8_t ClockPin, uint8_t Direction, uint16_t BitsToShift, uint16_t DelayTime, const uint32_t *value) {
  if (Direction == LSBFIRST) {
    uint16_t ElementsRequired = BitsToShift;
    ElementsRequired /= 32;
    uint8_t BitRemainder = BitsToShift;
    BitRemainder %= 32;
    if (BitRemainder == 0) {
      ElementsRequired--;
    }
    for (int i = ElementsRequired; i >= 0; i--) {
      if (i == 0 && BitRemainder != 0) {
        out_Dword(DataPin, ClockPin, Direction, BitRemainder, DelayTime, value[i]);
      }
      else {
        out_Dword(DataPin, ClockPin, Direction, 32, DelayTime, value[i]);
      }
    }
  }
  else if (Direction == MSBFIRST) {
    uint16_t ElementsRequired = BitsToShift;
    ElementsRequired /= 32;
    uint8_t BitRemainder = BitsToShift;
    BitRemainder %= 32;
    if (BitRemainder != 0) {
      ElementsRequired++;
    }
    for (int i = 0; i < ElementsRequired; i++) {
      if (i == (ElementsRequired - 1) && BitRemainder != 0) {
        out_Dword(DataPin, ClockPin, Direction, BitRemainder, DelayTime, value[i]);
      }
      else {
        out_Dword(DataPin, ClockPin, Direction, 32, DelayTime, value[i]);
      }
    }
  }
}

void ShiftXclass::in_DwordArray(uint8_t DataPin, uint8_t ClockPin, uint8_t Direction, uint16_t BitsToShift, uint16_t DelayTime, uint32_t *value) {
  if (Direction == LSBFIRST) {
    uint16_t ElementsRequired = BitsToShift;
    ElementsRequired /= 32;
    uint8_t BitRemainder = BitsToShift;
    BitRemainder %= 32;
    if (BitRemainder == 0) {
      ElementsRequired--;
    }
    for (int i = ElementsRequired; i >= 0; i--) {
      if (i == 0 && BitRemainder != 0) {
        value[i] = in_Dword(DataPin, ClockPin, Direction, BitRemainder, DelayTime);
      }
      else {
        value[i] = in_Dword(DataPin, ClockPin, Direction, 32, DelayTime);
      }
    }
  }
  else if (Direction == MSBFIRST) {
    uint16_t ElementsRequired = BitsToShift;
    ElementsRequired /= 32;
    uint8_t BitRemainder = BitsToShift;
    BitRemainder %= 32;
    if (BitRemainder != 0) {
      ElementsRequired++;
    }
    for (int i = 0; i < ElementsRequired; i++) {
      if (i == 0 && BitRemainder != 0) {
        value[i] = in_Dword(DataPin, ClockPin, Direction, BitRemainder, DelayTime);
      }
      else {
        value[i] = in_Dword(DataPin, ClockPin, Direction, 32, DelayTime);
      }
    }
  }
}

void ShiftXclass::outEXTCLK_DwordArray(bool SelectPinUsed, uint8_t SelectPin, uint8_t SelectPolarity, uint8_t DataPin, uint8_t ClockPin, uint8_t ClockPolarity, uint8_t Direction, uint16_t BitsToShift, uint32_t ClockToggleTimeout, const uint32_t *value) {
  if (Direction == LSBFIRST) {
    uint16_t ElementsRequired = BitsToShift;
    ElementsRequired /= 32;
    uint8_t BitRemainder = BitsToShift;
    BitRemainder %= 32;
    if (BitRemainder == 0) {
      ElementsRequired--;
    }
    for (int i = ElementsRequired; i >= 0; i--) {
      if (i == 0 && BitRemainder != 0) {
        outEXTCLK_Byte(SelectPinUsed, SelectPin, SelectPolarity, DataPin, ClockPin, ClockPolarity, Direction, BitRemainder, ClockToggleTimeout, value[i]);
      }
      else {
        outEXTCLK_Byte(SelectPinUsed, SelectPin, SelectPolarity, DataPin, ClockPin, ClockPolarity, Direction, 32, ClockToggleTimeout, value[i]);
      }
    }
  }
  else if (Direction == MSBFIRST) {
    uint16_t ElementsRequired = BitsToShift;
    ElementsRequired /= 32;
    uint8_t BitRemainder = BitsToShift;
    BitRemainder %= 32;
    if (BitRemainder != 0) {
      ElementsRequired++;
    }
    for (int i = 0; i < ElementsRequired; i++) {
      if (i == 0 && BitRemainder != 0) {
        outEXTCLK_Byte(SelectPinUsed, SelectPin, SelectPolarity, DataPin, ClockPin, ClockPolarity, Direction, BitRemainder, ClockToggleTimeout, value[i]);
      }
      else {
        outEXTCLK_Byte(SelectPinUsed, SelectPin, SelectPolarity, DataPin, ClockPin, ClockPolarity, Direction, 32, ClockToggleTimeout, value[i]);
      }
    }
  }
}

void ShiftXclass::inEXTCLK_DwordArray(bool SelectPinUsed, uint8_t SelectPin, uint8_t SelectPolarity, uint8_t DataPin, uint8_t ClockPin, uint8_t ClockPolarity, uint8_t Direction, uint16_t BitsToShift, uint32_t ClockToggleTimeout, uint32_t *value){
  if (Direction == LSBFIRST) {
    uint16_t ElementsRequired = BitsToShift;
    ElementsRequired /= 32;
    uint8_t BitRemainder = BitsToShift;
    BitRemainder %= 32;
    if (BitRemainder == 0) {
      ElementsRequired--;
    }
    for (int i = ElementsRequired; i >= 0; i--) {
      if (i == 0 && BitRemainder != 0) {
        value[i] = inEXTCLK_Byte(SelectPinUsed, SelectPin, SelectPolarity, DataPin, ClockPin, ClockPolarity, Direction, BitRemainder, ClockToggleTimeout);
      }
      else {
        value[i] = inEXTCLK_Byte(SelectPinUsed, SelectPin, SelectPolarity, DataPin, ClockPin, ClockPolarity, Direction, 32, ClockToggleTimeout);
      }
    }
  }
  else if (Direction == MSBFIRST) {
    uint16_t ElementsRequired = BitsToShift;
    ElementsRequired /= 32;
    uint8_t BitRemainder = BitsToShift;
    BitRemainder %= 32;
    if (BitRemainder != 0) {
      ElementsRequired++;
    }
    for (int i = 0; i < ElementsRequired; i++) {
      if (Timeout == true) {
        uint16_t temp = MSBbitShiftsRequired;
        MSBbitShiftsRequired = (31 - i);
        MSBbitShiftsRequired *= 32;
        MSBbitShiftsRequired += temp;
        break;
      }
      if (i == 0 && BitRemainder != 0) {
        value[i] = inEXTCLK_Byte(SelectPinUsed, SelectPin, SelectPolarity, DataPin, ClockPin, ClockPolarity, Direction, BitRemainder, ClockToggleTimeout);
      }
      else {
        value[i] = inEXTCLK_Byte(SelectPinUsed, SelectPin, SelectPolarity, DataPin, ClockPin, ClockPolarity, Direction, 32, ClockToggleTimeout);
      }
    }
  }
}

void ShiftXclass::out_Qword(uint8_t DataPin, uint8_t ClockPin, uint8_t Direction, uint8_t BitsToShift, uint16_t DelayTime, uint64_t value) {
  uint8_t ClockPinIdleState = digitalRead(ClockPin);
  uint64_t mask = 1;
  if (Direction == LSBFIRST) {
    for (int i = 0; i < BitsToShift; i++) {
      if ((mask & value) == 0) {
        digitalWrite(DataPin, LOW);
      }
      else {
        digitalWrite(DataPin, HIGH);
      }
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      digitalWrite(ClockPin, !ClockPinIdleState);
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      digitalWrite(ClockPin, ClockPinIdleState);
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      mask <<= 1;
    }
  }
  else if (Direction == MSBFIRST) {
    mask <<= (BitsToShift - 1);
    for (int i = 0; i < BitsToShift; i++) {
      if ((mask & value) == 0) {
        digitalWrite(DataPin, LOW);
      }
      else {
        digitalWrite(DataPin, HIGH);
      }
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      digitalWrite(ClockPin, !ClockPinIdleState);
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      digitalWrite(ClockPin, ClockPinIdleState);
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      if (mask == 9223372036854775808ULL) { // deal with unwanted sign bit
        mask = 4611686018427387904ULL;
      }
      else {
        mask >>= 1;
      }
    }
  }
}

uint64_t ShiftXclass::in_Qword(uint8_t DataPin, uint8_t ClockPin, uint8_t Direction, uint8_t BitsToShift, uint16_t DelayTime) {
  uint8_t ClockPinIdleState = digitalRead(ClockPin);
  uint64_t data = 0;
  uint64_t mask = 1;
  if (Direction == LSBFIRST) {
    for (int i = 0; i < BitsToShift; i++) {
      digitalWrite(ClockPin, !ClockPinIdleState);
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      if (digitalRead(DataPin) == HIGH) {
        data |= mask;
      }
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      digitalWrite(ClockPin, ClockPinIdleState);
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      mask <<= 1;
    }
  }
  else if (Direction == MSBFIRST) {
    mask <<= (BitsToShift - 1);
    for (int i = 0; i < BitsToShift; i++) {
      digitalWrite(ClockPin, !ClockPinIdleState);
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      if (digitalRead(DataPin) == HIGH) {
        data |= mask;
      }
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      digitalWrite(ClockPin, ClockPinIdleState);
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      if (mask == 9223372036854775808ULL) { // deal with unwanted sign bit
        mask = 4611686018427387904ULL;
      }
      else {
        mask >>= 1;
      }
    }
  }
  return data;
}

void ShiftXclass::outEXTCLK_Qword(bool SelectPinUsed, uint8_t SelectPin, uint8_t SelectPolarity, uint8_t DataPin, uint8_t ClockPin, uint8_t ClockPolarity, uint8_t Direction, uint8_t BitsToShift, uint32_t ClockToggleTimeout, uint64_t value) {
  uint64_t mask = 1;
  if (Direction == LSBFIRST) {
    for (int i = 0; i < BitsToShift; i++) {
      if (WaitForPulse(SelectPinUsed, SelectPin, SelectPolarity, ClockPin, ClockPolarity, ClockToggleTimeout) == false) {
        break;
      }
      if ((mask & value) == 0) {
        digitalWrite(DataPin, LOW);
      }
      else {
        digitalWrite(DataPin, HIGH);
      }
      mask <<= 1;
      if (WaitForPulse(SelectPinUsed, SelectPin, SelectPolarity, ClockPin, ClockPolarity, ClockToggleTimeout) == false) {
        break;
      }
    }
  }
  else if (Direction == MSBFIRST) {
    mask <<= (BitsToShift - 1);
    for (int i = 0; i < BitsToShift; i++) {
      if (WaitForPulse(SelectPinUsed, SelectPin, SelectPolarity, ClockPin, !ClockPolarity, ClockToggleTimeout) == false) {
        break;
      }
      if ((mask & value) == 0) {
        digitalWrite(DataPin, LOW);
      }
      else {
        digitalWrite(DataPin, HIGH);
      }
      if (mask == 9223372036854775808ULL) { // deal with unwanted sign bit
        mask = 4611686018427387904ULL;
      }
      else {
        mask >>= 1;
      }
      if (WaitForPulse(SelectPinUsed, SelectPin, SelectPolarity, ClockPin, ClockPolarity, ClockToggleTimeout) == false) {
        break;
      }
    }
  }
}

uint64_t ShiftXclass::inEXTCLK_Qword(bool SelectPinUsed, uint8_t SelectPin, uint8_t SelectPolarity, uint8_t DataPin, uint8_t ClockPin, uint8_t ClockPolarity, uint8_t Direction, uint8_t BitsToShift, uint32_t ClockToggleTimeout) {
  uint64_t data = 0;
  uint64_t mask = 1;
  MSBbitShiftsRequired = 0;
  if (Direction == LSBFIRST) {
    for (int i = 0; i < BitsToShift; i++) {
      if (WaitForPulse(SelectPinUsed, SelectPin, SelectPolarity, ClockPin, ClockPolarity, ClockToggleTimeout) == false) {
        break;
      }
      if (digitalRead(DataPin) == HIGH) {
        data |= mask;
      }
      mask <<= 1;
      if (WaitForPulse(SelectPinUsed, SelectPin, SelectPolarity, ClockPin, !ClockPolarity, ClockToggleTimeout) == false) {
        break;
      }
    }
  }
  else if (Direction == MSBFIRST) {
    mask <<= (BitsToShift - 1);
    for (int i = 0; i < BitsToShift; i++) {
      if (WaitForPulse(SelectPinUsed, SelectPin, SelectPolarity, ClockPin, ClockPolarity, ClockToggleTimeout) == false) {
        MSBbitShiftsRequired = (63 - i);
        break;
      }
      if (digitalRead(DataPin) == HIGH) {
        data |= mask;
      }
      if (mask == 9223372036854775808ULL) { // deal with unwanted sign bit
        mask = 4611686018427387904ULL;
      }
      else {
        mask >>= 1;
      }
      if (WaitForPulse(SelectPinUsed, SelectPin, SelectPolarity, ClockPin, !ClockPolarity, ClockToggleTimeout) == false) {
        break;
      }
    }
  }
  return data;
}

void ShiftXclass::out_QwordArray(uint8_t DataPin, uint8_t ClockPin, uint8_t Direction, uint16_t BitsToShift, uint16_t DelayTime, const uint64_t *value) {
  if (Direction == LSBFIRST) {
    uint16_t ElementsRequired = BitsToShift;
    ElementsRequired /= 64;
    uint8_t BitRemainder = BitsToShift;
    BitRemainder %= 64;
    if (BitRemainder == 0) {
      ElementsRequired--;
    }
    for (int i = ElementsRequired; i >= 0; i--) {
      if (i == 0 && BitRemainder != 0) {
        out_Qword(DataPin, ClockPin, Direction, BitRemainder, DelayTime, value[i]);
      }
      else {
        out_Qword(DataPin, ClockPin, Direction, 64, DelayTime, value[i]);
      }
    }
  }
  else if (Direction == MSBFIRST) {
    uint16_t ElementsRequired = BitsToShift;
    ElementsRequired /= 64;
    uint8_t BitRemainder = BitsToShift;
    BitRemainder %= 64;
    if (BitRemainder != 0) {
      ElementsRequired++;
    }

    for (int i = 0; i < ElementsRequired; i++) {
      if (i == (ElementsRequired - 1) && BitRemainder != 0) {
        out_Qword(DataPin, ClockPin, Direction, BitRemainder, DelayTime, value[i]);
      }
      else {
        out_Qword(DataPin, ClockPin, Direction, 64, DelayTime, value[i]);
      }
    }
  }
}

void ShiftXclass::in_QwordArray(uint8_t DataPin, uint8_t ClockPin, uint8_t Direction, uint16_t BitsToShift, uint16_t DelayTime, uint64_t *value) {
  if (Direction == LSBFIRST) {
    uint16_t ElementsRequired = BitsToShift;
    ElementsRequired /= 64;
    uint8_t BitRemainder = BitsToShift;
    BitRemainder %= 64;
    if (BitRemainder == 0) {
      ElementsRequired--;
    }
    for (int i = ElementsRequired; i >= 0; i--) {
      if (i == 0 && BitRemainder != 0) {
        value[i] = in_Qword(DataPin, ClockPin, Direction, BitRemainder, DelayTime);
      }
      else {
        value[i] = in_Qword(DataPin, ClockPin, Direction, 32, DelayTime);
      }
    }
  }
  else if (Direction == MSBFIRST) {
    uint16_t ElementsRequired = BitsToShift;
    ElementsRequired /= 64;
    uint8_t BitRemainder = BitsToShift;
    BitRemainder %= 64;
    if (BitRemainder != 0) {
      ElementsRequired++;
    }
    for (int i = 0; i < ElementsRequired; i++) {
      if (i == 0 && BitRemainder != 0) {
        value[i] = in_Qword(DataPin, ClockPin, Direction, BitRemainder, DelayTime);
      }
      else {
        value[i] = in_Qword(DataPin, ClockPin, Direction, 32, DelayTime);
      }
    }
  }
}

void ShiftXclass::outEXTCLK_QwordArray(bool SelectPinUsed, uint8_t SelectPin, uint8_t SelectPolarity, uint8_t DataPin, uint8_t ClockPin, uint8_t ClockPolarity, uint8_t Direction, uint16_t BitsToShift, uint32_t ClockToggleTimeout, const uint64_t *value) {
  if (Direction == LSBFIRST) {
    uint16_t ElementsRequired = BitsToShift;
    ElementsRequired /= 64;
    uint8_t BitRemainder = BitsToShift;
    BitRemainder %= 64;
    if (BitRemainder == 0) {
      ElementsRequired--;
    }
    for (int i = ElementsRequired; i >= 0; i--) {
      if (i == 0 && BitRemainder != 0) {
        outEXTCLK_Byte(SelectPinUsed, SelectPin, SelectPolarity, DataPin, ClockPin, ClockPolarity, Direction, BitRemainder, ClockToggleTimeout, value[i]);
      }
      else {
        outEXTCLK_Byte(SelectPinUsed, SelectPin, SelectPolarity, DataPin, ClockPin, ClockPolarity, Direction, 64, ClockToggleTimeout, value[i]);
      }
    }
  }
  else if (Direction == MSBFIRST) {
    uint16_t ElementsRequired = BitsToShift;
    ElementsRequired /= 64;
    uint8_t BitRemainder = BitsToShift;
    BitRemainder %= 64;
    if (BitRemainder != 0) {
      ElementsRequired++;
    }
    for (int i = 0; i < ElementsRequired; i++) {
      if (i == 0 && BitRemainder != 0) {
        outEXTCLK_Byte(SelectPinUsed, SelectPin, SelectPolarity, DataPin, ClockPin, ClockPolarity, Direction, BitRemainder, ClockToggleTimeout, value[i]);
      }
      else {
        outEXTCLK_Byte(SelectPinUsed, SelectPin, SelectPolarity, DataPin, ClockPin, ClockPolarity, Direction, 64, ClockToggleTimeout, value[i]);
      }
    }
  }
}

void ShiftXclass::inEXTCLK_QwordArray(bool SelectPinUsed, uint8_t SelectPin, uint8_t SelectPolarity, uint8_t DataPin, uint8_t ClockPin, uint8_t ClockPolarity, uint8_t Direction, uint16_t BitsToShift, uint32_t ClockToggleTimeout, uint64_t *value){
  if (Direction == LSBFIRST) {
    uint16_t ElementsRequired = BitsToShift;
    ElementsRequired /= 64;
    uint8_t BitRemainder = BitsToShift;
    BitRemainder %= 64;
    if (BitRemainder == 0) {
      ElementsRequired--;
    }
    for (int i = ElementsRequired; i >= 0; i--) {
      if (i == 0 && BitRemainder != 0) {
        value[i] = inEXTCLK_Byte(SelectPinUsed, SelectPin, SelectPolarity, DataPin, ClockPin, ClockPolarity, Direction, BitRemainder, ClockToggleTimeout);
      }
      else {
        value[i] = inEXTCLK_Byte(SelectPinUsed, SelectPin, SelectPolarity, DataPin, ClockPin, ClockPolarity, Direction, 64, ClockToggleTimeout);
      }
    }
  }
  else if (Direction == MSBFIRST) {
    uint16_t ElementsRequired = BitsToShift;
    ElementsRequired /= 64;
    uint8_t BitRemainder = BitsToShift;
    BitRemainder %= 64;
    if (BitRemainder != 0) {
      ElementsRequired++;
    }
    for (int i = 0; i < ElementsRequired; i++) {
      if (Timeout == true) {
        uint16_t temp = MSBbitShiftsRequired;
        MSBbitShiftsRequired = (63 - i);
        MSBbitShiftsRequired *= 64;
        MSBbitShiftsRequired += temp;
        break;
      }
      if (i == 0 && BitRemainder != 0) {
        value[i] = inEXTCLK_Byte(SelectPinUsed, SelectPin, SelectPolarity, DataPin, ClockPin, ClockPolarity, Direction, BitRemainder, ClockToggleTimeout);
      }
      else {
        value[i] = inEXTCLK_Byte(SelectPinUsed, SelectPin, SelectPolarity, DataPin, ClockPin, ClockPolarity, Direction, 64, ClockToggleTimeout);
      }
    }
  }
}

uint8_t ShiftXclass::transfer_Byte(uint8_t DataInPin, uint8_t DataOutPin, uint8_t ClockPin, uint8_t Direction, uint8_t BitsToShift, uint16_t DelayTime, uint8_t value, uint8_t ClockEdge) {
  uint8_t DataToReturn = 0;
  uint8_t ClockPinIdleState = digitalRead(ClockPin);
  uint8_t mask = 1;
  if (Direction == LSBFIRST) {
    for (int i = 0; i < BitsToShift; i++) {
      if ((mask & value) == 0) {
        digitalWrite(DataOutPin, LOW);
      }
      else {
        digitalWrite(DataOutPin, HIGH);
      }
      digitalWrite(ClockPin, !ClockPinIdleState);
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      if ((ClockEdge == RISING && ClockPinIdleState == LOW && digitalRead(DataInPin) == HIGH) || (ClockEdge == FALLING && ClockPinIdleState == HIGH && digitalRead(DataInPin) == HIGH)) {
        DataToReturn |= mask;
      }
      digitalWrite(ClockPin, ClockPinIdleState);
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      if ((ClockEdge == RISING && ClockPinIdleState == HIGH && digitalRead(DataInPin) == HIGH) || (ClockEdge == FALLING && ClockPinIdleState == LOW && digitalRead(DataInPin) == HIGH)) {
        DataToReturn |= mask;
      }
      mask <<= 1;
    }
  }
  else if (Direction == MSBFIRST) {
    mask <<= (BitsToShift - 1);
    for (int i = 0; i < BitsToShift; i++) {
      if ((mask & value) == 0) {
        digitalWrite(DataOutPin, LOW);
      }
      else {
        digitalWrite(DataOutPin, HIGH);
      }
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      digitalWrite(ClockPin, !ClockPinIdleState);
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      if ((ClockEdge == RISING && ClockPinIdleState == LOW && digitalRead(DataInPin) == HIGH) || (ClockEdge == FALLING && ClockPinIdleState == HIGH && digitalRead(DataInPin) == HIGH)) {
        DataToReturn |= mask;
      }
      digitalWrite(ClockPin, ClockPinIdleState);
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      if ((ClockEdge == RISING && ClockPinIdleState == HIGH && digitalRead(DataInPin) == HIGH) || (ClockEdge == FALLING && ClockPinIdleState == LOW && digitalRead(DataInPin) == HIGH)) {
        DataToReturn |= mask;
      }
      if (mask == 0x80) { // deal with unwanted sign bit
        mask = 0x40;
      }
      else {
        mask >>= 1;
      }
    }
  }
  return DataToReturn;
}

uint8_t ShiftXclass::transferEXTCLK_Byte(bool SelectPinUsed, uint8_t SelectPin, uint8_t SelectPolarity, uint8_t DataInPin, uint8_t DataOutPin, uint8_t ClockPin, uint8_t ClockPolarity, uint8_t Direction, uint8_t BitsToShift, uint32_t ClockToggleTimeout, uint8_t valueOut, uint8_t ClockEdge) {
  uint8_t DataToReturn = 0;
  uint8_t mask = 1;
  if (Direction == LSBFIRST) {
    for (int i = 0; i < BitsToShift; i++) {
      if (WaitForPulse(SelectPinUsed, SelectPin, SelectPolarity, ClockPin, !ClockPolarity, ClockToggleTimeout) == false) {
        break;
      }
      if ((mask & valueOut) == 0) {
        digitalWrite(DataOutPin, LOW);
      }
      else {
        digitalWrite(DataOutPin, HIGH);
      }
      if ((ClockEdge == RISING && ClockPolarity == HIGH && digitalRead(DataInPin) == HIGH) || (ClockEdge == FALLING && ClockPolarity == LOW && digitalRead(DataInPin) == HIGH)) {
        DataToReturn |= mask;
      }
      mask <<= 1;
      if (WaitForPulse(SelectPinUsed, SelectPin, SelectPolarity, ClockPin, ClockPolarity, ClockToggleTimeout) == false) {
        break;
      }
      if ((ClockEdge == RISING && ClockPolarity == LOW && digitalRead(DataInPin) == HIGH) || (ClockEdge == FALLING && ClockPolarity == HIGH && digitalRead(DataInPin) == HIGH)) {
        DataToReturn |= mask;
      }
    }
  }
  else if (Direction == MSBFIRST) {
    mask <<= (BitsToShift - 1);
    for (int i = 0; i < BitsToShift; i++) {
      if (WaitForPulse(SelectPinUsed, SelectPin, SelectPolarity, ClockPin, !ClockPolarity, ClockToggleTimeout) == false) {
        break;
      }
      if ((mask & valueOut) == 0) {
        digitalWrite(DataOutPin, LOW);
      }
      else {
        digitalWrite(DataOutPin, HIGH);
      }
      if ((ClockEdge == RISING && ClockPolarity == 0 && digitalRead(DataInPin) == HIGH) || (ClockEdge == FALLING && ClockPolarity != 0 && digitalRead(DataInPin) == HIGH)) {
        DataToReturn |= mask;
      }
      if (mask == 0x80) { // deal with unwanted sign bit
        mask = 0x40;
      }
      else {
        mask >>= 1;
      }
      if (WaitForPulse(SelectPinUsed, SelectPin, SelectPolarity, ClockPin, ClockPolarity, ClockToggleTimeout) == false) {
        break;
      }
      if ((ClockEdge == RISING && ClockPolarity != 0 && digitalRead(DataInPin) == HIGH) || (ClockEdge == FALLING && ClockPolarity == 0 && digitalRead(DataInPin) == HIGH)) {
        DataToReturn |= mask;
      }
    }
  }
  return DataToReturn;
}

void ShiftXclass::transfer_ByteArray(uint8_t DataInPin, uint8_t DataOutPin, uint8_t ClockPin, uint8_t Direction, uint16_t BitsToShift, uint16_t DelayTime, const uint8_t *valueOut, uint8_t *valueIn, uint8_t ClockEdge) {
  if (Direction == LSBFIRST) {
    uint16_t ElementsRequired = BitsToShift;
    ElementsRequired /= 8;
    uint8_t BitRemainder = BitsToShift;
    BitRemainder %= 8;
    if (BitRemainder == 0) {
      ElementsRequired--;
    }
    for (int i = ElementsRequired; i >= 0; i--) {
      if (i == 0 && BitRemainder != 0) {
        valueIn[i] = transfer_Byte(DataInPin, DataOutPin, ClockPin, Direction, BitRemainder, DelayTime, valueOut[i], ClockEdge);
      }
      else {
        valueIn[i] = transfer_Byte(DataInPin, DataOutPin, ClockPin, Direction, 8, DelayTime, valueOut[i], ClockEdge);
      }
    }
  }
  else if (Direction == MSBFIRST) {
    uint16_t ElementsRequired = BitsToShift;
    ElementsRequired /= 8;
    uint8_t BitRemainder = BitsToShift;
    BitRemainder %= 8;
    if (BitRemainder != 0) {
      ElementsRequired++;
    }
    for (int i = 0; i < ElementsRequired; i++) {
      if (i == 0 && BitRemainder != 0) {
        valueIn[i] = transfer_Byte(DataInPin, DataOutPin, ClockPin, Direction, BitRemainder, DelayTime, valueOut[i], ClockEdge);
      }
      else {
        valueIn[i] = transfer_Byte(DataInPin, DataOutPin, ClockPin, Direction, 8, DelayTime, valueOut[i], ClockEdge);
      }
    }
  }
}

void ShiftXclass::transferEXTCLK_ByteArray(bool SelectPinUsed, uint8_t SelectPin, uint8_t SelectPolarity, uint8_t DataInPin, uint8_t DataOutPin, uint8_t ClockPin, uint8_t ClockPolarity, uint8_t Direction, uint16_t BitsToShift, uint32_t ClockToggleTimeout, const uint8_t *valueOut, uint8_t *valueIn, uint8_t ClockEdge) {
  if (Direction == LSBFIRST) {
    uint16_t ElementsRequired = BitsToShift;
    ElementsRequired /= 8;
    uint8_t BitRemainder = BitsToShift;
    BitRemainder %= 8;
    if (BitRemainder == 0) {
      ElementsRequired--;
    }
    for (int i = ElementsRequired; i >= 0; i--) {
      if (i == 0 && BitRemainder != 0) {
        valueIn[i] = transferEXTCLK_Byte(SelectPinUsed, SelectPin, SelectPolarity, DataInPin, DataOutPin, ClockPin, ClockPolarity, Direction, BitRemainder, ClockToggleTimeout, valueOut[i], ClockEdge);
      }
      else {
        valueIn[i] = transferEXTCLK_Byte(SelectPinUsed, SelectPin, SelectPolarity, DataInPin, DataOutPin, ClockPin, ClockPolarity, Direction, 8, ClockToggleTimeout, valueOut[i], ClockEdge);
      }
    }
  }
  else if (Direction == MSBFIRST) {
    uint16_t ElementsRequired = BitsToShift;
    ElementsRequired /= 8;
    uint8_t BitRemainder = BitsToShift;
    BitRemainder %= 8;
    if (BitRemainder != 0) {
      ElementsRequired++;
    }
    for (int i = 0; i < ElementsRequired; i++) {
      if (i == 0 && BitRemainder != 0) {
        valueIn[i] = transferEXTCLK_Byte(SelectPinUsed, SelectPin, SelectPolarity, DataInPin, DataOutPin, ClockPin, ClockPolarity, Direction, BitRemainder, ClockToggleTimeout, valueOut[i], ClockEdge);
      }
      else {
        valueIn[i] = transferEXTCLK_Byte(SelectPinUsed, SelectPin, SelectPolarity, DataInPin, DataOutPin, ClockPin, ClockPolarity, Direction, 8, ClockToggleTimeout, valueOut[i], ClockEdge);
      }
    }
  }
}

uint16_t ShiftXclass::transfer_Word(uint8_t DataInPin, uint8_t DataOutPin, uint8_t ClockPin, uint8_t Direction, uint8_t BitsToShift, uint16_t DelayTime, uint16_t value, uint8_t ClockEdge) {
  uint16_t DataToReturn = 0;
  uint8_t ClockPinIdleState = digitalRead(ClockPin);
  uint16_t mask = 1;
  if (Direction == LSBFIRST) {
    for (int i = 0; i < BitsToShift; i++) {
      if ((mask & value) == 0) {
        digitalWrite(DataOutPin, LOW);
      }
      else {
        digitalWrite(DataOutPin, HIGH);
      }
      digitalWrite(ClockPin, !ClockPinIdleState);
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      if ((ClockEdge == RISING && ClockPinIdleState == LOW && digitalRead(DataInPin) == HIGH) || (ClockEdge == FALLING && ClockPinIdleState == HIGH && digitalRead(DataInPin) == HIGH)) {
        DataToReturn |= mask;
      }
      digitalWrite(ClockPin, ClockPinIdleState);
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      if ((ClockEdge == RISING && ClockPinIdleState == HIGH && digitalRead(DataInPin) == HIGH) || (ClockEdge == FALLING && ClockPinIdleState == LOW && digitalRead(DataInPin) == HIGH)) {
        DataToReturn |= mask;
      }
      mask <<= 1;
    }
  }
  else if (Direction == MSBFIRST) {
    mask <<= (BitsToShift - 1);
    for (int i = 0; i < BitsToShift; i++) {
      if ((mask & value) == 0) {
        digitalWrite(DataOutPin, LOW);
      }
      else {
        digitalWrite(DataOutPin, HIGH);
      }
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      digitalWrite(ClockPin, !ClockPinIdleState);
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      if ((ClockEdge == RISING && ClockPinIdleState == LOW && digitalRead(DataInPin) == HIGH) || (ClockEdge == FALLING && ClockPinIdleState == HIGH && digitalRead(DataInPin) == HIGH)) {
        DataToReturn |= mask;
      }
      digitalWrite(ClockPin, ClockPinIdleState);
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      if ((ClockEdge == RISING && ClockPinIdleState == HIGH && digitalRead(DataInPin) == HIGH) || (ClockEdge == FALLING && ClockPinIdleState == LOW && digitalRead(DataInPin) == HIGH)) {
        DataToReturn |= mask;
      }
      if (mask == 0x8000) { // deal with unwanted sign bit
        mask = 0x4000;
      }
      else {
        mask >>= 1;
      }
    }
  }
  return DataToReturn;
}

uint16_t ShiftXclass::transferEXTCLK_Word(bool SelectPinUsed, uint8_t SelectPin, uint8_t SelectPolarity, uint8_t DataInPin, uint8_t DataOutPin, uint8_t ClockPin, uint8_t ClockPolarity, uint8_t Direction, uint16_t BitsToShift, uint32_t ClockToggleTimeout, uint16_t valueOut, uint8_t ClockEdge) {
  uint16_t DataToReturn = 0;
  uint16_t mask = 1;
  if (Direction == LSBFIRST) {
    for (int i = 0; i < BitsToShift; i++) {
      if (WaitForPulse(SelectPinUsed, SelectPin, SelectPolarity, ClockPin, !ClockPolarity, ClockToggleTimeout) == false) {
        break;
      }
      if ((mask & valueOut) == 0) {
        digitalWrite(DataOutPin, LOW);
      }
      else {
        digitalWrite(DataOutPin, HIGH);
      }
      if ((ClockEdge == RISING && ClockPolarity == HIGH && digitalRead(DataInPin) == HIGH) || (ClockEdge == FALLING && ClockPolarity == LOW && digitalRead(DataInPin) == HIGH)) {
        DataToReturn |= mask;
      }
      mask <<= 1;
      if (WaitForPulse(SelectPinUsed, SelectPin, SelectPolarity, ClockPin, ClockPolarity, ClockToggleTimeout) == false) {
        break;
      }
      if ((ClockEdge == RISING && ClockPolarity == LOW && digitalRead(DataInPin) == HIGH) || (ClockEdge == FALLING && ClockPolarity == HIGH && digitalRead(DataInPin) == HIGH)) {
        DataToReturn |= mask;
      }
    }
  }
  else if (Direction == MSBFIRST) {
    mask <<= (BitsToShift - 1);
    for (int i = 0; i < BitsToShift; i++) {
      if (WaitForPulse(SelectPinUsed, SelectPin, SelectPolarity, ClockPin, !ClockPolarity, ClockToggleTimeout) == false) {
        break;
      }
      if ((mask & valueOut) == 0) {
        digitalWrite(DataOutPin, LOW);
      }
      else {
        digitalWrite(DataOutPin, HIGH);
      }
      if ((ClockEdge == RISING && ClockPolarity == 0 && digitalRead(DataInPin) == HIGH) || (ClockEdge == FALLING && ClockPolarity != 0 && digitalRead(DataInPin) == HIGH)) {
        DataToReturn |= mask;
      }
      if (mask == 0x8000) { // deal with unwanted sign bit
        mask = 0x4000;
      }
      else {
        mask >>= 1;
      }
      if (WaitForPulse(SelectPinUsed, SelectPin, SelectPolarity, ClockPin, ClockPolarity, ClockToggleTimeout) == false) {
        break;
      }
      if ((ClockEdge == RISING && ClockPolarity != 0 && digitalRead(DataInPin) == HIGH) || (ClockEdge == FALLING && ClockPolarity == 0 && digitalRead(DataInPin) == HIGH)) {
        DataToReturn |= mask;
      }
    }
  }
  return DataToReturn;
}

void ShiftXclass::transfer_WordArray(uint8_t DataInPin, uint8_t DataOutPin, uint8_t ClockPin, uint8_t Direction, uint16_t BitsToShift, uint16_t DelayTime, const uint16_t *valueOut, uint16_t *valueIn, uint8_t ClockEdge) {
  if (Direction == LSBFIRST) {
    uint16_t ElementsRequired = BitsToShift;
    ElementsRequired /= 16;
    uint8_t BitRemainder = BitsToShift;
    BitRemainder %= 16;
    if (BitRemainder == 0) {
      ElementsRequired--;
    }
    for (int i = ElementsRequired; i >= 0; i--) {
      if (i == 0 && BitRemainder != 0) {
        valueIn[i] = transfer_Byte(DataInPin, DataOutPin, ClockPin, Direction, BitRemainder, DelayTime, valueOut[i], ClockEdge);
      }
      else {
        valueIn[i] = transfer_Byte(DataInPin, DataOutPin, ClockPin, Direction, 16, DelayTime, valueOut[i], ClockEdge);
      }
    }
  }
  else if (Direction == MSBFIRST) {
    uint16_t ElementsRequired = BitsToShift;
    ElementsRequired /= 16;
    uint8_t BitRemainder = BitsToShift;
    BitRemainder %= 16;
    if (BitRemainder != 0) {
      ElementsRequired++;
    }
    for (int i = 0; i < ElementsRequired; i++) {
      if (i == 0 && BitRemainder != 0) {
        valueIn[i] = transfer_Byte(DataInPin, DataOutPin, ClockPin, Direction, BitRemainder, DelayTime, valueOut[i], ClockEdge);
      }
      else {
        valueIn[i] = transfer_Byte(DataInPin, DataOutPin, ClockPin, Direction, 16, DelayTime, valueOut[i], ClockEdge);
      }
    }
  }
}

void ShiftXclass::transferEXTCLK_WordArray(bool SelectPinUsed, uint8_t SelectPin, uint8_t SelectPolarity, uint8_t DataInPin, uint8_t DataOutPin, uint8_t ClockPin, uint8_t ClockPolarity, uint8_t Direction, uint16_t BitsToShift, uint32_t ClockToggleTimeout, const uint16_t *valueOut, uint16_t *valueIn, uint8_t ClockEdge) {
  if (Direction == LSBFIRST) {
    uint16_t ElementsRequired = BitsToShift;
    ElementsRequired /= 16;
    uint8_t BitRemainder = BitsToShift;
    BitRemainder %= 16;
    if (BitRemainder == 0) {
      ElementsRequired--;
    }
    for (int i = ElementsRequired; i >= 0; i--) {
      if (i == 0 && BitRemainder != 0) {
        valueIn[i] = transferEXTCLK_Byte(SelectPinUsed, SelectPin, SelectPolarity, DataInPin, DataOutPin, ClockPin, ClockPolarity, Direction, BitRemainder, ClockToggleTimeout, valueOut[i], ClockEdge);
      }
      else {
        valueIn[i] = transferEXTCLK_Byte(SelectPinUsed, SelectPin, SelectPolarity, DataInPin, DataOutPin, ClockPin, ClockPolarity, Direction, 16, ClockToggleTimeout, valueOut[i], ClockEdge);
      }
    }
  }
  else if (Direction == MSBFIRST) {
    uint16_t ElementsRequired = BitsToShift;
    ElementsRequired /= 16;
    uint8_t BitRemainder = BitsToShift;
    BitRemainder %= 16;
    if (BitRemainder != 0) {
      ElementsRequired++;
    }
    for (int i = 0; i < ElementsRequired; i++) {
      if (i == 0 && BitRemainder != 0) {
        valueIn[i] = transferEXTCLK_Byte(SelectPinUsed, SelectPin, SelectPolarity, DataInPin, DataOutPin, ClockPin, ClockPolarity, Direction, BitRemainder, ClockToggleTimeout, valueOut[i], ClockEdge);
      }
      else {
        valueIn[i] = transferEXTCLK_Byte(SelectPinUsed, SelectPin, SelectPolarity, DataInPin, DataOutPin, ClockPin, ClockPolarity, Direction, 16, ClockToggleTimeout, valueOut[i], ClockEdge);
      }
    }
  }
}

uint32_t ShiftXclass::transfer_Dword(uint8_t DataInPin, uint8_t DataOutPin, uint8_t ClockPin, uint8_t Direction, uint8_t BitsToShift, uint16_t DelayTime, uint32_t value, uint8_t ClockEdge) {
  uint32_t DataToReturn = 0;
  uint8_t ClockPinIdleState = digitalRead(ClockPin);
  uint32_t mask = 1;
  if (Direction == LSBFIRST) {
    for (int i = 0; i < BitsToShift; i++) {
      if ((mask & value) == 0) {
        digitalWrite(DataOutPin, LOW);
      }
      else {
        digitalWrite(DataOutPin, HIGH);
      }
      digitalWrite(ClockPin, !ClockPinIdleState);
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      if ((ClockEdge == RISING && ClockPinIdleState == LOW && digitalRead(DataInPin) == HIGH) || (ClockEdge == FALLING && ClockPinIdleState == HIGH && digitalRead(DataInPin) == HIGH)) {
        DataToReturn |= mask;
      }
      digitalWrite(ClockPin, ClockPinIdleState);
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      if ((ClockEdge == RISING && ClockPinIdleState == HIGH && digitalRead(DataInPin) == HIGH) || (ClockEdge == FALLING && ClockPinIdleState == LOW && digitalRead(DataInPin) == HIGH)) {
        DataToReturn |= mask;
      }
      mask <<= 1;
    }
  }
  else if (Direction == MSBFIRST) {
    mask <<= (BitsToShift - 1);
    for (int i = 0; i < BitsToShift; i++) {
      if ((mask & value) == 0) {
        digitalWrite(DataOutPin, LOW);
      }
      else {
        digitalWrite(DataOutPin, HIGH);
      }
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      digitalWrite(ClockPin, !ClockPinIdleState);
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      if ((ClockEdge == RISING && ClockPinIdleState == LOW && digitalRead(DataInPin) == HIGH) || (ClockEdge == FALLING && ClockPinIdleState == HIGH && digitalRead(DataInPin) == HIGH)) {
        DataToReturn |= mask;
      }
      digitalWrite(ClockPin, ClockPinIdleState);
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      if ((ClockEdge == RISING && ClockPinIdleState == HIGH && digitalRead(DataInPin) == HIGH) || (ClockEdge == FALLING && ClockPinIdleState == LOW && digitalRead(DataInPin) == HIGH)) {
        DataToReturn |= mask;
      }
      if (mask == 0x80000000) { // deal with unwanted sign bit
        mask = 0x40000000;
      }
      else {
        mask >>= 1;
      }
    }
  }
  return DataToReturn;
}

uint32_t ShiftXclass::transferEXTCLK_Dword(bool SelectPinUsed, uint8_t SelectPin, uint8_t SelectPolarity, uint8_t DataInPin, uint8_t DataOutPin, uint8_t ClockPin, uint8_t ClockPolarity, uint8_t Direction, uint8_t BitsToShift, uint32_t ClockToggleTimeout, uint32_t valueOut, uint8_t ClockEdge) {
  uint32_t DataToReturn = 0;
  uint32_t mask = 1;
  if (Direction == LSBFIRST) {
    for (int i = 0; i < BitsToShift; i++) {
      if (WaitForPulse(SelectPinUsed, SelectPin, SelectPolarity, ClockPin, !ClockPolarity, ClockToggleTimeout) == false) {
        break;
      }
      if ((mask & valueOut) == 0) {
        digitalWrite(DataOutPin, LOW);
      }
      else {
        digitalWrite(DataOutPin, HIGH);
      }
      if ((ClockEdge == RISING && ClockPolarity == HIGH && digitalRead(DataInPin) == HIGH) || (ClockEdge == FALLING && ClockPolarity == LOW && digitalRead(DataInPin) == HIGH)) {
        DataToReturn |= mask;
      }
      mask <<= 1;
      if (WaitForPulse(SelectPinUsed, SelectPin, SelectPolarity, ClockPin, ClockPolarity, ClockToggleTimeout) == false) {
        break;
      }
      if ((ClockEdge == RISING && ClockPolarity == LOW && digitalRead(DataInPin) == HIGH) || (ClockEdge == FALLING && ClockPolarity == HIGH && digitalRead(DataInPin) == HIGH)) {
        DataToReturn |= mask;
      }
    }
  }
  else if (Direction == MSBFIRST) {
    mask <<= (BitsToShift - 1);
    for (int i = 0; i < BitsToShift; i++) {
      if (WaitForPulse(SelectPinUsed, SelectPin, SelectPolarity, ClockPin, !ClockPolarity, ClockToggleTimeout) == false) {
        break;
      }
      if ((mask & valueOut) == 0) {
        digitalWrite(DataOutPin, LOW);
      }
      else {
        digitalWrite(DataOutPin, HIGH);
      }
      if ((ClockEdge == RISING && ClockPolarity == 0 && digitalRead(DataInPin) == HIGH) || (ClockEdge == FALLING && ClockPolarity != 0 && digitalRead(DataInPin) == HIGH)) {
        DataToReturn |= mask;
      }
      if (mask == 0x80000000) { // deal with unwanted sign bit
        mask = 0x40000000;
      }
      else {
        mask >>= 1;
      }
      if (WaitForPulse(SelectPinUsed, SelectPin, SelectPolarity, ClockPin, ClockPolarity, ClockToggleTimeout) == false) {
        break;
      }
      if ((ClockEdge == RISING && ClockPolarity != 0 && digitalRead(DataInPin) == HIGH) || (ClockEdge == FALLING && ClockPolarity == 0 && digitalRead(DataInPin) == HIGH)) {
        DataToReturn |= mask;
      }
    }
  }
  return DataToReturn;
}

void ShiftXclass::transfer_DwordArray(uint8_t DataInPin, uint8_t DataOutPin, uint8_t ClockPin, uint8_t Direction, uint16_t BitsToShift, uint16_t DelayTime, const uint32_t *valueOut, uint32_t *valueIn, uint8_t ClockEdge) {
  if (Direction == LSBFIRST) {
    uint16_t ElementsRequired = BitsToShift;
    ElementsRequired /= 32;
    uint8_t BitRemainder = BitsToShift;
    BitRemainder %= 32;
    if (BitRemainder == 0) {
      ElementsRequired--;
    }
    for (int i = ElementsRequired; i >= 0; i--) {
      if (i == 0 && BitRemainder != 0) {
        valueIn[i] = transfer_Byte(DataInPin, DataOutPin, ClockPin, Direction, BitRemainder, DelayTime, valueOut[i], ClockEdge);
      }
      else {
        valueIn[i] = transfer_Byte(DataInPin, DataOutPin, ClockPin, Direction, 32, DelayTime, valueOut[i], ClockEdge);
      }
    }
  }
  else if (Direction == MSBFIRST) {
    uint16_t ElementsRequired = BitsToShift;
    ElementsRequired /= 32;
    uint8_t BitRemainder = BitsToShift;
    BitRemainder %= 32;
    if (BitRemainder != 0) {
      ElementsRequired++;
    }
    for (int i = 0; i < ElementsRequired; i++) {
      if (i == 0 && BitRemainder != 0) {
        valueIn[i] = transfer_Byte(DataInPin, DataOutPin, ClockPin, Direction, BitRemainder, DelayTime, valueOut[i], ClockEdge);
      }
      else {
        valueIn[i] = transfer_Byte(DataInPin, DataOutPin, ClockPin, Direction, 32, DelayTime, valueOut[i], ClockEdge);
      }
    }
  }
}

void ShiftXclass::transferEXTCLK_DwordArray(bool SelectPinUsed, uint8_t SelectPin, uint8_t SelectPolarity, uint8_t DataInPin, uint8_t DataOutPin, uint8_t ClockPin, uint8_t ClockPolarity, uint8_t Direction, uint16_t BitsToShift, uint32_t ClockToggleTimeout, const uint32_t *valueOut, uint32_t *valueIn, uint8_t ClockEdge) {
  if (Direction == LSBFIRST) {
    uint16_t ElementsRequired = BitsToShift;
    ElementsRequired /= 32;
    uint8_t BitRemainder = BitsToShift;
    BitRemainder %= 32;
    if (BitRemainder == 0) {
      ElementsRequired--;
    }
    for (int i = ElementsRequired; i >= 0; i--) {
      if (i == 0 && BitRemainder != 0) {
        valueIn[i] = transferEXTCLK_Byte(SelectPinUsed, SelectPin, SelectPolarity, DataInPin, DataOutPin, ClockPin, ClockPolarity, Direction, BitRemainder, ClockToggleTimeout, valueOut[i], ClockEdge);
      }
      else {
        valueIn[i] = transferEXTCLK_Byte(SelectPinUsed, SelectPin, SelectPolarity, DataInPin, DataOutPin, ClockPin, ClockPolarity, Direction, 32, ClockToggleTimeout, valueOut[i], ClockEdge);
      }
    }
  }
  else if (Direction == MSBFIRST) {
    uint16_t ElementsRequired = BitsToShift;
    ElementsRequired /= 32;
    uint8_t BitRemainder = BitsToShift;
    BitRemainder %= 32;
    if (BitRemainder != 0) {
      ElementsRequired++;
    }
    for (int i = 0; i < ElementsRequired; i++) {
      if (i == 0 && BitRemainder != 0) {
        valueIn[i] = transferEXTCLK_Byte(SelectPinUsed, SelectPin, SelectPolarity, DataInPin, DataOutPin, ClockPin, ClockPolarity, Direction, BitRemainder, ClockToggleTimeout, valueOut[i], ClockEdge);
      }
      else {
        valueIn[i] = transferEXTCLK_Byte(SelectPinUsed, SelectPin, SelectPolarity, DataInPin, DataOutPin, ClockPin, ClockPolarity, Direction, 32, ClockToggleTimeout, valueOut[i], ClockEdge);
      }
    }
  }
}

uint64_t ShiftXclass::transfer_Qword(uint8_t DataInPin, uint8_t DataOutPin, uint8_t ClockPin, uint8_t Direction, uint8_t BitsToShift, uint16_t DelayTime, uint64_t value, uint8_t ClockEdge) {
  uint64_t DataToReturn = 0;
  uint8_t ClockPinIdleState = digitalRead(ClockPin);
  uint64_t mask = 1;
  if (Direction == LSBFIRST) {
    for (int i = 0; i < BitsToShift; i++) {
      if ((mask & value) == 0) {
        digitalWrite(DataOutPin, LOW);
      }
      else {
        digitalWrite(DataOutPin, HIGH);
      }
      digitalWrite(ClockPin, !ClockPinIdleState);
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      if ((ClockEdge == RISING && ClockPinIdleState == LOW && digitalRead(DataInPin) == HIGH) || (ClockEdge == FALLING && ClockPinIdleState == HIGH && digitalRead(DataInPin) == HIGH)) {
        DataToReturn |= mask;
      }
      digitalWrite(ClockPin, ClockPinIdleState);
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      if ((ClockEdge == RISING && ClockPinIdleState == HIGH && digitalRead(DataInPin) == HIGH) || (ClockEdge == FALLING && ClockPinIdleState == LOW && digitalRead(DataInPin) == HIGH)) {
        DataToReturn |= mask;
      }
      mask <<= 1;
    }
  }
  else if (Direction == MSBFIRST) {
    mask <<= (BitsToShift - 1);
    for (int i = 0; i < BitsToShift; i++) {
      if ((mask & value) == 0) {
        digitalWrite(DataOutPin, LOW);
      }
      else {
        digitalWrite(DataOutPin, HIGH);
      }
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      digitalWrite(ClockPin, !ClockPinIdleState);
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      if ((ClockEdge == RISING && ClockPinIdleState == LOW && digitalRead(DataInPin) == HIGH) || (ClockEdge == FALLING && ClockPinIdleState == HIGH && digitalRead(DataInPin) == HIGH)) {
        DataToReturn |= mask;
      }
      digitalWrite(ClockPin, ClockPinIdleState);
      if (DelayTime > 0) {
        delayMicroseconds(DelayTime);
      }
      if ((ClockEdge == RISING && ClockPinIdleState == HIGH && digitalRead(DataInPin) == HIGH) || (ClockEdge == FALLING && ClockPinIdleState == LOW && digitalRead(DataInPin) == HIGH)) {
        DataToReturn |= mask;
      }
      if (mask == 9223372036854775808ULL) { // deal with unwanted sign bit
        mask = 4611686018427387904ULL;
      }
      else {
        mask >>= 1;
      }
    }
  }
  return DataToReturn;
}

uint64_t ShiftXclass::transferEXTCLK_Qword(bool SelectPinUsed, uint8_t SelectPin, uint8_t SelectPolarity, uint8_t DataInPin, uint8_t DataOutPin, uint8_t ClockPin, uint8_t ClockPolarity, uint8_t Direction, uint8_t BitsToShift, uint32_t ClockToggleTimeout, uint64_t valueOut, uint8_t ClockEdge) {
  uint64_t DataToReturn = 0;
  uint64_t mask = 1;
  if (Direction == LSBFIRST) {
    for (int i = 0; i < BitsToShift; i++) {
      if (WaitForPulse(SelectPinUsed, SelectPin, SelectPolarity, ClockPin, !ClockPolarity, ClockToggleTimeout) == false) {
        break;
      }
      if ((mask & valueOut) == 0) {
        digitalWrite(DataOutPin, LOW);
      }
      else {
        digitalWrite(DataOutPin, HIGH);
      }
      if ((ClockEdge == RISING && ClockPolarity == HIGH && digitalRead(DataInPin) == HIGH) || (ClockEdge == FALLING && ClockPolarity == LOW && digitalRead(DataInPin) == HIGH)) {
        DataToReturn |= mask;
      }
      mask <<= 1;
      if (WaitForPulse(SelectPinUsed, SelectPin, SelectPolarity, ClockPin, ClockPolarity, ClockToggleTimeout) == false) {
        break;
      }
      if ((ClockEdge == RISING && ClockPolarity == LOW && digitalRead(DataInPin) == HIGH) || (ClockEdge == FALLING && ClockPolarity == HIGH && digitalRead(DataInPin) == HIGH)) {
        DataToReturn |= mask;
      }
    }
  }
  else if (Direction == MSBFIRST) {
    mask <<= (BitsToShift - 1);
    for (int i = 0; i < BitsToShift; i++) {
      if (WaitForPulse(SelectPinUsed, SelectPin, SelectPolarity, ClockPin, !ClockPolarity, ClockToggleTimeout) == false) {
        break;
      }
      if ((mask & valueOut) == 0) {
        digitalWrite(DataOutPin, LOW);
      }
      else {
        digitalWrite(DataOutPin, HIGH);
      }
      if ((ClockEdge == RISING && ClockPolarity == 0 && digitalRead(DataInPin) == HIGH) || (ClockEdge == FALLING && ClockPolarity != 0 && digitalRead(DataInPin) == HIGH)) {
        DataToReturn |= mask;
      }
      if (mask == 9223372036854775808ULL) { // deal with unwanted sign bit
        mask = 4611686018427387904ULL;
      }
      else {
        mask >>= 1;
      }
      if (WaitForPulse(SelectPinUsed, SelectPin, SelectPolarity, ClockPin, ClockPolarity, ClockToggleTimeout) == false) {
        break;
      }
      if ((ClockEdge == RISING && ClockPolarity != 0 && digitalRead(DataInPin) == HIGH) || (ClockEdge == FALLING && ClockPolarity == 0 && digitalRead(DataInPin) == HIGH)) {
        DataToReturn |= mask;
      }
    }
  }
  return DataToReturn;
}

void ShiftXclass::transfer_QwordArray(uint8_t DataInPin, uint8_t DataOutPin, uint8_t ClockPin, uint8_t Direction, uint16_t BitsToShift, uint16_t DelayTime, const uint64_t *valueOut, uint64_t *valueIn, uint8_t ClockEdge) {
  if (Direction == LSBFIRST) {
    uint16_t ElementsRequired = BitsToShift;
    ElementsRequired /= 64;
    uint8_t BitRemainder = BitsToShift;
    BitRemainder %= 64;
    if (BitRemainder == 0) {
      ElementsRequired--;
    }
    for (int i = ElementsRequired; i >= 0; i--) {
      if (i == 0 && BitRemainder != 0) {
        valueIn[i] = transfer_Byte(DataInPin, DataOutPin, ClockPin, Direction, BitRemainder, DelayTime, valueOut[i], ClockEdge);
      }
      else {
        valueIn[i] = transfer_Byte(DataInPin, DataOutPin, ClockPin, Direction, 32, DelayTime, valueOut[i], ClockEdge);
      }
    }
  }
  else if (Direction == MSBFIRST) {
    uint16_t ElementsRequired = BitsToShift;
    ElementsRequired /= 64;
    uint8_t BitRemainder = BitsToShift;
    BitRemainder %= 64;
    if (BitRemainder != 0) {
      ElementsRequired++;
    }
    for (int i = 0; i < ElementsRequired; i++) {
      if (i == 0 && BitRemainder != 0) {
        valueIn[i] = transfer_Byte(DataInPin, DataOutPin, ClockPin, Direction, BitRemainder, DelayTime, valueOut[i], ClockEdge);
      }
      else {
        valueIn[i] = transfer_Byte(DataInPin, DataOutPin, ClockPin, Direction, 32, DelayTime, valueOut[i], ClockEdge);
      }
    }
  }
}

void ShiftXclass::transferEXTCLK_QwordArray(bool SelectPinUsed, uint8_t SelectPin, uint8_t SelectPolarity, uint8_t DataInPin, uint8_t DataOutPin, uint8_t ClockPin, uint8_t ClockPolarity, uint8_t Direction, uint16_t BitsToShift, uint32_t ClockToggleTimeout, const uint64_t *valueOut, uint64_t *valueIn, uint8_t ClockEdge) {
  if (Direction == LSBFIRST) {
    uint16_t ElementsRequired = BitsToShift;
    ElementsRequired /= 64;
    uint8_t BitRemainder = BitsToShift;
    BitRemainder %= 64;
    if (BitRemainder == 0) {
      ElementsRequired--;
    }
    for (int i = ElementsRequired; i >= 0; i--) {
      if (i == 0 && BitRemainder != 0) {
        valueIn[i] = transferEXTCLK_Byte(SelectPinUsed, SelectPin, SelectPolarity, DataInPin, DataOutPin, ClockPin, ClockPolarity, Direction, BitRemainder, ClockToggleTimeout, valueOut[i], ClockEdge);
      }
      else {
        valueIn[i] = transferEXTCLK_Byte(SelectPinUsed, SelectPin, SelectPolarity, DataInPin, DataOutPin, ClockPin, ClockPolarity, Direction, 32, ClockToggleTimeout, valueOut[i], ClockEdge);
      }
    }
  }
  else if (Direction == MSBFIRST) {
    uint16_t ElementsRequired = BitsToShift;
    ElementsRequired /= 64;
    uint8_t BitRemainder = BitsToShift;
    BitRemainder %= 64;
    if (BitRemainder != 0) {
      ElementsRequired++;
    }
    for (int i = 0; i < ElementsRequired; i++) {
      if (i == 0 && BitRemainder != 0) {
        valueIn[i] = transferEXTCLK_Byte(SelectPinUsed, SelectPin, SelectPolarity, DataInPin, DataOutPin, ClockPin, ClockPolarity, Direction, BitRemainder, ClockToggleTimeout, valueOut[i], ClockEdge);
      }
      else {
        valueIn[i] = transferEXTCLK_Byte(SelectPinUsed, SelectPin, SelectPolarity, DataInPin, DataOutPin, ClockPin, ClockPolarity, Direction, 32, ClockToggleTimeout, valueOut[i], ClockEdge);
      }
    }
  }
}