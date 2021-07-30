#include <algorithm>
#include <cmath>
#include <iostream>
#include <string_view>
#include <unistd.h>
#include <ymir/CallularAutomata.hpp>
#include <ymir/Dungeon/ChestPlacer.hpp>
#include <ymir/DungeonBuilder.hpp>
#include <ymir/Noise.hpp>

constexpr char Ground = ' ';
constexpr char Wall = '#';
constexpr char Chest = 'C';

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
  unsigned NumRooms = Width * Height / (10 * 10);

  ymir::DungeonBuilder<char, ymir::WyHashRndEng, int> DB({Width, Height}, RE);
  DB.generate(Ground, Wall, NumRooms);

  ymir::Dungeon::addRandomChests(DB.getMap(), Ground, Wall, Chest,
                                 DB.getRooms(), RE);

  auto MapCopy = DB.getMap();
  auto ChestLocs =
      ymir::Dungeon::findPossibleChestLocations(DB.getRooms(), Ground, Wall);
  for (auto const &Chest : ChestLocs) {
    MapCopy.setTile(Chest.Pos, 'C');
  }
  unsigned RoomIdx = 0;
  for (auto const &Room : DB.getRooms()) {
    Room.M.forEach([&Room, &MapCopy](ymir::Point2d<int> Pos, char Tile) {
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
            << "# Rooms: " << DB.getRooms().size() << " / " << NumRooms
            << std::endl;

  unsigned Idx = 0;
  for (const auto &Room : DB.getRooms()) {
    std::cerr << "Room " << Idx << " has " << Room.getNumUsedDoors()
              << " used doors" << std::endl;
    for (const auto &Door : Room.Doors) {
      if (!Door.Used) {
        continue;
      }
      MapCopy.setTile(Room.Pos + Door.Pos, '@'); // std::to_string(Idx)[0]);
    }
    Idx++;
  }
  std::cout << MapCopy << std::endl;
}
