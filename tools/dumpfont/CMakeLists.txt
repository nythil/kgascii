INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${FREETYPE_INCLUDE_DIR_freetype2})
INCLUDE_DIRECTORIES(${FREETYPE_INCLUDE_DIR_ft2build})

SET(dumpfont_SRCS
    dump_font_command.hpp
    dump_font_command.cpp
    main.cpp
)

ADD_EXECUTABLE(dumpfont ${dumpfont_SRCS})
TARGET_LINK_LIBRARIES(dumpfont tools_common)
TARGET_LINK_LIBRARIES(dumpfont ${FREETYPE_LIBRARY})
TARGET_LINK_LIBRARIES(dumpfont ${Boost_LIBRARIES})

