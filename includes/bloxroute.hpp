#pragma once

#define ASIO_STANDALONE
#define WS_TLS

#ifdef WS_TLS
  #include <websocketpp/config/asio_client.hpp>
#else
  #include <websocketpp/config/asio_no_tls_client.hpp>
#endif

#include <websocketpp/client.hpp>

#include <transaction.hpp>
#include <utils.hpp>

using Transaction::ETHTransaction;

namespace BloXroute {
  // Example message: {"jsonrpc":"2.0","id":null,"method":"subscribe","params":{"subscription":"78ab93a9-d72a-4c7e-af3b-b6e53e0f5dff","result":{"txContents":{"input":"0xf305d7190000000000000000000000006de037ef9ad2725eb40118bb1702ebb27e4aeb2400000000000000000000000000000000000000000000003a33adee216bcee43a000000000000000000000000000000000000000000000039e92e572e0dd441ce0000000000000000000000000000000000000000000000000dcef33a6f838000000000000000000000000000bb421ebb2dbcc63db9f1f2f1466eb39a2334870d000000000000000000000000000000000000000000000000000000006064f670","gasPrice":"0x2e19b83c00"}}}}
  
  inline constexpr std::size_t MaxValueHexLength = 21;
  inline constexpr std::size_t MessageMethodOffset = 37;
  inline constexpr std::size_t MessageTokenOffset = 179;
  inline constexpr std::size_t MessageGasPriceOffset = 555;

  /**
   * @brief Boilerplate input data
   * 
   */
  inline constexpr char const* InputDataBoilerplate { "7ff36ab5000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000800000000000000000000000000000000000000000000000000000000000000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff0000000000000000000000000000000000000000000000000000000000000002000000000000000000000000c02aaa39b223fe8d0a0e5c4f27ead9083c756cc20000000000000000000000000000000000000000000000000000000000000000" };

  /**
   * @brief Boilerplate input data length
   * 
   */
  inline constexpr std::size_t InputDataLength { std::char_traits<char>::length(InputDataBoilerplate) };

  #ifdef WS_TLS
    using AsioClientConfig = websocketpp::config::asio_tls_client;
  #else
    using AsioClientConfig = websocketpp::config::asio_client;
  #endif

  struct CustomWSConfig : public AsioClientConfig {
    static const std::size_t connection_read_buffer_size = 1024;
    static const bool enable_multithreading = false;
  };

  using WSClient = websocketpp::client<CustomWSConfig>;

  struct ConnectionConfig {
    const char *address;
    const char *authorizationToken;
  };

  struct ListenerFilters {
    const char *targetTokenAddress;
    const char *minimumLiquidityETH;
    const char *maximumGasPrice;
  };

  struct TransactionInputData {
    const char *amountOutMin;
    const char *receiverAddress;
    const char *targetTokenAddress;
  };

  struct TransactionConfig {
    Byte privateKey[32];
    TransactionInputData *transactionInputData;
    Transaction::ETHValues *transactionValues;
  };

  class BloXrouteBot {
    WSClient mWSClient;
    websocketpp::connection_hdl mConnectionHandler;
    ListenerFilters *mListenerFilters;
    TransactionConfig *mTransactionConfig;
    ETHTransaction mTX;

    char mInputData[InputDataLength + 1];
    char mGasPrice[MaxValueHexLength];

    /**
     * @brief Set input data string based on the boilerplate
     * 
     */
    void setInputData();
    void sendMessage(char *message);
    void sendSubscribeMessage();
    bool validateTransaction(char *message);
    void extractGasPriceFromMessage(char *message);
    void sendTransaction();

    #ifdef WS_TLS
      websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context> onTLSInit(websocketpp::connection_hdl);
    #endif
    void onOpen(websocketpp::connection_hdl connectionHandler);
    void onMessage(websocketpp::connection_hdl connectionHandler, WSClient::message_ptr message);

    public:

    BloXrouteBot(ConnectionConfig *connectionConfig, ListenerFilters *filters, TransactionConfig *transactionConfig);
    void run();
  };
}