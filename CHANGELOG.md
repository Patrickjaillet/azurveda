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
- `vcpkg.json` (manifest mode) : dépendances `libjpeg-turbo`, `glfw3`.
- Options CMake `AZURVEDA_BUILD_GPL_MODULES`, `AZURVEDA_BUILD_MFC_EDITOR`,
  `AZURVEDA_BUILD_EXAMPLES`.
- CI GitHub Actions (`.github/workflows/ci.yml`) : build MSVC x64 + vcpkg, exécution de
  `Example01_SimpleSerialization`, vérification de CHANGELOG.md sur chaque PR,
  incrément automatique du `PATCH` de `VERSION` sur chaque build réussi de `main`.
- `/W4` activé projet entier (`CMakeLists.txt` racine).
- `/WX` (avertissements traités comme des erreurs) activé pour toutes les cibles
  AzurVeda, une fois le nettoyage `/W4` terminé (voir *Fixed*). Un garde-fou
  `/W1 /WX-` est appliqué spécifiquement aux fichiers `.c` du code tiers vendu
  (`VedaLibSoundMP3/CMakeLists.txt`, `VedaLibSoundXM/CMakeLists.txt`, via
  `set_source_files_properties`), qui n'est pas retenu par le même standard de
  qualité.
- `docs/screenshot.png` : capture d'écran de `VedaGUIWindowsMFC` référencée dans
  `README.md`, prise sur la machine de développement réelle.
- Numéro de version affiché dans `VedaGUIWindowsMFC` : barre de titre
  (`"UVeda vX.Y.Z - [DocTitre]"`, via `CMainFrame::OnCreate`) et boîte "À propos"
  (`"UVeda MFC GUI vX.Y.Z (hash-git)"`, remplace un texte statique codé en dur
  `"UVeda MFC GUI Version 0.8   02/2007"` datant de 2007).
- Étape CI (`.github/workflows/ci.yml`, job `changelog-check`) : si une PR modifie
  des ressources `VedaGUIWindowsMFC/*.rc`/`*.rc2`, `docs/screenshot.png` doit être
  mis à jour dans la même PR, sinon le build échoue.
- CI : build matrice `Debug`/`Release` (job `build`, `strategy.matrix.configuration`),
  nouveau preset CMake `ci-windows-x64-debug` dans `CMakePresets.json`.
- CI : job `release`, déclenché sur push d'un tag `v[0-9]+.[0-9]+.[0-9]+`, qui
  empaquette les exécutables `Example01/02/03` (+ `README.md`/`CHANGELOG.md`/
  `COPYING`/`docs/screenshot.png`) dans une archive zip et publie une release
  GitHub (`gh release create`) dont les notes commencent par le screenshot en
  aperçu suivi de la section `CHANGELOG.md` correspondant au tag.
- `git remote origin` pointé vers le dépôt réel
  (`https://github.com/Patrickjaillet/azurveda`).

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
- `VedaMachineOGLWinDxSound` : remplacement de la création de fenêtre/contexte OpenGL
  Win32 brute (`WNDCLASS`/`CreateWindowEx`/`ChoosePixelFormat`/`wglCreateContext`/
  boucle `PeekMessage`) par **GLFW** dans `OGLMachineWinDxSound.cpp` (utilisé par
  `DefaultMachine`, donc par `Example01/02/03`). La bascule plein écran ↔ fenêtré
  passe désormais par `glfwSetWindowMonitor()` plutôt que par
  `ChangeDisplaySettings()` + recréation manuelle du style de fenêtre. Le pipeline de
  rendu OpenGL fixe lui-même n'est pas modifié (toujours `GL/gl.h`, pas de contexte
  core profile). Le second chemin de création de contexte du projet,
  `VedaGUIWindowsMFC/OpenGLWnd.cpp` (contexte WGL sur une fenêtre enfant embarquée
  dans la vue scindée de l'éditeur MFC), reste inchangé — GLFW ne prend pas en
  charge nativement le rendu sur une HWND déjà embarquée dans une hiérarchie MFC ;
  voir `roadmap.md` Phase 5 pour la justification détaillée de cette limite de
  périmètre.
- `README.md` traduit intégralement en anglais (décision utilisateur, 2026-07-15) :
  c'était un choix éditorial distinct de la traduction Phase 2 (qui ne ciblait que
  les ressources `.rc` et le code source), pas un oubli.

### Fixed

