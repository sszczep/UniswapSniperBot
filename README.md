![project logo](https://github.com/sszczep/UniswapSniperBot/blob/main/img/logo.png?raw=true)

###### Optimized, fast and unsafe Uniswap sniping bot for buying new listings. 

# Table of content
- [How does it work?](https://github.com/sszczep/UniswapSniperBot#how-does-it-work)
  - [Pregeneration](https://github.com/sszczep/UniswapSniperBot#pregeneration)
- [Used libraries](https://github.com/sszczep/UniswapSniperBot#used-libraries)
- [Project structure](https://github.com/sszczep/UniswapSniperBot#project-structure)
  - [Directory structure](https://github.com/sszczep/UniswapSniperBot#directory-structure)
  - [Headers](https://github.com/sszczep/UniswapSniperBot#headers) 
- [Configuration](https://github.com/sszczep/UniswapSniperBot#configuration)
- [Installation guide](https://github.com/sszczep/UniswapSniperBot#installation-guide)
  - [Installing required packages on Debian](https://github.com/sszczep/UniswapSniperBot#installing-required-packages-on-debian) 
  - [Cloning repository](https://github.com/sszczep/UniswapSniperBot#cloning-repository)
  - [Building and running main executable](https://github.com/sszczep/UniswapSniperBot#building-and-running-main-executable)
  - [Building and running tests](https://github.com/sszczep/UniswapSniperBot#building-and-running-tests)
  - [Building and running benchmarks](https://github.com/sszczep/UniswapSniperBot#building-and-running-benchmarks)
  - [Generating documentation](https://github.com/sszczep/UniswapSniperBot#generating-documentation)
- [Documentation](https://sszczep.github.io/UniswapSniperBot)

# How does it work?
We use **BloXroute's** [streams](https://docs.bloxroute.com/streams/newtxs-and-pendingtxs) to listen to liquidity add transaction and call *swapExactETHForTokens* on **Uniswap V2 Router 02** contract. 
By sending our transaction with the same gas price, we have a very high chance of being very close to the original transaction in the block, hence buying tokens just after liquidity add and just before the price significantly rises. 

## Pregeneration
![pregen-diagram](https://github.com/sszczep/UniswapSniperBot/blob/main/img/pregen-diagram.png?raw=true)
We pregenerate transactions with the most probable gas prices to send them instantly, hence skip signing process which is rather slow. This solution turned out to be around **2.5x faster** on our testing machines.

# Used libraries
* [zaphoyd/websocketpp](https://github.com/zaphoyd/websocketpp)
* [bitcoin-core/secp256k1](https://github.com/bitcoin-core/secp256k1)
* [XKCP/XKCP](https://github.com/XKCP/XKCP)
* [google/googletest](https://github.com/google/googletest)
* [google/benchmark](https://github.com/google/benchmark)
* [doxygen/doxygen](https://github.com/doxygen/doxygen)
* [kcwongjoe/doxygen_theme_flat_design](https://github.com/kcwongjoe/doxygen_theme_flat_design)

# Project structure

## Directory structure
`libs/` - contains all external libraries, see [Used libraries](https://github.com/sszczep/UniswapSniperBot#used-libraries)  
`includes/` - contains all headers  
`tests/` - contains code testing  
`benchmarks/` - contains code benchmarking  
`doxygen/` - contains **Doxygen** configuration  
`img/` - contains images  
`libs.build/` - contains built libraries  
`build/` - contains built executables  
`docs/` - contains generated documentation

## Headers
`includes/utils.hpp` - converters and other utilities  
`includes/rlp.hpp` - Recursive Length Prefix Encoding used to serialize objects in Ethereum  
`includes/transaction.hpp` - creating and signing Ethereum transactions  
`includes/bot.hpp` - tools to parse **BloXroute** messages, build transaction data, etc.  
`includes/config.hpp` - configuration file, see [Configuration](https://github.com/sszczep/UniswapSniperBot#configuration)

# Configuration
Configuration is saved in `includes/config.hpp`.

- Config
  - Config::Transaction - transaction fields
    - Config::Transaction::Nonce - transaction nonce (hexadecimal)
    - Config::Transaction::Value - transaction value (hexadecimal, wei)
    - Config::Transaction::To - receiver of the transaction, mostly **Uniswap V2 Router 02** (address)
    - Config::Transaction::GasLimit - transaction gas limit (hexadecimal)
    - Config::Transaction::PrivateKey - private key of sending wallet
  - Config::Transaction::SwapExactETHForTokens - values to construct transaction data to call *SwapExactETHForTokens* method
    - Config::Transaction::SwapExactETHForTokens::AmountOutMin - minimum amount of tokens to receive from the swap (hexadecimal)
    - Config::Transaction::SwapExactETHForTokens::TokenAddress - token's address we want to buy (address)
    - Config::Transaction::SwapExactETHForTokens::ReceiverAddress - address of receiving wallet (address)
  - Config::BloXroute
    - Config::BloXroute::Connection - **BloXroute** Cloud API connection credentials
      - Config::BloXroute::Connection::Address - address of the server
      - Config::BloXroute::Connection::AuthToken - authorization token
    - Config::BloXroute::Filters - newTxs stream filters
      - Config::BloXroute::Filters::MaxGasPrice - maximum gas price of the transaction (we do not want to lose millions on gas, do we?) (decimal, wei)
      - Config::BloXroute::Filters::MinValue - minimum transaction value, skips fake liquidity adds or tokens with small liquidity (decimal, wei)
      - Config::BloXroute::Filters::TokenAddress - alias for `Config::SwapExactETHForTokens::TokenAddress`, left for consistency (**do not change!**)
  - Config::TransactionPreGen - configuration for transaction pregeneration, for further explanation see [Pregeneration](https://github.com/sszczep/UniswapSniperBot#pregeneration)
    - Config::TransactionPreGen::GasPriceGweiFrom - from gwei
    - Config::TransactionPreGen::GasPriceGweiTo - to gwei
    - Config::TransactionPreGen::GasPriceGweiDecimals - gwei decimals (eg. 1000 means generating transactions with gas price steps of 0.001 gwei)
    - Config::TransactionPreGen::ArraySize - precalculated based on above values (**do not change!**)
  - Config::Size
    - Config::Size::TransactionQuantityBuffer - size of transaction quantity buffer (**do not change!**)
    - Config::Size::TransactionAddressBuffer - size of transaction address buffer (**do not change!**)
    - Config::Size::TransactionDataBuffer - size of transaction data buffer, change when necessary (eg. when calling different method requiring more arguments)
    - Config::Size::TransactionRawBuffer - size of raw signed transaction, change when necessary (see above)
    - Config::Size::BloXrouteTransactionMessageString - size of both incoming and outcoming messages to the Cloud API

# Installation guide

## Installing required packages on Debian
```
sudo apt update
sudo apt install git make build-essential cmake autoconf libtool xsltproc libasio-dev
```

###### If you use macOS, you might have to manually link the OpenSSL 
```
ln -s /usr/local/opt/openssl/lib/*.dylib /usr/local/lib/
ln -s /usr/local/opt/openssl/lib/*.a /usr/local/lib/
```

## Cloning repository
```
git clone --recurse-submodules -j8 https://github.com/sszczep/UniswapSniperBot.git
cd UniswapSniperBot
```

## Building and running main executable
```
make main
./build/main
```

## Building and running tests
```
make test
./build/test
```

## Building and running benchmarks
```
make benchmark
./build/benchmark
```

## Generating documentation
```
make docs
```

###### Documentation is available [here](https://sszczep.github.io/UniswapSniperBot/).
