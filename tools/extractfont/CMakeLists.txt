INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${FREETYPE_INCLUDE_DIR_ft2build})
INCLUDE_DIRECTORIES(${FREETYPE_INCLUDE_DIR_freetype2})

SET(extractfont_SRCS
    extract_font_command.hpp
    extract_font_command.cpp
    main.cpp
)

ADD_EXECUTABLE(extractfont ${extractfont_SRCS})
TARGET_LINK_LIBRARIES(extractfont tools_common)
TARGET_LINK_LIBRARIES(extractfont ${FREETYPE_LIBRARY})
TARGET_LINK_LIBRARIES(extractfont ${Boost_LIBRARIES})

