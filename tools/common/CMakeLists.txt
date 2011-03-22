SET(tools_common_SRCS
    cmdline_tool.cpp
    cmdline_tool.hpp
    validate_optional.hpp
    console.hpp
)
IF(WIN32)
    SET(tools_common_SRCS ${tools_common_SRCS} console_win.cpp)
ELSEIF(UNIX)
    SET(tools_common_SRCS ${tools_common_SRCS} console_unix.cpp)
ENDIF()

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
ADD_LIBRARY(tools_common STATIC ${tools_common_SRCS})
TARGET_LINK_LIBRARIES(tools_common ${Boost_LIBRARIES})

