#include <utils.hpp>

#include <gtest/gtest.h>

TEST(Utils, hexCharToByte) {
  EXPECT_EQ(Utils::hexCharToByte('0' - 1), 0);
  EXPECT_EQ(Utils::hexCharToByte('0'), 0);
  EXPECT_EQ(Utils::hexCharToByte('9'), 9);
  EXPECT_EQ(Utils::hexCharToByte('9' + 1), 0);

  EXPECT_EQ(Utils::hexCharToByte('A' - 1), 0);
  EXPECT_EQ(Utils::hexCharToByte('A'), 10);
  EXPECT_EQ(Utils::hexCharToByte('F'), 15);
  EXPECT_EQ(Utils::hexCharToByte('F' + 1), 0);

  EXPECT_EQ(Utils::hexCharToByte('a' - 1), 0);
  EXPECT_EQ(Utils::hexCharToByte('a'), 10);
  EXPECT_EQ(Utils::hexCharToByte('f'), 15);
  EXPECT_EQ(Utils::hexCharToByte('f' + 1), 0);
}

TEST(Utils, byteToHexChar) {
  EXPECT_EQ(Utils::byteToHexChar(0), '0');
  EXPECT_EQ(Utils::byteToHexChar(9), '9');
  EXPECT_EQ(Utils::byteToHexChar(10), 'a');
  EXPECT_EQ(Utils::byteToHexChar(15), 'f');
  EXPECT_EQ(Utils::byteToHexChar(16), '0');
}

TEST(Utils, hexStringToBuffer) {
  Utils::Byte buffer[5];
  Utils::Buffer bufferEnd;

  bufferEnd = Utils::hexStringToBuffer("", 0, buffer);
  EXPECT_EQ(bufferEnd, nullptr);

  bufferEnd = Utils::hexStringToBuffer("0", 1, buffer);
  EXPECT_EQ(bufferEnd - buffer, 0);
  EXPECT_EQ(buffer[0], 0);

  bufferEnd = Utils::hexStringToBuffer("1C1e1769", 8, buffer);
  EXPECT_EQ(bufferEnd - buffer, 3);
  EXPECT_EQ(buffer[0], 28);
  EXPECT_EQ(buffer[1], 30);
  EXPECT_EQ(buffer[2], 23);
  EXPECT_EQ(buffer[3], 105);

  bufferEnd = Utils::hexStringToBuffer("D55f92Fd5", 9, buffer);
  EXPECT_EQ(bufferEnd - buffer, 4);
  EXPECT_EQ(buffer[0], 13);
  EXPECT_EQ(buffer[1], 85);
  EXPECT_EQ(buffer[2], 249);
  EXPECT_EQ(buffer[3], 47);
  EXPECT_EQ(buffer[4], 213);
}

TEST(Utils, hexStringToBufferNullTerminated) {
  Utils::Byte buffer[5];
  Utils::Buffer bufferEnd;

  bufferEnd = Utils::hexStringToBuffer("", buffer);
  EXPECT_EQ(bufferEnd, nullptr);

  bufferEnd = Utils::hexStringToBuffer("0", buffer);
  EXPECT_EQ(bufferEnd - buffer, 0);
  EXPECT_EQ(buffer[0], 0);

  bufferEnd = Utils::hexStringToBuffer("1C1e1769", buffer);
  EXPECT_EQ(bufferEnd - buffer, 3);
  EXPECT_EQ(buffer[0], 28);
  EXPECT_EQ(buffer[1], 30);
  EXPECT_EQ(buffer[2], 23);
  EXPECT_EQ(buffer[3], 105);

  bufferEnd = Utils::hexStringToBuffer("D55f92Fd5", buffer);
  EXPECT_EQ(bufferEnd - buffer, 4);
  EXPECT_EQ(buffer[0], 13);
  EXPECT_EQ(buffer[1], 85);
  EXPECT_EQ(buffer[2], 249);
  EXPECT_EQ(buffer[3], 47);
  EXPECT_EQ(buffer[4], 213);
}

