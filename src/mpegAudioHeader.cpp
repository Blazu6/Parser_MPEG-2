#include "mpegAudioHeader.h"
#include <iostream>

// tabele ISO/IEC 11172-3
static const int BITRATE_TABLE[2][3][16] = {
    {   // MPEG-2 & 2.5
      {0,  8,  16, 24, 32, 40, 48, 56,  64,  80,  96,112,128,144,160,0},  // Layer III
      {0, 32,  48, 56, 64, 80, 96,112, 128, 144, 160,176,192,224,256,0},  // Layer II
      {0, 32,  64, 96,128,160,192,224, 256, 288, 320,352,384,416,448,0}   // Layer I
    },
    {   // MPEG-1
      {0, 32,  40, 48, 56, 64, 80, 96, 112, 128, 160,192,224,256,320,0},
      {0, 32,  48, 56, 64, 80, 96,112, 128, 160, 192,224,256,320,384,0},
      {0, 32,  64, 96,128,160,192,224, 256, 320, 384,448,512,576,640,0}
    }
};

static const int SAMPLERATE_TABLE[4][3] = {
    {11025, 12000,  8000},   // versionBits = 0 → MPEG-2.5
    {    0,     0,     0},   // versionBits = 1 → reserved
    {22050, 24000, 16000},   // versionBits = 2 → MPEG-2
    {44100, 48000, 32000}    // versionBits = 3 → MPEG-1
};

void mpegHeader::Reset()
{
    m_frameSync               = 0;
    m_version                 = 0;
    m_layer                   = 0;
    m_protectionBit           = 0;
    m_bitrateIndex            = 0;
    m_samplingFrequencyIndex  = 0;
    m_paddingBit              = 0;
    m_privateBit              = 0;
    m_channelMode             = 0;
    m_modeExtension           = 0;
    m_copyRight               = 0;
    m_originalBit             = 0;
    m_emphasis                = 0;

    m_bitrate                 = 0;
    m_samplingRate            = 0;
    m_frameSize               = 0;
    m_headerLength            = 0;
    m_valid                   = false;
}

int32_t mpegHeader::Parse(const uint8_t* PacketBuffer)
{

    if (!PacketBuffer) {
        return -1;
    }

    // 1) podstawowy nagłówek = 4 bajty
    uint8_t hdrLen = 4;

    // 2) sync 11 bitów
    uint16_t sync = (uint16_t(PacketBuffer[0]) << 3)
                  | ((PacketBuffer[1] & 0xE0) >> 5);
    if (sync != 0x7FF) {
        return -1;
    }
    m_frameSync = sync;

    // 3) wersja i warstwa
    m_version = (PacketBuffer[1] & 0x18) >> 3;
    if (m_version == 1) return -1;      // reserved

    m_layer   = (PacketBuffer[1] & 0x06) >> 1;
    if (m_layer == 0)   return -1;      // reserved

    // 4) ochrona CRC
    m_protectionBit = PacketBuffer[1] & 0x01;
    if (m_protectionBit == 0) {
        // CRC jest obecne zaraz po 4 bajtach headra
        hdrLen += 2;
    }

    // 5) bitrate i samplerate
    m_bitrateIndex           = (PacketBuffer[2] & 0xF0) >> 4;
    if (m_bitrateIndex == 0 || m_bitrateIndex == 15) return -1;

    m_samplingFrequencyIndex = (PacketBuffer[2] & 0x0C) >> 2;
    if (m_samplingFrequencyIndex == 3) return -1;

    m_paddingBit             = (PacketBuffer[2] & 0x02) >> 1;
    m_privateBit             = (PacketBuffer[2] & 0x01);

    m_channelMode            = (PacketBuffer[3] & 0xC0) >> 6;
    m_modeExtension          = (PacketBuffer[3] & 0x30) >> 4;
    m_copyRight              = (PacketBuffer[3] & 0x08) >> 3;
    m_originalBit            = (PacketBuffer[3] & 0x04) >> 2;
    m_emphasis               =  PacketBuffer[3] & 0x03;

    // 6) obliczenia bitrate i samplerate
    int vt = (m_version == 3 ? 1 : 0);
    m_bitrate      = BITRATE_TABLE[vt][m_layer - 1][m_bitrateIndex];
    m_samplingRate = SAMPLERATE_TABLE[m_version][m_samplingFrequencyIndex];

    // 8) zapamiętaj długość nagłówka
    m_headerLength = hdrLen;
    m_valid = true;
    return m_headerLength;
}

void mpegHeader::Print() const
{
    std::cout << std::endl;
    if (!m_valid) {
        std::cout << "[Invalid MPEG header]\n";
        return;
    }

    static const char* verStr[] = {
        "MPEG-2.5", "reserved", "MPEG-2", "MPEG-1"
    };
    static const char* layerStr[] = {
        "reserved", "Layer III", "Layer II", "Layer I"
    };
    static const char* chanStr[] = {
        "stereo", "joint stereo", "dual", "mono"
    };

    std::cout
        << "MPEG Audio Header:\n"
        << "  Sync: 0x" << std::hex << m_frameSync << std::dec << "\n"
        << "  Version: "  << verStr[m_version]  << " (" << int(m_version)  << ")\n"
        << "  Layer: "    << layerStr[m_layer]  << " (" << int(m_layer)    << ")\n"
        << "  Protection: " << (m_protectionBit==0 ? "CRC" : "no CRC")
                                        << " (bit="<<int(m_protectionBit)<<")\n"
        << "  Bitrate: idx="<<int(m_bitrateIndex)
                   <<" ("<<m_bitrate<<" kbps)\n"
        << "  Sample rate: idx="<<int(m_samplingFrequencyIndex)
                   <<" ("<<m_samplingRate<<" Hz)\n"
        << "  Padding: " << int(m_paddingBit)
        << "  Private: " << int(m_privateBit) << "\n"
        << "  Channel mode: " << chanStr[m_channelMode]
                   << " (modeExt="<<int(m_modeExtension)<<")\n"
        << "  CopyRight: "    << int(m_copyRight)
        << "  Original: "    << int(m_originalBit)
        << "  Emphasis: "    << int(m_emphasis) << "\n"
        << "  Header length: " << int(m_headerLength) << " bytes\n";
}
