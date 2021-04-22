#include <string>

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

websocketpp::client<CustomWSConfig> wsClient;
websocketpp::connection_hdl connectionHandler;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

#include <config.hpp>
#include <utils.hpp>
#include <transaction.hpp>
#include <bot.hpp>

constexpr std::size_t MinGasPrice = 100;
constexpr std::size_t MaxGasPrice = 1000;

Config cfg("CONFIG");
Transaction tx;
Utils::Byte privateKeyBuffer[32];
char pregeneratedTxs[MaxGasPrice - MinGasPrice][1024];

#ifdef WS_TLS
  websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context> BloXrouteBot::onTLSInit(websocketpp::connection_hdl) {
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

void setTimer();

void sendPing(__attribute__((unused)) websocketpp::lib::error_code const &errorCode) {
  wsClient.ping(connectionHandler, "");
  setTimer();
}

void setTimer() {
  wsClient.set_timer(30000, sendPing);
}

void onOpen(websocketpp::connection_hdl handler) {
  connectionHandler = handler;

  char message[256];
  BloXrouteMessageBuilder::buildSubscribe(cfg.getValue("MIN_VALUE").c_str(), cfg.getValue("MAX_GAS_PRICE").c_str(), message);
  wsClient.send(handler, message, websocketpp::frame::opcode::text);

  setTimer();
}

void onMessage(websocketpp::connection_hdl handler, websocketpp::client<CustomWSConfig>::message_ptr message) {  
  char *messageStr = (char*) message->get_payload().c_str();

  if(BloXrouteMessageParser::validateTransaction(messageStr, cfg.getValue("TOKEN_ADDRESS").c_str())) {
    char gasPriceStr[BloXrouteMessageParser::maxHexLength + 1];
    BloXrouteMessageParser::extractGasPrice(messageStr, gasPriceStr);

    std::size_t gasPrice = std::stoull(gasPriceStr, NULL, 16);
    if(gasPrice % 1000000000 == 0 && gasPrice / 1000000000 >= MinGasPrice && gasPrice / 1000000000 <= MaxGasPrice) {
      wsClient.send(handler, pregeneratedTxs[gasPrice / 1000000000 - MinGasPrice], websocketpp::frame::opcode::text);
    } else {
      tx.setField(Transaction::Field::GasPrice, gasPriceStr);
      Utils::Byte transactionBuffer[512]; 
      std::size_t transactionBufferSize = tx.sign(privateKeyBuffer, transactionBuffer);

      char transactionString[1024];
      Utils::bufferToHexString(transactionBuffer, transactionBufferSize, transactionString, true);

      char message[1024];
      BloXrouteMessageBuilder::buildTransaction(transactionString, message);

      wsClient.send(handler, message, websocketpp::frame::opcode::text);
    }

    printf("Transaction sent\n");
    exit(0);
  }
}

int main() {
  Utils::hexStringToBuffer(cfg.getValue("PRIVATE_KEY").c_str(), privateKeyBuffer);
  
  char data[TransactionDataBuilder::dataLength + 1];
  TransactionDataBuilder::buildData(cfg.getValue("AMOUNT_OUT_MIN").c_str(), cfg.getValue("TOKEN_ADDRESS").c_str(), cfg.getValue("RECEIVER_ADDRESS").c_str(), data);

  // Pregenerate transactions
  tx.setField(Transaction::Field::Nonce, cfg.getValue("NONCE").c_str());
  tx.setField(Transaction::Field::GasLimit, cfg.getValue("GAS_LIMIT").c_str());
  tx.setField(Transaction::Field::To, cfg.getValue("ROUTER_ADDRESS").c_str());
  tx.setField(Transaction::Field::Value, cfg.getValue("VALUE").c_str());
  tx.setField(Transaction::Field::Data, data);

  for(std::size_t gasPrice = MinGasPrice; gasPrice <= MaxGasPrice; gasPrice++) {
    Utils::Byte gasPriceBuffer[8];
    std::size_t gasPriceBufferSize = Utils::intToBuffer(gasPrice * 1000000000, gasPriceBuffer);
    tx.setField(Transaction::Field::GasPrice, gasPriceBuffer, gasPriceBufferSize);

    Utils::Byte transactionBuffer[1024]; 
    std::size_t transactionBufferSize = tx.sign(privateKeyBuffer, transactionBuffer);

    char transactionString[1024];
    Utils::bufferToHexString(transactionBuffer, transactionBufferSize, transactionString, true);

    BloXrouteMessageBuilder::buildTransaction(transactionString, pregeneratedTxs[gasPrice - MinGasPrice]);
  }

  // Setup websocket client
  wsClient.init_asio();
  wsClient.clear_access_channels(websocketpp::log::alevel::all);

  #ifdef WS_TLS
    wsClient.set_tls_init_handler(onTLSInit);
  #endif

  wsClient.set_open_handler(onOpen);
  wsClient.set_message_handler(onMessage);

  websocketpp::lib::error_code errorCode;
  websocketpp::client<CustomWSConfig>::connection_ptr wsConnection = wsClient.get_connection(cfg.getValue("WEBSOCKET_ADDRESS").c_str(), errorCode);
  if(errorCode) exit(1);

  wsConnection->append_header("Authorization", cfg.getValue("AUTH_TOKEN").c_str());

  wsClient.connect(wsConnection);

  wsClient.run();
}