#pragma once

#include <cstdint>
#include <cstring>
#include <stdexcept>

/**
 * @brief Namespace holding all converters and other utilities.
 */
namespace Utils {
  using Byte = std::uint8_t;
  using Buffer = Byte*;

  /**
   * @brief Converts hexadecimal char to byte.
   * 
   * @param x input hexadecimal char
   * @return output byte 
   * 
   * @throws std::invalid_argument Throws when input is not valid hexadecimal char
   */
  inline Byte hexCharToByte(char x) {
    if(x >= '0' && x <= '9') return x - '0';
    if(x >= 'A' && x <= 'F') return x - 'A' + 10;
    if(x >= 'a' && x <= 'f') return x - 'a' + 10;
    throw std::invalid_argument("Invalid argument");
  }

  /**
   * @brief Converts byte to hexadecimal char.
   * 
   * @param x input byte
   * @return output char 
   * 
   * @throws std::invalid_argument Throws when input is not valid hexadecimal value
   */
  inline char byteToHexChar(Byte x) {
    if(x >= 0 && x <= 9) return x + '0';
    if(x >= 10 && x <= 15) return (x - 10) + 'a';
    throw std::invalid_argument("Invalid argument");
  }

  /**
   * @brief Converts hexadecimal string to buffer.
   * 
   * @param input input hexadecimal c-string
   * @param inputLength length of the input string
   * @param output output buffer
   * @param stripZeroes should input string be trimmed of leading zeroes
   * @return output buffer length
   */
  inline std::size_t hexStringToBuffer(const char *input, std::size_t inputLength, Buffer output, bool stripZeroes = false) {
    if(stripZeroes) {
      while(*input == '0') {
        ++input;
        --inputLength;
      }
    }
    if(inputLength == 0) return 0;

    std::size_t outputLength = (inputLength + 1) / 2;

    if(inputLength % 2 == 1) {
      *(output++) = hexCharToByte(*(input++));
      --inputLength;
    }

    while(inputLength > 0) {
      *(output++) = 16 * hexCharToByte(*input) + hexCharToByte(*(input + 1));
      input += 2;
      inputLength -= 2;
    }

    return outputLength;
  }

  /**
   * @brief Converts hexadecimal null-terminated string to buffer.
   * 
   * @param input input hexadecimal null-terminated c-string 
   * @param output output buffer
   * @param stripZeroes should input string be trimmed of leading zeroes
   * @return output buffer length
   */
  inline std::size_t hexStringToBuffer(const char *input, Buffer output, bool stripZeroes = false) {
    return hexStringToBuffer(input, strlen(input), output, stripZeroes);
  }

  /**
   * @brief Converts buffer to hexadecimal string.
   * 
   * @param input input buffer
   * @param inputLength length of the input buffer 
   * @param output output hexadecimal c-string, padded to even length
   * @param nullTerminated should string be null terminated, defaults to false
   * @return output c-string length (without null terminator)
   */
  inline std::size_t bufferToHexString(Buffer input, std::size_t inputLength, char *output, bool nullTerminated = false) {
    if(inputLength == 0) return 0;

    Buffer inputEnd = input + inputLength;
    while(input != inputEnd) {
      *output = byteToHexChar((*input / 16) % 16);
      *(output + 1) = byteToHexChar(*input % 16);

      ++input;
      output += 2;
    }

    if(nullTerminated) *output = '\0';
    
    return inputLength * 2;
  }

  /**
   * @brief Converts integer to buffer.
   * 
   * @param x input integer
   * @param output output buffer
   * @return output buffer length
   */
  inline std::size_t intToBuffer(std::uint64_t x, Buffer output) {
    std::size_t length;
    if(x > 0xFFFFFFFFFFFFFF) length = 8;
    else if(x > 0xFFFFFFFFFFFF) length = 7;
    else if(x > 0xFFFFFFFFFF) length = 6;
    else if(x > 0xFFFFFFFF) length = 5;
    else if(x > 0xFFFFFF) length = 4;
    else if(x > 0xFFFF) length = 3;
    else if(x > 0xFF) length = 2;
    else if(x > 0) length = 1;
    else if(x == 0) {
      *output = 0;
      return 1;
    }

    Buffer outputStart = output + length;
    for(; x != 0; x >>= 8) *(--outputStart) = x & 0xFF;
    return length;
  }
}