PROJECT(LIBKGASCII)

SET(kgascii_SRCS
    ft2pp/error.hpp 
    ft2pp/face.hpp 
    ft2pp/library.hpp 
    ft2pp/util.hpp 
    fontimage.cpp
    fontimage.hpp
    fontloader.cpp
    fontloader.hpp
    hexstring.cpp
    hexstring.hpp
    asciifier.cpp
    asciifier.hpp
    sequentialasciifier.cpp
    sequentialasciifier.hpp
    parallelasciifier.cpp
    parallelasciifier.hpp
    dynamicasciifier.cpp
    dynamicasciifier.hpp
    taskqueue.hpp
    glyphmatcher.cpp
    glyphmatcher.hpp
    glyphmatchercontext.cpp
    glyphmatchercontext.hpp
    policybasedglyphmatcher.hpp
    squaredeuclideandistance.hpp
    meansdistance.hpp
    textsurface.cpp
    textsurface.hpp
)
SET(Boost_USE_STATIC_LIBS        ON)
SET(Boost_USE_MULTITHREADED      ON)
SET(Boost_USE_STATIC_RUNTIME    OFF)
FIND_PACKAGE(Boost 1.46.0 COMPONENTS filesystem system thread REQUIRED)
FIND_PACKAGE(Freetype REQUIRED)
FIND_PACKAGE(Eigen3 REQUIRED)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${FREETYPE_INCLUDE_DIR_ft2build})
INCLUDE_DIRECTORIES(${EIGEN3_INCLUDE_DIR})
ADD_LIBRARY(kgascii SHARED ${kgascii_SRCS})
TARGET_LINK_LIBRARIES(kgascii ${FREETYPE_LIBRARY})
TARGET_LINK_LIBRARIES(kgascii ${Boost_LIBRARIES})
SET_TARGET_PROPERTIES(kgascii PROPERTIES DEFINE_SYMBOL KGASCII_EXPORTS)

