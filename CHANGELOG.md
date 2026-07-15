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

### Changed

- N/A

### Fixed

- N/A

### Removed

- N/A

[Unreleased]: https://github.com/OWNER/AzurVeda/compare/HEAD
