#pragma once

#include <cstdint>
#include <cstring>
#include <string>
#include <stdexcept>

#include "utils.hpp"

using namespace Utils;

// /**
//  * @brief Disables validation for faster processing speed
//  * 
//  */
// #define RLP_OPTIMIZE

/**
 * @brief Class for Recursive Length Prefix Encoding. 
 * It is the main encoding method used to serialize objects in Ethereum. 
 * 
 * @author Sebastian Szczepański
 * @see https://eth.wiki/fundamentals/rlp
 * 
 * @tparam BufferSize RLP buffer size
 * @tparam ElementsCount Number of elements to encode. Ethereum transactions require 9: nonce, gas price, gas limit, to, value, data, v, r, s
 * 
 * @todo Refactor getBuffer method - should return bufferEnd as pointer and take pointer to buffer as an argument
 * @todo Add decoding functionality
 */
template<std::size_t BufferSize = 1024, std::size_t ElementsCount = 9>
class RLP {

  private:

  /**
   * @brief Maximum number of bytes that encoded length takes.
   * 
   */
  static inline constexpr std::size_t MaximumEncodedLengthSize { 9 };

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
   * @brief Holds pointers to the buffer, represents start of the encoded block of certain element
   * 
   */
  Buffer mElementStart[1 + ElementsCount + 1] { mBuffer, mBuffer + MaximumEncodedLengthSize };

  /**
   * @brief Encodes data length, requires 9 bytes
   * 
   * @param length length of data
   * @param offset offset
   * @param output output buffer
   * @return Pointer to the last byte in the buffer 
   */
  static Buffer encodeLength(std::size_t length, std::size_t offset, Buffer output) {
    if(length < 56) {
      *output = length + offset;
      return output;
    }

    Buffer lengthBytesStart = output + 1;
    Buffer lengthBytesEnd = intToBuffer(length, lengthBytesStart);

    // Why 55?
    // Documentation outlines: 
    // If a string is more than 55 bytes long, the RLP encoding consists of a single byte with value 0xb7 [...]
    // If the total payload of a list is more than 55 bytes long, the RLP encoding consists of a single byte with value 0xf7 [...]
    // When encoding string, we specify offset of 128 (0x80), add 55 and receive 183 (0xb7)
    // When encoding total payload, we specify offset of 192 (0xc0), add 55 and receive 247 (0xf7)
    *output = (lengthBytesEnd - lengthBytesStart + 1) + offset + 55;
    return lengthBytesEnd;
  }

  public:

  /**
   * @brief Encodes the element
   * 
   * 
   * @param position position of the element
   * @param input input data
   * @throws std::out_of_range Throws when setting element on invalid position
   * @throws std::logic_error Throws when setting element whlie previous elements are not set
   */
  void setElement(std::size_t position, const char *input) {
    setElement(position, input, strlen(input));
  }

  /**
   * @brief Encodes the element
   * 
   * @param position position of the element
   * @param input input data
   * @param length input length
   * @throws std::out_of_range Throws when setting element on invalid position
   * @throws std::logic_error Throws when setting element whlie previous elements are not set
   */
  void setElement(std::size_t position, const char *input, std::size_t length) {
    #ifndef RLP_OPTIMIZE
      if(position < 1 || position > ElementsCount) throw std::out_of_range("Position out of range");
    #endif

    Buffer buffer = mElementStart[position];
    #ifndef RLP_OPTIMIZE
      if(buffer == nullptr) throw std::logic_error("Previous positions have not been filled");
    #endif
    
    Buffer bufferEnd = nullptr;

    if(length == 0) {
      *buffer = 128;
      bufferEnd = buffer;
    } else if(length == 1) {
      *buffer = hexCharToByte(input[0]);
      bufferEnd = buffer;
    } else if(length == 2) {
      Byte byte = 16 * hexCharToByte(input[0]) + hexCharToByte(input[1]);
      if(byte < 128) {
        *buffer = byte;
        bufferEnd = buffer;
      }
    } else {
      Buffer encodedLengthEnd = encodeLength((length + 1) / 2, 128, buffer);
      Buffer encodedInputEnd = hexStringToBuffer(input, length, encodedLengthEnd + 1);
      bufferEnd = encodedInputEnd;
    }

    mElementStart[position + 1] = bufferEnd + 1;
      
    #ifndef RLP_OPTIMIZE
      for(std::size_t i = position + 2; i <= ElementsCount; i++) mElementStart[i] = nullptr;
      mBufferEncoded = false;
    #endif
  }

