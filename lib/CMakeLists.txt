set(TARGET ymir)

set(SOURCE_FILES
  src/Map.cpp
)

add_library(${TARGET} STATIC ${SOURCE_FILES})

target_include_directories(${TARGET} PUBLIC include)
