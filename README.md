# AzurVeda

AzurVeda est un framework C++ expérimental pour le développement de jeux et de démos
temps réel (moteur 3D OpenGL, image, son MP3/XM, éditeur MFC), originellement écrit par
Victorien Ferry ([m4nkind.com](https://www.m4nkind.com)).

Cette édition du workspace est **exclusivement Windows 10/11**, cible les outils 2026
(Visual Studio 2026, C++23), et convertit progressivement le code historique (2003-2007)
vers une chaîne de build moderne. Voir [roadmap.md](roadmap.md) pour le plan de conversion
complet et son avancement.

## Modules

| Module | Rôle | Licence |
|---|---|---|
| `Veda` | Bibliothèque de base (types, conteneurs, utilitaires) | LGPL 2.1 |
| `VedaLib3DEngine` | Moteur de rendu 3D | LGPL 2.1 |
| `VedaLibImage` / `VedaLibImageJPEG` | Chargement/traitement d'images | LGPL 2.1 |
| `VedaLibMath` | Bibliothèque mathématique | LGPL 2.1 |
| `VedaLibSoundMP3` | Décodeur MP3 maison | **GPL 2** |
| `VedaLibSoundXM` | Lecteur de modules XM | LGPL 2.1 |
| `VedaLibDemo` | Utilitaires pour démos temps réel | LGPL 2.1 |
| `VedaMachineOGL` | Backend de rendu OpenGL | LGPL 2.1 |
| `VedaMachineOGLWinDxSound` | Sortie audio DirectSound (Windows) | LGPL 2.1 |
| `VedaGUIWindowsMFC` | Éditeur graphique (MFC) | **GPL 2** |

> ⚠️ Le code source respecte de facto la licence la plus restrictive de ses dépendances
> (GPL 2) dès lors qu'il lie `VedaLibSoundMP3` et/ou `VedaGUIWindowsMFC`. Voir `COPYING`.

## Prérequis

- Windows 10 ou Windows 11
- Visual Studio 2026 (workload "Développement Desktop en C++", composant optionnel
  "MFC et ATL" pour `VedaGUIWindowsMFC`)
- [vcpkg](https://github.com/microsoft/vcpkg) en mode manifeste pour les dépendances
  externes (libjpeg, etc.)
- CMake 3.28+ et Git

## Build

> État actuel : le workspace est en cours de migration (voir [roadmap.md](roadmap.md),
> Phase 1). Tant que la migration CMake/vcpkg n'est pas terminée, le build se fait via
> les solutions Visual Studio historiques ci-dessous.

1. Cloner le dépôt : `git clone <url-du-depot>`
2. Ouvrir `VedaWindowsLGPL.sln` (modules LGPL uniquement) ou `VedaWindowsGPL.sln`
   (inclut également `VedaLibSoundMP3` et `VedaGUIWindowsMFC`, sous GPL 2) dans
   Visual Studio 2026
3. Compiler la configuration `Release`/`x64`

Une fois la Phase 1 de la roadmap terminée, le build se fera via CMake :

```
cmake --preset windows-x64
cmake --build --preset windows-x64-release
```

## Versionnage

Le projet suit le [Semantic Versioning](https://semver.org/lang/fr/) (`MAJOR.MINOR.PATCH`) :

- Le numéro de version courant est stocké dans le fichier [`VERSION`](VERSION) à la
  racine du dépôt.
- À chaque tag Git de la forme `vMAJOR.MINOR.PATCH`, ce tag devient la source de vérité
  et prévaut sur le fichier `VERSION`.
- Le script `cmake/GenerateVersion.cmake` génère automatiquement un header `Version.h`
  (à partir de `cmake/Version.h.in`) à chaque build, exposant `AZURVEDA_VERSION_STRING`
  et `AZURVEDA_VERSION_GIT_HASH` au code source.

## Mise à jour du CHANGELOG

[`CHANGELOG.md`](CHANGELOG.md) suit le format [Keep a Changelog](https://keepachangelog.com/fr/1.1.0/).
**Toute PR ou tout commit qui modifie le comportement observable du logiciel doit
ajouter une entrée dans la section `[Unreleased]`** (catégorie `Added`, `Changed`,
`Fixed` ou `Removed`), avant d'être mergé. La CI (Phase 1/7 de la roadmap) fera
échouer les builds sur `main` si `CHANGELOG.md` n'a pas été modifié dans une PR
comportant des changements fonctionnels.

Lors d'une release, la section `[Unreleased]` est renommée avec le numéro de version
et la date (`## [MAJOR.MINOR.PATCH] - AAAA-MM-JJ`), et une nouvelle section
`[Unreleased]` vide est créée au-dessus.

## Capture d'écran

_À ajouter : capture d'écran du logiciel (`VedaGUIWindowsMFC`), à mettre à jour à
chaque changement visuel notable (voir roadmap, Phase 6)._

## Licence

Voir [`COPYING`](COPYING) et le fichier `COPYING` de chaque module. Le workspace
combine des composants LGPL 2.1 et GPL 2 (voir tableau ci-dessus et section
"Points de vigilance légaux" de [roadmap.md](roadmap.md)).
