#include "utils.hpp"

namespace Utils {
  Byte hexCharToByte(char x) {
    if(x >= '0' && x <= '9') return x - '0';
    if(x >= 'A' && x <= 'F') return x - 'A' + 10;
    if(x >= 'a' && x <= 'f') return x - 'a' + 10;
    return 0;
  }

  char byteToHexChar(Byte x) {
    if(x >= 0 && x <= 9) return x + '0';
    if(x >= 10 && x <= 15) return (x - 10) + 'a';
    return '0'; 
  }

  Buffer hexStringToBuffer(const char *input, std::size_t inputLength, Buffer output) {
    if(inputLength % 2 == 1) *(output++) = hexCharToByte(*(input++));
    while(*input != '\0') {
      *(output++) = 16 * hexCharToByte(*input) + hexCharToByte(*(input + 1));
      input += 2;
    }
    return output - 1;
  }

  Buffer hexStringToBuffer(const char *input, Buffer output) {
    return hexStringToBuffer(input, strlen(input), output);
  }

  char *bufferToHexString(Buffer input, size_t inputLength, char *output, bool nullTerminated) {
    Buffer inputEnd = input + inputLength;
    while(input != inputEnd) {
      *output = byteToHexChar((*input / 16) % 16);
      *(output + 1) = byteToHexChar(*input % 16);

      ++input;
      output += 2;
    }

    if(nullTerminated) {
      *output = '\0';
      return output;
    } else return output - 1;
  }

  Byte getMostSignificantNonZeroByte(std::uint64_t x) {
    Byte MSB;
    std::uint_fast8_t n = 7;
    while((MSB = (x >> 8 * (n--)) & 0xFF) == 0);

    return MSB;
  }

  Buffer intToBytes(std::uint64_t x, Buffer output) {
    Buffer outputStart = output + 8;
    Buffer outputEnd = outputStart;
    for(; x != 0; x >>= 8) *(--outputStart) = x & 0xFF;
    std::size_t length = outputEnd - outputStart;

    // Move elements to the beginning of buffer
    memmove(output, outputStart, length);

    return output + length - 1;
  }
}