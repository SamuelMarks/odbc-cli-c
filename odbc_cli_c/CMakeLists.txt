get_filename_component(EXEC_NAME "${CMAKE_CURRENT_SOURCE_DIR}" NAME)

###########
# Library #
###########

set(LIBRARY_NAME "lib${EXEC_NAME}")

set(Header_Files "odbc_cli_c.h" "odbc_runner.h")
source_group("${LIBRARY_NAME} Header Files" FILES "${Source_Files}")

set(Source_Files "odbc_cli_c.c" "odbc_runner.c")
source_group("${LIBRARY_NAME} Source Files" FILES "${Source_Files}")

add_library("${LIBRARY_NAME}" STATIC "${Header_Files}" "${Source_Files}")

include(GNUInstallDirs)
target_include_directories(
        "${LIBRARY_NAME}"
        PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
target_compile_definitions(
        "${LIBRARY_NAME}"
        INTERFACE
        "_${TARGET_ARCH}_"
)
set_target_properties("${LIBRARY_NAME}" PROPERTIES LINKER_LANGUAGE C)

set_property(TARGET "${LIBRARY_NAME}" PROPERTY VERSION "1.0.0")
set_property(TARGET "${LIBRARY_NAME}" PROPERTY SOVERSION "1")

#######
# CLI #
#######

set(Cli_Source_Files "odbc_runner.c" "main.c")
source_group("${EXEC_NAME} Source Files" FILES "${Source_Files}")

add_executable("${EXEC_NAME}" "${Cli_Source_Files}")

include(GNUInstallDirs)
include_directories("common")
target_include_directories(
        "${EXEC_NAME}"
        PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common>"
        "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_link_libraries("${EXEC_NAME}" PUBLIC "${LIBRARY_NAME}")

find_package(c89stringutils CONFIG REQUIRED)
target_link_libraries("${EXEC_NAME}" PRIVATE "c89stringutils" "c89stringutils_compiler_flags")

find_package(unixodbc REQUIRED)
target_link_libraries("${EXEC_NAME}" PRIVATE UNIX::odbc)

set_target_properties("${EXEC_NAME}" PROPERTIES LINKER_LANGUAGE C)

#################
# Install rules #
#################

include(GNUInstallDirs)

install(TARGETS "${EXEC_NAME}"
        DESTINATION "${CMAKE_INSTALL_BINDIR}")

install(FILES "common/odbc_cli_c_stdbool.h" ${Cli_Header_Files} ${Header_Files}
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

set(installable_libs "${LIBRARY_NAME}" "${PROJECT_NAME}_compiler_flags")
if (TARGET "${DEPENDANT_LIBRARY}")
    list(APPEND installable_libs "${DEPENDANT_LIBRARY}")
endif (TARGET "${DEPENDANT_LIBRARY}")

install(TARGETS ${installable_libs}
        EXPORT "${LIBRARY_NAME}Targets"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
