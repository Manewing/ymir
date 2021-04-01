#include "Room.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <string_view>
#include <unistd.h>
#include <ymir/CallularAutomata.hpp>
#include <ymir/Map.hpp>
#include <ymir/Noise.hpp>
#include "DungeonBuilder.hpp"

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

  ymir::WyHashRndEng RE;
  RE.seed(Seed);
  auto Room = ymir::generateRoom<int>(' ', '#', RE);
  // std::cout << Room;

  ymir::DungeonBuilder<char, ymir::WyHashRndEng> DB({80, 24}, RE);
  DB.generate(' ', '#');
  std::cout << DB.getMap();

  //generate_dungeon(Map, ' ', '#', RE);
  //std::cout << Map;
}
