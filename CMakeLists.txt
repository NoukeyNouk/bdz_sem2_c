cmake_minimum_required(VERSION 3.10)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(hash_table_lab C)

set(LIB_SOURCES 
    src/birdy_table/birdy_table.c 
    src/llrb_table/llrb_table.c
)

add_library(table SHARED ${LIB_SOURCES})

target_include_directories(table PUBLIC src/include)

set(APP_SOURCES
    src/main.c
    src/io/io.c
    src/game/engine.c
)

if(IMPL)
    target_compile_definitions(table PRIVATE ${IMPL})
endif()

add_executable(lab ${APP_SOURCES})

target_link_libraries(lab PRIVATE table)
