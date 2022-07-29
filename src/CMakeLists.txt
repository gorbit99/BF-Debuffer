set(SRC_FILES 
    ./main.c
    ./inputstream.h ./inputstream.c
    ./interpreter.h ./interpreter.c
    ./menu.h ./menu.c
    ./error.h ./error.c
    ./outputbuffer.h ./outputbuffer.c
    )

add_executable(bfdebugger "${SRC_FILES}")

target_compile_options(bfdebugger
    PRIVATE
    -Wall -Wextra -Wpedantic -Werror)

set_target_properties(bfdebugger PROPERTIES 
    C_STANDARD 99
    EXPORT_COMPILE_COMMANDS ON)

target_compile_options(bfdebugger PRIVATE
    "$<$<CONFIG:Debug>:-g>")

install(TARGETS bfdebugger DESTINATION bin)
