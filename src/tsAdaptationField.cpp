#include "tsAdaptationField.h"

void xTS_AdaptationField::Reset()
{
    m_AdaptationFieldControl = 0;
    m_AdaptationFieldLength = 0;
    m_DC = 0; 
    m_RA = 0;
    m_SP = 0;
    m_PR = 0;
    m_OR = 0;
    m_SF = 0;
    m_TP = 0;
    m_EX = 0;
    m_PCR = 0;
    m_OPCR = 0;
    m_StuffingBytes = 0;
}

int32_t xTS_AdaptationField::Parse(const uint8_t* PacketBuffer, uint8_t AdaptationFieldControl)
{
    if (PacketBuffer == nullptr) 
    {
        std::cerr << "Error: Adaptation field buffer is null!" << std::endl;
        return -1;
    }

    if (AdaptationFieldControl != 2 && AdaptationFieldControl != 3) 
    {
        return -1; 
    }

    m_AdaptationFieldLength = PacketBuffer[4];
    m_AdaptationFieldControl = AdaptationFieldControl;

    uint8_t index = 5;  // Pozycja bajtu flag
    
    m_DC = (PacketBuffer[index] & 0x80) >> 7;
    m_RA = (PacketBuffer[index] & 0x40) >> 6;
    m_SP = (PacketBuffer[index] & 0x20) >> 5;
    m_PR = (PacketBuffer[index] & 0x10) >> 4;
    m_OR = (PacketBuffer[index] & 0x08) >> 3;
    m_SF = (PacketBuffer[index] & 0x04) >> 2;
    m_TP = (PacketBuffer[index] & 0x02) >> 1;
    m_EX = (PacketBuffer[index] & 0x01);
    
    index++;

    if (m_PR) 
    {
        m_PCR_base = (PacketBuffer[index] << 25) |  
                     (PacketBuffer[index + 1] << 17) |
                     (PacketBuffer[index + 2] << 9)  | 
                     (PacketBuffer[index + 3] << 1)  | 
                     ((PacketBuffer[index + 4] & 0x80) >> 7); 
        m_PCR_extension = ((PacketBuffer[index + 4] & 0x01) << 8) | PacketBuffer[index + 5];
        m_PCR = (m_PCR_base * 300) + m_PCR_extension;
        index += 6; // Przesuwamy indeks o 6 bajtów (5 bajtów PCR + 1 bajt rezerwowy)
    } else 
    {
        m_PCR = 0;
    }

    if (m_OR) 
    { 
      m_OPCR_base = (PacketBuffer[index] << 25) | 
                    (PacketBuffer[index + 1] << 17) | 
                    (PacketBuffer[index + 2] << 9)  | 
                    (PacketBuffer[index + 3] << 1)  | 
                    ((PacketBuffer[index + 4] & 0x80) >> 7); 
      m_OPCR_extension = ((PacketBuffer[index + 4] & 0x01) << 8) | PacketBuffer[index + 5];
      m_OPCR = (m_OPCR_base * 300) + m_OPCR_extension; 
      index += 6; // Przesuwamy indeks o 6 bajtów (5 bajtów OPCR + 1 bajt rezerwowy)
    } else 
    {
      m_OPCR = 0;
    }

    m_StuffingBytes = m_AdaptationFieldLength - (index - 5);

    if (m_StuffingBytes < 0) 
    {
        m_StuffingBytes = 0;
    }

    return m_AdaptationFieldLength + 1;
}

void xTS_AdaptationField::Print() const
{
    printf("AF:L=%2d DC=%d RA=%d SP=%d PR=%d OR=%d SF=%d TP=%d EX=%d",
           m_AdaptationFieldLength, m_DC, m_RA, m_SP, m_PR, m_OR, m_SF, m_TP, m_EX);

    if (m_PR) 
    {
        printf(" PCR=%lu", m_PCR);
        printf(" (Time=%.6f s)", static_cast<double>(m_PCR) / ExtendedClockFrequency_Hz);
    }

    if (m_OR) 
    {
        printf(" OPCR=%lu", m_OPCR);
        printf(" (Time=%.6f s)", static_cast<double>(m_OPCR) / ExtendedClockFrequency_Hz);
    }

    printf(" StuffingBytes=%d", m_StuffingBytes);
}
