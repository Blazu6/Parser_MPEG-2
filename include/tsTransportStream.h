#pragma once
#include "tsCommon.h"
#include <string>
#include <iostream>
#include <cstring>

class xTS
{
public:
  static constexpr uint32_t TS_PacketLength  = 188;
  static constexpr uint32_t TS_HeaderLength  = 4;

  static constexpr uint32_t PES_HeaderLength = 6;

  static constexpr uint32_t BaseClockFrequency_Hz         =    90000; //Hz
  static constexpr uint32_t ExtendedClockFrequency_Hz     = 27000000; //Hz
  static constexpr uint32_t BaseClockFrequency_kHz        =       90; //kHz
  static constexpr uint32_t ExtendedClockFrequency_kHz    =    27000; //kHz
  static constexpr uint32_t BaseToExtendedClockMultiplier =      300;
};
