#include <gmock/gmock.h>

#include <transaction.hpp>
#include <utils.hpp>

// All tests were checked against @ethereumjs/tx node library
TEST(Transaction, signWithLeadingZeroes) {
  Transaction tx;

  tx.setField(Transaction::Field::Nonce, "000001");
  tx.setField(Transaction::Field::GasPrice, "0000000000");
  tx.setField(Transaction::Field::GasLimit, "00000010100000");
  tx.setField(Transaction::Field::To, "F0109fC8DF283027b6285cc889F5aA624EaC1F55");
  tx.setField(Transaction::Field::Data, "000000000000000000000000abc");
  tx.setField(Transaction::Field::Value, "0");

  Utils::Byte transaction[512];
  std::size_t transactionLength = tx.sign("4c0883a69102937d6231471b5dbb6204fe5129617082792ae468d01a3f362318", transaction);

  ASSERT_EQ(transactionLength, 113UL);
  Utils::Byte expectedOutput[] = { 248, 111, 1, 128, 132, 16, 16, 0, 0, 148, 240, 16, 159, 200, 223, 40, 48, 39, 182, 40, 92, 200, 137, 245, 170, 98, 78, 172, 31, 85, 128, 142, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 188, 37, 160, 229, 144, 33, 194, 210, 93, 9, 195, 102, 3, 95, 60, 69, 206, 86, 45, 202, 219, 96, 28, 99, 220, 42, 62, 91, 81, 124, 107, 217, 232, 34, 173, 160, 57, 33, 0, 159, 149, 97, 75, 57, 3, 25, 180, 246, 134, 130, 159, 20, 130, 36, 255, 203, 122, 76, 110, 186, 186, 92, 217, 164, 93, 230, 18, 143 };
  ASSERT_TRUE(memcmp(transaction, expectedOutput, 113) == 0);
}

// Example from https://github.com/ChainSafe/web3.js/issues/1170#issuecomment-356988036
TEST(Transaction, signWith63bitR) {
  Transaction tx;

  tx.setField(Transaction::Field::Nonce, "0");
  tx.setField(Transaction::Field::GasPrice, "D55698372431");
  tx.setField(Transaction::Field::GasLimit, "1E8480");
  tx.setField(Transaction::Field::To, "F0109fC8DF283027b6285cc889F5aA624EaC1F55");
  tx.setField(Transaction::Field::Data, "");
  tx.setField(Transaction::Field::Value, "3B9ACA00");

  Utils::Byte transaction[512];
  std::size_t transactionLength = tx.sign("4c0883a69102937d6231471b5dbb6204fe5129617082792ae468d01a3f362318", transaction);

  ASSERT_EQ(transactionLength, 108UL);
  Utils::Byte expectedOutput[] = { 248, 106, 128, 134, 213, 86, 152, 55, 36, 49, 131, 30, 132, 128, 148, 240, 16, 159, 200, 223, 40, 48, 39, 182, 40, 92, 200, 137, 245, 170, 98, 78, 172, 31, 85, 132, 59, 154, 202, 0, 128, 37, 160, 9, 235, 182, 202, 5, 122, 5, 53, 214, 24, 100, 98, 188, 11, 70, 91, 86, 28, 148, 162, 149, 189, 176, 98, 31, 193, 146, 8, 171, 20, 154, 156, 160, 68, 15, 253, 119, 92, 233, 26, 131, 58, 180, 16, 119, 114, 4, 213, 52, 26, 111, 159, 169, 18, 22, 166, 243, 238, 44, 5, 31, 234, 106, 4, 40 };
  ASSERT_TRUE(memcmp(transaction, expectedOutput, 108) == 0);
}

// Example from https://github.com/ChainSafe/web3.js/issues/1170#issuecomment-356988036
TEST(Transaction, signWith62bitRS) {
  Transaction tx;

  tx.setField(Transaction::Field::Nonce, "0");
  tx.setField(Transaction::Field::GasPrice, "0");
  tx.setField(Transaction::Field::GasLimit, "7C6D");
  tx.setField(Transaction::Field::To, "F0109fC8DF283027b6285cc889F5aA624EaC1F55");
  tx.setField(Transaction::Field::Data, "");
  tx.setField(Transaction::Field::Value, "0");

  Utils::Byte transaction[512];
  std::size_t transactionLength = tx.sign("4c0883a69102937d6231471b5dbb6204fe5129617082792ae468d01a3f362318", transaction);

  ASSERT_EQ(transactionLength, 95UL);
  Utils::Byte expectedOutput[] = { 248, 93, 128, 128, 130, 124, 109, 148, 240, 16, 159, 200, 223, 40, 48, 39, 182, 40, 92, 200, 137, 245, 170, 98, 78, 172, 31, 85, 128, 128, 38, 159, 34, 241, 123, 56, 175, 53, 40, 111, 251, 176, 198, 55, 108, 134, 236, 145, 194, 14, 203, 173, 147, 248, 73, 19, 160, 204, 21, 231, 88, 12, 217, 159, 131, 214, 225, 46, 130, 227, 84, 76, 180, 67, 153, 100, 213, 8, 125, 167, 143, 116, 206, 254, 236, 154, 69, 11, 22, 174, 23, 159, 216, 254, 32 };
  ASSERT_TRUE(memcmp(transaction, expectedOutput, 95) == 0);
}