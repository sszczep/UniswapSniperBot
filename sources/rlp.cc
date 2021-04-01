#include <stdexcept>

#include "rlp.hpp"

using RLP::RLPEncoder;

Buffer RLPEncoder::encodeLength(size_t length, std::size_t offset, Buffer output) {
  if(length < 56) {
    *output = length + offset;
    return output;
  }

  Buffer lengthBytesStart = output + 1;
  Buffer lengthBytesEnd = intToBytes(length, lengthBytesStart);

  *output = getMostSignificantNonZeroByte(offset + 55 + (lengthBytesEnd - lengthBytesStart + 1));

  return lengthBytesEnd;
}

void RLPEncoder::setElement(std::size_t position, const char *input) {
  setElement(position, input, strlen(input));
}

void RLPEncoder::setElement(std::size_t position, const char *input, std::size_t inputLength) {
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
}

void RLPEncoder::setElement(std::size_t position, Buffer input, std::size_t inputLength) {
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
}

void RLPEncoder::encode() {
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

std::pair<Buffer, size_t> RLPEncoder::getBuffer() {
  #ifndef RLP_OPTIMIZE
    if(!mBufferEncoded) throw std::logic_error("Buffer has not been encoded");
  #endif

  return { mElementStart[0], mElementStart[ElementsCount + 1] - mElementStart[0] };
}

char *RLPEncoder::getHexString(char *output, bool nullTerminated) {
  #ifndef RLP_OPTIMIZE
    if(!mBufferEncoded) throw std::logic_error("Buffer has not been encoded");
  #endif

  return bufferToHexString(mElementStart[0], mElementStart[ElementsCount + 1] - mElementStart[0], output, nullTerminated);
}