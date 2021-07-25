#include <algorithm>
#include <cmath>
#include <iostream>
#include <string_view>
#include <unistd.h>
#include <ymir/CallularAutomata.hpp>
#include <ymir/DungeonBuilder.hpp>
#include <ymir/Noise.hpp>

void usage(const char *ProgName) {
  std::cerr << "usage: " << ProgName << "<width> <height> <seed>?" << std::endl;
}

int main(int Argc, char *Argv[]) {
  int Width = 80, Height = 24;
  uint64_t Seed = std::random_device()();
  if (Argc == 3) {
    Width = std::stoul(Argv[1]);
    Height = std::stoul(Argv[2]);
  } else if (Argc == 4 || Argc == 5) {
    Width = std::stoul(Argv[1]);
    Height = std::stoul(Argv[2]);
    Seed = std::stoul(Argv[3]);
  } else if (Argc != 1) {
    usage(Argv[0]);
    return 1;
  }

  ymir::WyHashRndEng RE;
  RE.seed(Seed);

  // FIXME room min/max currently fixed to 5,5 - 10,10
  unsigned Rooms = Width * Height / (10 * 10);

  ymir::DungeonBuilder<char, ymir::WyHashRndEng, int> DB({Width, Height}, RE);
  DB.generate(/*Ground=*/' ', /*Wall=*/'#', Rooms);

  auto MapCopy = DB.getMap();
  for (auto const &Room : DB.getRooms()) {
    Room.M.forEach([&Room, &MapCopy](ymir::Point2d<int> Pos, char Tile) {
      if (Tile == '#') {
        MapCopy.setTile(Room.Pos + Pos, '-');
      }
    });
  }
  for (auto const &Hallway : DB.getHallways()) {
    MapCopy.fillRect('x', Hallway);
  }

  std::cout << MapCopy << std::endl
            << std::string(Width, '=') << std::endl
            << std::endl
            << DB.getMap() << std::endl
            << std::endl
            << "Seed: " << Seed << std::endl
            << "# Rooms: " << DB.getRooms().size() << " / " << Rooms
            << std::endl;
}
