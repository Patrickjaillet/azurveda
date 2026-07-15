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
- `/W4` activé projet entier (`CMakeLists.txt` racine).

### Changed

- `VedaLibImageJPEG` : remplacement de la copie vendue d'IJG libjpeg 6b par la
  dépendance vcpkg `libjpeg-turbo` ; le module ne contient plus que le code propre à
  AzurVeda (`ImageJPEG.cpp/.h`, `RegisterLibImageJPEG.cpp/.h`).
- Traduction intégrale en anglais des ressources visibles utilisateur de
  `VedaGUIWindowsMFC` : `VedaDemoOGLMfcGui.rc` (menus, string table, boîtes de
  dialogue, `VERSIONINFO`) et `res/VedaDemoOGLMfcGui.rc2`, y compris la métadonnée de
  langue de la ressource compilée (`LANG_FRENCH` → `LANG_ENGLISH, SUBLANG_ENGLISH_US`,
  codepage `040c` → `0409`). Deux messages `TRACE0` en français traduits dans
  `VedaGUIWindowsMFC/MainFrm.cpp`.
- Suppression de l'intégralité des commentaires du code source AzurVeda (255 fichiers
  `.cpp`/`.h`/`.c`, tous les modules sauf le code tiers vendu `VedaLibSoundMP3`
  (décodeur MP3, fork de libmad) et `VedaLibSoundXM` (moteur XM `uniminixm`), non
  écrit par AzurVeda et donc hors périmètre). Les 158 fichiers qui portaient la
  mention de licence LGPL 2.1 en commentaire d'en-tête conservent une ligne unique
  `// SPDX-License-Identifier: LGPL-2.1-only` à la place de l'ancien bloc Doxygen.
