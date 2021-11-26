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

Tests can be run as follows:

```
cd <root>/build/bin
./test_viewer
```

## Current Application

The application can be started with

```
cd build/bin
./viewer
```

Command-line options are available for selecting the files and setting various preferences.

The window is composed of:

1. A chart with the data
2. A **start** button to start and restart streaming the data.
3. A **stop** button to stop streaming the data.
4. A **Slinding window**  text input which selects the window used for
   computing the baseline: a value above zero will subtract a baseline from the data. The baseline is a sliding mean.
5. A **quit** button for quitting the application
6. A **Automated zoom** button for applying an automated range to the y axis. The latter is based on the data currently displayed.
7. Two **y-axis min/max** inputs for selecting the y-axis range. Changing these manually stops the automation.

## Things to improve

1. Currently, zipped data cannot be loaded. The data structure is compatible with
   non-periodic time samples, but we are missing the code for unzipping files.
2. QML is not used: this is my first time using Qt. I did not explore this functionnality.
3. The MVC structure is not quite there: I believe Qt has the necessary framework. I did not explore this.
4. No tests on the GUI itself: I'm sure Qt has means to do so. I did not explore this.
5. Code documentation: missing for lack of time.
6. The current chart does not display the fact that data points may be missing.
   It could be done by displaying broken lines.
