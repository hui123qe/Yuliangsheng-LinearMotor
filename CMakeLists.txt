cmake_minimum_required(VERSION 3.21)

project(Linear_motor_905 LANGUAGES CXX)

if(NOT WIN32 OR NOT MINGW)
    message(FATAL_ERROR "Linear_motor_905 requires a 64-bit MinGW toolchain on Windows.")
endif()

if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "Linear_motor_905 requires a 64-bit compiler.")
endif()

if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR
   NOT CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL "7.3.0")
    message(FATAL_ERROR "Linear_motor_905 requires the MinGW GCC 7.3.0 toolchain shipped with Qt 5.12.11.")
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(
    Qt5 5.12.11 EXACT REQUIRED
    COMPONENTS
        Concurrent
        Core
        Gui
        Network
        PrintSupport
        Widgets
        Xml
)

set(ACSCL_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/ACS/ACSCL_x64.LIB")
if(NOT EXISTS "${ACSCL_LIBRARY}")
    message(FATAL_ERROR "ACSCL import library not found: ${ACSCL_LIBRARY}")
endif()

add_library(ACS::CL_x64 UNKNOWN IMPORTED)
set_target_properties(
    ACS::CL_x64
    PROPERTIES
        IMPORTED_LOCATION "${ACSCL_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/ACS"
)

add_executable(
    Linear_motor_905 WIN32
    main/main.cpp
    main/widget.cpp
    main/widget.h
    main/datacollectworker.cpp
    main/datacollectworker.h
    main/ACSC.h
    login/login.cpp
    login/login.h
    login/login.ui
    log_system/logwindow.cpp
    log_system/logwindow.h
    log_system/savelog.cpp
    log_system/savelog.h
    NtcPtc/ITCPDevice.h
    NtcPtc/ModbusTemperatureServer.cpp
    NtcPtc/ModbusTemperatureServer.h
    tem_ModbusTcp/TemperatureModule.cpp
    tem_ModbusTcp/TemperatureModule.h
    tt_tools/ttools.cpp
    tt_tools/ttools.h
    tt_tools/tt_tem.hpp
    t_QCustomPlot/qcustomplot.cpp
    t_QCustomPlot/qcustomplot.h
    main/widget.ui
    search.qrc
)

target_include_directories(
    Linear_motor_905
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/login"
        "${CMAKE_CURRENT_SOURCE_DIR}/log_system"
        "${CMAKE_CURRENT_SOURCE_DIR}/main"
        "${CMAKE_CURRENT_SOURCE_DIR}/NtcPtc"
        "${CMAKE_CURRENT_SOURCE_DIR}/tem_ModbusTcp"
        "${CMAKE_CURRENT_SOURCE_DIR}/tt_tools"
        "${CMAKE_CURRENT_SOURCE_DIR}/t_QCustomPlot"
)

target_link_libraries(
    Linear_motor_905
    PRIVATE
        ACS::CL_x64
        Qt5::Concurrent
        Qt5::Core
        Qt5::Gui
        Qt5::Network
        Qt5::PrintSupport
        Qt5::Widgets
        Qt5::Xml
)

set(QT_RUNTIME_BIN_DIR "D:/Qt/Qt5.12.11/5.12.11/mingw73_64/bin")

find_program(
    WINDEPLOYQT_EXECUTABLE
    NAMES windeployqt.exe
    HINTS "${QT_RUNTIME_BIN_DIR}"
    NO_DEFAULT_PATH
    REQUIRED
)

# Run explicitly with: cmake --build --preset <build-preset> --target deploy_qt_runtime
# Keeping this target out of the default build avoids redeploying after every build.
add_custom_target(
    deploy_qt_runtime
    COMMAND
        "${CMAKE_COMMAND}" -E env
        "PATH=${QT_RUNTIME_BIN_DIR};$ENV{PATH}"
        "${WINDEPLOYQT_EXECUTABLE}"
        "$<IF:$<CONFIG:Debug>,--debug,--release>"
        --compiler-runtime
        "$<TARGET_FILE:Linear_motor_905>"
    DEPENDS Linear_motor_905
    COMMENT "Deploying the Qt and MinGW runtime beside Linear_motor_905"
    VERBATIM
)
