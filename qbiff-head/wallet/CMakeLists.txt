cmake_minimum_required(VERSION 2.6)
project(qbiff-pwd)

# skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH  TRUE)

# when building, use the install RPATH already
# (so it doesn't need to relink when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE) 

# the RPATH to be used when installing
SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
 
find_package(KDE4 REQUIRED)
include(KDE4Defaults)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories( ${KDE4_INCLUDES} )
 
set(qbiff-pwd_SRCS src/main.cpp)
 
kde4_add_executable(qbiff-pwd ${qbiff-pwd_SRCS})
target_link_libraries(qbiff-pwd ${KDE4_KDEUI_LIBS})

install(TARGETS qbiff-pwd DESTINATION ${BIN_INSTALL_DIR})
