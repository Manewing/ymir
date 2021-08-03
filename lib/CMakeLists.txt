set(TARGET ymir)

set(SOURCE_FILES
  src/Config/AnyDict.cpp
  src/Config/Parser.cpp
  src/Config/String.cpp
  src/Dungeon/BuilderBase.cpp
  src/Dungeon/BuilderPass.cpp
  src/Map.cpp
  src/TypeHelpers.cpp
  src/Types.cpp
)

add_library(${TARGET} SHARED ${SOURCE_FILES})
add_library(${TARGET}_Static STATIC ${SOURCE_FILES})

set_target_properties(${TARGET}_Static PROPERTIES OUTPUT_NAME ${TARGET})
set_property(TARGET ${TARGET}_Static PROPERTY POSITION_INDEPENDENT_CODE 1)

target_include_directories(${TARGET} PUBLIC include)
target_include_directories(${TARGET}_Static PUBLIC include)
