#pragma once

#include <cstdint>
#include <cstring>
#include <string>

#include "utils.hpp"

using namespace Utils;

/**
 * @brief Functions for Recursive Length Prefix Encoding. 
 * It is the main encoding method used to serialize objects in Ethereum. 
 * 
 * @see https://eth.wiki/fundamentals/rlp
 */
namespace RLP {
  /**
   * @brief Struct holding single item data - its byte representation and length.
   */
  struct Item {
    Buffer buffer;
    std::size_t length;
  };

  /**
   * @brief Encodes length.
   * 
   * @param length length
   * @param offset offset
   * @param output output buffer
   * @return output buffer length
   */
  inline std::size_t encodeLength(std::size_t length, std::size_t offset, Buffer output) {
    if(length < 56) {
      *output = length + offset;
      return 1;
    }

    std::size_t bytesLength = intToBuffer(length, output + 1);

    // Why 55?
    // Documentation outlines: 
    // If a string is more than 55 bytes long, the RLP encoding consists of a single byte with value 0xb7 [...]
    // If the total payload of a list is more than 55 bytes long, the RLP encoding consists of a single byte with value 0xf7 [...]
    // When encoding string, we specify offset of 128 (0x80), add 55 and receive 183 (0xb7)
    // When encoding total payload, we specify offset of 192 (0xc0), add 55 and receive 247 (0xf7)
    *output = bytesLength + offset + 55;
    return bytesLength + 1;
  }

  /**
   * @brief Encodes single item.
   * 
   * @param input input item
   * @param output output buffer
   * @return output buffer length
   */
  inline std::size_t encode(Item *input, Buffer output) {
    // Empty item encoding returns 0x80
    if(input->length == 0) {
      *output = 0x80;
      return 1;
    }

    // For a single byte whose value is in the [0x00, 0x7f] range, that byte is its own RLP encoding
    if(input->length == 1 && *(input->buffer) < 0x80){
      *output = *(input->buffer);
      return 1;
    }

    std::size_t encodedLengthLength = encodeLength(input->length, 0x80, output);
    memcpy(output + encodedLengthLength, input->buffer, input->length);

    return encodedLengthLength + input->length;
  }

  /**
   * @brief Encodes list of items.
   * 
   * @param input list of input items
   * @param inputLength list length
   * @param output output buffer
   * @return output buffer length
   */
  inline std::size_t encode(Item input[], std::size_t inputLength, Buffer output) {
    std::size_t payloadLength = 0;
    for(std::size_t i = 0; i < inputLength; i++) {
      // Shift output buffer by 9 as it is maximum encodeLength length
      payloadLength += encode(input + i, output + payloadLength + 9);
    }

    // Encode payload length
    std::size_t encodedLengthLength = encodeLength(payloadLength, 0xc0, output);

    // Move payload to the encoded length
    memmove(output + encodedLengthLength, output + 9, payloadLength);

    return encodedLengthLength + payloadLength;
  }
}