#ifndef YMIR_DUNGEON_ROOM_ENTITY_PLACER_HPP
#define YMIR_DUNGEON_ROOM_ENTITY_PLACER_HPP

#include <ymir/Dungeon/Context.hpp>
#include <ymir/Dungeon/RandomBuilder.hpp>

namespace ymir::Dungeon {
template <typename TileType, typename TileCord, typename RandomEngineType>
class RoomEntityPlacer : public RandomBuilder<RandomEngineType> {
public:
  static const char *Type;
  using BaseType = RandomBuilder<RandomEngineType>;

public:
  using RandomBuilder<RandomEngineType>::RandomBuilder;

  const char *getType() const override { return Type; }

  void init(BuilderPass &Pass, BuilderContext &C) override;
  void run(BuilderPass &Pass, BuilderContext &C) override;

private:
  std::string Layer;
  TileType RoomEntity = TileType();
  float RoomPercentage = 5.0;
  unsigned RoomCountMin = 1;
  unsigned RoomCountMax = 1;
  bool CheckBlocksDoor = true;
};

template <typename T, typename U, typename RE>
const char *RoomEntityPlacer<T, U, RE>::Type = "room_entity_placer";

template <typename T, typename U>
std::vector<ymir::Dungeon::Object<U>>
findPossibleRoomEntityLocations(const Dungeon::Room<T, U> &Room,
                                bool CheckBlocksDoor = true) {
  std::vector<ymir::Dungeon::Object<U>> Locations;
  Room.M.forEach([&Locations, &Room, CheckBlocksDoor](ymir::Point2d<U> Pos,
                                                      const T &Tile) {
    if (Tile != T() || (CheckBlocksDoor && Room.blocksDoor(Pos))) {
      return;
    }
    auto Count = Room.M.getNotNeighborCount(Pos, T());
    if (Count < 3) {
      return;
    }
    Locations.push_back({Pos + Room.Pos, ymir::Dir2d::NONE, false});
  });
  return Locations;
}

template <typename T, typename U>
std::vector<ymir::Dungeon::Object<U>> findPossibleRoomEntityLocations(
    const std::list<ymir::Dungeon::Room<T, U>> &Rooms,
    bool CheckBlocksDoor = true) {
  std::vector<ymir::Dungeon::Object<U>> Locations;
  for (const auto &Room : Rooms) {
    auto Locs = findPossibleRoomEntityLocations(Room, CheckBlocksDoor);
    Locations.insert(Locations.end(), Locs.begin(), Locs.end());
  }
  return Locations;
}

// 5% => A room entity in every ~20 rooms
template <typename T, typename U, typename RndEngType>
void addRandomRoomEntitys(Map<T, U> &M, T RoomEntity,
                          const std::list<ymir::Dungeon::Room<T, U>> &Rooms,
                          RndEngType RndEng, float RoomPercentage,
                          unsigned RoomCountMin, unsigned RoomCountMax,
                          bool CheckBlocksDoor = true) {
  std::uniform_real_distribution<float> Uni(0.0, 100);
  for (const auto &Room : Rooms) {
    if (Uni(RndEng) > RoomPercentage) {
      continue;
    }
    std::uniform_int_distribution<unsigned> RndRoomCount(RoomCountMin,
                                                         RoomCountMax);
    std::size_t RoomCount = RndRoomCount(RndEng);
    auto Locations = findPossibleRoomEntityLocations(Room, CheckBlocksDoor);
    std::shuffle(Locations.begin(), Locations.end(), RndEng);
    auto Count = std::min(Locations.size(), RoomCount);
    for (const auto &Loc : Locations) {
      if (M.getTile(Loc.Pos) != T()) {
        continue;
      }
      M.setTile(Loc.Pos, RoomEntity);
      if (--Count == 0) {
        break;
      }
    }
  }
}

template <typename T, typename U, typename RE>
void RoomEntityPlacer<T, U, RE>::init(BuilderPass &Pass, BuilderContext &C) {
  BaseType::init(Pass, C);
  Layer = this->template getCfg<std::string>("layer");
  RoomEntity = this->template getCfg<T>("entity");
  RoomPercentage = this->template getCfg<float>("room_percentage");
  CheckBlocksDoor =
      this->template getCfgOr<bool>("check_blocks_door", CheckBlocksDoor);
  RoomCountMin = this->template getCfgOr<unsigned>("room_count_min", RoomCountMin);
  RoomCountMax = this->template getCfgOr<unsigned>("room_count_max", RoomCountMax);
}

template <typename T, typename U, typename RE>
void RoomEntityPlacer<T, U, RE>::run(BuilderPass &Pass, BuilderContext &C) {
  BaseType::run(Pass, C);
  auto &Ctx = C.get<Context<T, U>>();
  // TODO drop the map all together and add objects to rooms instead
  addRandomRoomEntitys(Ctx.Map.get(Layer), RoomEntity, Ctx.Rooms, this->RndEng,
                       RoomPercentage, RoomCountMin, RoomCountMax,
                       CheckBlocksDoor);
}

} // namespace ymir::Dungeon

#endif // #ifndef YMIR_DUNGEON_ROOM_ENTITY_PLACER_HPP