#include "tsCommon.h"

class mpegHeader
{
    protected:
    
        uint16_t m_frameSync;
        uint8_t m_version;
        uint8_t m_layer;
        uint8_t m_protectionBit;
        uint8_t m_bitrateIndex;
        uint8_t m_samplingFrequencyIndex;
        uint8_t m_paddingBit;
        uint8_t m_privateBit;
        uint8_t m_channelMode;
        uint8_t m_modeExtension;
        uint8_t m_copyRight;
        uint8_t m_originalBit;
        uint8_t m_emphasis;


        // obliczone wartości
        int32_t  m_bitrate;        // kbps
        uint32_t m_samplingRate;   // Hz
        uint32_t m_frameSize;      // bytes
        uint8_t  m_headerLength;    // długość nagłówka MPEG (4 lub 6 bajtów)
        bool     m_valid;
        

    public:
        void Reset();
        int32_t Parse(const uint8_t* PacketBuffer);
        void Print() const;
};