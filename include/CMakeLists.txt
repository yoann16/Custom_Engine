# -----------------------------------------------------------------------------
# Liste des headers publics du moteur.
# Ajoute ici chaque nouveau .h / .hpp que tu crees dans include/.
# Ex : ${CMAKE_CURRENT_SOURCE_DIR}/Renderer.h
# -----------------------------------------------------------------------------
set(ENGINE_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/DX12.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Vulkan.h
    PARENT_SCOPE
)
