# Roadmap de conversion — AzurVeda (édition Windows-only, cible outils 2026)

> Périmètre : `Veda`, `VedaGUIWindowsMFC`, `VedaLib3DEngine`, `VedaLibDemo`, `VedaLibImage`,
> `VedaLibImageJPEG`, `VedaLibMath`, `VedaLibSoundMP3`, `VedaLibSoundXM`, `VedaMachineOGL`,
> `VedaMachineOGLWinDxSound`. Les portages Amiga, Dreamcast et Linux sont hors périmètre
> (retirés de ce workspace).
>
> Ce document est **vivant** : toute fonctionnalité ajoutée au projet doit être ajoutée ici
> (nouvelle case à cocher, dans la phase concernée) avant ou pendant son développement.

## Conventions de développement

- [ ] Langage source entièrement en **Anglais** (noms de variables, fonctions, classes)
- [ ] **Aucun commentaire** dans le code source
- [ ] Compatibilité stricte **Windows 10/11** uniquement
- [ ] Chaque fonctionnalité ajoutée doit être répercutée dans ce ROADMAP.md
- [ ] Sérialisation automatique des versions du logiciel pour chaque étape
- [ ] Chaque modification doit être répercutée pour l'utilisateur final dans le CHANGELOG.md
- [ ] Le README.md doit être créé et mis à jour pour l'utilisateur final à chaque modification et avec un screenshot du logiciel

---

## Phase 0 — Fondations & gouvernance du projet (~1-2 jours)