TEST(Utils, bufferToHexString) {
  Utils::Byte buffer[5];
  char out[10];
  char *outEnd;

  outEnd = Utils::bufferToHexString(buffer, 0, out);
  EXPECT_EQ(outEnd, nullptr);

  buffer[0] = 0;
  outEnd = Utils::bufferToHexString(buffer, 1, out);
  EXPECT_EQ(outEnd - out, 1);
  EXPECT_EQ(memcmp("00", out, 2), 0);

  buffer[0] = 28;
  buffer[1] = 30;
  buffer[2] = 23;
  buffer[3] = 105;
  outEnd = Utils::bufferToHexString(buffer, 4, out);
  EXPECT_EQ(outEnd - out, 7);
  EXPECT_EQ(memcmp("1c1e1769", out, 8), 0);

  buffer[0] = 13;
  buffer[1] = 85;
  buffer[2] = 249;
  buffer[3] = 47;
  buffer[4] = 213;
  outEnd = Utils::bufferToHexString(buffer, 5, out);
  EXPECT_EQ(outEnd - out, 9);
  EXPECT_EQ(memcmp("0d55f92fd5", out, 10), 0);
}

TEST(Utils, bufferToHexStringNullTerminated) {
  Utils::Byte buffer[5];
  char out[11];
  char *outEnd;

  outEnd = Utils::bufferToHexString(buffer, 0, out, true);
  EXPECT_EQ(outEnd, nullptr);

  buffer[0] = 0;
  outEnd = Utils::bufferToHexString(buffer, 1, out, true);
  EXPECT_EQ(outEnd - out, 2);
  ASSERT_STREQ("00", out);

  buffer[0] = 28;
  buffer[1] = 30;
  buffer[2] = 23;
  buffer[3] = 105;
  outEnd = Utils::bufferToHexString(buffer, 4, out, true);
  EXPECT_EQ(outEnd - out, 8);
  ASSERT_STREQ("1c1e1769", out);

  buffer[0] = 13;
  buffer[1] = 85;
  buffer[2] = 249;
  buffer[3] = 47;
  buffer[4] = 213;
  outEnd = Utils::bufferToHexString(buffer, 5, out, true);
  EXPECT_EQ(outEnd - out, 10);
  ASSERT_STREQ("0d55f92fd5", out);
}

TEST(Utils, getMostSignificantNonZeroByte) {
  EXPECT_EQ(Utils::getMostSignificantNonZeroByte(0), 0);
  EXPECT_EQ(Utils::getMostSignificantNonZeroByte(1), 1);
  EXPECT_EQ(Utils::getMostSignificantNonZeroByte(471734121), 28);
  EXPECT_EQ(Utils::getMostSignificantNonZeroByte(57276968917), 13);
}

TEST(Utils, intToBuffer) {
  Utils::Byte buffer[8];
  Utils::Buffer bufferEnd;

  bufferEnd = Utils::intToBuffer(0, buffer);
  EXPECT_EQ(bufferEnd - buffer, 0);
  EXPECT_EQ(buffer[0], 0);

  bufferEnd = Utils::intToBuffer(1, buffer);
  EXPECT_EQ(bufferEnd - buffer, 0);
  EXPECT_EQ(buffer[0], 1);

  bufferEnd = Utils::intToBuffer(471734121, buffer);
  EXPECT_EQ(bufferEnd - buffer, 3);
  EXPECT_EQ(buffer[0], 28);
  EXPECT_EQ(buffer[1], 30);
  EXPECT_EQ(buffer[2], 23);
  EXPECT_EQ(buffer[3], 105);

  bufferEnd = Utils::intToBuffer(57276968917, buffer);
  EXPECT_EQ(bufferEnd - buffer, 4);
  EXPECT_EQ(buffer[0], 13);
  EXPECT_EQ(buffer[1], 85);
  EXPECT_EQ(buffer[2], 249);
  EXPECT_EQ(buffer[3], 47);
  EXPECT_EQ(buffer[4], 213);
}