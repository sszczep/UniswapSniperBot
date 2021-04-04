#include <gmock/gmock.h>

#include <utils.hpp>

TEST(Utils, hexCharToByte) {
  ASSERT_EQ(Utils::hexCharToByte('0' - 1), 0);
  ASSERT_EQ(Utils::hexCharToByte('0'), 0);
  ASSERT_EQ(Utils::hexCharToByte('9'), 9);
  ASSERT_EQ(Utils::hexCharToByte('9' + 1), 0);

  ASSERT_EQ(Utils::hexCharToByte('A' - 1), 0);
  ASSERT_EQ(Utils::hexCharToByte('A'), 10);
  ASSERT_EQ(Utils::hexCharToByte('F'), 15);
  ASSERT_EQ(Utils::hexCharToByte('F' + 1), 0);

  ASSERT_EQ(Utils::hexCharToByte('a' - 1), 0);
  ASSERT_EQ(Utils::hexCharToByte('a'), 10);
  ASSERT_EQ(Utils::hexCharToByte('f'), 15);
  ASSERT_EQ(Utils::hexCharToByte('f' + 1), 0);
}

TEST(Utils, byteToHexChar) {
  ASSERT_EQ(Utils::byteToHexChar(0), '0');
  ASSERT_EQ(Utils::byteToHexChar(9), '9');
  ASSERT_EQ(Utils::byteToHexChar(10), 'a');
  ASSERT_EQ(Utils::byteToHexChar(15), 'f');
  ASSERT_EQ(Utils::byteToHexChar(16), '0');
}

TEST(Utils, hexStringToBufferNullTerminated) {
  Utils::Byte buffer[5];
  Utils::Buffer bufferEnd;

  bufferEnd = Utils::hexStringToBuffer("", buffer);
  ASSERT_EQ(bufferEnd, nullptr);

  bufferEnd = Utils::hexStringToBuffer("0", buffer);
  ASSERT_EQ(bufferEnd - buffer, 0);
  ASSERT_EQ(buffer[0], 0);

  bufferEnd = Utils::hexStringToBuffer("1C1e1769", buffer);
  ASSERT_EQ(bufferEnd - buffer, 3);
  ASSERT_EQ(buffer[0], 28);
  ASSERT_EQ(buffer[1], 30);
  ASSERT_EQ(buffer[2], 23);
  ASSERT_EQ(buffer[3], 105);

  bufferEnd = Utils::hexStringToBuffer("D55f92Fd5", buffer);
  ASSERT_EQ(bufferEnd - buffer, 4);
  ASSERT_EQ(buffer[0], 13);
  ASSERT_EQ(buffer[1], 85);
  ASSERT_EQ(buffer[2], 249);
  ASSERT_EQ(buffer[3], 47);
  ASSERT_EQ(buffer[4], 213);
}

TEST(Utils, hexStringToBuffer) {
  Utils::Byte buffer[5];
  Utils::Buffer bufferEnd;
  char input[9];

  bufferEnd = Utils::hexStringToBuffer(input, 0, buffer);
  ASSERT_EQ(bufferEnd, nullptr);

  memcpy(input, "0", 1);
  bufferEnd = Utils::hexStringToBuffer(input, 1, buffer);
  ASSERT_EQ(bufferEnd - buffer, 0);
  ASSERT_EQ(buffer[0], 0);

  memcpy(input, "1C1e1769", 8);
  bufferEnd = Utils::hexStringToBuffer(input, 8, buffer);
  ASSERT_EQ(bufferEnd - buffer, 3);
  ASSERT_EQ(buffer[0], 28);
  ASSERT_EQ(buffer[1], 30);
  ASSERT_EQ(buffer[2], 23);
  ASSERT_EQ(buffer[3], 105);

  memcpy(input, "D55f92Fd5", 9);
  bufferEnd = Utils::hexStringToBuffer(input, 9, buffer);
  ASSERT_EQ(bufferEnd - buffer, 4);
  ASSERT_EQ(buffer[0], 13);
  ASSERT_EQ(buffer[1], 85);
  ASSERT_EQ(buffer[2], 249);
  ASSERT_EQ(buffer[3], 47);
  ASSERT_EQ(buffer[4], 213);
}

