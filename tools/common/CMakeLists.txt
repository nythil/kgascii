SET(tools_common_SRCS
    cmdlinetool.cpp
    cmdlinetool.hpp
)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
ADD_LIBRARY(tools_common STATIC ${tools_common_SRCS})
TARGET_LINK_LIBRARIES(tools_common ${Boost_LIBRARIES})

