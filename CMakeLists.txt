cmake_minimum_required(VERSION 3.5)

project(copybook VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QT_LIBS
    PrintSupport
    Widgets
)

find_package(QT NAMES Qt6 Qt5 COMPONENTS ${QT_LIBS} LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${QT_LIBS} LinguistTools REQUIRED)

set(TS_FILES copybook_zh_CN.ts)

set(PROJECT_SOURCES
    common.h
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(copybook WIN32
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET copybook APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(copybook SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(copybook WIN32
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_compile_definitions(copybook
    PRIVATE	QT_DEPRECATED_WARNINGS QT_RESTRICTED_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII
            QT_NO_CAST_FROM_BYTEARRAY QT_USE_QSTRINGBUILDER QT_NO_PROCESS_COMBINED_ARGUMENT_START
            RAPIDJSON_HAS_STDSTRING=1 WIN32_LEAN_AND_MEAN
)
target_precompile_headers(copybook PRIVATE "common.h")

foreach(L ${QT_LIBS})
    target_precompile_headers(copybook PRIVATE <Qt${L}>)
    target_link_libraries(copybook PRIVATE Qt${QT_VERSION_MAJOR}::${L})
endforeach()

set_target_properties(copybook PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(copybook)
endif()
