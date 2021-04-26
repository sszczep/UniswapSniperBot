#pragma once

#include <cstdlib>

namespace Config {
  namespace Transaction {
    // Transaction nonce
    inline constexpr char Nonce[] = "1";

    // Transaction value
    inline constexpr char Value[] = "0de0b6b3a7640000";

    // Receiver of the transaction (Uniswap V2 Router 02)
    inline constexpr char To[] = "7a250d5630B4cF539739dF2C5dAcb4c659F2488D";

    // Gas limit of transaction (200000 units is enough for most swapExactETHForTokens calls)
    inline constexpr char GasLimit[] = "30d40";

    // Private key of sending wallet
    inline constexpr char PrivateKey[] = "4c0883a69102937d6231471b5dbb6204fe5129617082792ae468d01a3f362318";

    namespace SwapExactETHForTokens {
      // Minimum amount of tokens to receive from the swap
      inline constexpr char AmountOutMin[] = "3635C9ADC5DEA00000";

      // Token address
      inline constexpr char TokenAddress[] = "48bef6bd05bd23b5e6800cf0406e524b517af250";

      // Address of receiving wallet
      inline constexpr char ReceiverAddress[] = "f82d59152f33E6F65Aa4aE1a3B38eD2Ca1B7633b";
    }
  }

  namespace BloXroute {
    namespace Connection {
      // BloXroute Cloud API server address
      inline constexpr char Address[] = "ws://localhost:3000";

      // BloXroute Cloud API auth token
      inline constexpr char AuthToken[] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
    }

    namespace Filters {
      // Listen for transactions below specified max gas price
      inline constexpr char MaxGasPrice[] = "1000000000000";

      // Listen for transactions above specified min value (avoid tokens with small added liquidity)
      inline constexpr char MinValue[] = "0";

      // Token address, alias to Config::Transaction::SwapExactETHForTokens::TokenAddress
      inline constexpr char *TokenAddress = (char*) Config::Transaction::SwapExactETHForTokens::TokenAddress;
    }
  }

  namespace TransactionPreGen {
    inline constexpr uint64_t GasPriceGweiFrom = 100;
    inline constexpr uint64_t GasPriceGweiTo = 500;
    inline constexpr uint64_t GasPriceGweiDecimals = 100;

    inline constexpr std::size_t ArraySize = (GasPriceGweiTo - GasPriceGweiFrom) * GasPriceGweiDecimals + 1;
  }

  namespace Size {
    inline constexpr std::size_t TransactionQuantityBuffer = 32;
    inline constexpr std::size_t TransactionAddressBuffer = 20;
    inline constexpr std::size_t TransactionDataBuffer = 512;
    inline constexpr std::size_t TransactionRawBuffer = 512;

    inline constexpr std::size_t BloXrouteTransactionMessageString = 1024;
  }
}