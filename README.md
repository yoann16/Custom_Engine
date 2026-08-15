# Custom_Engine

## Objectif

Développer un moteur de rendu maison capable d'afficher la **même scène** (à commencer par un simple triangle coloré) indifféremment via **Vulkan** ou **DirectX12**, avec possibilité de choisir/basculer le backend graphique.

## À propos de ce projet

La configuration CMake (structure du projet, gestion des dépendances via vcpkg, compilation automatisée des shaders Slang, intégration Visual Studio / Ninja) ainsi que la mise en place initiale du build ont été réalisées avec l'aide d'une IA (Claude, Anthropic). Le code applicatif du moteur (logique de rendu, gestion des ressources, etc.) est écrit et compris pas à pas par moi au fil de l'apprentissage.

## État actuel

- ✅ Build fonctionnel sous Windows via **Ninja** et **Visual Studio 2022**
- ✅ Backend **Vulkan** actif : instance, device, swapchain, pipeline, triangle affiché (suivi du [tuto officiel Vulkan](https://docs.vulkan.org/tutorial/latest/00_Introduction.html))
- ✅ Shaders écrits en **Slang**, compilés automatiquement en SPIR-V via `slangc` (fourni par le Vulkan SDK)
- ⏳ Backend **DirectX12** : dépendances configurées (DirectX-Headers, D3D12), implémentation du triangle à faire
- ⏳ Abstraction commune Vulkan / DX12 (choix du backend à la compilation ou au runtime)

## Prérequis

- Visual Studio 2022 (workload *Desktop development with C++*)
- [Vulkan SDK](https://vulkan.lunarg.com/) (fournit aussi `slangc` pour compiler les shaders Slang)
- [vcpkg](https://github.com/microsoft/vcpkg) (mode classique — voir `BUILD.md`)
- CMake ≥ 3.21, Ninja

## Build

Voir [`BUILD.md`](./BUILD.md) pour les instructions détaillées (installation des dépendances, configuration, compilation).

Résumé rapide :

```
vcpkg install glfw3:x64-windows glm:x64-windows spirv-reflect:x64-windows directx-headers:x64-windows

REM Build Ninja (ligne de commande)
cmake --preset windows-default
cmake --build build

REM Build Visual Studio (genere un .sln)
cmake --preset windows-vs2022
```

## Structure du projet

```
Custom_Engine/
├── CMakeLists.txt        # Configuration racine : options des backends, dependances
├── CMakePresets.json      # Presets Ninja et Visual Studio
├── include/                # Headers publics du moteur
├── shaders/                 # Shaders Slang (.slang), compiles en SPIR-V au build
├── src/                      # Code source du moteur
└── BUILD.md                   # Instructions de build detaillees
```

## Backends graphiques

| Backend | Statut | Activation |
|---|---|---|
| Vulkan | Triangle fonctionnel | `ENGINE_ENABLE_VULKAN` (ON par defaut) |
| DirectX12 | En cours | `ENGINE_ENABLE_DX12` (ON par defaut sur Windows) |

Les deux backends peuvent être activés/désactivés indépendamment via les options CMake correspondantes.
