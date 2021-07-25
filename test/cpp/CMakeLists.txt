set (TARGET YmirUnittest)

add_executable(
  ${TARGET}
  DungeonDoorTest.cpp
  DungeonRoomTest.cpp
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
