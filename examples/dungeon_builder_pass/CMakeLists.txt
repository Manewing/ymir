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