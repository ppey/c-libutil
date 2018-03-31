cmake_minimum_required(VERSION 3.7)
project(libUtil)

set(CMAKE_C_STANDARD 99)
# Use -msse2 or -msse4.2 for older x86 cpus


set(CMAKE_C_FLAGS "-pthread -march=native -Ofast -Wall -Wextra -Wpedantic -Wno-variadic-macros -Wno-unused-parameter")
#set(CMAKE_C_FLAGS "-march=native -Ofast -Wall -Wextra -Wpedantic -Wno-variadic-macros -Wno-unused-parameter -fsanitize=address,leak,undefined")
#set(CMAKE_C_FLAGS "-march=native -Ofast -Wall -Wextra -Wpedantic -Wno-variadic-macros -Wno-unused-parameter -fsanitize=thread")


set(SOURCE_TEST_ALL
        src/main_test.c
        src/map.c
        src/slice.c
        src/list.c
        src/list_test.c
        src/stack.c
        src/threadPool.c
        src/threadPool_test.c
        src/libUtil_Internal.h)

set(SOURCE_ALL
        src/map.c
        src/slice.c
        src/list.c
        src/stack.c
        src/threadPool.c
        src/libUtil_Internal.h)

#add_library(libutilStatic SHARED ${SOURCE_ALL})
#add_library(libutilDynamic STATIC ${SOURCE_ALL})
add_executable(testAll ${SOURCE_TEST_ALL})

#target_link_libraries(testAll m)
#target_link_libraries(libutilStatic m)
#target_link_libraries(libutilDynamic m)

