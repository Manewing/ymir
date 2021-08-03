#ifndef YMIR_DUNGEON_LOOP_PLACER_HPP
#define YMIR_DUNGEON_LOOP_PLACER_HPP

#include <ymir/Dungeon/BuilderBase.hpp>
#include <ymir/Noise.hpp>

namespace ymir::Dungeon {

template <typename TileType, typename TileCord, typename RndEngType>
class LoopPlacer : public BuilderBase {
public:
  static const char *Type;

public:
  using BuilderBase::BuilderBase;

  const char *getType() const override { return Type; }

  void init(BuilderPass &Pass, BuilderContext &C) override;
  void run(BuilderPass &Pass, BuilderContext &C) override;

protected:
  unsigned MaxLoops = 0;
  unsigned MaxUsedDoors = 0;
  std::optional<TileType> Ground;
};

template <typename T, typename U, typename RE>
const char *LoopPlacer<T, U, RE>::Type = "loop_placer";

template <typename U>
bool checkIfOpposing(Point2d<U> SrcPos, Dir2d SrcDir, Point2d<U> TgtPos,
                     Dir2d TgtDir) {
  if (SrcDir.opposing() != TgtDir) {
    return false;
  }
  switch (SrcDir) {
  case Dir2d::LEFT:
    return SrcPos.X >= TgtPos.X;
  case Dir2d::RIGHT:
    return SrcPos.X <= TgtPos.X;
  case Dir2d::UP:
    return SrcPos.Y >= TgtPos.Y;
  case Dir2d::DOWN:
    return SrcPos.Y <= TgtPos.Y;
  default:
    break;
  }
  return false;
}

template <typename T, typename U, typename RE>
std::optional<Dungeon::Hallway<T, U>>
getLoopHallway(Context<T, U, RE> &Ctx, Dungeon::Room<T, U> &Source,
               Dungeon::Room<T, U> &Target) {
  std::vector<Dungeon::Hallway<T, U>> LoopHallways;
  for (auto &SrcDoor : Source.Doors) {
    for (auto &TgtDoor : Target.Doors) {
      auto SrcPos = Source.Pos + SrcDoor.Pos;
      auto TgtPos = Target.Pos + TgtDoor.Pos;
      if (!checkIfOpposing(SrcPos, SrcDoor.Dir, TgtPos, TgtDoor.Dir) ||
          SrcDoor.Used || TgtDoor.Used) {
        continue;
      }
      if ((SrcPos.X == TgtPos.X && SrcDoor.Dir.isVertical()) ||
          (SrcPos.Y == TgtPos.Y && SrcDoor.Dir.isHorizontal())) {
        auto HallwayRect = Context<T, U, RE>::getHallwayRect(SrcPos, TgtPos);
        if (Ctx.doesHallwayFit(HallwayRect, &Target, &Source)) {
          LoopHallways.push_back(Dungeon::Hallway<T, U>{
              HallwayRect, &Source, &SrcDoor, &Target, &TgtDoor});
        }
      }
    }
  }
  if (LoopHallways.empty()) {
    return std::nullopt;
  }
  return *randomIterator(LoopHallways.begin(), LoopHallways.end(), Ctx.RndEng);
}

template <typename T, typename U, typename RE>
void LoopPlacer<T, U, RE>::init(BuilderPass &Pass, BuilderContext &C) {
  BuilderBase::init(Pass, C);
  Ground = getPass().cfg().template get<T>("dungeon/ground");
  MaxLoops = getPass().cfg().template get<unsigned>("loop_placer/max_loops");
  MaxUsedDoors =
      getPass().cfg().template get<unsigned>("loop_placer/max_used_doors");
}

template <typename T, typename U, typename RE>
void LoopPlacer<T, U, RE>::run(BuilderPass &Pass, BuilderContext &C) {
  BuilderBase::init(Pass, C);
  auto &Ctx = C.get<Context<T, U, RE>>();

  // FIXME factor out into standalone func
  std::vector<Dungeon::Hallway<T, U>> LoopHallways;
  for (auto It = Ctx.Rooms.begin(); It != Ctx.Rooms.end(); It++) {
    if (It->getNumUsedDoors() >= MaxUsedDoors) {
      continue;
    }
    for (auto NextIt = It; NextIt != Ctx.Rooms.end(); NextIt++) {
      if (It == NextIt || Ctx.haveRoomsHallway(*It, *NextIt) ||
          NextIt->getNumUsedDoors() >= MaxUsedDoors) {
        continue;
      }
      auto LoopHallway = getLoopHallway(Ctx, *It, *NextIt);
      if (LoopHallway) {
        LoopHallways.push_back(*LoopHallway);
      }
    }
  }

  // FIXME factor out into standalone func
  std::shuffle(LoopHallways.begin(), LoopHallways.end(), Ctx.RndEng);
  for (std::size_t Idx = 0; Idx < MaxLoops && Idx < LoopHallways.size();
       Idx++) {
    auto &Hallway = LoopHallways.at(Idx);
    Hallway.SrcDoor->Used = true;
    Hallway.DstDoor->Used = true;
    Ctx.Hallways.push_back(Hallway);

    // FIXME do this somewhere else?
    // FIXME use configured layer
    Ctx.Map.get(0).fillRect(*Ground, Hallway.Rect);
  }
}

} // namespace ymir::Dungeon

#endif // #ifndef YMIR_DUNGEON_LOOP_PLACER_HPP