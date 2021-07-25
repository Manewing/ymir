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
  unsigned RoomIdx = 0;
  for (auto const &Room : DB.getRooms()) {
    Room.M.forEach(
        [&Room, &MapCopy](ymir::Point2d<int> Pos, char Tile) {
          if (Tile == '#') {
            MapCopy.setTile(Room.Pos + Pos, '-');
          }
        });
    int X = 0;
    auto RoomIdxStr = std::to_string(RoomIdx);
    for (const char Char : RoomIdxStr) {
      auto Pos = Room.Pos + ymir::Point2d<int>(Room.rect().Size.W / 2 + X -
                                                   RoomIdxStr.size() / 2,
                                               Room.rect().Size.H / 2);

      MapCopy.setTile(Pos, Char);
      X++;
    }
    ymir::Dungeon::internal::markDoors(MapCopy, Room.Doors, Room.Pos);
    RoomIdx++;
  }
  for (auto const &Hallway : DB.getHallways()) {
    MapCopy.fillRect('x', Hallway.Rect);
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
