cmake_minimum_required(VERSION 3.7)

set(BIN_NAME dde-transactionprotect)

project(${BIN_NAME})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

# Find the library
find_package(PkgConfig REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5DBus REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Concurrent REQUIRED)
find_package(DtkWidget REQUIRED)
find_package(PolkitQt5-1)

add_executable(${BIN_NAME}
    "main.cpp"
    "transaction_service.h"
    "transaction_service.cpp"
    "protection_manager.h"
    "protection_manager.cpp")

target_include_directories(${BIN_NAME} PUBLIC
    ${DtkWidget_INCLUDE_DIRS}
    ${Qt5DBus_INCLUDE_DIRS}
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Concurrent_INCLUDE_DIRS}
    ${OBJECT_BINARY_DIR}
    )

target_link_libraries(${BIN_NAME}
    ${DtkWidget_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
    ${Qt5DBus_LIBRARIES}
    ${Qt5Core_LIBRARIES}
    ${Qt5Concurrent_LIBRARIES}
    PolkitQt5-1::Agent
    )

#if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX /usr)
#endif ()

# bin
install(TARGETS ${BIN_NAME} DESTINATION bin/)

# systemd service
install(FILES misc/systemd/org.deepin.dde.TransactionProtect.service
    DESTINATION lib/systemd/system/
    )

# dbus service
install(FILES misc/dbus/org.deepin.dde.TransactionProtect.service
    DESTINATION share/dbus-1/services/
    )

# root dbus
install(FILES misc/org.deepin.dde.TransactionProtect.conf
    DESTINATION share/dbus-1/system.d/
    )
