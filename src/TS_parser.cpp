#include "tsCommon.h"
#include "tsTransportStream.h"
#include "tsPacketHeader.h"
#include "tsAdaptationField.h"
#include "xPesPacketHeader.h"
#include "xPesAssembler.h"
#include <iostream>
#include <fstream>

//=============================================================================================================================================================================

int main(int argc, char *argv[], char *envp[])
{
  if (argc < 2) 
  {
    std::cerr << "Usage: " << argv[0] << " <input file>" << std::endl;
    return EXIT_FAILURE;
  }

  FILE *inputFile = fopen(argv[1], "rb");
  if (!inputFile) 
  {
    std::cerr << "Error: Could not open file " << argv[1] << std::endl;
    return EXIT_FAILURE;
  }

  FILE* outputMP2  = fopen("output.mp2", "wb");
  FILE* output264 = fopen("output.264", "wb");
  if (!outputMP2 || !output264) 
  {
      std::cerr << "Error: Could not open one of the output files" << std::endl;
      return EXIT_FAILURE;
  }

  xPES_Assembler assemblerMP2, assembler264;
  assemblerMP2.Init(136);
  assembler264.Init(174);

  xTS_PacketHeader    TS_PacketHeader;
  xTS_AdaptationField TS_AdaptationField;

  int32_t TS_PacketId = 0;
  uint8_t TS_PacketBuffer[xTS::TS_PacketLength];
  size_t bytesRead;
  long prevPos = 0;

  while (!feof(inputFile) && TS_PacketId <500000) {
    prevPos = ftell(inputFile);
    bytesRead = fread(TS_PacketBuffer, 1, xTS::TS_PacketLength, inputFile);
    if (bytesRead != xTS::TS_PacketLength) {
      if (feof(inputFile)) {
        break; // Koniec pliku
      } else {
        std::cerr << "Error: Could not read a full TS packet" << std::endl;
        fclose(inputFile);
        return EXIT_FAILURE;
      }
    }

    TS_PacketHeader.Reset();
    if (TS_PacketHeader.Parse(TS_PacketBuffer) != xTS::TS_HeaderLength) {
      std::cerr << "Error: Failed to parse TS packet header" << std::endl;
      fclose(inputFile);
      return EXIT_FAILURE;
    }

    TS_AdaptationField.Reset();
    uint16_t pid = TS_PacketHeader.getPID();
    if (TS_PacketHeader.getSyncByte() == 'G' && (pid == 136 || pid == 174)) 
    {
      if (TS_PacketHeader.hasAdaptationField())
      {
        TS_AdaptationField.Parse(TS_PacketBuffer, TS_PacketHeader.getAFC());
      }
    
      printf("%010d ", TS_PacketId);
      TS_PacketHeader.Print();
      if (TS_PacketHeader.hasAdaptationField()) 
      {
        printf(" ");
        TS_AdaptationField.Print();
      }

      xPES_Assembler& asmblr = (pid == 136 ? assemblerMP2 : assembler264);
      FILE*         out    = (pid == 136 ? outputMP2    : output264);

      auto Result = asmblr.AbsorbPacket(TS_PacketBuffer, &TS_PacketHeader, &TS_AdaptationField);
      switch(Result)
      {
        case xPES_Assembler::eResult::StreamPackedLost : printf(" PcktLost "); break;
        case xPES_Assembler::eResult::AssemblingStarted : printf(" Started "); asmblr.PrintPESH(); break;
        case xPES_Assembler::eResult::AssemblingContinue: printf(" Continue "); break;
        case xPES_Assembler::eResult::AssemblingFinished: printf(" Finished "); {
          fseek(inputFile, prevPos, SEEK_SET);
          printf("PES: Len=%d HeadLen=%d DataLen=%d",
            asmblr.getNumPacketBytes(), asmblr.getPESHLength(), 
            asmblr.getNumPacketBytes() - asmblr.getPESHLength()); 
          uint8_t* PES_Data = asmblr.getPacket();
          int32_t PES_Size = asmblr.getNumPacketBytes();
          uint8_t PES_HeaderLength = asmblr.getPESHLength();

          fwrite(PES_Data + PES_HeaderLength, 1, PES_Size - PES_HeaderLength, out);
          break;
        }
        default: break;
      }
      printf("\n");
    }
    TS_PacketId++;
  }

  fclose(inputFile);
  fclose(outputMP2);
  fclose(output264);
  return EXIT_SUCCESS;
}

//=============================================================================================================================================================================