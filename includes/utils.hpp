#pragma once

#include <cstdint>
#include <cstring>

namespace Utils {
  using Byte = std::uint8_t;
  using Buffer = Byte*;

  /**
   * @brief Converts hexadecimal char to byte.
   * 
   * @param x hexadecimal char
   * @return Byte 
   */
  Byte hexCharToByte(char x);

  /**
   * @brief Converts byte to hexadecimal char
   * 
   * @param x Byte
   * @return char 
   */
  char byteToHexChar(Byte x);

  /**
   * @brief Converts hex string to buffer
   * 
   * @param input c string 
   * @param inputLength length of the input string
   * @param output output buffer
   * @return pointer to the last byte
   */
  Buffer hexStringToBuffer(const char *input, std::size_t inputLength, Buffer output);

  /**
   * @brief Converts hexadecimal string to buffer
   * 
   * @param input null-terminated c string 
   * @param output output buffer
   * @return pointer to the last byte
   */
  Buffer hexStringToBuffer(const char *input, Buffer output);

  /**
   * @brief 
   * 
   * @param input buffer
   * @param inputLength length of the input buffer 
   * @param output output string
   * @param nullTerminated should string be null terminated, defaults to false
   * @return pointer to the last char 
   */
  char *bufferToHexString(Buffer input, size_t inputLength, char *output, bool nullTerminated = false);

  /**
   * @brief Get the most significant non zero byte
   * 
   * @param x input integer
   * @return Byte 
   */
  Byte getMostSignificantNonZeroByte(std::uint64_t x);

  /**
   * @brief Convert integer to bytes
   * 
   * @param x input integer
   * @param output output buffer
   * @return pointer to the last byte 
   */
  Buffer intToBytes(std::uint64_t x, Buffer output);
}