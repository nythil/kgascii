PROJECT(LIBKGASCII)

SET(kgascii_SRCS
    ft2pp/error.hpp 
    ft2pp/face.hpp 
    ft2pp/library.hpp 
    ft2pp/util.hpp 
    asciifier.hpp
    dynamic_asciifier.hpp
    font_image.cpp
    font_image.hpp
    font_image_loader.hpp
    font_pca.cpp
    font_pca.hpp
    font_pcanalyzer.cpp
    font_pcanalyzer.hpp
    ft2_font_image_loader.hpp
    ft2_font_loader.cpp
    ft2_font_loader.hpp
    glyph_matcher.hpp
    glyph_matcher_context.hpp
    glyph_matcher_context_factory.cpp
    glyph_matcher_context_factory.hpp
    hexstring.cpp
    hexstring.hpp
    kgascii_api.hpp
    kgascii_config.hpp
    means_distance.hpp
    mutual_information_glyph_matcher.hpp
    parallel_asciifier.hpp
    pca_glyph_matcher.cpp
    pca_glyph_matcher.hpp
    pca_reconstruction_font_image_loader.cpp
    pca_reconstruction_font_image_loader.hpp
    policy_based_glyph_matcher.hpp
    sequential_asciifier.hpp
    squared_euclidean_distance.hpp
    surface.hpp
    surface_algorithm.hpp
    surface_container.hpp
    surface_fwd.hpp
    symbol.hpp
    task_queue.hpp
    text_surface.hpp
)
SET(Boost_USE_STATIC_LIBS        ON)
SET(Boost_USE_MULTITHREADED      ON)
SET(Boost_USE_STATIC_RUNTIME    OFF)
FIND_PACKAGE(Boost 1.46.0 COMPONENTS filesystem system thread REQUIRED)
FIND_PACKAGE(Freetype REQUIRED)
FIND_PACKAGE(Eigen3 REQUIRED)

IF(BUILD_SHARED_LIBS)
    SET(KGASCII_DLL ON)
ENDIF()

CONFIGURE_FILE(
    "${PROJECT_SOURCE_DIR}/kgascii_config.hpp.in" 
    "${PROJECT_BINARY_DIR}/kgascii_config.hpp"
)

INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR})
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${FREETYPE_INCLUDE_DIR_ft2build})
INCLUDE_DIRECTORIES(${FREETYPE_INCLUDE_DIR_freetype2})
INCLUDE_DIRECTORIES(${EIGEN3_INCLUDE_DIR})
ADD_LIBRARY(kgascii ${kgascii_SRCS})
TARGET_LINK_LIBRARIES(kgascii ${FREETYPE_LIBRARY})
TARGET_LINK_LIBRARIES(kgascii ${Boost_LIBRARIES})
IF(WIN32)
    SET_TARGET_PROPERTIES(kgascii PROPERTIES DEFINE_SYMBOL KGASCII_DLL_EXPORTS)
ENDIF(WIN32)

