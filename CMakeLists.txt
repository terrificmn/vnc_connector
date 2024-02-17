cmake_minimum_required(VERSION 3.16)

project(vnc_connector VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(yaml-cpp_DIR /usr/lib64/cmake/yaml-cpp) # -- rocky
set(yaml-cpp_DIR /usr/lib/x86_64-linux-gnu/cmake/yaml-cpp) # --ubuntu

find_package(Qt6 6.2 COMPONENTS Quick REQUIRED)

find_package(
    yaml-cpp REQUIRED
)

qt_add_executable(appvnc_connector
    main.cpp
    ssh_helper.h ssh_helper.cpp
)

qt_add_qml_module(appvnc_connector
    URI vnc_connector
    VERSION 1.0
    QML_FILES main.qml 
)

set_target_properties(appvnc_connector PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appvnc_connector
    PRIVATE Qt6::Quick
    ${YAML_CPP_LIBRARIES}
    )

install(TARGETS appvnc_connector
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
