#pragma once

#include <utils.hpp>

/**
 * @brief Utilities to build transaction data to call swapExactETHForTokens on Uniswap V2 Rotuer 02 contract.
 * 
 * @see https://uniswap.org/docs/v2/smart-contracts/router02/#swapexactethfortokens
 */
namespace TransactionDataBuilder
{
  /**
   * @brief Boilerplate transaction data.
   * 
   * Boilerplate transaction data.
   * Includes WETH token address as path[0] and maximum possible deadline.
   * It is missing amountOutMin, path[1] and to values.
   */
  inline constexpr char const *DataBoilerplate{"7ff36ab5000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000800000000000000000000000000000000000000000000000000000000000000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff0000000000000000000000000000000000000000000000000000000000000002000000000000000000000000c02aaa39b223fe8d0a0e5c4f27ead9083c756cc20000000000000000000000000000000000000000000000000000000000000000"};

  /**
   * @brief Boilerplate transaction data length.
   * @see DataBoilerplate
   */
  inline constexpr std::size_t DataLength{std::char_traits<char>::length(DataBoilerplate)};

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
  inline std::size_t buildData(const char *amountOutMin, const char *targetTokenAddress, const char *receiverAddress, char *output)
  {
    std::size_t amountOutMinLength = strlen(amountOutMin);

    memcpy(output, DataBoilerplate, DataLength);
    memcpy(output + 8 + 64 - amountOutMinLength, amountOutMin, amountOutMinLength);
    memcpy(output + 8 + 3 * 64 - 40, receiverAddress, 40);
    memcpy(output + 8 + 7 * 64 - 40, targetTokenAddress, 40);

    output[DataLength] = '\0';

    return DataLength;
  }
}