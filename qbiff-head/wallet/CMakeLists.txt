cmake_minimum_required(VERSION 2.6)
project(qbiff-pwd)
 
find_package(KDE4 REQUIRED)
include(KDE4Defaults)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories( ${KDE4_INCLUDES} )
 
set(qbiff-pwd_SRCS src/main.cpp)
 
kde4_add_executable(qbiff-pwd ${qbiff-pwd_SRCS})
target_link_libraries(qbiff-pwd ${KDE4_KDEUI_LIBS})

install(TARGETS qbiff-pwd DESTINATION ${BIN_INSTALL_DIR})
