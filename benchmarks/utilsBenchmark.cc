#include <benchmark/benchmark.h>

#include <utils.hpp>

static void hexCharToByte(benchmark::State &state) {
  for(auto _ : state) {
    benchmark::DoNotOptimize(Utils::hexCharToByte('f'));
  }

  state.SetItemsProcessed(state.iterations());
}

static void byteToHexChar(benchmark::State &state) {
  for(auto _ : state) {
    benchmark::DoNotOptimize(Utils::byteToHexChar(0x0f));
  }

  state.SetItemsProcessed(state.iterations());
}

static void hexStringToBuffer(benchmark::State &state) {
  Utils::Byte output[8];

  for(auto _ : state) {
    benchmark::DoNotOptimize(Utils::hexStringToBuffer("ffffffffffffffff", 16, output));
  }

  state.SetItemsProcessed(state.iterations());
}

static void hexStringToBufferNT(benchmark::State &state) {
  Utils::Byte output[8];

  for(auto _ : state) {
    benchmark::DoNotOptimize(Utils::hexStringToBuffer("ffffffffffffffff", output));
  }

  state.SetItemsProcessed(state.iterations());
}

static void bufferToHexString(benchmark::State &state) {
  Utils::Byte input[8] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
  char output[16];

  for(auto _ : state) {
    benchmark::DoNotOptimize(Utils::bufferToHexString(input, 8, output));
  }

  state.SetItemsProcessed(state.iterations());
}

static void intToBuffer(benchmark::State &state) {
  const std::uint64_t x = 0xFFFFFFFFFFFFFFFF;
  Utils::Byte output[8];

  for(auto _ : state) {
    benchmark::DoNotOptimize(Utils::intToBuffer(x, output));
  }

  state.SetItemsProcessed(state.iterations());
}

BENCHMARK(hexCharToByte)->Name("Utils::hexCharToByte");
BENCHMARK(byteToHexChar)->Name("Utils::byteToHexChar");
BENCHMARK(hexStringToBuffer)->Name("Utils::hexStringToBuffer");
BENCHMARK(hexStringToBufferNT)->Name("Utils::hexStringToBuffer (null-terminated)");
BENCHMARK(bufferToHexString)->Name("Utils::bufferToHexString");
BENCHMARK(intToBuffer)->Name("Utils::intToBuffer");