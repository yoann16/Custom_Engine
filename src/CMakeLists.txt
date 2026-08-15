# -----------------------------------------------------------------------------
# Liste des sources du moteur.
# Ajoute ici chaque nouveau .cpp que tu crees dans src/.
# Ex : ${CMAKE_CURRENT_SOURCE_DIR}/Renderer.cpp
# -----------------------------------------------------------------------------
set(ENGINE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    # ${CMAKE_CURRENT_SOURCE_DIR}/Renderer.cpp
)

add_executable(Custom_Engine ${ENGINE_SOURCES} ${ENGINE_HEADERS})

target_include_directories(Custom_Engine PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(Custom_Engine PRIVATE
    glfw
    glm::glm
)

if(ENGINE_ENABLE_VULKAN)
    target_link_libraries(Custom_Engine PRIVATE
        Vulkan::Vulkan
        unofficial::spirv-reflect
    )
    target_compile_definitions(Custom_Engine PRIVATE ENGINE_USE_VULKAN)
endif()

if(ENGINE_ENABLE_DX12)
    target_link_libraries(Custom_Engine PRIVATE
        Microsoft::DirectX-Headers
        d3d12.lib
        dxgi.lib
        d3dcompiler.lib
    )
    target_compile_definitions(Custom_Engine PRIVATE ENGINE_USE_DX12)
endif()
