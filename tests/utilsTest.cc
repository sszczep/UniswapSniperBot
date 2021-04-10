#include <gmock/gmock.h>

#include <utils.hpp>

TEST(Utils, hexCharToByte) {
  ASSERT_THROW(Utils::hexCharToByte('0' - 1), std::invalid_argument);
  ASSERT_EQ(Utils::hexCharToByte('0'), 0);
  ASSERT_EQ(Utils::hexCharToByte('9'), 9);
  ASSERT_THROW(Utils::hexCharToByte('9' + 1), std::invalid_argument);

  ASSERT_THROW(Utils::hexCharToByte('A' - 1), std::invalid_argument);
  ASSERT_EQ(Utils::hexCharToByte('A'), 10);
  ASSERT_EQ(Utils::hexCharToByte('F'), 15);
  ASSERT_THROW(Utils::hexCharToByte('F' + 1), std::invalid_argument);

  ASSERT_THROW(Utils::hexCharToByte('a' - 1), std::invalid_argument);
  ASSERT_EQ(Utils::hexCharToByte('a'), 10);
  ASSERT_EQ(Utils::hexCharToByte('f'), 15);
  ASSERT_THROW(Utils::hexCharToByte('f' + 1), std::invalid_argument);
}

TEST(Utils, byteToHexChar) {
  ASSERT_EQ(Utils::byteToHexChar(0), '0');
  ASSERT_EQ(Utils::byteToHexChar(9), '9');
  ASSERT_EQ(Utils::byteToHexChar(10), 'a');
  ASSERT_EQ(Utils::byteToHexChar(15), 'f');
  ASSERT_THROW(Utils::byteToHexChar(16), std::invalid_argument);
}

TEST(Utils, hexStringToBuffer) {
  char input[9];
  Utils::Byte output[5];
  std::size_t outputLength;

  outputLength = Utils::hexStringToBuffer(input, 0, output);
  ASSERT_EQ(outputLength, 0UL);

  memcpy(input, "0", 1);
  outputLength = Utils::hexStringToBuffer(input, 1, output);
  ASSERT_EQ(outputLength, 1UL);
  ASSERT_EQ(output[0], 0);

  memcpy(input, "1C1e1769", 8);
  outputLength = Utils::hexStringToBuffer(input, 8, output);
  ASSERT_EQ(outputLength, 4UL);
  ASSERT_EQ(output[0], 28);
  ASSERT_EQ(output[1], 30);
  ASSERT_EQ(output[2], 23);
  ASSERT_EQ(output[3], 105);

  memcpy(input, "D55f92Fd5", 9);
  outputLength = Utils::hexStringToBuffer(input, 9, output);
  ASSERT_EQ(outputLength, 5UL);
  ASSERT_EQ(output[0], 13);
  ASSERT_EQ(output[1], 85);
  ASSERT_EQ(output[2], 249);
  ASSERT_EQ(output[3], 47);
  ASSERT_EQ(output[4], 213);

  memcpy(input, "000", 3);
  outputLength = Utils::hexStringToBuffer(input, 3, output, true);
  ASSERT_EQ(outputLength, 0UL);

  memcpy(input, "00012", 5);
  outputLength = Utils::hexStringToBuffer(input, 5, output, true);
  ASSERT_EQ(outputLength, 1UL);
  ASSERT_EQ(output[0], 18);
}

