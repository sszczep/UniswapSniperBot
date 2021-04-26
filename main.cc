#include <charconv>

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include <config.hpp>
#include <utils.hpp>
#include <transaction.hpp>
#include <bot.hpp>

// websocketpp includes

#define ASIO_STANDALONE

#ifdef WS_TLS
  #include <websocketpp/config/asio_client.hpp>
  using AsioClientConfig = websocketpp::config::asio_tls_client;
#else
  #include <websocketpp/config/asio_no_tls_client.hpp>
  using AsioClientConfig = websocketpp::config::asio_client;
#endif

#include <websocketpp/client.hpp>

struct CustomWSConfig : public AsioClientConfig {
  static const std::size_t connection_read_buffer_size = 1024;
  static const bool enable_multithreading = false;
};

// Global variables, do not do that at home kids

Utils::Byte privateKey[32];
Transaction tx;
char pregenTxs[Config::TransactionPreGen::ArraySize][Config::Size::BloXrouteTransactionMessageString];

websocketpp::client<CustomWSConfig> wsClient;
websocketpp::connection_hdl wsConnectionHdl;
websocketpp::client<CustomWSConfig>::timer_ptr wsTimer;

// Forward declare functions

#ifdef WS_TLS
  websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context> onTLSInit(websocketpp::connection_hdl);
#endif

void onOpen(websocketpp::connection_hdl connectionHdl);
void onMessage(websocketpp::connection_hdl connectionHdl, websocketpp::client<CustomWSConfig>::message_ptr message);
void onClose(websocketpp::connection_hdl connectionHdl);
void sendPing(__attribute__((unused)) websocketpp::lib::error_code const &errorCode);
void setTimer();

int main () {
  // Convert private key to buffer

  Utils::hexStringToBuffer(Config::Transaction::PrivateKey, privateKey);

  // Generate transaction data

  char data[TransactionDataBuilder::DataLength + 1];
  TransactionDataBuilder::buildData(
    Config::Transaction::SwapExactETHForTokens::AmountOutMin, 
    Config::Transaction::SwapExactETHForTokens::TokenAddress, 
    Config::Transaction::SwapExactETHForTokens::ReceiverAddress,
    data
  );

  // Print debug info

  uint64_t gasLimit, value;
  std::from_chars(Config::Transaction::GasLimit, Config::Transaction::GasLimit + strlen(Config::Transaction::GasLimit), gasLimit, 16);
  std::from_chars(Config::Transaction::Value, Config::Transaction::Value + strlen(Config::Transaction::Value), value, 16);
  printf("Transaction fields:\n");
  printf("Nonce: %s\n", Config::Transaction::Nonce);
  printf("Gas price: to be determined\n");
  printf("Gas limit: %" PRIu64 "\n", gasLimit);
  printf("To: 0x%s\n", Config::Transaction::To);
  printf("Value: %" PRIu64 " wei\n", value);
  printf("Data: 0x%s\n", data);

  printf("\nListener filters:\n");
  printf("Maximum gas price: %s wei\n", Config::BloXroute::Filters::MaxGasPrice);
  printf("Minimum value: %s wei\n", Config::BloXroute::Filters::MinValue);

  // Set transaction fields

  tx.setField(Transaction::Field::Nonce, Config::Transaction::Nonce);
  tx.setField(Transaction::Field::GasLimit, Config::Transaction::GasLimit);
  tx.setField(Transaction::Field::To, Config::Transaction::To);
  tx.setField(Transaction::Field::Value, Config::Transaction::Value);
  tx.setField(Transaction::Field::Data, data);
  
  // Pregenerate transactions

  printf("\nPregenerating transactions...\n");

  Utils::Byte transactionBuffer[Config::Size::TransactionRawBuffer];
  char transactionString[Config::Size::TransactionRawBuffer * 2];

  for(
    std::size_t gasPrice = Config::TransactionPreGen::GasPriceGweiFrom * Config::TransactionPreGen::GasPriceGweiDecimals; 
    gasPrice <= Config::TransactionPreGen::GasPriceGweiTo * Config::TransactionPreGen::GasPriceGweiDecimals; 
    gasPrice++
  ) {
    Utils::Byte gasPriceBuffer[8];
    std::size_t gasPriceBufferSize = Utils::intToBuffer(
      gasPrice * (1000000000 / Config::TransactionPreGen::GasPriceGweiDecimals), 
      gasPriceBuffer
    );
    tx.setField(Transaction::Field::GasPrice, gasPriceBuffer, gasPriceBufferSize);

    std::size_t transactionBufferSize = tx.sign(privateKey, transactionBuffer);

    Utils::bufferToHexString(transactionBuffer, transactionBufferSize, transactionString, true);

    BloXrouteMessageBuilder::buildTransaction(
      transactionString, 
      pregenTxs[gasPrice - Config::TransactionPreGen::GasPriceGweiFrom * Config::TransactionPreGen::GasPriceGweiDecimals]
    );
  }

  printf(
    "Successfully pregenerated transactions with gas price from %" PRIu64 " to %" PRIu64 " gwei (%zu in total)\n",
    Config::TransactionPreGen::GasPriceGweiFrom,
    Config::TransactionPreGen::GasPriceGweiTo,
    Config::TransactionPreGen::ArraySize   
  );

  // Connect to BloXroute Cloud API

  printf("\nConnecting to %s...\n", Config::BloXroute::Connection::Address);

  wsClient.init_asio();
  wsClient.clear_access_channels(websocketpp::log::alevel::all);
  wsClient.clear_error_channels(websocketpp::log::elevel::all);

  #ifdef WS_TLS
    wsClient.set_tls_init_handler(onTLSInit);
  #endif

  wsClient.set_open_handler(onOpen);
  wsClient.set_message_handler(onMessage);
  wsClient.set_close_handler(onClose);

  websocketpp::lib::error_code errorCode;
  websocketpp::client<CustomWSConfig>::connection_ptr wsConnection = wsClient.get_connection(Config::BloXroute::Connection::Address, errorCode);
  if(errorCode) exit(1);

  wsConnection->append_header("Authorization", Config::BloXroute::Connection::AuthToken);

  wsClient.connect(wsConnection);

  wsClient.run();
}

