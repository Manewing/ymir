#ifndef YMIR_DUNGEON_RANDOM_ROOM_GENERATOR_HPP
#define YMIR_DUNGEON_RANDOM_ROOM_GENERATOR_HPP

#include <string>
#include <tuple>
#include <vector>
#include <ymir/Dungeon/RoomGenerator.hpp>

namespace ymir::Dungeon {

using RoomProbsType = std::vector<std::pair<std::string, float>>;

template <typename TileType, typename TileCord, typename RndEngType>
class RandomRoomGenerator
    : public RoomGenerator<TileType, TileCord, RndEngType> {
public:
  using RoomGeneratorType = RoomGenerator<TileType, TileCord, RndEngType>;

public:
  static const char *Name;

public:
  RandomRoomGenerator() = default;
  const char *getName() const override { return Name; }

  void init(BuilderPass &Pass, BuilderContext &C) override;

  Room<TileType, TileCord> generate() override;
  Map<TileType, TileCord> generateRoomMap(Size2d<TileCord> SIze) override;

private:
  std::vector<std::pair<RoomGeneratorType *, float>> RoomGenProbs;
};

template <typename T, typename U, typename RE>
const char *RandomRoomGenerator<T, U, RE>::Name = "random_room_generator";

template <typename T, typename U, typename RE>
void RandomRoomGenerator<T, U, RE>::init(BuilderPass &Pass, BuilderContext &C) {
  RoomGeneratorType::init(Pass, C);
  auto RoomProbs = this->getPass().cfg()
                       .getSubDict("random_room_generator/room_probs/")
                       .template toVec<float>();

  RoomGenProbs.clear();
  RoomGenProbs.reserve(RoomProbs.size());
  for (auto const &[RoomGenName, Prob] : RoomProbs) {
    assert(Prob > 0.0 && Prob <= 1.0); // TODO
    auto *RoomGen =
        &this->getPass().template get<RoomGeneratorType>(RoomGenName);
    RoomGenProbs.emplace_back(RoomGen, Prob);
  }
  std::sort(RoomGenProbs.begin(), RoomGenProbs.end(),
            [](const auto &A, const auto &B) { return A.second < B.second; });

  // TODO default value? (see above)
  assert(!RoomGenProbs.empty());

  RoomGenProbs.back().second = 1.0;
}

template <typename T, typename U, typename RE>
Room<T, U> RandomRoomGenerator<T, U, RE>::generate() {
  std::uniform_real_distribution<float> Uni(0.0, 1.0);
  auto Value = Uni(this->getCtx().RndEng);

  for (auto const &[RoomGen, Prob] : RoomGenProbs) {
    if (Value <= Prob) {
      return RoomGen->generate();
    }
  }

  throw std::runtime_error("should never be reached"); // FIXME
}

template <typename T, typename U, typename RE>
Map<T, U> RandomRoomGenerator<T, U, RE>::generateRoomMap(Size2d<U> /*Size*/) {
  throw std::runtime_error("Not implmeneted");
}

} // namespace ymir::Dungeon

#endif // #ifndef YMIR_DUNGEON_RANDOM_ROOM_GENERATOR_HPP