TEST(Utils, bufferToHexString) {
  Utils::Byte buffer[5];
  char out[10];
  char *outEnd;

  outEnd = Utils::bufferToHexString(buffer, 0, out);
  ASSERT_EQ(outEnd, nullptr);

  buffer[0] = 0;
  outEnd = Utils::bufferToHexString(buffer, 1, out);
  ASSERT_EQ(outEnd - out, 1);
  ASSERT_THAT(out, ::testing::StartsWith("00"));

  buffer[0] = 28;
  buffer[1] = 30;
  buffer[2] = 23;
  buffer[3] = 105;
  outEnd = Utils::bufferToHexString(buffer, 4, out);
  ASSERT_EQ(outEnd - out, 7);
  ASSERT_THAT(out, ::testing::StartsWith("1c1e1769"));

  buffer[0] = 13;
  buffer[1] = 85;
  buffer[2] = 249;
  buffer[3] = 47;
  buffer[4] = 213;
  outEnd = Utils::bufferToHexString(buffer, 5, out);
  ASSERT_EQ(outEnd - out, 9);
  ASSERT_THAT(out, ::testing::StartsWith("0d55f92fd5"));
}

TEST(Utils, bufferToHexStringNullTerminated) {
  Utils::Byte buffer[5];
  char out[11];
  char *outEnd;

  outEnd = Utils::bufferToHexString(buffer, 0, out, true);
  ASSERT_EQ(outEnd, nullptr);

  buffer[0] = 0;
  outEnd = Utils::bufferToHexString(buffer, 1, out, true);
  ASSERT_EQ(outEnd - out, 2);
  ASSERT_STREQ("00", out);

  buffer[0] = 28;
  buffer[1] = 30;
  buffer[2] = 23;
  buffer[3] = 105;
  outEnd = Utils::bufferToHexString(buffer, 4, out, true);
  ASSERT_EQ(outEnd - out, 8);
  ASSERT_STREQ("1c1e1769", out);

  buffer[0] = 13;
  buffer[1] = 85;
  buffer[2] = 249;
  buffer[3] = 47;
  buffer[4] = 213;
  outEnd = Utils::bufferToHexString(buffer, 5, out, true);
  ASSERT_EQ(outEnd - out, 10);
  ASSERT_STREQ("0d55f92fd5", out);
}

TEST(Utils, getMostSignificantNonZeroByte) {
  ASSERT_EQ(Utils::getMostSignificantNonZeroByte(0), 0);
  ASSERT_EQ(Utils::getMostSignificantNonZeroByte(1), 1);
  ASSERT_EQ(Utils::getMostSignificantNonZeroByte(471734121), 28);
  ASSERT_EQ(Utils::getMostSignificantNonZeroByte(57276968917), 13);
}

TEST(Utils, intToBuffer) {
  Utils::Byte buffer[8];
  Utils::Buffer bufferEnd;

  bufferEnd = Utils::intToBuffer(0, buffer);
  ASSERT_EQ(bufferEnd - buffer, 0);
  ASSERT_EQ(buffer[0], 0);

  bufferEnd = Utils::intToBuffer(1, buffer);
  ASSERT_EQ(bufferEnd - buffer, 0);
  ASSERT_EQ(buffer[0], 1);

  bufferEnd = Utils::intToBuffer(471734121, buffer);
  ASSERT_EQ(bufferEnd - buffer, 3);
  ASSERT_EQ(buffer[0], 28);
  ASSERT_EQ(buffer[1], 30);
  ASSERT_EQ(buffer[2], 23);
  ASSERT_EQ(buffer[3], 105);

  bufferEnd = Utils::intToBuffer(57276968917, buffer);
  ASSERT_EQ(bufferEnd - buffer, 4);
  ASSERT_EQ(buffer[0], 13);
  ASSERT_EQ(buffer[1], 85);
  ASSERT_EQ(buffer[2], 249);
  ASSERT_EQ(buffer[3], 47);
  ASSERT_EQ(buffer[4], 213);
}