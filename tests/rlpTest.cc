#include <gmock/gmock.h>

// Hacky solution to access private members of the class
#define private public
#include <rlp.hpp>

TEST(RLP, encodeLength) {
  Byte buffer[9];
  Buffer bufferEnd;

  // If a string is 0-55 bytes long, the RLP encoding consists of a single byte with value 0x80 plus the length of the string
  // The range of the first byte is thus [0x80, 0xb7]
  bufferEnd = RLP<0, 0>::encodeLength(0, 0x80, buffer);
  ASSERT_EQ(bufferEnd - buffer, 0);
  ASSERT_EQ(buffer[0], 0x80);

  bufferEnd = RLP<0, 0>::encodeLength(55, 0x80, buffer);
  ASSERT_EQ(bufferEnd - buffer, 0);
  ASSERT_EQ(buffer[0], 0xb7);

  // If a string is more than 55 bytes long, the RLP encoding consists of a single byte with value 0xb7 
  // plus the length in bytes of the length of the string in binary form, followed by the length of the string.
  // The range of the first byte is thus [0xb8, 0xbf].
  // For example, a length-1024 string would be encoded as \xb9\x04\x00
  bufferEnd = RLP<0, 0>::encodeLength(1024, 0x80, buffer);
  ASSERT_EQ(bufferEnd - buffer, 2);
  ASSERT_EQ(buffer[0], 0xb9);
  ASSERT_EQ(buffer[1], 0x04);
  ASSERT_EQ(buffer[2], 0x00);
}

TEST(RLP, setElementNullTerminatedString) {
  RLP<1024, 1> rlp;

  rlp.setElement(1, "");
  ASSERT_EQ(rlp.mElementStart[2] - rlp.mElementStart[1], 1);
  ASSERT_EQ(rlp.mElementStart[1][0], 0x80);

  rlp.setElement(1, "0");
  ASSERT_EQ(rlp.mElementStart[2] - rlp.mElementStart[1], 1);
  ASSERT_EQ(rlp.mElementStart[1][0], 0x00);

  rlp.setElement(1, "F");
  ASSERT_EQ(rlp.mElementStart[2] - rlp.mElementStart[1], 1);
  ASSERT_EQ(rlp.mElementStart[1][0], 0x0f);

  rlp.setElement(1, "400");
  ASSERT_EQ(rlp.mElementStart[2] - rlp.mElementStart[1], 3);
  ASSERT_EQ(rlp.mElementStart[1][0], 0x82);
  ASSERT_EQ(rlp.mElementStart[1][1], 0x04);
  ASSERT_EQ(rlp.mElementStart[1][2], 0x00);
}

TEST(RLP, setElementNullTerminatedStringThrows) {
  RLP<1024, 2> rlp;

  try {
    rlp.setElement(0, "");
    FAIL() << "Expected std::out_of_range";
  } catch(std::out_of_range const &err) {
    ASSERT_EQ(std::string("Position out of range"), err.what());
  } catch(...) {
    FAIL() << "Expected std::out_of_range";
  }

  try {
    rlp.setElement(3, "");
    FAIL() << "Expected std::out_of_range";
  } catch(std::out_of_range const &err) {
    ASSERT_EQ(std::string("Position out of range"), err.what());
  } catch(...) {
    FAIL() << "Expected std::out_of_range";
  }

  try {
    rlp.setElement(2, "");
    FAIL() << "Expected std::logic_error";
  } catch(std::logic_error const &err) {
    ASSERT_EQ(std::string("Previous positions have not been filled"), err.what());
  } catch(...) {
    FAIL() << "Expected std::logic_error";
  }
}

