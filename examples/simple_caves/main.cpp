#include <cmath>
#include <iostream>
#include <string_view>
#include <unistd.h>
#include <ymir/CallularAutomata.hpp>
#include <ymir/Map.hpp>
#include <ymir/Noise.hpp>

template <typename TileType, typename U>
void generate_caves(ymir::Map<TileType, U> &M, TileType Ground, TileType Wall,
                    uint64_t Seed) {
  std::default_random_engine RE;
  RE.seed(Seed);

  // Make entire map walls
  M.fillRect(Wall);

  // Generate initial ground tiles
  const float GroundChance = 0.65f;
  ymir::fillRectRandom(
      M, Ground, GroundChance, RE,
      ymir::Rect2d<U>{{1, 1}, {M.getSize().W - 2, M.getSize().H - 2}});

  // Run replacement
  const std::size_t ReplaceThres = 4, Iterations = 6;
  for (std::size_t Idx = 0; Idx < Iterations; Idx++) {
    ymir::celat::replace(M, Ground, Wall, ReplaceThres);
  }

  // Smooth generation
  const std::size_t SmoothThres = 5;
  ymir::celat::generate(M, Ground, SmoothThres);

  // Kill isolated ground
  ymir::celat::generate(M, Wall, 8);
}

void usage(const char *ProgName) {
  std::cerr << "usage: " << ProgName << " <seed>?" << std::endl;
}

int main(int Argc, char *Argv[]) {
  uint64_t Seed = 0;
  if (Argc == 2) {
    if (std::string_view(Argv[1]) == "-h") {
      usage(Argv[0]);
      return 0;
    }
    Seed = std::stoul(Argv[1]);
  } else if (Argc != 1) {
    usage(Argv[0]);
    return 1;
  }
  ymir::Map<char> Map(80, 24);

  generate_caves(Map, ' ', '#', Seed);
  std::cout << Map;
}
