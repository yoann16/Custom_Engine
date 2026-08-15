cmake_minimum_required(VERSION 3.21)
project(Custom_Engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Evite un bug d'interaction CMake/MSVC ou FindThreads (declenche par glfw3)
# tente de compiler un test pthread en dialecte CXX26 et plante inutilement.
# pthread n'existe pas sur Windows de toute facon : on force les threads natifs.
if(WIN32)
    set(CMAKE_USE_WIN32_THREADS_INIT 1)
endif()

# ---------------------------------------------------------------------------
# Options de build : on peut activer/desactiver chaque backend independamment
# ---------------------------------------------------------------------------
option(ENGINE_ENABLE_VULKAN "Compiler le backend Vulkan" ON)
option(ENGINE_ENABLE_DX12   "Compiler le backend DirectX12 (Windows only)" ${WIN32})

if(ENGINE_ENABLE_DX12 AND NOT WIN32)
    message(WARNING "DX12 desactive : uniquement disponible sur Windows.")
    set(ENGINE_ENABLE_DX12 OFF)
endif()

# ---------------------------------------------------------------------------
# Dependances communes (via vcpkg)
# ---------------------------------------------------------------------------
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

# ---------------------------------------------------------------------------
# Backend Vulkan
# ---------------------------------------------------------------------------
if(ENGINE_ENABLE_VULKAN)
    find_package(Vulkan REQUIRED)   # utilise VULKAN_SDK (installe via LunarG)
    find_package(unofficial-spirv-reflect CONFIG REQUIRED)
    message(STATUS "Backend Vulkan : ACTIVE  (SDK: ${Vulkan_INCLUDE_DIRS})")
endif()

# ---------------------------------------------------------------------------
# Backend DirectX12 (Windows uniquement)
# ---------------------------------------------------------------------------
if(ENGINE_ENABLE_DX12)
    find_package(directx-headers CONFIG REQUIRED)
    message(STATUS "Backend DX12 : ACTIVE")
endif()

# ---------------------------------------------------------------------------
# Sous-dossiers : include/ liste les headers, src/ definit la cible
# ---------------------------------------------------------------------------
add_subdirectory(include)
add_subdirectory(src)
