#pragma once

#include <cstdint>
#include <cstring>

/**
 * @brief Namespace holding all converters and other utilities
 * 
 */
namespace Utils {
  using Byte = std::uint8_t;
  using Buffer = Byte*;

  /**
   * @brief Converts hexadecimal char to byte.
   * 
   * @param x hexadecimal char
   * @return Byte 
   */
  inline Byte hexCharToByte(char x) {
    if(x >= '0' && x <= '9') return x - '0';
    if(x >= 'A' && x <= 'F') return x - 'A' + 10;
    if(x >= 'a' && x <= 'f') return x - 'a' + 10;
    return 0;
  }

  /**
   * @brief Converts byte to hexadecimal char
   * 
   * @param x byte
   * @return Char 
   */
  inline char byteToHexChar(Byte x) {
    if(x >= 0 && x <= 9) return x + '0';
    if(x >= 10 && x <= 15) return (x - 10) + 'a';
    return '0'; 
  }

  /**
   * @brief Converts hexadecimal string to buffer
   * 
   * @param input c string 
   * @param inputLength length of the input string
   * @param output output buffer
   * @return Pointer to the last byte, nullptr if string is empty
   */
  inline Buffer hexStringToBuffer(const char *input, std::size_t inputLength, Buffer output) {
    if(inputLength == 0) return nullptr;
    if(inputLength % 2 == 1) {
      *(output++) = hexCharToByte(*(input++));
      inputLength--;
    }
    while(inputLength > 0) {
      *(output++) = 16 * hexCharToByte(*input) + hexCharToByte(*(input + 1));
      input += 2;
      inputLength -= 2;
    }
    return output - 1;
  }

  /**
   * @brief Converts hexadecimal string to buffer
   * 
   * @param input null-terminated c string 
   * @param output output buffer
   * @return Pointer to the last byte
   */
  inline Buffer hexStringToBuffer(const char *input, Buffer output) {
    return hexStringToBuffer(input, strlen(input), output);
  }

  /**
   * @brief Converts buffer to hexadecimal string
   * 
   * @param input buffer
   * @param inputLength length of the input buffer 
   * @param output output string, padded to even length
   * @param nullTerminated should string be null terminated, defaults to false
   * @return Pointer to the last char, nullptr if buffer is empty
   */
  inline char *bufferToHexString(Buffer input, size_t inputLength, char *output, bool nullTerminated = false) {
    if(inputLength == 0) return nullptr;
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

  /**
   * @brief Get the most significant non zero byte
   * 
   * @param x input integer
   * @return Byte 
   */
  inline Byte getMostSignificantNonZeroByte(std::uint64_t x) {
    if(x == 0) return 0;

    Byte MSB;
    std::uint_fast8_t n = 7;
    while((MSB = (x >> 8 * (n--)) & 0xFF) == 0);

    return MSB;
  }

  /**
   * @brief Convert integer to buffer
   * 
   * @param x input integer, requires 8 bytes
   * @param output output buffer
   * @return Pointer to the last byte 
   */
  inline Buffer intToBuffer(std::uint64_t x, Buffer output) {
    Buffer outputStart = output + 8;
    Buffer outputEnd = outputStart;

    if(x == 0) {
      *output = 0;
      return output;
    }

    for(; x != 0; x >>= 8) *(--outputStart) = x & 0xFF;
    std::size_t length = outputEnd - outputStart;

    // Move elements to the beginning of buffer
    memmove(output, outputStart, length);

    return output + length - 1;
  }
}