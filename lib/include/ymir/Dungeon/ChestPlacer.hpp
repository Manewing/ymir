#ifndef YMIR_DUNGEON_CHEST_PLACER_HPP
#define YMIR_DUNGEON_CHEST_PLACER_HPP

#include <ymir/Dungeon/BuilderBase.hpp>
#include <ymir/Dungeon/Context.hpp>

namespace ymir::Dungeon {

template <typename TileType, typename TileCord>
std::vector<ymir::Dungeon::Object<TileCord>>
findPossibleChestLocations(const Dungeon::Room<TileType, TileCord> &Room,
                           TileType Ground, TileType Wall);

template <typename TileType, typename TileCord>
std::vector<ymir::Dungeon::Object<TileCord>> findPossibleChestLocations(
    const std::list<ymir::Dungeon::Room<TileType, TileCord>> &Rooms,
    TileType Ground, TileType Wall);

// 5% => A chest in every ~20 rooms
template <typename TileType, typename TileCord, typename RndEngType>
void addRandomChests(
    Map<TileType, TileCord> &M, TileType Ground, TileType Wall, TileType Chest,
    const std::list<ymir::Dungeon::Room<TileType, TileCord>> &Rooms,
    RndEngType RndEng, double RoomChestPercentage = 5.0);

template <typename TileType, typename TileCord, typename RandomEngineType>
class ChestPlacer : public BuilderBase {
public:
  static const char *Name;

public:
  ChestPlacer() = default;

  const char *getName() const override { return Name; }

  void run(BuilderPass & /*Pass*/, BuilderContext &C) override {
    auto &Ctx = C.get<Context<TileType, TileCord, RandomEngineType>>();

    // FIXME make this configurable
    const double RoomChestPercentage = 5.0;
    addRandomChests(Ctx.M, Ctx.Ground, Ctx.Wall, Ctx.Chest, Ctx.Rooms,
                    Ctx.RngEng, RoomChestPercentage);
  }
};

template <typename T, typename U, typename RE>
const char *ChestPlacer<T, U, RE>::Name = "chest_placer";

// TODO move
template <typename T, typename U>
std::vector<ymir::Dungeon::Object<U>>
findPossibleChestLocations(const Dungeon::Room<T, U> &Room, T Ground, T Wall) {
  std::vector<ymir::Dungeon::Object<U>> Locations;
  Room.M.forEach([&Locations, &Room, Ground, Wall](ymir::Point2d<U> Pos,
                                                   const T &Tile) {
    if (Tile != Ground || Room.blocksDoor(Pos)) {
      return;
    }
    auto Count = Room.M.getNeighborCount(Pos, Wall);
    if (Count < 3) {
      return;
    }
    Locations.push_back({Pos + Room.Pos, ymir::Dir2d::NONE, false});
  });
  return Locations;
}

template <typename T, typename U>
std::vector<ymir::Dungeon::Object<U>>
findPossibleChestLocations(const std::list<ymir::Dungeon::Room<T, U>> &Rooms,
                           T Ground, T Wall) {
  std::vector<ymir::Dungeon::Object<U>> Locations;
  for (const auto &Room : Rooms) {
    auto Locs = findPossibleChestLocations(Room, Ground, Wall);
    Locations.insert(Locations.end(), Locs.begin(), Locs.end());
  }
  return Locations;
}

// 5% => A chest in every ~20 rooms
template <typename T, typename U, typename RndEngType>
void addRandomChests(Map<T, U> &M, T Ground, T Wall, T Chest,
                     const std::list<ymir::Dungeon::Room<T, U>> &Rooms,
                     RndEngType RndEng, double RoomChestPercentage) {
  std::uniform_real_distribution<double> Uni(0.0, 100);
  for (const auto &Room : Rooms) {
    if (Uni(RndEng) > RoomChestPercentage) {
      continue;
    }
    auto Locations = findPossibleChestLocations(Room, Ground, Wall);
    auto It = randomIterator(Locations.begin(), Locations.end(), RndEng);
    M.setTile(It->Pos, Chest);
  }
}

} // namespace ymir::Dungeon

#endif // #ifndef YMIR_DUNGEON_CHEST_PLACER_HPP