TEST(RLP, setElementString) {
  RLP<1024, 1> rlp;
  char input[3];

  rlp.setElement(1, input, 0);
  ASSERT_EQ(rlp.mElementStart[2] - rlp.mElementStart[1], 1);
  ASSERT_EQ(rlp.mElementStart[1][0], 0x80);

  input[0] = '0';
  rlp.setElement(1, input, 1);
  ASSERT_EQ(rlp.mElementStart[2] - rlp.mElementStart[1], 1);
  ASSERT_EQ(rlp.mElementStart[1][0], 0x00);

  input[0] = 'F';
  rlp.setElement(1, input, 1);
  ASSERT_EQ(rlp.mElementStart[2] - rlp.mElementStart[1], 1);
  ASSERT_EQ(rlp.mElementStart[1][0], 0x0f);

  input[0] = '4', input[1] = '0', input[2] = '0';
  rlp.setElement(1, input, 3);
  ASSERT_EQ(rlp.mElementStart[2] - rlp.mElementStart[1], 3);
  ASSERT_EQ(rlp.mElementStart[1][0], 0x82);
  ASSERT_EQ(rlp.mElementStart[1][1], 0x04);
  ASSERT_EQ(rlp.mElementStart[1][2], 0x00);
}

TEST(RLP, setElementStringThrows) {
  RLP<1024, 2> rlp;
  char *input = nullptr;

  try {
    rlp.setElement(0, input, 0);
    FAIL() << "Expected std::out_of_range";
  } catch(std::out_of_range const &err) {
    ASSERT_EQ(std::string("Position out of range"), err.what());
  } catch(...) {
    FAIL() << "Expected std::out_of_range";
  }

  try {
    rlp.setElement(3, input, 0);
    FAIL() << "Expected std::out_of_range";
  } catch(std::out_of_range const &err) {
    ASSERT_EQ(std::string("Position out of range"), err.what());
  } catch(...) {
    FAIL() << "Expected std::out_of_range";
  }

  try {
    rlp.setElement(2, input, 0);
    FAIL() << "Expected std::logic_error";
  } catch(std::logic_error const &err) {
    ASSERT_EQ(std::string("Previous positions have not been filled"), err.what());
  } catch(...) {
    FAIL() << "Expected std::logic_error";
  }
}

TEST(RLP, setElementBuffer) {
  RLP<1024, 1> rlp;
  Byte input[2];

  rlp.setElement(1, input, 0);
  ASSERT_EQ(rlp.mElementStart[2] - rlp.mElementStart[1], 1);
  ASSERT_EQ(rlp.mElementStart[1][0], 0x80);

  input[0] = 0x00;
  rlp.setElement(1, input, 1);
  ASSERT_EQ(rlp.mElementStart[2] - rlp.mElementStart[1], 1);
  ASSERT_EQ(rlp.mElementStart[1][0], 0x00);

  input[0] = 0x0F;
  rlp.setElement(1, input, 1);
  ASSERT_EQ(rlp.mElementStart[2] - rlp.mElementStart[1], 1);
  ASSERT_EQ(rlp.mElementStart[1][0], 0x0f);

  input[0] = 0x04, input[1] = 0x00;
  rlp.setElement(1, input, 2);
  ASSERT_EQ(rlp.mElementStart[2] - rlp.mElementStart[1], 3);
  ASSERT_EQ(rlp.mElementStart[1][0], 0x82);
  ASSERT_EQ(rlp.mElementStart[1][1], 0x04);
  ASSERT_EQ(rlp.mElementStart[1][2], 0x00);
}

TEST(RLP, setElementBufferThrows) {
  RLP<1024, 2> rlp;
  Buffer input = nullptr;

  try {
    rlp.setElement(0, input, 0);
    FAIL() << "Expected std::out_of_range";
  } catch(std::out_of_range const &err) {
    ASSERT_EQ(std::string("Position out of range"), err.what());
  } catch(...) {
    FAIL() << "Expected std::out_of_range";
  }

  try {
    rlp.setElement(3, input, 0);
    FAIL() << "Expected std::out_of_range";
  } catch(std::out_of_range const &err) {
    ASSERT_EQ(std::string("Position out of range"), err.what());
  } catch(...) {
    FAIL() << "Expected std::out_of_range";
  }

  try {
    rlp.setElement(2, input, 0);
    FAIL() << "Expected std::logic_error";
  } catch(std::logic_error const &err) {
    ASSERT_EQ(std::string("Previous positions have not been filled"), err.what());
  } catch(...) {
    FAIL() << "Expected std::logic_error";
  }
}

