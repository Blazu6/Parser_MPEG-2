#pragma once
#include "tsTransportStream.h"

class xTS_PacketHeader : public xTS
{
public:
  enum class ePID : uint16_t
  {
    PAT  = 0x0000,
    CAT  = 0x0001,
    TSDT = 0x0002,
    IPMT = 0x0003,
    NIT  = 0x0010,
    SDT  = 0x0011,
    NuLL = 0x1FFF,
  };

protected:
  uint8_t  m_SB; //Sync Byte
  uint8_t  m_E;  //Error flag
  uint8_t  m_S;  //Start flag
  uint8_t  m_T;  //Priority flag
  uint16_t m_PID;
  uint8_t  m_TSC;
  uint8_t  m_AFC;
  uint8_t  m_CC;

public:
  void     Reset();
  int32_t  Parse(const uint8_t* Input);
  void     Print() const;

public:
  uint8_t  getSyncByte() const { return m_SB; } 
  uint8_t  getErrorFlag() const { return m_E; }
  uint8_t  getStartFlag() const { return m_S; }
  uint8_t  getPriorityFlag() const { return m_T; }
  uint16_t getPID() const {return m_PID; }
  uint8_t getTSC() const { return m_TSC; }
  uint8_t getAFC() const { return m_AFC; }
  uint8_t getCC() const { return m_CC; } 

public:
  bool     hasAdaptationField() const { return (m_AFC == 2 || m_AFC == 3); }
  bool     hasPayload        () const { return (m_AFC == 1 || m_AFC == 3); }
};