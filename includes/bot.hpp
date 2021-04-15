#pragma once

#include <utils.hpp>

/**
 * @brief Utilities to build transaction data to call swapExactETHForTokens on Uniswap V2 Rotuer 02 contract.
 * 
 * @see https://uniswap.org/docs/v2/smart-contracts/router02/#swapexactethfortokens
 */
namespace TransactionDataBuilder {
  /**
   * @brief Boilerplate transaction data.
   * 
   * Boilerplate transaction data.
   * Includes WETH token address as path[0] and maximum possible deadline.
   * It is missing amountOutMin, path[1] and to values.
   */
  inline constexpr char const * dataBoilerplate { "7ff36ab5000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000800000000000000000000000000000000000000000000000000000000000000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff0000000000000000000000000000000000000000000000000000000000000002000000000000000000000000c02aaa39b223fe8d0a0e5c4f27ead9083c756cc20000000000000000000000000000000000000000000000000000000000000000" };

  /**
   * @brief Boilerplate transaction data length.
   * @see dataBoilerplate
   */
  inline constexpr std::size_t dataLength { std::char_traits<char>::length(dataBoilerplate) };

  /**
   * @brief Builds swapExactETHForTokens input data.
   * 
   * @param amountOutMin minimum amount of output tokens
   * @param targetTokenAddress address of output token (path[1])
   * @param receiverAddress (to)
   * @param output 
   * 
   * @return output length
   */
  inline std::size_t buildData(const char *amountOutMin, const char *targetTokenAddress, const char *receiverAddress, char *output) {
    std::size_t amountOutMinLength = strlen(amountOutMin);

    memcpy(output, dataBoilerplate, dataLength);
    memcpy(output + 8 + 64 - amountOutMinLength, amountOutMin, amountOutMinLength);
    memcpy(output + 8 + 3 * 64 - 40, receiverAddress, 40);
    memcpy(output + 8 + 7 * 64 - 40, targetTokenAddress, 40);

    output[dataLength] = '\0';

    return dataLength;
  }
}

/**
 * @brief Utilities to parse incoming BloXroute messages.
 */
namespace BloXrouteMessageParser {
  /**
   * @brief Maximum hex string length (0xFFFFFFFFFFFFFFFF).
   */
  inline constexpr std::size_t maxHexLength = 16;

  /**
   * @brief Position of the method name in the message string.
   */
  inline constexpr std::size_t methodPosition = 37;

  /**
   * @brief Position of the token address in the message string.
   */
  inline constexpr std::size_t tokenPosition = 179;

  /**
   * @brief Position of the gas price hex value in the message string.
   */
  inline constexpr std::size_t gasPricePosition = 555;

  /**
   * @brief Check if message is of "subscribe" method and regards specified token address.
   * 
   * @param message input message
   * @param targetTokenAddress target token address to check against
   * @return boolean value if message is valid and should be further processed
   */
  inline bool validateTransaction(const char *message, const char *targetTokenAddress) {
    return 
         memcmp(message + methodPosition, "subscribe", 9) == 0
      && memcmp(message + tokenPosition, targetTokenAddress, 40) == 0;
  }

  /**
   * @brief Extract gas price hex value from the message.
   * 
   * @param message input message
   * @param output output gas price
   * @return output gas price length
   */
  inline std::size_t extractGasPrice(const char *message, char *output) {
    const char *gasPriceStart = message + gasPricePosition;
    const char *gasPriceEnd = strchr(gasPriceStart, '\"');
    std::size_t gasPriceLength = gasPriceEnd - gasPriceStart;

    memcpy(output, gasPriceStart, gasPriceLength);
    output[gasPriceLength] = '\0';

    return gasPriceLength;
  }
}

/**
 * @brief Utilities to build BloXroute messages.
 */
namespace BloXrouteMessageBuilder {
  /**
   * @brief Builds subscribe message.
   * 
   * @param minimumLiquidityETH minimum transaction value
   * @param maximumGasPrice maximum transactino gas price
   * @param output output message
   * @return output message length
   */
  inline std::size_t buildSubscribe(const char *minimumLiquidityETH, const char *maximumGasPrice, char *output) {
    strcpy(output, "{\"method\":\"subscribe\",\"params\":[\"newTxs\",{\"include\":[\"tx_contents.input\",\"tx_contents.gas_price\"],\"filters\":\"method_id = f305d719 and to = 0x7a250d5630B4cF539739dF2C5dAcb4c659F2488D and value >= ");
    strcat(output, minimumLiquidityETH);
    strcat(output, " and gas_price <= ");
    strcat(output, maximumGasPrice);
    strcat(output, "\"}]}");

    return strlen(output);
  }

  /**
   * @brief Builds transaction message.
   * 
   * @param rawTransaction signed transaction to send
   * @param output output message
   * @return output message length
   */
  inline std::size_t buildTransaction(const char *rawTransaction, char *output) {
    strcpy(output, "{\"method\":\"blxr_tx\",\"params\":{\"transaction\":\"");
    strcat(output, rawTransaction);
    strcat(output, "\"}}");

    return strlen(output);
  }
}