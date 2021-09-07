#ifndef ShiftX_h
#define ShiftX_h

#include <Arduino.h>

class ShiftXclass {
  public:

    void out_Byte(uint8_t DataPin, uint8_t ClockPin, uint8_t Direction, uint8_t BitsToShift, uint16_t DelayTime, uint8_t value);
    uint8_t in_Byte(uint8_t DataPin, uint8_t ClockPin, uint8_t Direction, uint8_t BitsToShift, uint16_t DelayTime);
    void outEXTCLK_Byte(bool SelectPinUsed, uint8_t SelectPin, uint8_t SelectPolarity, uint8_t DataPin, uint8_t ClockPin, uint8_t ClockPolarity, uint8_t Direction, uint8_t BitsToShift, uint32_t ClockToggleTimeout, uint8_t value);
    uint8_t inEXTCLK_Byte(bool SelectPinUsed, uint8_t SelectPin, uint8_t SelectPolarity, uint8_t DataPin, uint8_t ClockPin, uint8_t ClockPolarity, uint8_t Direction, uint8_t BitsToShift, uint32_t ClockToggleTimeout);

    void out_ByteArray(uint8_t DataPin, uint8_t ClockPin, uint8_t Direction, uint16_t BitsToShift, uint16_t DelayTime, const uint8_t *value);
    void in_ByteArray(uint8_t DataPin, uint8_t ClockPin, uint8_t Direction, uint16_t BitsToShift, uint16_t DelayTime, uint8_t *value);
    void outEXTCLK_ByteArray(bool SelectPinUsed, uint8_t SelectPin, uint8_t SelectPolarity, uint8_t DataPin, uint8_t ClockPin, uint8_t ClockPolarity, uint8_t Direction, uint16_t BitsToShift, uint32_t ClockToggleTimeout, const uint8_t *value);
    void inEXTCLK_ByteArray(bool SelectPinUsed, uint8_t SelectPin, uint8_t SelectPolarity, uint8_t DataPin, uint8_t ClockPin, uint8_t ClockPolarity, uint8_t Direction, uint16_t BitsToShift, uint32_t ClockToggleTimeout, uint8_t *value);

    void out_Word(uint8_t DataPin, uint8_t ClockPin, uint8_t Direction, uint8_t BitsToShift, uint16_t DelayTime, uint16_t value);
    uint16_t in_Word(uint8_t DataPin, uint8_t ClockPin, uint8_t Direction, uint8_t BitsToShift, uint16_t DelayTime);
    void outEXTCLK_Word(bool SelectPinUsed, uint8_t SelectPin, uint8_t SelectPolarity, uint8_t DataPin, uint8_t ClockPin, uint8_t ClockPolarity, uint8_t Direction, uint8_t BitsToShift, uint32_t ClockToggleTimeout, uint16_t value);
    uint16_t inEXTCLK_Word(bool SelectPinUsed, uint8_t SelectPin, uint8_t SelectPolarity, uint8_t DataPin, uint8_t ClockPin, uint8_t ClockPolarity, uint8_t Direction, uint8_t BitsToShift, uint32_t ClockToggleTimeout);

    void out_WordArray(uint8_t DataPin, uint8_t ClockPin, uint8_t Direction, uint16_t BitsToShift, uint16_t DelayTime, const uint16_t *value);
    void in_WordArray(uint8_t DataPin, uint8_t ClockPin, uint8_t Direction, uint16_t BitsToShift, uint16_t DelayTime, uint16_t *value);
    void outEXTCLK_WordArray(bool SelectPinUsed, uint8_t SelectPin, uint8_t SelectPolarity, uint8_t DataPin, uint8_t ClockPin, uint8_t ClockPolarity, uint8_t Direction, uint16_t BitsToShift, uint32_t ClockToggleTimeout, const uint16_t *value);
    void inEXTCLK_WordArray(bool SelectPinUsed, uint8_t SelectPin, uint8_t SelectPolarity, uint8_t DataPin, uint8_t ClockPin, uint8_t ClockPolarity, uint8_t Direction, uint16_t BitsToShift, uint32_t ClockToggleTimeout, uint16_t *value);

