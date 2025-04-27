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

void xPES_Assembler::Init(int32_t PID) {
    m_PID = PID;
    m_BufferSize = 65536;  // Initial buffer size
    m_DataOffset = 0;
    m_LastContinuityCounter = -1;
    m_Started = false;
    m_PESHLength = 0;
}



xPES_Assembler::eResult xPES_Assembler::AbsorbPacket(
const uint8_t* TransportStreamPacket, const xTS_PacketHeader* PacketHeader, const xTS_AdaptationField* AdaptationField){

    if (PacketHeader->getPID() != m_PID) {
        return eResult::UnexpectedPID;
    }

    int start, payload;

    start = PacketHeader->hasAdaptationField() ? xTS::TS_HeaderLength + AdaptationField->getAdaptationFieldLength() + 1
    : xTS::TS_HeaderLength;//pozycja od ktorej zaczyna sie payload
    payload = xTS::TS_PacketLength - start;//dlugosc payloadu

        // Korekta dla specjalnego przypadku AdaptationFieldLength == 0
    if (PacketHeader->hasAdaptationField() && AdaptationField->getAdaptationFieldLength() == 0) {
        start --;
        payload ++;
    }

    if (m_Started){
        if (PacketHeader->getStartFlag()){
            m_Started = false;
            return eResult::AssemblingFinished;
        } else if (m_LastContinuityCounter != -1){
            if (PacketHeader->getCC() - m_LastContinuityCounter != 1){
                if (PacketHeader->getCC() == 0 && m_LastContinuityCounter == 15){
                    m_LastContinuityCounter = 0;
                } else {
                   return eResult::StreamPackedLost;
                }
            }
        }
    } else {
        if (PacketHeader->getStartFlag()){
            xBufferReset();
            Init(m_PID);
            m_PESH.Reset();
            m_PESHLength = m_PESH.Parse(&TransportStreamPacket[start]);
        }
        m_Started = true;
    }
    xBufferAppend(&TransportStreamPacket[start], payload);

    if (PacketHeader->getStartFlag()){
        return eResult::AssemblingStarted;
    }

    return eResult::AssemblingContinue;
}





void xPES_Assembler::xBufferReset() {
    if (m_Buffer != nullptr) {
        delete[] m_Buffer;
        m_Buffer = nullptr;
    }
    m_BufferSize = 65536;  // Przywróć standardowy rozmiar początkowy bufora
    m_DataOffset = 0;
    m_Buffer = new uint8_t[m_BufferSize];  // Zainicjalizuj nowy bufor o tym rozmiarze
}


void xPES_Assembler::xBufferAppend(const uint8_t *input, int32_t size) {
    //inicjalizuje w init buffer na 65536 wiec bedize trzeba potencjalnie go powiekszac
    //obecny offset czyli ile juz jest danych w buforze
    if (m_DataOffset + size > m_BufferSize){
        uint32_t newSize = m_BufferSize;
        while (newSize < m_DataOffset + size) {
            newSize *= 2; // podwajamy rozmiar
        }
        uint8_t* newBuffer = new uint8_t[newSize];
        memcpy(newBuffer, m_Buffer, m_DataOffset);//do nowo utworzono wiekszego bufora kopiujemy stare dane
        delete[] m_Buffer; //usuwamy stary bufor
        m_Buffer = newBuffer; //przypisujemy nowy bufor
        m_BufferSize = newSize; //aktualizujemy rozmiar bufora
    }
    //Kopiujemy nowe dane z inputa
    memcpy(m_Buffer + m_DataOffset, input, size);//+ooffset bo chcemy dodac tylko nowe dane na koniec buffora
    m_DataOffset += size; //aktualizujemy offset
}




    

