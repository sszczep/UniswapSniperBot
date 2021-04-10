#include <gmock/gmock.h>

#include <rlp.hpp>
#include <utils.hpp>

TEST(RLP, encodeLength) {
  Utils::Byte output[2];
  std::size_t outputLength;

  // Single item encoding
  outputLength = RLP::encodeLength(0, 0x80, output);
  ASSERT_EQ(outputLength, 1);
  ASSERT_EQ(output[0], 0x80);

  outputLength = RLP::encodeLength(55, 0x80, output);
  ASSERT_EQ(outputLength, 1);
  ASSERT_EQ(output[0], 0xb7);

  outputLength = RLP::encodeLength(56, 0x80, output);
  ASSERT_EQ(outputLength, 2);
  ASSERT_EQ(output[0], 0xb8);
  ASSERT_EQ(output[1], 56);

  // Payload encoding
  outputLength = RLP::encodeLength(0, 0xc0, output);
  ASSERT_EQ(outputLength, 1);
  ASSERT_EQ(output[0], 0xc0);

  outputLength = RLP::encodeLength(55, 0xc0, output);
  ASSERT_EQ(outputLength, 1);
  ASSERT_EQ(output[0], 0xf7);

  outputLength = RLP::encodeLength(56, 0xc0, output);
  ASSERT_EQ(outputLength, 2);
  ASSERT_EQ(output[0], 0xf8);
  ASSERT_EQ(output[1], 56);
}

TEST(RLP, encodeItem) {
  Utils::Byte inputBuffer[56];
  RLP::Item input = { .buffer = inputBuffer, .length = 0 };
  Utils::Byte output[58];
  std::size_t outputLength;

  input.length = 0;
  outputLength = RLP::encodeItem(&input, output);
  ASSERT_EQ(outputLength, 1);
  ASSERT_EQ(output[0], 0x80);

  input.length = 1;
  input.buffer[0] = 0x00;
  outputLength = RLP::encodeItem(&input, output);
  ASSERT_EQ(outputLength, 1);
  ASSERT_EQ(output[0], 0x00);

  input.length = 1;
  input.buffer[0] = 0x7f;
  outputLength = RLP::encodeItem(&input, output);
  ASSERT_EQ(outputLength, 1);
  ASSERT_EQ(output[0], 0x7f);

  input.length = 1;
  input.buffer[0] = 0x80;
  outputLength = RLP::encodeItem(&input, output);
  ASSERT_EQ(outputLength, 2);
  ASSERT_EQ(output[0], 0x81);
  ASSERT_EQ(output[1], 0x80);

  input.length = 55;
  memset(input.buffer, 0xff, 55);
  outputLength = RLP::encodeItem(&input, output);
  ASSERT_EQ(outputLength, 56);
  ASSERT_EQ(output[0], 0xb7);
  ASSERT_TRUE(memcmp(input.buffer, output + 1, 55) == 0);

  input.length = 56;
  memset(input.buffer, 0xff, 56);
  outputLength = RLP::encodeItem(&input, output);
  ASSERT_EQ(outputLength, 58);
  ASSERT_EQ(output[0], 0xb8);
  ASSERT_EQ(output[1], 56);
  ASSERT_TRUE(memcmp(input.buffer, output + 2, 56) == 0);
}

TEST(RLP, encodeList) {
  Utils::Byte inputBuffer1[54];
  Utils::Buffer inputBuffer2 = nullptr;
  RLP::Item input[] = {{ .buffer = inputBuffer1, .length = 0 }, { .buffer = inputBuffer2, .length = 0 }};
  Utils::Byte output[65];
  std::size_t outputLength;

  // Empty list
  outputLength = RLP::encodeList(input, 0, output);
  ASSERT_EQ(outputLength, 1);
  ASSERT_EQ(output[0], 0xc0);

  // One empty item
  input[0].length = 0;
  outputLength = RLP::encodeList(input, 1, output);
  ASSERT_EQ(outputLength, 2);
  ASSERT_EQ(output[0], 0xc1);
  ASSERT_EQ(output[1], 0x80);

  // One item, payload size = 55 bytes
  input[0].length = 54;
  memset(input[0].buffer, 0xff, 54);
  outputLength = RLP::encodeList(input, 1, output);
  ASSERT_EQ(outputLength, 56);
  ASSERT_EQ(output[0], 0xf7);
  ASSERT_EQ(output[1], 0xb6);
  ASSERT_TRUE(memcmp(input[0].buffer, output + 2, 54) == 0);

  // Two items, payload size = 56 bytes
  input[0].length = 54;
  memset(input[0].buffer, 0xff, 54);
  input[1].length = 0;
  outputLength = RLP::encodeList(input, 2, output);
  ASSERT_EQ(outputLength, 58);
  ASSERT_EQ(output[0], 0xf8);
  ASSERT_EQ(output[1], 56);
  ASSERT_EQ(output[2], 0xb6);
  ASSERT_TRUE(memcmp(input[0].buffer, output + 3, 54) == 0);
  ASSERT_EQ(output[57], 0x80);
}