# New-AMUL is compiled primarily into a library so that it can be easily
# accessed from unit tests etc.

SET(
    THE_LIB_SOURCES
    atom.cpp
)
SET(
    THE_LIB_HEADERS
    atom.h
    buffer.h
)
ADD_LIBRARY(${THE_LIB_NAME} ${THE_LIB_SOURCES})

SET(
    THE_EXE_SOURCES
    the-main.cpp
)
SET(
    THE_EXE_HEADERS
)
ADD_EXECUTABLE(
    ${THE_EXE_NAME}

    ${THE_EXE_SOURCES}
    ${THE_EXE_HEADERS}

    ${THE_LIB_HEADERS}
)
TARGET_LINK_LIBRARIES(${THE_EXE_NAME} PRIVATE "${THE_LIB_NAME}")