    void out_Dword(uint8_t DataPin, uint8_t ClockPin, uint8_t Direction, uint8_t BitsToShift, uint16_t DelayTime, uint32_t value);
    uint32_t in_Dword(uint8_t DataPin, uint8_t ClockPin, uint8_t Direction, uint8_t BitsToShift, uint16_t DelayTime);
    void outEXTCLK_Dword(bool SelectPinUsed, uint8_t SelectPin, uint8_t SelectPolarity, uint8_t DataPin, uint8_t ClockPin, uint8_t ClockPolarity, uint8_t Direction, uint8_t BitsToShift, uint32_t ClockToggleTimeout, uint32_t value);
    uint32_t inEXTCLK_Dword(bool SelectPinUsed, uint8_t SelectPin, uint8_t SelectPolarity, uint8_t DataPin, uint8_t ClockPin, uint8_t ClockPolarity, uint8_t Direction, uint8_t BitsToShift, uint32_t ClockToggleTimeout);

    void out_DwordArray(uint8_t DataPin, uint8_t ClockPin, uint8_t Direction, uint16_t BitsToShift, uint16_t DelayTime, const uint32_t *value);
    void in_DwordArray(uint8_t DataPin, uint8_t ClockPin, uint8_t Direction, uint16_t BitsToShift, uint16_t DelayTime, uint32_t *value);
    void outEXTCLK_DwordArray(bool SelectPinUsed, uint8_t SelectPin, uint8_t SelectPolarity, uint8_t DataPin, uint8_t ClockPin, uint8_t ClockPolarity, uint8_t Direction, uint16_t BitsToShift, uint32_t ClockToggleTimeout, const uint32_t *value);
    void inEXTCLK_DwordArray(bool SelectPinUsed, uint8_t SelectPin, uint8_t SelectPolarity, uint8_t DataPin, uint8_t ClockPin, uint8_t ClockPolarity, uint8_t Direction, uint16_t BitsToShift, uint32_t ClockToggleTimeout, uint32_t *value);

    void out_Qword(uint8_t DataPin, uint8_t ClockPin, uint8_t Direction, uint8_t BitsToShift, uint16_t DelayTime, uint64_t value);
    uint64_t in_Qword(uint8_t DataPin, uint8_t ClockPin, uint8_t Direction, uint8_t BitsToShift, uint16_t DelayTime);
    void outEXTCLK_Qword(bool SelectPinUsed, uint8_t SelectPin, uint8_t SelectPolarity, uint8_t DataPin, uint8_t ClockPin, uint8_t ClockPolarity, uint8_t Direction, uint8_t BitsToShift, uint32_t ClockToggleTimeout, uint64_t value);
    uint64_t inEXTCLK_Qword(bool SelectPinUsed, uint8_t SelectPin, uint8_t SelectPolarity, uint8_t DataPin, uint8_t ClockPin, uint8_t ClockPolarity, uint8_t Direction, uint8_t BitsToShift, uint32_t ClockToggleTimeout);

    void out_QwordArray(uint8_t DataPin, uint8_t ClockPin, uint8_t Direction, uint16_t BitsToShift, uint16_t DelayTime, const uint64_t *value);
    void in_QwordArray(uint8_t DataPin, uint8_t ClockPin, uint8_t Direction, uint16_t BitsToShift, uint16_t DelayTime, uint64_t *value);
    void outEXTCLK_QwordArray(bool SelectPinUsed, uint8_t SelectPin, uint8_t SelectPolarity, uint8_t DataPin, uint8_t ClockPin, uint8_t ClockPolarity, uint8_t Direction, uint16_t BitsToShift, uint32_t ClockToggleTimeout, const uint64_t *value);
    void inEXTCLK_QwordArray(bool SelectPinUsed, uint8_t SelectPin, uint8_t SelectPolarity, uint8_t DataPin, uint8_t ClockPin, uint8_t ClockPolarity, uint8_t Direction, uint16_t BitsToShift, uint32_t ClockToggleTimeout, uint64_t *value);

    bool Timeout = false;

    private:
    bool WaitForPulse(bool SelectPinUsed, uint8_t SelectPin, uint8_t SelectPolarity, uint8_t ClockPin, uint8_t ClockPolarity, uint32_t ClockToggleTimeout);

};

extern ShiftXclass ShiftX;

#endif