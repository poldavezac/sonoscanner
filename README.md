# SonoScanner Exercise

## Input

Sonoscanner suggests the following

```
Application de revue d’un signal ECG

 

 Description de l’application :

- Lecture des données et reconstruction de la séquence complète de l’ECG pour la durée totale de celle-ci.

- Une fenêtre de largeur 6 s (avec un bouton play/stop) permet de faire défiler le résultat à la bonne cadence.

- Fonction de correction de la dérive du signal en Y en faisant une moyenne sur fenêtre glissante (par exemple). Cette dernière est réglable par l’utilisateur.

- Fonction de gain/zoom vertical des données au choix :

-          Zoom fixe

-          Auto zoom

-          Réglage par un contrôle UI

Objectifs et moyens mis en œuvre :

-          Développement d’une application graphique et de ses tests unitaires.

-          En C++ et par exemple Qt/QML

-          Tests unitaires avec Google Test.

-          Partager le code sur GitHub.

-          Documenter le readme avec les éléments nécessaires pour compiler et tester le code et une description de l’application.

Données d’entrée :

-          Fichier data.7z

-          Données ECG sur 16 bits signés à 500 Hz par blocs de tailles variables

-          Séquence ECG d’environ 24s.
```

## Installation

The installation relies on:

* [conan](https://conan.io/) for managing external libraries
* `cmake` for compilation

It has been tested using:
* `ubuntu 21.10`
* `gcc 11.2.0`

Installation is done using:

```
git clone ...
cd sonoscanner
mkdir build
conan install ... --build=missing
cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build .
```
