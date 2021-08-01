#include <filesystem>
#include <string_view>
#include <ymir/Config/Parser.hpp>
#include <ymir/Config/Types.hpp>
#include <ymir/Dungeon/BuilderPass.hpp>
#include <ymir/Dungeon/CaveRoomGenerator.hpp>
#include <ymir/Dungeon/ChestPlacer.hpp>
#include <ymir/Dungeon/LoopPlacer.hpp>
#include <ymir/Dungeon/RandomRoomGenerator.hpp>
#include <ymir/Dungeon/RectRoomGenerator.hpp>
#include <ymir/Dungeon/RoomPlacer.hpp>

// FIXME get rid of template parameter for RandEngType
template <typename TileType, typename TileCord, typename RandEngType>
void registerBuilders(ymir::Dungeon::BuilderPass &Pass) {
  using T = TileType;
  using U = TileCord;
  using RE = RandEngType;
  Pass.registerBuilder<ymir::Dungeon::CaveRoomGenerator<T, U, RE>>();
  Pass.registerBuilder<ymir::Dungeon::RectRoomGenerator<T, U, RE>>();
  Pass.registerBuilder<ymir::Dungeon::RandomRoomGenerator<T, U, RE>>();
  Pass.registerBuilder<ymir::Dungeon::ChestPlacer<T, U, RE>>();
  Pass.registerBuilder<ymir::Dungeon::RoomPlacer<T, U, RE>>();
  Pass.registerBuilder<ymir::Dungeon::LoopPlacer<T, U, RE>>();
}

int main(int Argc, char *Argv[]) {
  std::filesystem::path CfgFile;
  unsigned Seed = 0;
  if (Argc == 2) {
    CfgFile = Argv[1];
  } else if (Argc == 3) {
    CfgFile = Argv[1];
    if (std::string_view(Argv[2]) == "rand") {
      Seed = std::random_device()();
    } else {
      Seed = std::stoul(Argv[2]);
    }
  } else {
    std::cerr << "usage: " << Argv[0] << " <cfgfile> <seed>?" << std::endl;
    return 1;
  }

  // Load configuration file
  ymir::Config::Parser CfgParser;
  ymir::Config::registerYmirTypes<int>(CfgParser);
  CfgParser.parse(CfgFile);
  const auto &Cfg = CfgParser.getCfg();

  // Create new builder pass and register builders at it
  ymir::Dungeon::BuilderPass Pass;
  registerBuilders<char, int, ymir::WyHashRndEng>(Pass);

  for (auto const &[Alias, Builder] :
       Cfg.getSubDict("builder_alias/").toVec<std::string>()) {
    Pass.setBuilderAlias(Builder, Alias);
  }

  Pass.setSequence(Cfg.getSubDict("sequence").values<std::string>());
  Pass.configure(Cfg);

  ymir::WyHashRndEng RE;
  RE.seed(Cfg.getOr<int>("dungeon/seed", Seed));
  ymir::Dungeon::Context<char, int, ymir::WyHashRndEng> Ctx(
      RE, ymir::Map<char, int>{Cfg.get<ymir::Size2d<int>>("dungeon/size")});

  Pass.init(Ctx);
  Pass.run(Ctx);

  std::cout << Ctx.M << std::endl << std::endl
            << "Seed: " << Seed << std::endl;
}