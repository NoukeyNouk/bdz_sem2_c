cmake_minimum_required(VERSION 3.10)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(hash_table_lab C)

# ==========================================
# 1. Сборка динамической библиотеки (.so)
# ==========================================
# Укажи файлы, которые реализуют логику (например, дерево и хэш-таблицу)
set(LIB_SOURCES 
    src/birdy_table/birdy_table.c 
    src/llrb_table/llrb_table.c
)

# add_library с флагом SHARED говорит CMake собрать .so файл
add_library(table SHARED ${LIB_SOURCES})

# Указываем, где библиотеке искать заголовочные файлы
target_include_directories(table PUBLIC src/include)

set(APP_SOURCES
    src/main.c
    src/io/io.c
    src/game/engine.c
)

# ==========================================
# 2. Обработка макроса (IMPL=TREE_IMPL)
# ==========================================
# Проверяем, передали ли нам переменную IMPL из командной строки
if(IMPL)
    # Это эквивалентно добавлению #define IMPL TREE_IMPL (или что там передали)
    # во все файлы библиотеки my_structs
    target_compile_definitions(table PRIVATE ${IMPL})
    
    # Если макрос нужен еще и в main.c, раскомментируй строку ниже:
    # target_compile_definitions(lab PRIVATE IMPL=${IMPL})
endif()

# ==========================================
# 3. Сборка основной программы и линковка
# ==========================================
# Здесь оставляем только файлы самой программы (например, main)
add_executable(lab ${APP_SOURCES})

# Линкуем нашу динамическую библиотеку my_structs к исполняемому файлу lab
target_link_libraries(lab PRIVATE table)
