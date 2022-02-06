#ifndef YMIR_DUNGEON_CHEST_PLACER_HPP
#define YMIR_DUNGEON_CHEST_PLACER_HPP

#include <ymir/Dungeon/Context.hpp>
#include <ymir/Dungeon/RandomBuilder.hpp>

namespace ymir::Dungeon {
template <typename TileType, typename TileCord, typename RandomEngineType>
class ChestPlacer : public RandomBuilder<RandomEngineType> {
public:
  static const char *Type;

public:
  using RandomBuilder<RandomEngineType>::RandomBuilder;

  const char *getType() const override { return Type; }

  void init(BuilderPass &Pass, BuilderContext &C) override;
  void run(BuilderPass &Pass, BuilderContext &C) override;

private:
  std::string Layer;
  std::optional<TileType> Wall;
  std::optional<TileType> Chest;
  float RoomChestPercentage = 5.0;
};

template <typename T, typename U, typename RE>
const char *ChestPlacer<T, U, RE>::Type = "chest_placer";

template <typename T, typename U>
std::vector<ymir::Dungeon::Object<U>>
findPossibleChestLocations(const Dungeon::Room<T, U> &Room, T Ground, T Wall) {
  std::vector<ymir::Dungeon::Object<U>> Locations;
  Room.M.forEach(
      [&Locations, &Room, Ground, Wall](ymir::Point2d<U> Pos, const T &Tile) {
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
                     RndEngType RndEng, float RoomChestPercentage) {
  std::uniform_real_distribution<float> Uni(0.0, 100);
  for (const auto &Room : Rooms) {
    if (Uni(RndEng) > RoomChestPercentage) {
      continue;
    }
    auto Locations = findPossibleChestLocations(Room, Ground, Wall);
    if (Locations.empty()) {
      continue;
    }
    auto It = randomIterator(Locations.begin(), Locations.end(), RndEng);
    M.setTile(It->Pos, Chest);
  }
}

template <typename T, typename U, typename RE>
void ChestPlacer<T, U, RE>::init(BuilderPass &Pass, BuilderContext &C) {
  BuilderBase::init(Pass, C);
  Layer = this->template getCfg<std::string>("layer");
  Wall = this->template getCfg<T>("wall", "dungeon/wall");
  Chest = this->template getCfg<T>("chest", "dungeon/chest");
  RoomChestPercentage = this->template getCfg<float>("room_chest_percentage");
}

template <typename T, typename U, typename RE>
void ChestPlacer<T, U, RE>::run(BuilderPass &Pass, BuilderContext &C) {
  BuilderBase::run(Pass, C);
  auto &Ctx = C.get<Context<T, U>>();
  // TODO drop the map all together and add objects to rooms instead
  addRandomChests(Ctx.Map.get(Layer), T(), *Wall, *Chest, Ctx.Rooms,
                  this->RndEng, RoomChestPercentage);
}

} // namespace ymir::Dungeon

#endif // #ifndef YMIR_DUNGEON_CHEST_PLACER_HPP