TEST(Utils, hexStringToBufferNullTerminated) {
  Utils::Byte output[5];
  std::size_t outputLength;

  outputLength = Utils::hexStringToBuffer("", output);
  ASSERT_EQ(outputLength, 0UL);

  outputLength = Utils::hexStringToBuffer("0", output);
  ASSERT_EQ(outputLength, 1UL);
  ASSERT_EQ(output[0], 0);

  outputLength = Utils::hexStringToBuffer("1C1e1769", output);
  ASSERT_EQ(outputLength, 4UL);
  ASSERT_EQ(output[0], 28);
  ASSERT_EQ(output[1], 30);
  ASSERT_EQ(output[2], 23);
  ASSERT_EQ(output[3], 105);

  outputLength = Utils::hexStringToBuffer("D55f92Fd5", output);
  ASSERT_EQ(outputLength, 5UL);
  ASSERT_EQ(output[0], 13);
  ASSERT_EQ(output[1], 85);
  ASSERT_EQ(output[2], 249);
  ASSERT_EQ(output[3], 47);
  ASSERT_EQ(output[4], 213);

  outputLength = Utils::hexStringToBuffer("000", output, true);
  ASSERT_EQ(outputLength, 0UL);

  outputLength = Utils::hexStringToBuffer("00012", output, true);
  ASSERT_EQ(outputLength, 1UL);
  ASSERT_EQ(output[0], 18);
}

TEST(Utils, bufferToHexString) {
  Utils::Byte input[5];
  char output[10];
  std::size_t outputLength;

  outputLength = Utils::bufferToHexString(input, 0, output);
  ASSERT_EQ(outputLength, 0UL);

  input[0] = 0;
  outputLength = Utils::bufferToHexString(input, 1, output);
  ASSERT_EQ(outputLength, 2UL);
  ASSERT_THAT(output, ::testing::StartsWith("00"));

  input[0] = 28;
  input[1] = 30;
  input[2] = 23;
  input[3] = 105;
  outputLength = Utils::bufferToHexString(input, 4, output);
  ASSERT_EQ(outputLength, 8UL);
  ASSERT_THAT(output, ::testing::StartsWith("1c1e1769"));

  input[0] = 13;
  input[1] = 85;
  input[2] = 249;
  input[3] = 47;
  input[4] = 213;
  outputLength = Utils::bufferToHexString(input, 5, output);
  ASSERT_EQ(outputLength, 10UL);
  ASSERT_THAT(output, ::testing::StartsWith("0d55f92fd5"));
}

TEST(Utils, bufferToHexStringNullTerminated) {
  Utils::Byte input[5];
  char output[11];
  std::size_t outputLength;

  outputLength = Utils::bufferToHexString(input, 0, output, true);
  ASSERT_EQ(outputLength, 0UL);

  input[0] = 0;
  outputLength = Utils::bufferToHexString(input, 1, output, true);
  ASSERT_EQ(outputLength, 2UL);
  ASSERT_STREQ("00", output);

  input[0] = 28;
  input[1] = 30;
  input[2] = 23;
  input[3] = 105;
  outputLength = Utils::bufferToHexString(input, 4, output, true);
  ASSERT_EQ(outputLength, 8UL);
  ASSERT_STREQ("1c1e1769", output);

  input[0] = 13;
  input[1] = 85;
  input[2] = 249;
  input[3] = 47;
  input[4] = 213;
  outputLength = Utils::bufferToHexString(input, 5, output, true);
  ASSERT_EQ(outputLength, 10UL);
  ASSERT_STREQ("0d55f92fd5", output);
}

TEST(Utils, intToBuffer) {
  Utils::Byte output[8];
  std::size_t outputLength;

  outputLength = Utils::intToBuffer(0, output);
  ASSERT_EQ(outputLength, 1UL);
  ASSERT_EQ(output[0], 0);

  outputLength = Utils::intToBuffer(471734121, output);
  ASSERT_EQ(outputLength, 4UL);
  ASSERT_EQ(output[0], 28);
  ASSERT_EQ(output[1], 30);
  ASSERT_EQ(output[2], 23);
  ASSERT_EQ(output[3], 105);

  outputLength = Utils::intToBuffer(57276968917, output);
  ASSERT_EQ(outputLength, 5UL);
  ASSERT_EQ(output[0], 13);
  ASSERT_EQ(output[1], 85);
  ASSERT_EQ(output[2], 249);
  ASSERT_EQ(output[3], 47);
  ASSERT_EQ(output[4], 213);
}