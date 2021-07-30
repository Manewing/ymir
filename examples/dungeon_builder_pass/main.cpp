#include <ymir/Dungeon/BuilderPass.hpp>
#include <ymir/Dungeon/CaveRoomGenerator.hpp>
#include <ymir/Dungeon/ChestPlacer.hpp>
#include <ymir/Dungeon/RoomPlacer.hpp>
#include <ymir/Dungeon/LoopPlacer.hpp>
#include <ymir/Dungeon/RectRoomGenerator.hpp>

// FIXME get rid of template parameter for RandEngType
template <typename TileType, typename TileCord, typename RandEngType>
void registerBuilders(ymir::Dungeon::BuilderPass &Pass) {
  using T = TileType;
  using U = TileCord;
  using RE = RandEngType;
  Pass.registerBuilder<ymir::Dungeon::CaveRoomGenerator<T, U, RE>>();
  Pass.registerBuilder<ymir::Dungeon::RectRoomGenerator<T, U, RE>>();
  Pass.registerBuilder<ymir::Dungeon::ChestPlacer<T, U, RE>>();
  Pass.registerBuilder<ymir::Dungeon::RoomPlacer<T, U, RE>>();
  Pass.registerBuilder<ymir::Dungeon::LoopPlacer<T, U, RE>>();
}

int main() {
  ymir::Dungeon::BuilderPass Pass;
  registerBuilders<char, int, ymir::WyHashRndEng>(Pass);

  Pass.setBuilderAlias("rect_room_generator", "room_generator");
  Pass.setSequence({"room_placer",  "loop_placer", "chest_placer"});

  Pass.configure({
      {"ground", ' '},
      {"wall", '#'},
      {"chest", 'C'},
      {"room_generator/room_size_min_max", ymir::Rect2d<int>{{5, 5}, {10, 10}}},
      {"room_placer/num_new_room_attempts", 30U},
      {"chest_placer/room_chest_percentage", static_cast<float>(10.0)},
      {"loop_placer/max_loops", 30U},
      {"loop_placer/max_used_doors", 5U},
  });

  ymir::WyHashRndEng RE;
  RE.seed(0);
  ymir::Dungeon::Context<char, int, ymir::WyHashRndEng> Ctx(
      RE, ymir::Map<char, int>{80, 24});

  Pass.init(Ctx);
  Pass.run(Ctx);

  std::cout << Ctx.M << std::endl;
}