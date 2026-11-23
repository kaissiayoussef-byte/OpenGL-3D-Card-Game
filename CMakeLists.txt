cmake_minimum_required(VERSION 3.10)
project(GameOpenJL)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# مسارات ملفات الهيدر والمكتبات
include_directories(${CMAKE_SOURCE_DIR}/include)
link_directories(${CMAKE_SOURCE_DIR}/lib)

# الملفات المصدرية
add_executable(GameOpenJL
    src/main.cpp
    src/glad.c
)

# ربط مكتبات OpenGL و GLFW لنظام Windows مع MinGW
target_link_libraries(GameOpenJL
    glfw3
    opengl32
    gdi32
)

# 1. نسخ glfw3.dll تلقائياً بجانب ملف التشغيل
add_custom_command(TARGET GameOpenJL POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/lib/glfw3.dll"
    $<TARGET_FILE_DIR:GameOpenJL>
)

# 2. نسخ مجلد assets تلقائياً إلى مجلد ملف التشغيل بعد كل Build
add_custom_command(TARGET GameOpenJL POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:GameOpenJL>/assets"
)