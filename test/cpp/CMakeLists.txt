set (TARGET YmirUnittest)

add_executable(
  ${TARGET}
  AlgorithmDijkstraTest.cpp
  AlgorithmLineOfSightTest.cpp
  AlgorithmVectorAlgebraTest.cpp
  ConfigParserTest.cpp
  ConfigTypesTest.cpp
  DungeonDoorTest.cpp
  DungeonRoomTest.cpp
  LayeredMapTest.cpp
  LoggingTest.cpp
  MapTest.cpp
  NoiseTest.cpp
  StringTest.cpp
  TypesTest.cpp
)

target_link_libraries(${TARGET}
  gtest
  gmock
  gtest_main
  ymir
)

add_test(
  NAME ${TARGET}
  COMMAND ${TARGET}
)
