# Custom_Engine — Guide de build

## 1. Prérequis à installer

1. **Visual Studio 2022** avec le workload *"Desktop development with C++"*
   → te donne le compilateur MSVC + le **Windows SDK** (nécessaire pour DX12 : `d3d12.h`, `dxgi1_6.h`...).
2. **Vulkan SDK** : https://vulkan.lunarg.com/sdk/home#windows
   → installe le SDK complet (headers, loader, validation layers, `glslangValidator`, et **`slangc`** pour compiler les shaders Slang).
   → vérifie ensuite dans un terminal : `echo %VULKAN_SDK%` doit afficher un chemin.
3. **vcpkg** (si pas déjà fait) :
   ```
   git clone https://github.com/microsoft/vcpkg.git
   cd vcpkg
   .\bootstrap-vcpkg.bat
   ```
   Puis définis la variable d'environnement `VCPKG_ROOT` pointant vers ce dossier.
4. **CMake ≥ 3.21** et **Ninja** (souvent installés avec VS, sinon `winget install Kitware.CMake` / `winget install Ninja-build.Ninja`).
5. **Installer les libs en mode classique** (une seule fois, globalement — pas besoin de refaire ça par projet) :
   ```
   vcpkg install glfw3:x64-windows glm:x64-windows spirv-reflect:x64-windows directx-headers:x64-windows
   ```
   Vérifie ensuite avec `vcpkg list` que tout apparaît.

   Note : `slangc` (compilateur de shaders Slang) n'a **pas** besoin de vcpkg — il est fourni directement par le Vulkan SDK installé à l'étape 2 (`%VULKAN_SDK%\Bin\slangc.exe`).

## 2. Structure du projet

```
Custom_Engine/
├── CMakeLists.txt        # Racine : options des backends, dependances (find_package)
├── CMakePresets.json      # Presets "windows-default" (Ninja) et "windows-vs2022" (Visual Studio)
├── include/
│   └── CMakeLists.txt      # Liste des headers du moteur (ENGINE_HEADERS)
├── shaders/
│   ├── CMakeLists.txt       # Compilation automatique .slang -> .spv via slangc
│   └── shader.slang          # Shader unique (vertex + fragment, entry points vertMain/fragMain)
└── src/
    ├── CMakeLists.txt         # Definit la cible executable Custom_Engine
    └── main.cpp                 # Point d'entree du moteur
```

Pas de `vcpkg.json` — le projet est en **mode vcpkg classique** (libs installées globalement, étape 5 ci-dessus), pas en mode manifeste.

## 3. Build

Deux façons de builder, au choix (elles utilisent le même code source) :

### Option A — Ligne de commande (Ninja)

Depuis un terminal **x64 Native Tools Command Prompt for VS 2022** (nécessaire pour que Ninja trouve `cl.exe`/`link.exe`) :

```
cd C:\Dev\Custom_Engine
cmake --preset windows-default
cmake --build build
.\build\src\Custom_Engine.exe
```

### Option B — Visual Studio (génère un `.sln`)

Depuis n'importe quel terminal (pas besoin du terminal x64 spécial ici — le générateur Visual Studio gère ça tout seul) :

```
cd C:\Dev\Custom_Engine
cmake --preset windows-vs2022
```

Puis ouvre `build-vs\Custom_Engine.sln` (ou `cmake --open build-vs`). Dans Visual Studio : clic droit sur `Custom_Engine` dans l'explorateur de solutions → **"Définir comme projet de démarrage"**, puis F5.

> Astuce : après tout changement dans un `CMakeLists.txt` (nouveau fichier, nouvelle dépendance...), il faut relancer `cmake --preset ...` pour régénérer les fichiers de build/la solution — un simple `cmake --build` ne suffit pas.

## 4. Emplacement du projet — important

**Ne place jamais ce repo dans `Documents`, `Bureau`, ou un dossier synchronisé (OneDrive, Nextcloud, Google Drive...)**. Un dossier de build C++ génère des centaines de fichiers temporaires par compilation, ce qui entre régulièrement en conflit avec les protections/indexations de ces dossiers (verrous de fichiers, erreurs `LNK1104`, `PDB API call failed`...). Utilise un chemin neutre et court, ex. `C:\Dev\Custom_Engine`.

## 5. Exécuter

Le programme ouvre une fenêtre "Vulkan" et affiche un triangle coloré (dégradé rouge/vert/bleu par sommet).

Le shader compilé (`shaders/slang.spv`) est chargé via un **chemin absolu** injecté à la compilation (macro `SHADER_DIR`), donc l'exécutable fonctionne peu importe comment tu le lances (terminal, double-clic, F5 dans VS) — pas besoin d'être positionné dans un dossier précis.

## 6. Si ça casse

- `find_package(Vulkan REQUIRED)` échoue → `VULKAN_SDK` n'est pas défini, relance un terminal après install du SDK.
- `find_package(glfw3 CONFIG REQUIRED)` (ou `glm`, `unofficial-spirv-reflect`, `directx-headers`) échoue → vérifie que `VCPKG_ROOT` est bien exporté, que le preset utilise bien le bon toolchain, ET que tu as bien lancé `vcpkg install ...` (étape 5) avant de configurer.
- `find_program(SLANGC_EXECUTABLE ...)` échoue → vérifie que `%VULKAN_SDK%\Bin\slangc.exe` existe bien (fourni par le Vulkan SDK).
- Erreurs de link DX12 (`d3d12.lib` introuvable) → vérifie que le Windows SDK est bien installé via le VS Installer.
- `LNK1104: cannot open file` ou `PDB API call failed` → voir section 4 (emplacement du dossier). Si ça persiste même hors dossier synchronisé, nettoie le cache de build (`rmdir /s /q build` ou `build-vs`) et reconfigure entièrement.
- Erreurs `designated initialization can only be used to initialize aggregate class types` sur des structs `vk::...` → la macro `VULKAN_HPP_NO_STRUCT_CONSTRUCTORS` doit être définie (déjà fait dans `src/CMakeLists.txt`) ; si l'erreur persiste, vérifie qu'elle n'a pas été retirée par erreur.
