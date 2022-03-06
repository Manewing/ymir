#include <filesystem>
#include <string_view>
#include <ymir/Config/Parser.hpp>
#include <ymir/Config/Types.hpp>
#include <ymir/Dungeon/BuilderPass.hpp>
#include <ymir/Dungeon/CaveRoomGenerator.hpp>
#include <ymir/Dungeon/CelAltMapFiller.hpp>
#include <ymir/Dungeon/RoomEntityPlacer.hpp>
#include <ymir/Dungeon/LoopPlacer.hpp>
#include <ymir/Dungeon/MapFiller.hpp>
#include <ymir/Dungeon/RandomRoomGenerator.hpp>
#include <ymir/Dungeon/RectRoomGenerator.hpp>
#include <ymir/Dungeon/StartEndPlacer.hpp>
#include <ymir/Dungeon/FilterPlacer.hpp>
#include <ymir/Dungeon/RoomPlacer.hpp>
#include <ymir/Terminal.hpp>

template <typename TileType, typename TileCord, typename RandEngType>
void registerBuilders(ymir::Dungeon::BuilderPass &Pass) {
  using T = TileType;
  using U = TileCord;
  using RE = RandEngType;
  Pass.registerBuilder<ymir::Dungeon::CaveRoomGenerator<T, U, RE>>();
  Pass.registerBuilder<ymir::Dungeon::RectRoomGenerator<T, U, RE>>();
  Pass.registerBuilder<ymir::Dungeon::RandomRoomGenerator<T, U, RE>>();
  Pass.registerBuilder<ymir::Dungeon::RoomEntityPlacer<T, U, RE>>();
  Pass.registerBuilder<ymir::Dungeon::RoomPlacer<T, U, RE>>();
  Pass.registerBuilder<ymir::Dungeon::LoopPlacer<T, U, RE>>();
  Pass.registerBuilder<ymir::Dungeon::MapFiller<T, U>>();
  Pass.registerBuilder<ymir::Dungeon::CelAltMapFiller<T, U, RE>>();
  Pass.registerBuilder<ymir::Dungeon::StartEndPlacer<T, U, RE>>();
  Pass.registerBuilder<ymir::Dungeon::FilterPlacer<T, U, RE>>();
}

int main(int Argc, char *Argv[]) {
  std::filesystem::path CfgFile;
  std::optional<unsigned> Seed;
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
  auto &Cfg = CfgParser.getCfg();

  // If seed is provided override configuration
  if (Seed) {
    Cfg["dungeon/seed"] = *Seed;
  } else {
    Seed = Cfg.getOr<unsigned>("dungeon/seed", 0);
  }

  // Create new builder pass and register builders at it
  ymir::Dungeon::BuilderPass Pass;
  registerBuilders<ymir::ColoredUniChar, int, ymir::WyHashRndEng>(Pass);

  for (auto const &[Alias, Builder] :
       Cfg.getSubDict("builder_alias/").toVec<std::string>()) {
    Pass.setBuilderAlias(Builder, Alias);
  }

  Pass.setSequence(Cfg.asList<std::string>("sequence/"));
  Pass.configure(Cfg);

  const auto Layers = Cfg.asList<std::string>("layers/");
  const auto Size = Cfg.get<ymir::Size2d<int>>("dungeon/size");
  ymir::LayeredMap<ymir::ColoredUniChar, int> Map(Layers, Size);

  ymir::Dungeon::Context<ymir::ColoredUniChar, int> Ctx(Map);
  ymir::Logger Log("dungeon_builder");
  Log.addHandler(std::make_shared<ymir::StderrLogHandler>());
  Ctx.Log = &Log;

  Pass.init(Ctx);
  Pass.run(Ctx);

  std::cout << Ctx.Map.render() << std::endl
            << std::endl
            << "Seed: " << *Seed << std::endl;
}