TEST(RLP, encode) {
  RLP<1024, 0> emptyPayload;
  emptyPayload.encode();
  ASSERT_EQ(emptyPayload.mElementStart[1] - emptyPayload.mElementStart[0], 1);
  ASSERT_EQ(emptyPayload.mElementStart[0][0], 0xc0);

  RLP<1024, 1> payload0;
  payload0.mElementStart[2] = payload0.mElementStart[1] + 0;
  payload0.encode();
  ASSERT_EQ(payload0.mElementStart[1] - payload0.mElementStart[0], 1);
  ASSERT_EQ(payload0.mElementStart[0][0], 0xc0);

  RLP<1024, 1> payload55;
  payload55.mElementStart[2] = payload55.mElementStart[1] + 55;
  payload55.encode();
  ASSERT_EQ(payload55.mElementStart[1] - payload55.mElementStart[0], 1);
  ASSERT_EQ(payload55.mElementStart[0][0], 0xf7);

  RLP<1024, 1> payload56;
  payload56.mElementStart[2] = payload56.mElementStart[1] + 56;
  payload56.encode();
  ASSERT_EQ(payload56.mElementStart[1] - payload56.mElementStart[0], 2);
  ASSERT_EQ(payload56.mElementStart[0][0], 0xf8);
  ASSERT_EQ(payload56.mElementStart[0][1], 56);
}

TEST(RLP, encodeThrows) {
  RLP<1024, 1> rlp;
  
  try {
    rlp.encode();
    FAIL() << "Expected std::logic_error";
  } catch(std::logic_error const &err) {
    ASSERT_EQ(std::string("Buffer has not been filled. Cannot encode"), err.what());
  } catch(...) {
    FAIL() << "Expected std::logic_error";
  }
}

TEST(RLP, getBuffer) {
  RLP<1024, 4> rlp; 

  rlp.setElement(1, "");
  rlp.setElement(2, "0");
  rlp.setElement(3, "F");
  rlp.setElement(4, "400");
  rlp.encode();

  auto [buffer, bufferLength] = rlp.getBuffer();

  ASSERT_EQ(bufferLength, 7);
  ASSERT_EQ(buffer[0], 0xc6);
  ASSERT_EQ(buffer[1], 0x80);
  ASSERT_EQ(buffer[2], 0x00);
  ASSERT_EQ(buffer[3], 0x0f);
  ASSERT_EQ(buffer[4], 0x82);
  ASSERT_EQ(buffer[5], 0x04);
  ASSERT_EQ(buffer[6], 0x00);
}

TEST(RLP, getBufferThrows) {
  RLP<1024, 0> rlp;

  try {
  rlp.getBuffer();
  FAIL() << "Expected std::logic_error";
} catch(std::logic_error const &err) {
  ASSERT_EQ(std::string("Buffer has not been encoded"), err.what());
} catch(...) {
  FAIL() << "Expected std::logic_error";
}
}

TEST(RLP, getHexString) {
  RLP<1024, 4> rlp; 

  rlp.setElement(1, "");
  rlp.setElement(2, "0");
  rlp.setElement(3, "F");
  rlp.setElement(4, "400");
  rlp.encode();

  char output[14];
  char *outputEnd = rlp.getHexString(output, false);
  ASSERT_EQ(outputEnd - output + 1, 14);
  ASSERT_THAT(output, ::testing::StartsWith("c680000f820400"));

  char outputNullTerminated[15];
  char *outputNullTerminatedEnd = rlp.getHexString(outputNullTerminated, true);
  ASSERT_EQ(outputNullTerminatedEnd - outputNullTerminated + 1, 15);
  ASSERT_STREQ(outputNullTerminated, "c680000f820400");
}

TEST(RLP, getHexStringThrows) {
  RLP<1024, 0> rlp;
  char *output = nullptr;

  try {
    rlp.getHexString(output);
    FAIL() << "Expected std::logic_error";
  } catch(std::logic_error const &err) {
    ASSERT_EQ(std::string("Buffer has not been encoded"), err.what());
  } catch(...) {
    FAIL() << "Expected std::logic_error";
  }
}