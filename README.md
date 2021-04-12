![project logo](https://github.com/sszczep/UniswapSniperBot/blob/main/logo.jpg?raw=true)

###### Optimized, fast and unsafe Uniswap sniping bot for buying new listings. 

## How does it work?
We use **BloXroute's** [streams](https://docs.bloxroute.com/streams/newtxs-and-pendingtxs) to listen to liquidity add transaction and call *swapExactETHForTokens* on **Uniswap V2 Router 02** contract. 
By sending our transaction with the same gas price, we have a very high chance of being very close to the original transaction in the block, hence buying tokens just after liquidity add and just before the price significantly rises. 

## Used libraries
* [zaphoyd/websocketpp](https://github.com/zaphoyd/websocketpp)
* [bitcoin-core/secp256k1](https://github.com/bitcoin-core/secp256k1)
* [XKCP/XKCP](https://github.com/XKCP/XKCP)
* [google/googletest](https://github.com/google/googletest)
* [google/benchmark](https://github.com/google/benchmark)
* [doxygen/doxygen](https://github.com/doxygen/doxygen)
* [kcwongjoe/doxygen_theme_flat_design](https://github.com/kcwongjoe/doxygen_theme_flat_design)

## Installation guide

### Installing required packages on Debian
```
sudo apt update
sudo apt install git make build-essential cmake autoconf libtool xsltproc libasio-dev
```

###### If you use macOS, you might have to manually link the OpenSSL 
```
ln -s /usr/local/opt/openssl/lib/*.dylib /usr/local/lib/
ln -s /usr/local/opt/openssl/lib/*.a /usr/local/lib/
```

### Cloning repository
```
git clone --recurse-submodules -j8 https://github.com/sszczep/UniswapSniperBot.git
cd UniswapSniperBot
```

### Building and running main executable
```
make main
./build/main
```

### Building and running tests
```
make test
./build/test
```

### Building and running benchmarks
```
make benchmark
./build/benchmark
```

### Generating documentation
```
make docs
```

###### Documentation is available [here](https://sszczep.github.io/UniswapSniperBot/).