- `cmake/GenerateVersion.cmake` : `azurveda_add_version_header_target()` ne peut
  être appelé qu'une fois par binaire de sortie — l'appeler à la fois pour `Veda`
  et pour `VedaGUIWindowsMFC` avec le même fichier `generated/Version.h` fait
  échouer Ninja (`multiple rules generate generated/Version.h`, deux cibles
  personnalisées ne peuvent pas déclarer le même `BYPRODUCTS`). `VedaGUIWindowsMFC`
  dépend maintenant directement de la cible `Veda_UpdateVersionHeader` existante
  via `add_dependencies()` plutôt que d'en créer une seconde.
- Placeholders d'URL de dépôt restants (`https://github.com/OWNER/AzurVeda` dans
  `CHANGELOG.md`, `<repo-url>` dans `README.md`) remplacés par l'URL réelle du
  dépôt une fois celle-ci connue.
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
- Les 425 avertissements `/W4` restants dans le code AzurVeda (voir *Known issues*
  précédent) ont été traités intégralement, catégorie par catégorie. Le code
  AzurVeda compile désormais sans aucun avertissement `/W4`. Plusieurs vrais bugs
  ont été trouvés et corrigés au passage, pas seulement des avertissements
  silencieux :
  - `VedaLib3DEngine/lwo2reader.h` : `struct LwoPolyToVertex` référençait un tag de
    structure fantôme au lieu de `struct _LwoPolyToVertex`.
  - `VedaLib3DEngine/lwo2reader.c` : compteurs `unsigned int` utilisés avec des
    sentinelles `-1` (fonctionnait par un rebouclage non garanti) ; retypés en `int`.
    Pointeur `plwclip` non initialisé quand `nbclip == 0`, exploitable via un
    fichier LWO incohérent contenant un chunk `CLIP` malgré tout.
  - `VedaLibDemo/Object3DFontArial.h`/`.cpp` : `m_fontc` déclaré `signed char[]`
    corrompait les valeurs d'octet supérieures à 127, utilisées comme indices de
    triangle et coordonnées de texture normalisées sur 0-255.
  - `Veda/VirtualMachine.h` : `InternalViewPort` (classe abstraite) n'avait pas de
    destructeur virtuel malgré un `delete` via pointeur de base dans
    `OGLMachine.cpp` — comportement indéfini réel, pas juste un avertissement.
  - `VedaMachineOGLWinDxSound/OGLMachineWinDxSound.cpp` : condition toujours fausse
    `if(vmr_OK != vmr_OK) return res;` qui avalait silencieusement un échec
    d'initialisation de la classe de base.
  - `VedaLibMath/EquationSinus3D.cpp` : les champs `mSer_IndexMultiplier_Frequence`
    et `mSer_IndexMultiplier_Amplitude`, de type `PackDynamicFloat`, étaient
    enregistrés via la macro `REGISTER_MEMBER_PACKFLOAT` au lieu de
    `REGISTER_MEMBER_PACKDYNAMICFLOAT`. Cette macro appelle `Set(_DefaultVal)`, qui
    pour un `PackDynamicFloat` résout vers `PackDynamicType::Set(unsigned int)` — une
    méthode sans rapport — corrompant silencieusement l'état interne ("shape") au
    lieu de fixer une valeur flottante par défaut. Recherche exhaustive confirmant
    qu'il s'agit du seul cas de ce pattern dans la base de code.
  - `Veda/BaseType.cpp::WriteFile` : `pserializedFormAfter` n'était jamais affecté
    si `fopen()` échouait, et sa valeur non initialisée était lue juste après.
- `VedaGUIWindowsMFC/res/VedaDemoOGLMfcGui.manifest` était **orphelin** : jamais
  référencé depuis `VedaDemoOGLMfcGui.rc` ni depuis l'ancien `.vcproj`/le nouveau
  `CMakeLists.txt`, donc jamais embarqué dans l'exécutable (seul le manifeste
  minimal auto-généré par le linker MSVC l'était). Corrigé : `processorArchitecture`
  `X86` → `amd64` (projet x64 exclusif), ajout d'un bloc `<compatibility>` ne
  déclarant que le GUID `supportedOS` Windows 10/11, ajout de
  `<dpiAwareness>PerMonitorV2</dpiAwareness>`, et câblage réel via
  `target_link_options(... "/MANIFEST:EMBED" "/MANIFESTINPUT:...")` dans
  `VedaGUIWindowsMFC/CMakeLists.txt`. Vérifié avec `mt.exe -inputresource` sur le
  binaire compilé.
