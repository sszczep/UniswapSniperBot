#pragma once

#include <cstdint>
#include <cstring>
#include <string>
#include <stdexcept>

#include "utils.hpp"

using namespace Utils;

/**
 * Disables validation for faster processing speed
 * 
 */
// #define RLP_OPTIMIZE

/**
 * Class for Recursive Length Prefix Encoding. 
 * It is the main encoding method used to serialize objects in Ethereum. 
 * 
 * @author Sebastian Szczepański
 * @version 1.0
 * @see https://eth.wiki/fundamentals/rlp
 * 
 * @tparam BufferSize RLP buffer size
 * @tparam ElementsCount Number of elements to encode. Ethereum transactions require 9: nonce, gas price, gas limit, to, value, data, v, r, s
 * 
 * @todo Check if the algorithm matches official definition
 */
template<std::size_t BufferSize = 9, std::size_t ElementsCount = 1024>
class RLP {

  /**
   * Maximum number of bytes that encoded length takes.
   * 
   */
  static inline constexpr std::size_t MaximumEncodedLengthSize { 9 };


  /**
   * Buffer holding encoded data
   * 
   */
  Byte mBuffer[BufferSize];

  #ifndef RLP_OPTIMIZE
    /**
     * Has the buffer been fully encoded
     * 
     */
    bool mBufferEncoded { false };
  #endif

  /**
   * Holds pointers to the buffer, represents start of the encoded block of certain element
   * 
   */
  Buffer mElementStart[1 + ElementsCount + 1] { mBuffer, mBuffer + MaximumEncodedLengthSize };

  /**
   * Encodes data length, requires 9 bytes
   * 
   * @param length length of data
   * @param offset offset
   * @param output output buffer
   * @return Pointer to the last byte in the buffer 
   */
  static Buffer encodeLength(size_t length, std::size_t offset, Buffer output) {
    if(length < 56) {
      *output = length + offset;
      return output;
    }

    Buffer lengthBytesStart = output + 1;
    Buffer lengthBytesEnd = intToBuffer(length, lengthBytesStart);

    *output = getMostSignificantNonZeroByte(offset + 55 + (lengthBytesEnd - lengthBytesStart + 1));

    return lengthBytesEnd;
  }

  public:

  /**
   * Encodes the element
   * 
   * 
   * @param position position of the element
   * @param input input data
   * @return Pointer to the last byte of the encoded element 
   * @throws std::out_of_range Throws when setting element on invalid position
   * @throws std::logic_error Throws when setting element whlie previous elements are not set
   */
  Buffer setElement(std::size_t position, const char *input) {
    return setElement(position, input, strlen(input));
  }

  /**
   * Encodes the element
   * 
   * @param position position of the element
   * @param input input data
   * @param inputLength input length
   * @return Pointer to the last byte of the encoded element 
   * @throws std::out_of_range Throws when setting element on invalid position
   * @throws std::logic_error Throws when setting element whlie previous elements are not set
   */
  Buffer setElement(std::size_t position, const char *input, std::size_t inputLength) {
    #ifndef RLP_OPTIMIZE
      if(position > ElementsCount) throw std::out_of_range("Position out of range");
    #endif

    Buffer buffer = mElementStart[position];
    #ifndef RLP_OPTIMIZE
      if(buffer == nullptr) throw std::logic_error("Previous positions have not been filled");
    #endif
    
    Buffer bufferEnd = nullptr;

    if(inputLength == 0) {
      buffer[0] = 128;
      bufferEnd = buffer;
    } else if(inputLength == 1) {
      buffer[0] = hexCharToByte(input[0]);
      bufferEnd = buffer;
    } else if(inputLength == 2) {
      Byte byte = 16 * hexCharToByte(input[0]) + hexCharToByte(input[1]);
      if(byte < 128) {
        buffer[0] = byte;
        bufferEnd = buffer;
      }
    } else {
      Buffer encodedLengthEnd = encodeLength((inputLength + 1) / 2, 128, buffer);
      Buffer encodedInputEnd = hexStringToBuffer(input, inputLength, encodedLengthEnd + 1);
      bufferEnd = encodedInputEnd;
    }

    mElementStart[position + 1] = bufferEnd + 1;
      
    #ifndef RLP_OPTIMIZE
      for(std::size_t i = position + 2; i <= ElementsCount; i++) mElementStart[i] = nullptr;
      mBufferEncoded = false;
    #endif

    return bufferEnd;
  }

  /**
   * Encodes the element
   * 
   * @param position position of the element
   * @param input input data
   * @param inputLength input length
   * @return Pointer to the last byte of the encoded element 
   * @throws std::out_of_range Throws when setting element on invalid position
   * @throws std::logic_error Throws when setting element whlie previous elements are not set
   */
  Buffer setElement(std::size_t position, Buffer input, std::size_t inputLength) {
    #ifndef RLP_OPTIMIZE
      if(position > ElementsCount) throw std::out_of_range("Position out of range");
    #endif

    Buffer buffer = mElementStart[position];
    #ifndef RLP_OPTIMIZE
      if(buffer == nullptr) throw std::logic_error("Previous positions have not been filled");
    #endif
    
    Buffer bufferEnd = nullptr;

    if(inputLength == 1 && input[0] < 128) {
      buffer[0] = input[0];
      bufferEnd = buffer;
    } else {
      Buffer encodedLengthEnd = encodeLength(inputLength, 128, buffer);
      memcpy(encodedLengthEnd + 1, input, inputLength);
      bufferEnd = encodedLengthEnd + inputLength;
    }
    
    mElementStart[position + 1] = bufferEnd + 1;
      
    #ifndef RLP_OPTIMIZE
      for(std::size_t i = position + 2; i <= ElementsCount; i++) mElementStart[i] = nullptr;
      mBufferEncoded = false;
    #endif
    
    return bufferEnd;
  }

  /**
   * Encodes the buffer
   * 
   * @return Encoded buffer start pointer and its length
   * @throws std::logic_error Throws when trying to encode the buffer without setting all elements first
   */
  std::pair<Buffer, std::size_t> encode() {
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

    return { encodedLengthStart, encodedLengthLength + encodedDataLength };
  }

  /**
   * Get the encoded buffer
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