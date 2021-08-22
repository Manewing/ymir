set (TARGET YmirUnittest)

add_executable(
  ${TARGET}
  AlgorithmDijkstraTest.cpp
  ConfigParserTest.cpp
  ConfigTypesTest.cpp
  DungeonDoorTest.cpp
  DungeonRoomTest.cpp
  StringTest.cpp
  TypesTest.cpp
)

target_link_libraries(${TARGET}
  gtest
  gmock
  gtest_main
  pthread
  ymir
)

add_test(
  NAME ${TARGET}
  COMMAND ${TARGET}
)
