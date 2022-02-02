#include <utils.hpp>
#include <transaction.hpp>
#include <bot.hpp>

// Utils::Byte privateKey[32];
// Transaction tx;
// char pregenTxs[Config::TransactionPreGen::ArraySize][Config::Size::BloXrouteTransactionMessageString];

int main()
{
  Transaction tx;

  const char privateKeyHex[] = "ed8bbd6198275067fa0b4ddaf2d6f86c0e476b53407daa0b8962272ec9ed50ae";
  Utils::Byte privateKey[32];

  // Convert private key to buffer
  Utils::hexStringToBuffer(privateKeyHex, privateKey);

  // Generate transaction data
  char data[TransactionDataBuilder::DataLength + 1];
  TransactionDataBuilder::buildData(
      "0",
      "f4d2888d29D722226FafA5d9B24F9164c092421E",
      "523abAC73A6c3de879B15bf962A3582CBa488450",
      data);

  // Set transaction fields
  tx.setField(Transaction::Field::Nonce, "0");
  tx.setField(Transaction::Field::GasPrice, "1000000000000");
  tx.setField(Transaction::Field::GasLimit, "30d40");
  tx.setField(Transaction::Field::To, "7a250d5630B4cF539739dF2C5dAcb4c659F2488D");
  tx.setField(Transaction::Field::Value, "0de0b6b3a7640000");
  tx.setField(Transaction::Field::Data, data);

  Utils::Byte transactionBuffer[1024];
  std::size_t transactionBufferSize = tx.sign(privateKey, transactionBuffer);

  char transactionString[1024 * 2];
  Utils::bufferToHexString(transactionBuffer, transactionBufferSize, transactionString, true);

  printf("0x%s\n", transactionString);
}