set(TARGET procedural_dungeon)

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
  NAME procedural_dungeon_example
  COMMAND
    ${PROJECT_SOURCE_DIR}/test/scripts/check_output.py
      --ref ${CMAKE_CURRENT_SOURCE_DIR}/ref_80_24_42.txt
      --target ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}
      --args 80 24 42
)