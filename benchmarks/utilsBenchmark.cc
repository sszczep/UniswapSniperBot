#include <benchmark/benchmark.h>

#include <utils.hpp>

static void hexCharToByte(benchmark::State &state) {
  for(auto _ : state) {
    benchmark::DoNotOptimize(Utils::hexCharToByte('f'));
  }
}

static void byteToHexChar(benchmark::State &state) {
  for(auto _ : state) {
    benchmark::DoNotOptimize(Utils::byteToHexChar(0x0f));
  }
}

static void hexStringToBuffer(benchmark::State &state) {
  Utils::Byte output[32];

  for(auto _ : state) {
    benchmark::DoNotOptimize(Utils::hexStringToBuffer("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff", 64, output));
  }
}

static void hexStringToBufferNT(benchmark::State &state) {
  Utils::Byte output[32];

  for(auto _ : state) {
    benchmark::DoNotOptimize(Utils::hexStringToBuffer("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff", output));
  }
}

static void bufferToHexString(benchmark::State &state) {
  Utils::Byte input[32];
  char output[64];

  memset(input, 0xFF, 32);

  for(auto _ : state) {
    benchmark::DoNotOptimize(Utils::bufferToHexString(input, 32, output));
  }
}

static void intToBuffer(benchmark::State &state) {
  const std::uint64_t x = 0xFFFFFFFFFFFFFFFF;
  Utils::Byte output[8];

  for(auto _ : state) {
    benchmark::DoNotOptimize(Utils::intToBuffer(x, output));
  }
}

BENCHMARK(hexCharToByte)->Name("Utils::hexCharToByte");
BENCHMARK(byteToHexChar)->Name("Utils::byteToHexChar");
BENCHMARK(hexStringToBuffer)->Name("Utils::hexStringToBuffer");
BENCHMARK(hexStringToBufferNT)->Name("Utils::hexStringToBuffer (null-terminated)");
BENCHMARK(bufferToHexString)->Name("Utils::bufferToHexString");
BENCHMARK(intToBuffer)->Name("Utils::intToBuffer");