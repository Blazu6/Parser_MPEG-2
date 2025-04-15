#include "tsPacketHeader.h"

void xTS_PacketHeader::Reset()
{
  m_SB = 0;  
  m_E = 0;   
  m_S = 0;   
  m_T = 0;   
  m_PID = 0; 
  m_TSC = 0; 
  m_AFC = 0; 
  m_CC = 0;  
}

int32_t xTS_PacketHeader::Parse(const uint8_t* Input)
{
  if (Input == nullptr) 
  {
    return -1;
  }

  m_SB = Input[0];

  if (m_SB != 0x47) 
  { 
    return -1;
  }

  m_E = (Input[1] & 0x80) >> 7;
  m_S = (Input[1] & 0x40) >> 6;
  m_T = (Input[1] & 0x20) >> 5;
  m_PID = ((Input[1] & 0x1F) << 8) | Input[2]; 
  m_TSC = (Input[3] & 0xC0) >> 6;
  m_AFC = (Input[3] & 0x30) >> 4;
  m_CC = Input[3] & 0x0F;
  return 4;
}

void xTS_PacketHeader::Print() const
{
  printf("SB=%2d E=%d S=%d P=%d PID=%4d TSC=%d AF=%d CC=%2d",
         m_SB, m_E, m_S, m_T, m_PID, m_TSC, m_AFC, m_CC);
}