INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Boost_GIL_2_INCLUDE_DIRS})

SET(txtrender_SRCS
    render_text_command.hpp
    render_text_command.cpp
    main.cpp
)

ADD_EXECUTABLE(txtrender ${txtrender_SRCS})
TARGET_LINK_LIBRARIES(txtrender tools_common)
TARGET_LINK_LIBRARIES(txtrender ${Boost_LIBRARIES})

