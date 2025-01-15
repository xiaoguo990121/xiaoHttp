cmake_minimum_required(VERSION 3.10)
project(xiaoHttp)

option(BUILD_SHARED_LIBS "Build xiaoHttp as a shared lib" OFF)
option(BUILD_TESTING "Build tests" OFF)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_modules/)

set(XIAOHTTP_MAJOR_VERSION 0)
set(XIAOHTTP_MINOR_VERSION 0)
set(XIAOHTTP_PATCH_VERSION 1)
set(XIAOHTTP_VERSION ${XIAOHTTP_MAJOR_VERSION}.${XIAOHTTP_MINOR_VERSION}.${XIAOHTTP_PATCH_VERSION})

include(GNUInstallDirs)
set(INSTALL_BIN_DIR 
    ${CMAKE_INSTALL_BINDIR}
    CACHE PATH "Installation directory for binaries")
set(INSTALL_LIB_DIR
    ${CMAKE_INSTALL_LIBDIR}
    CACHE PATH "Installation directory for libraries")
set(INSTALL_INCLUDE_DIR
    ${CMAKE_INSTALL_INCLUDEDIR}
    CACHE PATH "Installation directory for header files")
set(DEF_INSTALL_XIAOHTTP_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/XiaoHttp)
set(INSTALL_XIAONET_CMAKE_DIR 
    ${DEF_INSTALL_XIAOHTTP_CMAKE_DIR}
    CACHE PATH "Installation directory for cmake files")
message("${INSTALL_XIAOHTTP_CMAKE_DIR}")

add_library(${PROJECT_NAME})
if(BUILD_SHARED_LIBS)
    list(
        FIND
        CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES
        "${CMAKE_INSTALL_PREFIX}/${INSTALL_LIB_DIR}"
        isSystemDir
    )

    if("${isSystemDir}" STREQUAL "-1")
        set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${INSTALL_LIB_DIR}")
    endif("${isSystemDir}" STREQUAL "-1")

    set_target_properties(
        ${PROJECT_NAME}
        PROPERTIES VERSION
                    ${XIAOLOG_VERSION}
                    SOVERSION
                    ${XIAOLOG_MAJOR_VERSION}   
    )
endif(BUILD_SHARED_LIBS)

include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME} EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/exports/xiaoHttp/exports.h)

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20)
set_target_properties(${PROJECT_NAME} PROPERTIES EXPORT_NAME XiaoLog)

include(cmake/ParseAndAddDrogonTests.cmake)

set(XIAOHTTP_SOURCES
    lib/src/CacheFile.cpp
    lib/src/HttpUtils.cpp
    lib/src/DrClassMap.cpp
    lib/src/xiaoHttp_test.cpp
)

set(private_headers
    lib/src/CacheFile.h
    lib/src/HttpUtils.h
)

set(XIAOHTTP_HEADERS
    lib/inc/xiaoHttp/DrClassMap.h
    lib/inc/xiaoHttp/DrObject.h
    lib/inc/xiaoHttp/xiaoHttp_test.h
    lib/inc/xiaoHttp/Session.h
)


message("${CMAKE_CURRENT_BINARY_DIR}")
target_include_directories(
    ${PROJECT_NAME}
    PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}> $<INSTALL_INTERFACE:${INSTALL_INCLUDE_DIR}>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/exports>
    PRIVATE ${PROJECT_SOURCE_DIR}
)

set(PROJECT_BASE_PATH ${PROJECT_SOURCE_DIR})

if(BUILD_TESTING)
    message(STATUS "Building tests")
    enable_testing()
    add_subdirectory(lib/tests)
    add_subdirectory(xiaoNet/tests)
endif()

target_sources(
    ${PROJECT_NAME}
    PRIVATE ${XIAOHTTP_SOURCES}
            ${private_headers}
            ${CMAKE_CURRENT_BINARY_DIR}/exports/xiaoHttp/exports.h)


find_package(XiaoLog)
target_link_libraries(${PROJECT_NAME} PRIVATE XiaoLog::XiaoLog)

find_package(XiaoNet)
target_link_libraries(${PROJECT_NAME} PRIVATE XiaoNet::XiaoNet)

find_package(Jsoncpp REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC Jsoncpp_lib)
list(APPEND INCLUDE_DIRS_FOR_DYNAMIC_VIEW ${JSONCPP_INCLUDE_DIRS})

# install(
#     TARGETS xiaoNet
#     EXPORT XiaoNetTargets
#     RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
#     ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
#     LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
# )

# install(FILES ${CMAKE_CURRENT_BINARY_DIR}/exports/xiaoNet/exports.h DESTINATION ${INSTALL_INCLUDE_DIR}/xiaoNet)
# install(FILES ${public_net_headers} DESTINATION ${INSTALL_INCLUDE_DIR}/xiaoNet)
# install(FILES ${public_utils_headers} DESTINATION ${INSTALL_INCLUDE_DIR}/xiaoNet)

# include(CMakePackageConfigHelpers)
# configure_package_config_file(
#     cmake/templates/XiaoNetConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/XiaoNetConfig.cmake
#     INSTALL_DESTINATION ${INSTALL_XIAONET_CMAKE_DIR}
# )

# write_basic_package_version_file(
#     ${CMAKE_CURRENT_BINARY_DIR}/XiaoNetConfigVersion.cmake
#     VERSION ${XIAONET_VERSION}
#     COMPATIBILITY SameMajorVersion
# )

# install(
#     FILES "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/XiaoNetConfig.cmake"
#           "${CMAKE_CURRENT_BINARY_DIR}/XiaoNetConfigVersion.cmake"
#     DESTINATION "${INSTALL_XIAONET_CMAKE_DIR}"
#     COMPONENT dev
# )

# install(
#     EXPORT XiaoNetTargets
#     DESTINATION "${INSTALL_XIAONET_CMAKE_DIR}"
#     NAMESPACE XiaoNet::
#     COMPONENT dev
# )
