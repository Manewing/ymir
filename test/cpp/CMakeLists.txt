set (TARGET YmirUnittest)

add_executable(
  ${TARGET}
  ConfigTypesTest.cpp
  DungeonDoorTest.cpp
  DungeonRoomTest.cpp
  TypesTest.cpp
  StringTest.cpp
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