  /**
   * @brief Encodes the element
   * 
   * @param position position of the element
   * @param input input data
   * @param length input length
   * @throws std::out_of_range Throws when setting element on invalid position
   * @throws std::logic_error Throws when setting element whlie previous elements are not set
   */
  void setElement(std::size_t position, Buffer input, std::size_t length) {
    #ifndef RLP_OPTIMIZE
      if(position < 1 || position > ElementsCount) throw std::out_of_range("Position out of range");
    #endif

    Buffer buffer = mElementStart[position];
    #ifndef RLP_OPTIMIZE
      if(buffer == nullptr) throw std::logic_error("Previous positions have not been filled");
    #endif
    
    Buffer bufferEnd = nullptr;

    if(length == 1 && input[0] < 128) {
      *buffer = *input;
      bufferEnd = buffer;
    } else {
      Buffer encodedLengthEnd = encodeLength(length, 128, buffer);
      memcpy(encodedLengthEnd + 1, input, length);
      bufferEnd = encodedLengthEnd + length;
    }
    
    mElementStart[position + 1] = bufferEnd + 1;
      
    #ifndef RLP_OPTIMIZE
      for(std::size_t i = position + 2; i <= ElementsCount; i++) mElementStart[i] = nullptr;
      mBufferEncoded = false;
    #endif
  }

  /**
   * @brief Encodes the buffer
   * 
   * @throws std::logic_error Throws when trying to encode the buffer without setting all elements first
   */
  void encode() {
    #ifndef RLP_OPTIMIZE
      if(mElementStart[ElementsCount + 1] == nullptr) throw std::logic_error("Buffer has not been filled. Cannot encode");
    #endif

    std::size_t encodedDataLength = mElementStart[ElementsCount + 1] - mElementStart[1];

    Buffer encodedLengthEnd = encodeLength(encodedDataLength, 192, mBuffer);
    std::size_t encodedLengthLength = (encodedLengthEnd - mBuffer) + 1;
    Buffer encodedLengthStart = mBuffer + (MaximumEncodedLengthSize - encodedLengthLength);

    memmove(encodedLengthStart, mBuffer, encodedLengthLength);

    mElementStart[0] = encodedLengthStart;

    #ifndef RLP_OPTIMIZE
      mBufferEncoded = true;
    #endif 
  }

  /**
   * @brief Get the encoded buffer
   * 
   * @return Encoded buffer start pointer and its length
   * @throws std::logic_error Throws when getting not encoded buffer
   */
  std::pair<Buffer, std::size_t> getBuffer() {
    #ifndef RLP_OPTIMIZE
      if(!mBufferEncoded) throw std::logic_error("Buffer has not been encoded");
    #endif

    return { mElementStart[0], mElementStart[ElementsCount + 1] - mElementStart[0] };
  }

  /**
   * @brief Get the hexadecimal representation of the encoded buffer
   * 
   * @param output output string
   * @param nullTerminated should string be null terminated, defaults to false
   * @return Pointer to the last char
   * @throws std::logic_error Throws when getting not encoded buffer
   */
  char *getHexString(char *output, bool nullTerminated = false) {
    #ifndef RLP_OPTIMIZE
      if(!mBufferEncoded) throw std::logic_error("Buffer has not been encoded");
    #endif

    return bufferToHexString(mElementStart[0], mElementStart[ElementsCount + 1] - mElementStart[0], output, nullTerminated);
  }
};