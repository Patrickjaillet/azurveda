# Changelog

Tous les changements notables apportés à ce projet seront documentés dans ce fichier.

Le format est basé sur [Keep a Changelog](https://keepachangelog.com/fr/1.1.0/),
et ce projet adhère au [Semantic Versioning](https://semver.org/lang/fr/).

## [Unreleased]

### Added

- Initialisation du dépôt Git pour le workspace AzurVeda.
- `CHANGELOG.md` (ce fichier).
- `README.md` décrivant le projet, les prérequis et la procédure de build.
- `.gitignore` pour les artefacts de build (`Debug/`, `Release/`, `x64/`, `.vs/`, `out/`, etc.).
- Schéma de versionnage sémantique (`MAJOR.MINOR.PATCH`) piloté par un fichier `VERSION`
  et les tags Git, avec génération automatique d'un header `Version.h` via
  `cmake/GenerateVersion.cmake`.
- Build **CMake** (x64, MSVC, C++23) : un `CMakeLists.txt` par module
  (`Veda`, `VedaLibMath`, `VedaLibImage`, `VedaLibImageJPEG`, `VedaLibSoundMP3`,
  `VedaLibSoundXM`, `VedaMachineOGL`, `VedaMachineOGLWinDxSound`, `VedaLib3DEngine`,
  `VedaLibDemo`, `VedaGUIWindowsMFC`, `Veda/CodeExamples`) plus un `CMakeLists.txt`
  racine et `CMakePresets.json` (`windows-x64`, `windows-x64-lgpl`, `ci-windows-x64`).
- `vcpkg.json` (manifest mode) : dépendance `libjpeg-turbo`.
- Options CMake `AZURVEDA_BUILD_GPL_MODULES`, `AZURVEDA_BUILD_MFC_EDITOR`,
  `AZURVEDA_BUILD_EXAMPLES`.
- CI GitHub Actions (`.github/workflows/ci.yml`) : build MSVC x64 + vcpkg, exécution de
  `Example01_SimpleSerialization`, vérification de CHANGELOG.md sur chaque PR,
  incrément automatique du `PATCH` de `VERSION` sur chaque build réussi de `main`.

### Changed

- `VedaLibImageJPEG` : remplacement de la copie vendue d'IJG libjpeg 6b par la
  dépendance vcpkg `libjpeg-turbo` ; le module ne contient plus que le code propre à
  AzurVeda (`ImageJPEG.cpp/.h`, `RegisterLibImageJPEG.cpp/.h`).

### Fixed

- N/A

### Removed

- Copie vendue d'IJG libjpeg 6b dans `VedaLibImageJPEG` (~24 fichiers `.c`/`.h`),
  remplacée par `libjpeg-turbo` via vcpkg.

[Unreleased]: https://github.com/OWNER/AzurVeda/compare/HEAD
