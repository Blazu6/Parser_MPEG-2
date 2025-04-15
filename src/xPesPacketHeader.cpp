#include "xPesPacketHeader.h"


void xPES_PacketHeader::Reset()
{
  m_PacketStartCodePrefix = 0;
  m_StreamId = 0;
  m_PacketLength = 0;
}

int32_t xPES_PacketHeader::Parse(const uint8_t* Input)
{
  if (Input == nullptr) 
  {
    return -1;
  }

  m_PacketStartCodePrefix = (Input[0] << 16) | (Input[1] << 8) | Input[2];

  if (m_PacketStartCodePrefix != 0x000001)
  {
    std::cerr << "Invalid PES packet start code prefix!" << std::endl;
    return -1;
  }

  m_StreamId = Input[3];
  m_PacketLength = (Input[4] << 8) | Input[5];

  // Domyślnie zakładamy, że PES ma rozszerzony nagłówek
  m_HasExtendedHeader = true;

  switch (m_StreamId)
  {
      case eStreamId_program_stream_map:
      case eStreamId_padding_stream:
      case eStreamId_private_stream_2:
      case eStreamId_ECM:
      case eStreamId_EMM:
      case eStreamId_program_stream_directory:
      case eStreamId_DSMCC_stream:
      case eStreamId_ITUT_H222_1_type_E:
          m_HasExtendedHeader = false;
          break;
  }

  int headerLength = 6; // 6 bajtów nagłówka PES

  if (m_HasExtendedHeader)
  {
    headerLength += 3; //+3 bajty bo te flagi
    PTS_DTS_flags = (Input[7] & 0xC0) >> 6; // PTS/DTS flags
    if (PTS_DTS_flags == 2)//Tylko PTS
    {
      m_PTS = (((uint64_t)Input[9]  & 0x0E) << 29) | //32..30 4 bity pierwsze pomijamy wyciagamy kolejne 3 i ostatni to marker
                    (((uint64_t)Input[10]) << 22) | //29..22
                    ((((uint64_t)Input[11] & 0xFE)) << 14) | //21..14
                    (((uint64_t)Input[12]) << 7) | //13..7
                    (((uint64_t)Input[13]) >> 1); //6..0
      headerLength += 5; // +5 bajtów bo 4 bajty PTS + 1 bajt rezerwowy
    }
    else if (PTS_DTS_flags == 3)//DTS I PTS
    {
      m_PTS = (((uint64_t)Input[9]  & 0x0E) << 29) |
                    (((uint64_t)Input[10]) << 22) |
                    ((((uint64_t)Input[11] & 0xFE)) << 14) |
                    (((uint64_t)Input[12]) << 7) |
                    (((uint64_t)Input[13]) >> 1);
      m_DTS = (((uint64_t)Input[14] & 0x0E) << 29) |
                    (((uint64_t)Input[15]) << 22) |
                    ((((uint64_t)Input[16] & 0xFE)) << 14) |
                    (((uint64_t)Input[17]) << 7) |
                    (((uint64_t)Input[18]) >> 1);
      
      headerLength += 10; // +10 bajtów bo 8 bajtów PTS i DTS + 2 bajty rezerwowe
    }
  }
  return headerLength;
}

void xPES_PacketHeader::Print() const
{
  printf(" PES:PSCP=%1d SID=%3d L=%4d",
         m_PacketStartCodePrefix, m_StreamId, m_PacketLength);

  if (m_PTS != 0) 
  {
    double timeInSeconds = (double)m_PTS / (double)xTS::BaseClockFrequency_Hz;
    printf(" PTS=%" PRIu64 " (Time=%.6f s)", m_PTS, timeInSeconds);
  }
    
  if (m_DTS != 0) 
  {
      printf(" DTS=%" PRIu64, (unsigned long long)m_DTS);
  }
}