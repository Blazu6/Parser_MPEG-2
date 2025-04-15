#pragma once
#include "tsTransportStream.h"

class xTS_AdaptationField : public xTS
{
    protected:
        //setup
        uint8_t m_AdaptationFieldControl;
        //mandatory fields
        uint8_t m_AdaptationFieldLength;
        uint8_t m_DC; 
        uint8_t m_RA;
        uint8_t m_SP;
        uint8_t m_PR;
        uint8_t m_OR;
        uint8_t m_SF;
        uint8_t m_TP;
        uint8_t m_EX;
        //optional fields - PCR, OPCR, Stuffing bytes
        uint64_t m_PCR;
        uint64_t m_PCR_base;
        uint16_t m_PCR_extension;

        uint64_t m_OPCR;
        uint64_t m_OPCR_base;
        uint16_t m_OPCR_extension;

        uint8_t m_StuffingBytes;

    public:
        void Reset();
        int32_t Parse(const uint8_t* PacketBuffer, uint8_t AdaptationFieldControl);
        void Print() const;

    public:
        //mandatory fields
        uint8_t getAdaptationFieldLength () const { return m_AdaptationFieldLength ; }
        //derived values
        uint32_t getNumBytes () const { return 1 + m_AdaptationFieldLength; }
};