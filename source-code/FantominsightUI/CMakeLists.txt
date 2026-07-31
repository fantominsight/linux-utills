cmake_minimum_required(VERSION 3.16)

project(FantominsightUI VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Network)

qt_add_executable(FantominsightUI
    main.cpp
    MainWindow.h
    MainWindow.cpp
    Page.h
    pages/Base64Page.h
    pages/Base64Page.cpp
    pages/CommandPage.h
    pages/CommandPage.cpp
    pages/CpuUsagePage.h
    pages/CpuUsagePage.cpp
    pages/DisksPage.h
    pages/DisksPage.cpp
    pages/DnsPage.h
    pages/DnsPage.cpp
    pages/HashPage.h
    pages/HashPage.cpp
    pages/NetworkPage.h
    pages/NetworkPage.cpp
    pages/PasswordPage.h
    pages/PasswordPage.cpp
    pages/PingPage.h
    pages/PingPage.cpp
    pages/PortPage.h
    pages/PortPage.cpp
    pages/RamPage.h
    pages/RamPage.cpp
    pages/SysInfoPage.h
    pages/SysInfoPage.cpp
    pages/TracePage.h
    pages/TracePage.cpp
)

target_include_directories(FantominsightUI PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(FantominsightUI PRIVATE Qt6::Widgets Qt6::Network)
