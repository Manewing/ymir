set(TARGET dijkstra_map)

set(SOURCE_FILES
  main.cpp
)

add_executable(${TARGET}
  ${SOURCE_FILES}
)

target_link_libraries(${TARGET}
  ymir
)