- [x] Initialiser un dépôt Git (l'archive source actuelle n'a pas d'historique)
- [x] Créer `CHANGELOG.md` au format [Keep a Changelog](https://keepachangelog.com/fr/1.1.0/) (`Unreleased`, `Added`, `Changed`, `Fixed`, `Removed`)
- [x] Créer `README.md` avec : description du projet, prérequis (Windows 10/11, Visual Studio 2026, vcpkg), instructions de build (capture d'écran du logiciel en attente, cf. Phase 6 — le logiciel n'a pas encore été compilé/exécuté dans cette conversion)
- [x] Définir un schéma de versionnage sémantique (`MAJOR.MINOR.PATCH`) et le brancher sur Git (tag → génération automatique d'un header `Version.h` via un script CMake, exécuté à chaque build)
- [x] Ajouter `.gitignore` (dossiers `Debug/`, `Release/`, `x64/`, `.vs/`, `out/`)
- [x] Documenter dans le `README.md` la procédure de mise à jour du `CHANGELOG.md` (obligatoire à chaque PR/commit fonctionnel)

## Phase 1 — Outillage moderne (~2-3 jours)

- [x] Remplacer les fichiers `.vcproj` (Visual Studio .NET 2003, Format Version 8.00) par des fichiers projet **Visual Studio 2026** (`.vcxproj`) générés via **CMake** (`CMakeLists.txt` par module + racine) — les anciens `.vcproj`/`.sln` sont conservés à titre de référence historique le temps de valider le build CMake sur une vraie machine Windows/VS2026, puis seront supprimés
- [x] Cibler exclusivement la plateforme **x64**, toolset MSVC le plus récent (équivalent 2026), standard **C++23** (`CMakeLists.txt` échoue si `CMAKE_SIZEOF_VOID_P != 8` ou si le compilateur n'est pas MSVC)
- [x] Introduire **vcpkg** (manifest mode, `vcpkg.json`) pour les dépendances externes : `libjpeg-turbo` remplace la copie vendue d'IJG libjpeg 6b dans `VedaLibImageJPEG` (le module ne conserve que `ImageJPEG.cpp/.h` et `RegisterLibImageJPEG.cpp/.h`). Le décodeur MP3 (GPL2, fork de libmad) et le lecteur XM restent des sources internes tel quel, un remplacement par un paquet vcpkg étant une décision de licence (Phase à part) plutôt qu'un simple choix d'outillage
- [x] Mettre en place une CI **GitHub Actions** (`windows-latest`, build MSVC + exécution des exemples `Veda/CodeExamples`) — `Example01_SimpleSerialization` (console, sans GPU) est exécuté ; `Example02`/`Example03` (OpenGL + DirectSound) sont seulement compilés, faute de GPU/périphérique audio sur les runners hébergés
- [x] Faire en sorte que chaque build réussi sur la branche principale mette à jour automatiquement le numéro de version (Phase 0) et échoue si `CHANGELOG.md` n'a pas été modifié dans la même PR
- [x] **Build validé réellement** (pas seulement écrit) : toolchain installée en local (CMake, Ninja, vcpkg, Visual Studio Build Tools 2022 + MFC/ATL — VS2026 n'existe pas encore publiquement), `cmake --preset ci-windows-x64 -DAZURVEDA_BUILD_MFC_EDITOR=ON && cmake --build --preset ci-windows-x64-release` compile avec succès les 12 modules, `VedaGUIWindowsMFC.exe` et les 3 `CodeExamples`. Plusieurs bugs de compilation réels (pas seulement des warnings) ont été trouvés et corrigés à cette occasion : littéraux chaîne assignés à `char*`/`LPSTR` non-const (`Veda/PackStruct.cpp`, `PackULong_Enums.cpp`, `PackULong_Flags.cpp`, `VedaGUIWindowsMFC/Dialog_PackListDynamic.cpp`, `LeftView.cpp`), signature `OnTimer(UINT)` obsolète en 64-bit (doit être `UINT_PTR`, `OpenGLWnd.cpp/.h`, `PreviewManagerView.cpp/.h`), `goto` traversant une initialisation dans le chargeur LWO (`VedaLib3DEngine/Object3DMesh3D.cpp`, cf. Phase 3), assembleur inline x86 `FPM_INTEL` incompatible avec MSVC x64 dans le décodeur MP3 (remplacé par le chemin portable `FPM_64BIT` de libmad), en-tête DirectX legacy `dxerr8.h` absent du SDK moderne (retiré, non utilisé), `_AFXDLL` non défini automatiquement par `CMAKE_MFC_FLAG` sous le générateur Ninja, `WINVER`/`_WIN32_WINNT` jamais définis (ajoutés en visant Windows 10/11)

## Phase 2 — Traduction en anglais & nettoyage éditorial (~3-5 jours)

- [ ] Renommer toutes les variables, fonctions, classes, fichiers en anglais (le code contient actuellement des identifiants et messages en français, notamment dans `VedaGUIWindowsMFC`)
- [ ] Traduire les chaînes de caractères visibles utilisateur (menus, dialogues `.rc`, messages d'erreur) en anglais
- [ ] Supprimer **l'intégralité** des commentaires du code source (tous fichiers `.cpp`/`.h`)
- [ ] Supprimer les fichiers `*_old.cpp` / `*_old.h` obsolètes (ex. `MainFrm_old.cpp`) une fois vérifié qu'ils ne sont plus référencés
- [ ] Vérifier qu'aucune régression de compilation n'a été introduite par le renommage (build CI vert)

## Phase 3 — Portage 64-bit & C++ moderne (~3-5 jours)

- [ ] Corriger les troncatures pointeur → entier 32 bits, héritées d'une époque où `sizeof(void*) == sizeof(int)` (ex. `Veda/BaseType.cpp`, `VedaLibSoundMP3/MP3SoundObject.cpp`, `VedaLibDemo/MarchinCubeSpace.cpp`) en utilisant `intptr_t`/`uintptr_t`/`ptrdiff_t`
- [ ] Supprimer les ~227 occurrences du mot-clé `register` (non standard depuis C++17)
- [x] Corriger les `goto` traversant une initialisation de variable (ex. chargeur LWO dans `VedaLib3DEngine/Object3DMesh3D.cpp`) — corrigé en Phase 1 car il s'agissait d'une erreur de compilation bloquante (pas juste un warning), la portée complète du portage 64-bit/C++ moderne (troncatures pointeur, `register`, `/W4 /WX`) reste à faire
- [ ] Traiter les avertissements `-Wall -Wextra` / `/W4` restants (retours `fread`/`write` ignorés, etc.)
- [ ] Activer `/W4 /WX` (warnings comme erreurs) une fois le nettoyage terminé, pour éviter toute régression future

## Phase 4 — Audio moderne (~2-3 jours)

- [ ] Remplacer **DirectSound** (`VedaMachineOGLWinDxSound`), API dépréciée par Microsoft, par **WASAPI** ou **XAudio2**
- [ ] Conserver inchangés les décodeurs internes (MP3 maison, lecteur XM maison) : seule la couche de sortie audio change
- [ ] Valider la latence et la stabilité sur Windows 10 et Windows 11

## Phase 5 — Rendu graphique (~variable selon ambition)

- [ ] **Option minimale (recommandée)** : conserver le pipeline OpenGL fixe existant, mais moderniser la création de fenêtre/contexte (remplacer le code Win32 brut par une couche plus robuste, ex. GLFW), toujours compatible Windows 10/11
- [ ] **Option ambitieuse (hors périmètre "conversion", plutôt refonte)** : migrer vers un pipeline programmable (OpenGL 4.x core ou Direct3D 11/12), à envisager seulement si un objectif de rendu moderne est explicitement demandé

## Phase 6 — Éditeur MFC (`VedaGUIWindowsMFC`) (~2-4 jours)

- [ ] Installer le composant optionnel "MFC et ATL" pour Visual Studio 2026 et valider la compilation du projet existant
- [ ] Nettoyer/traduire les ressources `.rc` (menus, dialogues, chaînes) en anglais
- [ ] Mettre à jour le manifeste applicatif (`VedaDemoOGLMfcGui.manifest`) pour cibler explicitement Windows 10/11 (aucune compatibilité legacy)
- [ ] Prendre une capture d'écran à jour du logiciel pour le `README.md` à chaque changement visuel notable

## Phase 7 — Automatisation versioning / CHANGELOG / README (~1-2 jours)

- [ ] Script de build (CMake ou PowerShell) qui : incrémente automatiquement le `PATCH` à chaque build CI réussi sur `main`, et le `MINOR`/`MAJOR` sur tag manuel
- [ ] Générer le numéro de version dans le binaire (à afficher dans la barre de titre / boîte "À propos" de `VedaGUIWindowsMFC`)
- [ ] Vérification CI bloquante : toute PR modifiant le comportement observable doit inclure une entrée `CHANGELOG.md` et, si l'UI change, une capture d'écran mise à jour dans `README.md`

## Phase 8 — CI/CD complet (~1-2 jours)

- [ ] Étendre la CI GitHub Actions : build matrice (Debug/Release, x64) sur `windows-latest`
- [ ] Publier automatiquement les artefacts de build (exécutables + exemples) et le `CHANGELOG.md` extrait à chaque tag de version
- [ ] Publier une release GitHub avec le screenshot du `README.md` en aperçu

---

## Estimation globale

| Phase | Contenu | Effort estimé |
|---|---|---|
| 0 | Fondations (Git, versioning, README, CHANGELOG) | 1-2 j |
| 1 | Outillage (CMake, vcpkg, VS2026, CI) | 2-3 j |
| 2 | Traduction anglais + suppression commentaires | 3-5 j |
| 3 | Portage 64-bit / C++ moderne | 3-5 j |
| 4 | Audio moderne (WASAPI/XAudio2) | 2-3 j |
| 5 | Rendu graphique (option minimale) | 2 j |
| 6 | Éditeur MFC | 2-4 j |
| 7 | Automatisation versioning/changelog/screenshot | 1-2 j |
| 8 | CI/CD complet | 1-2 j |

**Total réaliste : ~3-4 semaines** pour un développeur C++ Windows à l'aise avec MFC et les anciens codebases.

## Points de vigilance légaux

- `VedaLibSoundMP3` (décodeur MP3 maison) est en licence **GPL 2**
- `VedaGUIWindowsMFC` est également en **GPL 2**
- Le reste du code (`Veda`, `VedaLib*`, `VedaMachineOGL*`) est en **LGPL 2.1**
- À respecter si le projet converti est redistribué, même en interne
