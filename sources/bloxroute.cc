#include <bloxroute.hpp>

using BloXroute::BloXrouteBot;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

void BloXrouteBot::setInputData() {
  std::size_t amountOutMinLength = strlen(mTransactionConfig->transactionInputData->amountOutMin);

  memcpy(mInputData, InputDataBoilerplate, InputDataLength);
  memcpy(mInputData + 8 + 64 - amountOutMinLength, mTransactionConfig->transactionInputData->amountOutMin, amountOutMinLength);
  memcpy(mInputData + 8 + 3 * 64 - 40, mTransactionConfig->transactionInputData->receiverAddress, 40);
  memcpy(mInputData + 8 + 7 * 64 - 40, mTransactionConfig->transactionInputData->targetTokenAddress, 40);

  mInputData[InputDataLength] = '\0';
}

void BloXrouteBot::sendMessage(char *message) {
  mWSClient.send(mConnectionHandler, message, websocketpp::frame::opcode::text);
}

void BloXrouteBot::sendSubscribeMessage() {
  char message[512];

  strcpy(message, "{\"method\":\"subscribe\",\"params\":[\"newTxs\",{\"include\":[\"tx_contents.input\", \"tx_contents.gas_price\"],\"filters\":\"method_id = f305d719 and to = 0x7a250d5630B4cF539739dF2C5dAcb4c659F2488D and value >= ");
  strcat(message, mListenerFilters->minimumLiquidityETH);
  strcat(message, " and gas_price <= ");
  strcat(message, mListenerFilters->maximumGasPrice);
  strcat(message, "\"}]}");

  sendMessage(message);
}

bool BloXrouteBot::validateTransaction(char *message) {
  return 
       memcmp(message + MessageMethodOffset, "subscribe", 9) == 0
    && memcmp(message + MessageTokenOffset, mListenerFilters->targetTokenAddress, 40) == 0;
}

void BloXrouteBot::extractGasPriceFromMessage(char *message) {
  char *gasPriceStart = message + MessageGasPriceOffset;
  char *gasPriceEnd = strchr(gasPriceStart, '\"');
  memcpy(mGasPrice, gasPriceStart, gasPriceEnd - gasPriceStart);
  mGasPrice[gasPriceEnd - gasPriceStart] = '\0';
}

void BloXrouteBot::sendTransaction() {
  char rawTransaction[1024];
  mTX.setValues(mTransactionConfig->transactionValues);
  mTX.sign(mTransactionConfig->privateKey);
  mTX.getHexString(rawTransaction, true);

  char message[1024];
  strcpy(message, "{\"method\": \"blxr_tx\", \"params\": {\"transaction\": \"");
  strcat(message, rawTransaction);
  strcat(message, "\"}}");

  sendMessage(message);

  exit(0);
}

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

void BloXrouteBot::onOpen(websocketpp::connection_hdl connectionHandler) {
  mConnectionHandler = connectionHandler;

  sendSubscribeMessage();
}

void BloXrouteBot::onMessage(websocketpp::connection_hdl connectionHandler, WSClient::message_ptr message) {
  char *messageStr = (char*) message->get_payload().c_str();

  if(validateTransaction(messageStr)) {
    extractGasPriceFromMessage(messageStr);
    sendTransaction();
  }
}

BloXrouteBot::BloXrouteBot(ConnectionConfig *connectionConfig, ListenerFilters *listenerFilters, TransactionConfig *transactionConfig)
  : mListenerFilters(listenerFilters), mTransactionConfig(transactionConfig) {
  // Init asio
  mWSClient.init_asio();

  // Suspense all logs
  mWSClient.clear_access_channels(websocketpp::log::alevel::all);

  // Attach listeners

  #ifdef WS_TLS
    mWSClient.set_tls_init_handler(bind(&BloXrouteBot::onTLSInit, this, ::_1));
  #endif 

  mWSClient.set_open_handler(bind(&BloXrouteBot::onOpen, this, ::_1));
  mWSClient.set_message_handler(bind(&BloXrouteBot::onMessage, this, ::_1, ::_2));

  // Create new connection
  websocketpp::lib::error_code errorCode;
  WSClient::connection_ptr wsConnection = mWSClient.get_connection(connectionConfig->address, errorCode);
  if (errorCode) exit(1);

  // Append Authorization header
  wsConnection->append_header("Authorization", connectionConfig->authorizationToken);

  // Set connection
  mWSClient.connect(wsConnection);

  // Link transaction values to mGasPrice and mInputData
  mTransactionConfig->transactionValues->gasPrice = mGasPrice;
  mTransactionConfig->transactionValues->data = mInputData;

  // Generate input data
  setInputData();
}

void BloXrouteBot::run() {
  mWSClient.run();
}