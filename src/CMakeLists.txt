set(SRC_FILES 
    ./main.c
    ./inputstream.h ./inputstream.c
    ./interpreter.h ./interpreter.c)

add_executable(program "${SRC_FILES}")

target_compile_options(program
    PRIVATE
    -Wall -Wextra -Wpedantic -Werror)

set_target_properties(program PROPERTIES 
    C_STANDARD 99
    EXPORT_COMPILE_COMMANDS ON)

target_compile_options(program PRIVATE
    "$<$<CONFIG:Debug>:-g>")
