# -----------------------------------------------------------------------------
# Compilation du shader Slang en SPIR-V via slangc, en suivant la fonction
# CMake officielle du tuto Vulkan :
# https://docs.vulkan.org/tutorial/latest/03_Drawing_a_triangle/02_Graphics_pipeline_basics/01_Shader_modules.html
#
# slangc est fourni par le Vulkan SDK (Bin/slangc.exe).
# -----------------------------------------------------------------------------
find_program(SLANGC_EXECUTABLE
    NAMES slangc slangc.exe
    HINTS "$ENV{VULKAN_SDK}/Bin" "$ENV{VULKAN_SDK}/bin"
    REQUIRED
)
message(STATUS "slangc trouve : ${SLANGC_EXECUTABLE}")

# Ici SHADERS_DIR = ce dossier meme (shaders/), pas besoin de re-ajouter /shaders
# comme le fait la fonction originale du tuto (qui, elle, est appelee depuis la racine).
set(SHADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SHADER_SLANG_SOURCES ${SHADERS_DIR}/shader.slang)
set(ENTRY_POINTS -entry vertMain -entry fragMain)

add_custom_command(
    OUTPUT  ${SHADERS_DIR}/slang.spv
    COMMAND ${SLANGC_EXECUTABLE} ${SHADER_SLANG_SOURCES} -target spirv -profile spirv_1_4 -emit-spirv-directly -fvk-use-entrypoint-name ${ENTRY_POINTS} -o slang.spv
    WORKING_DIRECTORY ${SHADERS_DIR}
    DEPENDS ${SHADER_SLANG_SOURCES}
    COMMENT "Compilation shader Slang"
    VERBATIM
)

add_custom_target(CompileShaders DEPENDS ${SHADERS_DIR}/slang.spv)

# Remonte la liste des shaders vers le CMakeLists racine, pour affichage dans l'IDE
set(ENGINE_SHADERS ${SHADER_SLANG_SOURCES} PARENT_SCOPE)