- Texte français oublié par la Phase 2, qui ne ciblait que les ressources `.rc` et
  les commentaires du code source : `VedaGUIWindowsMFC/VedaDemoOGLMfcGui.h`
  (commentaires + message `#error` en français), `VedaGUIWindowsMFC/VedaDemoOGLMfcGui.reg`
  (commentaires).

### Removed

- Copie vendue d'IJG libjpeg 6b dans `VedaLibImageJPEG` (~24 fichiers `.c`/`.h`),
  remplacée par `libjpeg-turbo` via vcpkg.
- `VedaGUIWindowsMFC/MainFrm_old.cpp`/`.h` (fichiers obsolètes, non référencés par
  aucun `.vcproj`/`CMakeLists.txt`).
- `VedaGUIWindowsMFC/ReadMe.txt` : texte AppWizard générique en français, obsolète
  (référençait des fichiers qui n'existent plus dans le projet, ex.
  `VedaDemoOGLMfcGuiView.h/.cpp`, `.vcproj`), non référencé ailleurs dans le dépôt.
- 134 occurrences du mot-clé `register` (non standard depuis C++17) dans le code
  AzurVeda (`Veda`, `VedaLibDemo`, `VedaLibImage`, `VedaMachineOGL`,
  `VedaLibSoundMP3/MP3SoundObject.cpp`). Le code tiers vendu (fork de libmad,
  `uniminixm`) n'est pas modifié.
- `dsound.lib`/`dxguid.lib` retirés de `VedaMachineOGLWinDxSound/CMakeLists.txt`
  (remplacement DirectSound → XAudio2), remplacés par `ole32`.
- `roadmap.md` (demande utilisateur, 2026-07-15) : les 8 phases qu'il décrivait
  sont désormais toutes terminées. Les liens `[roadmap.md](roadmap.md)` et
  mentions de phase associés ont été retirés de `README.md` et
  `.github/workflows/ci.yml` ; les mentions déjà présentes dans les entrées
  historiques de ce fichier sont conservées telles quelles (`CHANGELOG.md`
  documente l'état du projet au moment de chaque changement, pas l'état actuel).

### Known issues

- Le code tiers vendu (`VedaLibSoundMP3` fork de libmad, `VedaLibSoundXM`
  `uniminixm`) conserve 6 avertissements sous `/W1` (`C4311`/`C4312`, troncatures
  pointeur dans `uniminixm_Init.c`/`uniminixm_StereoFloatMixer.c`), non traités car
  hors périmètre AzurVeda ; `/WX-` les laisse non bloquants pour le build. Voir
  `roadmap.md` Phase 3.
- Le portage DirectSound → XAudio2 (`VedaMachineOGLWinDxSound`) compile et
  s'exécute sans crash (`Example02`/`Example03` testés 8s sans erreur), mais
  **n'a pas été validé à l'oreille** (pas de test audio audible possible dans cet
  environnement automatisé) ni testé sur Windows 11 (développé/testé uniquement
  sur Windows 10 Entreprise LTSC 10.0.17763). Voir `roadmap.md` Phase 4.
- Le portage Win32 brut → GLFW (`VedaMachineOGLWinDxSound`) compile et s'exécute
  sans crash (`Example02`/`Example03`/`VedaGUIWindowsMFC.exe` testés plusieurs
  secondes sans erreur). Une capture d'écran de `VedaGUIWindowsMFC` a depuis été
  prise avec succès (`docs/screenshot.png`, Phase 6), confirmant qu'un contexte
  OpenGL réel se crée et s'affiche sans erreur visible, mais **le rendu 3D
  effectif (mesh, texture) n'a pas été vérifié visuellement** faute de scène de
  démonstration chargée lors de la capture. Voir `roadmap.md` Phase 5.
- `docs/screenshot.png` a été pris sur une machine Windows en locale française :
  le titre de fenêtre affiche "Sans titre" au lieu de "Untitled". Ce texte provient
  du texte par défaut intégré à la DLL MFC partagée (`_AFXDLL`) dont Windows charge
  la variante satellite localisée selon la locale système, et non d'une ressource
  `.rc` d'AzurVeda (déjà entièrement traduite, voir Phase 2) ; il apparaîtrait
  "Untitled" sur une machine en locale anglaise, sans changement de code
  nécessaire.

[Unreleased]: https://github.com/Patrickjaillet/azurveda/compare/HEAD