- `VedaMachineOGLWinDxSound` : remplacement de **DirectSound** par **XAudio2**
  (2.9, livré avec Windows 10/11, aucune lib supplémentaire à lier). Le modèle
  « circular buffer » DirectSound (lock/unlock d'une région active en lecture) est
  remplacé par la soumission de buffers en rotation (4 buffers de 1024 frames,
  profondeur de file cible 3), pilotée par le même thread de notification (poll
  20 ms) qu'auparavant, désormais basé sur `IXAudio2SourceVoice::GetState()` plutôt
  que sur `GetCurrentPosition()`. Les décodeurs (MP3, XM) et toute l'interface
  publique de `VirtualMachine` liée au son sont strictement inchangés — seule la
  couche de sortie change, conformément à `roadmap.md` Phase 4.

### Fixed

- Bugs de compilation réels révélés en validant le build CMake sur une machine
  Windows/MSVC réelle (VS Build Tools 2022 + MFC/ATL, en attendant VS2026) :
  - Littéraux chaîne assignés à des pointeurs `char*`/`LPSTR` non-const, rejetés en
    C++23 strict (`Veda/PackStruct.cpp`, `Veda/PackULong_Enums.cpp`,
    `Veda/PackULong_Flags.cpp`, `VedaGUIWindowsMFC/Dialog_PackListDynamic.cpp`,
    `VedaGUIWindowsMFC/LeftView.cpp`).
  - Signature `OnTimer(UINT)` obsolète : le macro `ON_WM_TIMER()` de MFC exige
    `OnTimer(UINT_PTR)` en 64-bit (`VedaGUIWindowsMFC/OpenGLWnd.cpp/.h`,
    `VedaGUIWindowsMFC/PreviewManagerView.cpp/.h`).
  - `goto` traversant une initialisation de variable dans le chargeur LWO
    (`VedaLib3DEngine/Object3DMesh3D.cpp`).
  - Assembleur inline x86 (`__asm`) du décodeur MP3, invalide sous MSVC x64 : le
    chemin `FPM_INTEL` de libmad est remplacé par le chemin portable `FPM_64BIT`.
  - En-tête DirectX legacy `dxerr8.h` (absent du SDK Windows moderne) retiré de
    `VedaMachineOGLWinDxSound/OGLMachineWinDxSound_Sound.cpp` : non utilisé.
  - `_AFXDLL` non défini automatiquement par `CMAKE_MFC_FLAG` sous le générateur
    Ninja ; ajouté explicitement dans `VedaGUIWindowsMFC/CMakeLists.txt`.
  - `WINVER`/`_WIN32_WINNT` jamais définis (ajoutés à `0x0A00`, ciblant
    Windows 10/11 conformément aux conventions du projet).
  - `cmake/GenerateVersion.cmake` : `CMAKE_CURRENT_LIST_DIR`, évalué dans le corps
    d'une fonction, pointait vers le répertoire de l'appelant plutôt que `cmake/`,
    faisant échouer la génération de `Version.h`.
- Troncatures pointeur → entier 32 bits (bugs 64-bit réels, pas de simples
  avertissements) corrigées avec `intptr_t`/`uintptr_t`/`ptrdiff_t` :
  `Veda/BaseType.cpp` et `VedaLibSoundMP3/MP3SoundObject.cpp` (soustractions de
  pointeurs castées en `int`/`unsigned int`), `VedaLibDemo/MarchinCubeSpace.cpp`
  (arrondi d'alignement mémoire sur 32 octets via un masque `int` — corrompait
  entièrement le pointeur en 64-bit), `VedaLibDemo/Object3DMarchCube.cpp` (pointeur
  stocké dans un `int` puis ré-additionné et recasté), `VedaGUIWindowsMFC/LeftView.cpp`
  (`NMHDR::idFrom` est `UINT_PTR` dans le SDK Win32, pas `UINT` — la variable locale
  tronquait le handle de fenêtre avant même le cast `(HWND)`).

### Removed

- Copie vendue d'IJG libjpeg 6b dans `VedaLibImageJPEG` (~24 fichiers `.c`/`.h`),
  remplacée par `libjpeg-turbo` via vcpkg.
- `VedaGUIWindowsMFC/MainFrm_old.cpp`/`.h` (fichiers obsolètes, non référencés par
  aucun `.vcproj`/`CMakeLists.txt`).
- 134 occurrences du mot-clé `register` (non standard depuis C++17) dans le code
  AzurVeda (`Veda`, `VedaLibDemo`, `VedaLibImage`, `VedaMachineOGL`,
  `VedaLibSoundMP3/MP3SoundObject.cpp`). Le code tiers vendu (fork de libmad,
  `uniminixm`) n'est pas modifié.
- `dsound.lib`/`dxguid.lib` retirés de `VedaMachineOGLWinDxSound/CMakeLists.txt`
  (remplacement DirectSound → XAudio2), remplacés par `ole32`.

### Known issues

- Avec `/W4`, il reste 425 avertissements dans le code AzurVeda (principalement
  `C4244` conversions avec perte potentielle, `C4100` paramètres non utilisés,
  `C4189` variables locales non utilisées, `C4996` fonctions CRT dépréciées,
  `C4018`/`C4389`/`C4245` signé/non signé) et 72 dans le code tiers vendu (non
  traité, hors périmètre). Aucun n'est une troncature pointeur→entier 32 bits — ces
  dernières ont toutes été corrigées (voir *Fixed*). `/WX` n'est donc pas encore
  activé ; voir `roadmap.md` Phase 3.
- Le portage DirectSound → XAudio2 (`VedaMachineOGLWinDxSound`) compile et
  s'exécute sans crash (`Example02`/`Example03` testés 8s sans erreur), mais
  **n'a pas été validé à l'oreille** (pas de test audio audible possible dans cet
  environnement automatisé) ni testé sur Windows 11 (développé/testé uniquement
  sur Windows 10 Entreprise LTSC 10.0.17763). Voir `roadmap.md` Phase 4.

[Unreleased]: https://github.com/OWNER/AzurVeda/compare/HEAD
