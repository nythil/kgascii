PROJECT(LIBKGASCII)

SET(kgascii_SRCS
    ft2pp/error.hpp 
    ft2pp/face.hpp 
    ft2pp/library.hpp 
    ft2pp/util.hpp 
    internal/enum_wrapper.hpp 
    internal/ft2_font_loader.hpp 
    internal/glyph_matcher_registration.hpp 
    dynamic_asciifier.hpp
    dynamic_glyph_matcher.hpp
    font.hpp
    font_image.hpp
    font_io.hpp
    font_pca.hpp
    ft2_font_loader.hpp
    glyph_matcher_context_factory.hpp
    image_io.hpp
    kgascii_api.hpp
    kgascii_config.hpp
    means_distance.hpp
    mutual_information_glyph_matcher.hpp
    parallel_asciifier.hpp
    pca_glyph_matcher.hpp
    pca_reconstruction_font_loader.hpp
    policy_based_glyph_matcher.hpp
    sequential_asciifier.hpp
    srgb.hpp
    squared_euclidean_distance.hpp
    symbol.hpp
    task_queue.hpp
    text_surface.hpp
)

IF(BUILD_SHARED_LIBS)
    SET(KGASCII_DLL ON)
ENDIF()

CONFIGURE_FILE(
    "${PROJECT_SOURCE_DIR}/kgascii_config.hpp.in" 
    "${PROJECT_BINARY_DIR}/kgascii_config.hpp"
)

INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR})
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Boost_GIL_2_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${FREETYPE_INCLUDE_DIR_ft2build})
INCLUDE_DIRECTORIES(${FREETYPE_INCLUDE_DIR_freetype2})
INCLUDE_DIRECTORIES(${EIGEN3_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${JPEG_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${TIFF_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${PNG_INCLUDE_DIR})

ADD_LIBRARY(kgascii STATIC ${kgascii_SRCS})
SET_TARGET_PROPERTIES(kgascii PROPERTIES LINKER_LANGUAGE CXX)

