cmake_minimum_required(VERSION 3.10)
project(snake3d CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SYSROOT "/tmp/local/sysroot")

include_directories(${SYSROOT}/usr/include)
link_directories(${SYSROOT}/usr/lib/x86_64-linux-gnu)

add_executable(snake3d src/main.cpp)

# glut static, everything else dynamic
target_link_libraries(snake3d
    ${SYSROOT}/usr/lib/x86_64-linux-gnu/libglut.a
    GLU GL X11 Xext xcb Xau Xdmcp Xi Xxf86vm
    m pthread dl
)

target_compile_options(snake3d PRIVATE -I${SYSROOT}/usr/include)
target_link_options(snake3d PRIVATE
    -L${SYSROOT}/usr/lib/x86_64-linux-gnu
    -Wl,-rpath,\$ORIGIN
    -Wl,-rpath,/usr/lib/x86_64-linux-gnu
)
