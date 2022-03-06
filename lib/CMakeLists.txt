set(TARGET ymir)

set(HEADER_FILES
  include/ymir/Algorithm/Dijkstra.hpp
  include/ymir/Algorithm/DijkstraIo.hpp
  include/ymir/Algorithm/LineOfSight.hpp
  include/ymir/CallularAutomata.hpp
  include/ymir/Config/AnyDict.hpp
  include/ymir/Config/Parser.hpp
  include/ymir/Config/String.hpp
  include/ymir/Config/Types.hpp
  include/ymir/DebugTile.hpp
  include/ymir/Dungeon/BuilderBase.hpp
  include/ymir/Dungeon/BuilderPass.hpp
  include/ymir/Dungeon/CaveRoomGenerator.hpp
  include/ymir/Dungeon/CelAltMapFiller.hpp
  include/ymir/Dungeon/Context.hpp
  include/ymir/Dungeon/Door.hpp
  include/ymir/Dungeon/FilterPlacer.hpp
  include/ymir/Dungeon/Hallway.hpp
  include/ymir/Dungeon/LoopPlacer.hpp
  include/ymir/Dungeon/MapFiller.hpp
  include/ymir/Dungeon/Object.hpp
  include/ymir/Dungeon/RandomBuilder.hpp
  include/ymir/Dungeon/RandomRoomGenerator.hpp
  include/ymir/Dungeon/RectRoomGenerator.hpp
  include/ymir/Dungeon/Room.hpp
  include/ymir/Dungeon/RoomEntityPlacer.hpp
  include/ymir/Dungeon/RoomGenerator.hpp
  include/ymir/Dungeon/RoomPlacer.hpp
  include/ymir/Dungeon/StartEndPlacer.hpp
  include/ymir/Enum.hpp
  include/ymir/LayeredMap.hpp
  include/ymir/Map.hpp
  include/ymir/MapFilter.hpp
  include/ymir/MapIo.hpp
  include/ymir/Noise.hpp
  include/ymir/Terminal.hpp
  include/ymir/TypeHelpers.hpp
  include/ymir/Types.hpp
)

set(SOURCE_FILES
  src/Algorithm/DijkstraIo.cpp
  src/Config/AnyDict.cpp
  src/Config/Parser.cpp
  src/Config/String.cpp
  src/Dungeon/BuilderBase.cpp
  src/Dungeon/BuilderPass.cpp
  src/Logging.cpp
  src/Map.cpp
  src/MapIo.cpp
  src/Terminal.cpp
  src/TypeHelpers.cpp
  src/Types.cpp
)

add_library(${TARGET} SHARED ${HEADER_FILES} ${SOURCE_FILES})
add_library(${TARGET}_Static STATIC ${HEADER_FILES} ${SOURCE_FILES})

set_target_properties(${TARGET}_Static PROPERTIES OUTPUT_NAME ${TARGET})
set_property(TARGET ${TARGET}_Static PROPERTY POSITION_INDEPENDENT_CODE 1)

target_include_directories(${TARGET} PUBLIC include)
target_include_directories(${TARGET}_Static PUBLIC include)

install(
  DIRECTORY include/
  DESTINATION include/
  COMPONENT header_files
  FILES_MATCHING PATTERN "*.hpp"
)


install(
  TARGETS ${TARGET} ${TARGET}_Static
)
