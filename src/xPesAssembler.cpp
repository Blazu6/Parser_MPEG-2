#include "xPesAssembler.h"



// Constructor: initialize variables and buffer pointer
xPES_Assembler::xPES_Assembler()
    : m_PID(-1),
      m_Buffer(nullptr),
      m_BufferSize(0),
      m_DataOffset(0),
      m_LastContinuityCounter(-1),
      m_Started(false)
{
    // Optionally, allocate initial memory or leave it for Init()
}

// Destructor: free allocated memory
xPES_Assembler::~xPES_Assembler() {
    if (m_Buffer) {
        delete[] m_Buffer;
        m_Buffer = nullptr;
    }
}



xPES_Assembler::eResult xPES_Assembler::AbsorbPacket(const uint8_t* TransportStreamPacket, const xTS_PacketHeader* PacketHeader, const xTS_AdaptationField* AdaptationField)
{

    if (PacketHeader->getPID() != m_PID) {
        return eResult::UnexpectedPID;
    }
    
    switch (m_PID) 
    {
        case 136:
            file_extension = ".mp2";
            break;

        case 174:
            file_extension = ".264";
            break;
    }

    file_name = std::to_string(m_PID) + file_extension;

    uint8_t curentCC = PacketHeader->getCC();
    if (m_LastContinuityCounter >= 0 && (m_LastContinuityCounter + 1) % 16 != curentCC) {
        return eResult::StreamPackedLost;
    }
    m_LastContinuityCounter = curentCC;

    int payloadStart = 4; // TS nagłówek
    if (PacketHeader->hasAdaptationField()) {
        payloadStart += AdaptationField->getAdaptationFieldLength() + 1;
    }

    const uint8_t* payload = TransportStreamPacket + payloadStart;
    int payloadSize = 188 - payloadStart;

    if (PacketHeader->getStartFlag() == 1) {  // flaga startu – początek PES
        m_PESH.Reset();
        m_PESHLength = m_PESH.Parse(payload);
        xBufferReset();
        // Ustawiamy bufor na długość z pola PES_packet_length plus 6 bajtów nagłówka
        m_BufferSize = m_PESH.getPacketLength() + 6;
        if (m_BufferSize > 0) {
            m_Buffer = new uint8_t[m_BufferSize];
        }
        // Kopiujemy cały payload, włączając nagłówek PES (6 bajtów) – nie odejmujemy 6
        xBufferAppend(payload, payloadSize);
        return eResult::AssemblingStarted;
    }
    else {
        xBufferAppend(payload, payloadSize);
        if (m_BufferSize == m_DataOffset) {
          FILE* file = fopen(file_name.c_str(), "ab");  // Otwieramy plik w trybie binarnym (append)
          if (file != nullptr) {
              fwrite(m_Buffer, sizeof(uint8_t), m_DataOffset, file);  // Zapisujemy dane
              fclose(file);  // Zamykamy plik
          }
          return eResult::AssemblingFinished;
        }
        return eResult::AssemblingContinue;
    }
}





void xPES_Assembler::xBufferReset()
{
  if (m_Buffer != nullptr) {
      delete[] m_Buffer;
      m_Buffer = nullptr;
  }
  m_BufferSize = 0;
  m_DataOffset = 0;
}

void xPES_Assembler::xBufferAppend(const uint8_t *input, int32_t size) 
{
   if (m_DataOffset + size > m_BufferSize) {
        std::cerr << "Error: Buffer overflow!" << std::endl;
        return;
    }
    memcpy(m_Buffer + m_DataOffset, input, size); // Kopiowanie danych do bufora 
    // poczatek miejsca gdzie kopiowac, skad kopiowac, dlugosc do skopiowania
    m_DataOffset += size; // Aktualizacja offsetu
}



    

