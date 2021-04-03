#include <rlp.hpp>

#include <gtest/gtest.h>

// TODO: Seperate tests by methods, probably use fixtures
// TODO: Test encode() return value
// TODO: Introduce #define GTEST making all private members public to test internal methods in classes

// We do not test getHexString method without null terminator as it is covered in utilsTest
TEST(RLP, encode) {
  std::pair<Buffer, std::size_t> bufferPair;
  char out[1024];

  // Test examples from https://eth.wiki/fundamentals/rlp
  RLP<1024, 0> rlpEmptyList;
  rlpEmptyList.encode();

  bufferPair = rlpEmptyList.getBuffer();
  EXPECT_EQ(bufferPair.second, 1);
  EXPECT_EQ(bufferPair.first[0], 0xc0);

  rlpEmptyList.getHexString(out, true);
  ASSERT_STREQ(out, "c0");

  RLP<1024, 1> rlpEmptyString;
  rlpEmptyString.setElement(1, "");
  rlpEmptyString.encode();

  bufferPair = rlpEmptyString.getBuffer();
  EXPECT_EQ(bufferPair.second, 2);
  EXPECT_EQ(bufferPair.first[0], 0xc1);
  EXPECT_EQ(bufferPair.first[1], 0x80);

  rlpEmptyString.getHexString(out, true);
  ASSERT_STREQ(out, "c180");

  RLP<1024, 1> rlp0;
  rlp0.setElement(1, "0");
  rlp0.encode();

  bufferPair = rlp0.getBuffer();
  EXPECT_EQ(bufferPair.second, 2);
  EXPECT_EQ(bufferPair.first[0], 0xc1);
  EXPECT_EQ(bufferPair.first[1], 0x00);

  rlp0.getHexString(out, true);
  ASSERT_STREQ(out, "c100");

  RLP<1024, 1> rlp15;
  rlp15.setElement(1, "F");
  rlp15.encode();

  bufferPair = rlp15.getBuffer();
  EXPECT_EQ(bufferPair.second, 2);
  EXPECT_EQ(bufferPair.first[0], 0xc1);
  EXPECT_EQ(bufferPair.first[1], 0x0f);
  
  rlp15.getHexString(out, true);
  ASSERT_STREQ(out, "c10f");

  RLP<1024, 1> rlp1024;
  rlp1024.setElement(1, "400");
  rlp1024.encode();

  bufferPair = rlp1024.getBuffer();
  EXPECT_EQ(bufferPair.second, 4);
  EXPECT_EQ(bufferPair.first[0], 0xc3);
  EXPECT_EQ(bufferPair.first[1], 0x82);
  EXPECT_EQ(bufferPair.first[2], 0x04);
  EXPECT_EQ(bufferPair.first[3], 0x00);

  rlp1024.getHexString(out, true);
  ASSERT_STREQ(out, "c3820400");

  // Random generated ethereum transaction data
  RLP<> rlpTx;
  rlpTx.setElement(1, "bc15e7419cc5c51e0c412061a2bc7489");
  rlpTx.setElement(2, "bdb73b38746da36ce0bbe18fc1c1e368");
  rlpTx.setElement(3, "efac551d7b81f962d01ff3138ea79c61");
  rlpTx.setElement(4, "7a250d5630B4cF539739dF2C5dAcb4c659F2488D");
  rlpTx.setElement(5, "756cc3b144204ca61ace747c687bdfd3");
  rlpTx.setElement(6, "7b5b9e4c5c967e58f70d07a210440f68");
  rlpTx.setElement(7, "1");
  rlpTx.setElement(8, "");
  rlpTx.setElement(9, "");
  rlpTx.encode();
  rlpTx.getHexString(out, true);
  ASSERT_STREQ(out, "f86d90bc15e7419cc5c51e0c412061a2bc748990bdb73b38746da36ce0bbe18fc1c1e36890efac551d7b81f962d01ff3138ea79c61947a250d5630b4cf539739df2c5dacb4c659f2488d90756cc3b144204ca61ace747c687bdfd3907b5b9e4c5c967e58f70d07a210440f68018080");

  rlpTx.setElement(7, "25");
  Byte buffer8[32] = { 0xee, 0x43, 0x27, 0x43, 0xdf, 0xf8, 0x00, 0x5a, 0x4d, 0xb7, 0xf5, 0x8f, 0xca, 0xaa, 0x8c, 0xe6, 0x48, 0xed, 0x23, 0xf8, 0xf9, 0xf8, 0x5d, 0x91, 0x20, 0x23, 0x17, 0x65, 0x03, 0x2d, 0x9f, 0xc9 };
  Byte buffer9[32] = { 0x18, 0xcd, 0xc1, 0x0d, 0xe1, 0x44, 0x0f, 0x71, 0xff, 0xcf, 0x03, 0x9b, 0x02, 0xaa, 0x59, 0xf1, 0x08, 0x32, 0xc4, 0x12, 0xb1, 0x49, 0xdc, 0x44, 0xd6, 0x1a, 0x57, 0x6a, 0xb7, 0x44, 0x5f, 0x82 };
  rlpTx.setElement(8, buffer8, 32);
  rlpTx.setElement(9, buffer9, 32);
  rlpTx.encode();
  rlpTx.getHexString(out, true);
  ASSERT_STREQ(out, "f8ad90bc15e7419cc5c51e0c412061a2bc748990bdb73b38746da36ce0bbe18fc1c1e36890efac551d7b81f962d01ff3138ea79c61947a250d5630b4cf539739df2c5dacb4c659f2488d90756cc3b144204ca61ace747c687bdfd3907b5b9e4c5c967e58f70d07a210440f6825a0ee432743dff8005a4db7f58fcaaa8ce648ed23f8f9f85d9120231765032d9fc9a018cdc10de1440f71ffcf039b02aa59f10832c412b149dc44d61a576ab7445f82");
}

