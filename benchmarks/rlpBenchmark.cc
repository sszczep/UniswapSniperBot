#include <benchmark/benchmark.h>

#include <rlp.hpp>
#include <utils.hpp>

static void encodeLength(benchmark::State &state) {
  Utils::Byte output[2];

  for(auto _ : state) {
    benchmark::DoNotOptimize(RLP::encodeLength(255, 0x80, output));
  }
}

static void encodeItem(benchmark::State &state) {
  Utils::Byte buffer[255];
  memset(buffer, 0xFF, 255);

  RLP::Item item {
    .buffer = buffer,
    .length = 255,
  };

  Utils::Byte output[2 + 255];

  for(auto _ : state) {
    benchmark::DoNotOptimize(RLP::encodeItem(&item, output));
  }
}

static void encodeList(benchmark::State &state) {
  Utils::Byte buffer[255];
  memset(buffer, 0xFF, 255);

  RLP::Item item {
    .buffer = buffer,
    .length = 255,
  };

  RLP::Item items[] = { item, item, item, item, item, item, item, item, item };

  Utils::Byte output[9 + (2 + 255) * 9];

  for(auto _ : state) {
    benchmark::DoNotOptimize(RLP::encodeList(items, 9, output));
  }
}

BENCHMARK(encodeLength)->Name("RLP::encodeLength");
BENCHMARK(encodeItem)->Name("RLP::encodeItem");
BENCHMARK(encodeList)->Name("RLP::encodeList");