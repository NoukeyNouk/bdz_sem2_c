cmake_minimum_required(VERSION 3.10)
	set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
	
	project(hash_table_lab C)
	
	file(GLOB_RECURSE SOURCES "src/*.c")
	
	add_executable(lab ${SOURCES})
	
	target_include_directories(lab PUBLIC src/include)