TEST(RLP, throws) {
  RLP<1024, 2> rlp;
  Buffer buffer;
  char out[1024];

  try {
    rlp.setElement(0, "");
    FAIL() << "Expected std::out_of_range";
  } catch(std::out_of_range const &err) {
    EXPECT_EQ(std::string("Position out of range"), err.what());
  } catch(...) {
    FAIL() << "Expected std::out_of_range";
  }

  try {
    rlp.setElement(3, "");
    FAIL() << "Expected std::out_of_range";
  } catch(std::out_of_range const &err) {
    EXPECT_EQ(std::string("Position out of range"), err.what());
  } catch(...) {
    FAIL() << "Expected std::out_of_range";
  }

  try {
    rlp.setElement(2, "");
    FAIL() << "Expected std::logic_error";
  } catch(std::logic_error const &err) {
    EXPECT_EQ(std::string("Previous positions have not been filled"), err.what());
  } catch(...) {
    FAIL() << "Expected std::logic_error";
  }

  try {
    rlp.setElement(0, buffer, 0);
    FAIL() << "Expected std::out_of_range";
  } catch(std::out_of_range const &err) {
    EXPECT_EQ(std::string("Position out of range"), err.what());
  } catch(...) {
    FAIL() << "Expected std::out_of_range";
  }
  
  try {
    rlp.setElement(3, buffer, 0);
    FAIL() << "Expected std::out_of_range";
  } catch(std::out_of_range const &err) {
    EXPECT_EQ(std::string("Position out of range"), err.what());
  } catch(...) {
    FAIL() << "Expected std::out_of_range";
  }

  try {
    rlp.setElement(2, buffer, 0);
    FAIL() << "Expected std::logic_error";
  } catch(std::logic_error const &err) {
    EXPECT_EQ(std::string("Previous positions have not been filled"), err.what());
  } catch(...) {
    FAIL() << "Expected std::logic_error";
  }

  try {
    rlp.encode();
    FAIL() << "Expected std::logic_error";
  } catch(std::logic_error const &err) {
    EXPECT_EQ(std::string("Buffer has not been filled. Cannot encode"), err.what());
  } catch(...) {
    FAIL() << "Expected std::logic_error";
  }

  try {
    rlp.getBuffer();
    FAIL() << "Expected std::logic_error";
  } catch(std::logic_error const &err) {
    EXPECT_EQ(std::string("Buffer has not been encoded"), err.what());
  } catch(...) {
    FAIL() << "Expected std::logic_error";
  }

  try {
    rlp.getHexString(out);
    FAIL() << "Expected std::logic_error";
  } catch(std::logic_error const &err) {
    EXPECT_EQ(std::string("Buffer has not been encoded"), err.what());
  } catch(...) {
    FAIL() << "Expected std::logic_error";
  }
}
