#pragma once

#include <cstdint>
#include <cstring>
#include <string>

#include "utils.hpp"

using namespace Utils;

// /**
//  * @brief Disables validation for faster processing speed
//  * 
//  */
// #define RLP_OPTIMIZE

namespace RLP {
  /**
   * @brief Maximum number of bytes that encoded length takes.
   * 
   */
  inline constexpr std::size_t MaximumEncodedLengthSize { 9 };

  /**
   * @brief RLP buffer size
   * 
   */
  inline constexpr std::size_t BufferSize { 1024 };

  /**
   * @brief Number of elements to encode. Ethereum transactions require 9: nonce, gas price, gas limit, to, value, data, v, r, s
   * 
   */
  inline constexpr std::size_t ElementsCount { 9 };

  /**
   * @brief Class for RLP encoding. Used for encoding Ethereum transactions
   * 
   */
  class RLPEncoder {
    /**
     * @brief Buffer holding encoded data
     * 
     */
    Byte mBuffer[BufferSize];

    #ifndef RLP_OPTIMIZE
      /**
       * @brief Has the buffer been fully encoded
       * 
       */
      bool mBufferEncoded { false };
    #endif

    /**
     * @brief Holds pointers to the buffer, represents encoded block of certain element
     * 
     */
    Buffer mElementStart[1 + ElementsCount + 1] { mBuffer, mBuffer + MaximumEncodedLengthSize };

    /**
     * @brief Encodes data length, requires 9 bytes
     * 
     * @param length length of data
     * @param offset offset
     * @param output output buffer
     * @return Buffer 
     */
    static Buffer encodeLength(size_t length, std::size_t offset, Buffer output);

    public:

    /**
     * @brief Encodes the element
     * 
     * 
     * @param position position of the element
     * @param input input data
     */
    void setElement(std::size_t position, const char *input);

    /**
     * @brief Encodes the element
     * 
     * @param position position of the element
     * @param input input data
     * @param inputLength input length
     */
    void setElement(std::size_t position, const char *input, std::size_t inputLength);

    /**
     * @brief Encodes the element
     * 
     * @param position position of the element
     * @param input input data
     * @param inputLength input length
     */
    void setElement(std::size_t position, Buffer input, std::size_t inputLength);

    /**
     * @brief Encodes the buffer
     * 
     */
    void encode();

    /**
     * @brief Get the encoded buffer
     * 
     * @return buffer start pointer and its length
     */
    std::pair<Buffer, size_t> getBuffer();

    /**
     * @brief Get the hexadecimal representation of the buffer
     * 
     * @param output output string
     * @param nullTerminated should string be null terminated, defaults to false
     * @return pointer to the last char
     */
    char *getHexString(char *output, bool nullTerminated = false);
  };
}