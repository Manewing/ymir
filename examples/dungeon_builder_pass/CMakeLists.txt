set(TARGET dungeon_builder_pass)

set(SOURCE_FILES
  main.cpp
)

add_executable(${TARGET}
  ${SOURCE_FILES}
)

target_link_libraries(${TARGET}
  ymir
)

target_include_directories(${TARGET} PUBLIC include)

add_test(
  NAME ${TARGET}_rect_dungeon
  COMMAND
    ${PROJECT_SOURCE_DIR}/test/scripts/check_output.py
      --ref ${CMAKE_CURRENT_SOURCE_DIR}/references/rect_dungeon.txt
      --target ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}
      --args ${CMAKE_CURRENT_SOURCE_DIR}/configs/rect_dungeon.cfg 2039660508
)

add_test(
  NAME ${TARGET}_cave_dungeon
  COMMAND
    ${PROJECT_SOURCE_DIR}/test/scripts/check_output.py
      --ref ${CMAKE_CURRENT_SOURCE_DIR}/references/cave_dungeon.txt
      --target ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}
      --args ${CMAKE_CURRENT_SOURCE_DIR}/configs/cave_dungeon.cfg 3551281415
)

add_test(
  NAME ${TARGET}_random_dungeon
  COMMAND
    ${PROJECT_SOURCE_DIR}/test/scripts/check_output.py
      --ref ${CMAKE_CURRENT_SOURCE_DIR}/references/random_dungeon.txt
      --target ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}
      --args ${CMAKE_CURRENT_SOURCE_DIR}/configs/random_dungeon.cfg 355145
)