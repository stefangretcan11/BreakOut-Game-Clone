cmake_minimum_required(VERSION 3.17)
project(SDL3_Project_SelfContained)

set(CMAKE_CXX_STANDARD 17)

# --- 1. CONFIGURARE GLM ---
# Include rădăcina proiectului pentru a putea face #include <glm/glm.hpp>
include_directories(${CMAKE_SOURCE_DIR})


# --- 2. CONFIGURARE SDL3 (MANUALĂ) ---

# A. HEADERE (.h)
# Îi spunem unde să caute SDL.h. Adăugăm ambele variante posibile:
include_directories("${CMAKE_SOURCE_DIR}/SDL3/include")
include_directories("${CMAKE_SOURCE_DIR}/SDL3")

# B. LIBRĂRIA (.lib)
# Căutăm fișierul SDL3.lib în folderul tău SDL3
find_library(SDL3_LIB_FILE
        NAMES SDL3
        PATHS
        "${CMAKE_SOURCE_DIR}/SDL3"
        "${CMAKE_SOURCE_DIR}/SDL3/lib"
        "${CMAKE_SOURCE_DIR}/SDL3/x86_64-w64-mingw32/lib" # Structură comună la download
        NO_DEFAULT_PATH
)

if(NOT SDL3_LIB_FILE)
    message(FATAL_ERROR "NU AM GĂSIT SDL3.lib! Verifică dacă în folderul SDL3 există un fișier SDL3.lib")
else()
    message(STATUS "Am găsit librăria SDL3 la: ${SDL3_LIB_FILE}")
endif()


# --- 3. EXECUTABILUL ---
add_executable(SDL3_Project_SelfContained main.cpp circle.cpp circle.h)


# --- 4. LINKARE ---
# Legăm executabilul de fișierul .lib găsit mai sus
target_link_libraries(SDL3_Project_SelfContained PRIVATE ${SDL3_LIB_FILE})