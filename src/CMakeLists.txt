# -----------------------------------------------------------------------------
# Liste des sources du moteur.
# Ajoute ici chaque nouveau .cpp que tu crees dans src/.
# Ex : ${CMAKE_CURRENT_SOURCE_DIR}/Renderer.cpp
# -----------------------------------------------------------------------------
set(ENGINE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Vulkan.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DX12.cpp
)

add_executable(Custom_Engine ${ENGINE_SOURCES} ${ENGINE_HEADERS} ${ENGINE_SHADERS})

# Les .slang ne sont pas compiles par le compilateur C++ (ils passent par slangc
# via CompileShaders) : on les marque comme "non compilables" pour que VS les
# affiche sans essayer de les passer a cl.exe.
if(ENGINE_SHADERS)
    set_source_files_properties(${ENGINE_SHADERS} PROPERTIES HEADER_FILE_ONLY TRUE)
endif()

# Recree dans Visual Studio une arborescence de filtres qui reflete la vraie
# structure de dossiers sur le disque (src/, include/, shaders/), au lieu de
# tout mettre a plat dans "Source Files".
source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${ENGINE_SOURCES} ${ENGINE_HEADERS} ${ENGINE_SHADERS})

# Force le repertoire de travail du debogueur VS a la racine du projet,
# pour que les chemins relatifs comme "shaders/slang.spv" fonctionnent
# aussi bien via F5 dans Visual Studio qu'en ligne de commande.
set_target_properties(Custom_Engine PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

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
    target_compile_definitions(Custom_Engine PRIVATE
        ENGINE_USE_VULKAN
        VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
        SHADER_DIR="${CMAKE_SOURCE_DIR}/shaders"
    )
    add_dependencies(Custom_Engine CompileShaders)
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