#ifdef WS_TLS
  websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context> onTLSInit(websocketpp::connection_hdl) {
    websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context> context = 
      websocketpp::lib::make_shared<asio::ssl::context>(asio::ssl::context::sslv23);

    context->set_options(
        asio::ssl::context::default_workarounds 
      | asio::ssl::context::no_sslv2 
      | asio::ssl::context::no_sslv3 
      | asio::ssl::context::single_dh_use
    );

    context->set_verify_mode(asio::ssl::verify_none);

    return context;
  }
#endif

void onOpen(websocketpp::connection_hdl connectionHdl) {
  wsConnectionHdl = connectionHdl;

  char message[256];
  BloXrouteMessageBuilder::buildSubscribe(Config::BloXroute::Filters::MinValue, Config::BloXroute::Filters::MaxGasPrice, message);
  wsClient.send(connectionHdl, message, websocketpp::frame::opcode::text);
  printf("Sent subscribe message\n");
  printf("Listening on Cloud API...\n");

  // Ping connection every 30 seconds
  setTimer();
}

void onMessage(websocketpp::connection_hdl connectionHdl, websocketpp::client<CustomWSConfig>::message_ptr message) {
  char *messageStr = (char*) message->get_payload().c_str();

  if(!BloXrouteMessageParser::validateTransaction(messageStr, Config::BloXroute::Filters::TokenAddress)) {
    printf("\nReceived message: %s\n", messageStr);
    return;
  }

  // Get gas price from transaction

  char gasPriceStr[Config::Size::TransactionQuantityBuffer * 2 + 1];
  std::size_t gasPriceStrLength = BloXrouteMessageParser::extractGasPrice(messageStr, gasPriceStr);

  if(gasPriceStrLength <= 16) {
    uint64_t gasPrice = 0;
    std::from_chars(gasPriceStr, gasPriceStr + gasPriceStrLength, gasPrice, 16);

    if(gasPrice % (1000000000 / Config::TransactionPreGen::GasPriceGweiDecimals) == 0) {
      gasPrice /= (1000000000 / Config::TransactionPreGen::GasPriceGweiDecimals);

      if(
        gasPrice >= Config::TransactionPreGen::GasPriceGweiFrom * Config::TransactionPreGen::GasPriceGweiDecimals
        && gasPrice <= Config::TransactionPreGen::GasPriceGweiTo * Config::TransactionPreGen::GasPriceGweiDecimals
      ) {
        wsClient.send(connectionHdl, pregenTxs[gasPrice - Config::TransactionPreGen::GasPriceGweiFrom * Config::TransactionPreGen::GasPriceGweiDecimals], websocketpp::frame::opcode::text);
        printf("\nReceived message: %s\n", messageStr);
        printf("Sent pregenerated transaction: %s\n", pregenTxs[gasPrice - Config::TransactionPreGen::GasPriceGweiFrom * Config::TransactionPreGen::GasPriceGweiDecimals]);
        printf("\nClosing connection...\n");
        wsClient.close(connectionHdl, websocketpp::close::status::normal, "Connection closed by client");
        return;
      }
    }

    tx.setField(Transaction::Field::GasPrice, gasPriceStr);

    Utils::Byte transactionBuffer[Config::Size::TransactionRawBuffer]; 
    std::size_t transactionBufferSize = tx.sign(privateKey, transactionBuffer);

    char transactionString[Config::Size::TransactionRawBuffer * 2];
    Utils::bufferToHexString(transactionBuffer, transactionBufferSize, transactionString, true);

    char message[Config::Size::BloXrouteTransactionMessageString];
    BloXrouteMessageBuilder::buildTransaction(transactionString, message);

    wsClient.send(connectionHdl, message, websocketpp::frame::opcode::text);
    printf("\nReceived message: %s\n", messageStr);
    printf("Sent transaction: %s\n", message);
    printf("\nClosing connection...\n");
    wsClient.close(connectionHdl, websocketpp::close::status::normal, "Connection closed by client.");
  }
}

void onClose(websocketpp::connection_hdl connectionHdl) {
  wsTimer->cancel();
  
  websocketpp::client<CustomWSConfig>::connection_ptr connection = wsClient.get_con_from_hdl(connectionHdl);
  printf(
    "Connection closed, code: %s, reason: %s\n", 
    websocketpp::close::status::get_string(connection->get_remote_close_code()).c_str(),
    connection->get_remote_close_reason().c_str()
  );
}

void sendPing(websocketpp::lib::error_code const &errorCode) {
  if(errorCode) return;

  wsClient.ping(wsConnectionHdl, "");
  setTimer();
}

void setTimer() {
  wsTimer = wsClient.set_timer(30000, sendPing);
}