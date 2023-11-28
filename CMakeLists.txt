cmake_minimum_required(VERSION 3.7)

project(dde-guard)

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

add_executable(${PROJECT_NAME}
    "main.cpp"
    "guard.h"
    "guard.cpp"
    "recorder.h"
    "recorder.cpp")

target_include_directories(dde-guard PUBLIC
    ${DtkWidget_INCLUDE_DIRS}
    ${Qt5DBus_INCLUDE_DIRS}
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Concurrent_INCLUDE_DIRS}
    ${OBJECT_BINARY_DIR}
    )

target_link_libraries(dde-guard
    ${DtkWidget_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
    ${Qt5DBus_LIBRARIES}
    ${Qt5Core_LIBRARIES}
    ${Qt5Concurrent_LIBRARIES}
    PolkitQt5-1::Agent
    )

# Install settings
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX /usr)
endif ()

install(FILES misc/org.deepin.dde.Guard.service
    DESTINATION lib/systemd/user/
)

install(FILES misc/dbus/org.deepin.dde.Guard.service
    DESTINATION share/dbus-1/services/
)

# TODO 以下内容为调试使用

# 模拟应用安装的服务文件
install(FILES misc/create_account.service
    DESTINATION /var/lib/deepin/dde-guard/services/
)

# 模拟应用安装的policy文件，用于操作时提权
install(FILES misc/org.deepin.dde.guard.delete_user.policy
    DESTINATION share/polkit-1/actions/
)

# 模拟应用安装的check脚本
install(FILES misc/check_desktop.sh
    DESTINATION bin/
)

# 模拟应用安装的repair脚本,注意，脚本需要有可执行权限
install(FILES misc/delete_user.sh
    DESTINATION bin/
)
