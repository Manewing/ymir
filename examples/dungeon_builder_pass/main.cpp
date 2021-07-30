#include <ymir/Dungeon/BuilderPass.hpp>
#include <ymir/Dungeon/CaveRoomGenerator.hpp>
#include <ymir/Dungeon/ChestPlacer.hpp>
#include <ymir/Dungeon/RoomPlacer.hpp>

// FIXME get rid of template parameter for RandEngType
template <typename TileType, typename TileCord, typename RandEngType>
void registerBuilders(ymir::Dungeon::BuilderPass &Pass) {
  using T = TileType;
  using U = TileCord;
  using RE = RandEngType;
  Pass.registerBuilder<ymir::Dungeon::CaveRoomGenerator<T, U, RE>>();
  // Pass.registerBuilder<
  //    ymir::Dungeon::ChestPlacer<TileType, TileCord, RandEng>>();
  Pass.registerBuilder<ymir::Dungeon::RoomPlacer<T, U, RE>>();
}

int main() {
  ymir::Dungeon::BuilderPass Pass;
  registerBuilders<char, int, ymir::WyHashRndEng>(Pass);

  Pass.setBuilderAlias("cave_room_generator", "room_generator");
  Pass.setSequence({"room_placer"});

  Pass.configure({
      {"ground", ' '},
      {"wall", '#'},
      {"chest", 'C'},
      {"room_placer/num_new_room_attempts", 30U},
  });

  ymir::WyHashRndEng RE;
  ymir::Dungeon::Context<char, int, ymir::WyHashRndEng> Ctx(
      RE, ymir::Map<char, int>{80, 24});

  Pass.init(Ctx);
  Pass.run(Ctx);

  std::cout << Ctx.M << std::endl;
}