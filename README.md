# numerical-methods

Numerical Methods coursework project with:
- `nm-lib/`: a C++17 library + small console programs (tests/drivers) implementing core numerical methods.
- `webapp/`: a Node/Express API + React/Vite UI that can render the course chapters as web pages.


## What's implemented

**Referat 1 - Approximation & Linear Systems**
- Root finding: bisection, Newton (tangent), regula falsi, secant
- Linear systems: Gaussian elimination + worked example

**Referat 2 - Iterative Methods & Newton for Systems**
- Iterative solvers for linear systems: Jacobi, Gauss–Seidel
- Newton method for nonlinear systems

## Project structure

- `nm-lib/include/`
  - `core/`: `Matrix`, `Vector`
  - `linear/`: `GaussianElimination`, `Jacobi`, `GaussSeidel`, `LinearSystem`
  - `nonlinear/`: `RootFinding`, `Newton`, `ScalarEquation`, `NonlinearSystem`
  - `utils/`: exceptions + rounding helpers
- `nm-lib/src/`: implementations mirroring `include/`
- `nm-lib/tests/`: small console drivers (`tema1_rootfinding.cpp`, `tema2_gauss.cpp`, `tema3_iterative.cpp`, `tema4_newton_systems.cpp`)
- `webapp/server/`: Express API
- `webapp/client/`: React UI

## Build & run

CLI equivalent for the library app (from `nm-lib/`):

`g++ -std=c++17 -g -O0 -Iinclude -Isrc -o app.exe main.cpp src/core/*.cpp src/linear/*.cpp src/nonlinear/*.cpp src/utils/*.cpp`

## Compile & run tests (C++)

The test files are small console programs under `nm-lib/tests/`:
- `tema1_rootfinding.cpp`
- `tema2_gauss.cpp`
- `tema3_iterative.cpp`
- `tema4_newton_systems.cpp`

### Option A: use the repo build script (Windows)
From repo root:
- Build everything (C++ + webapp): `./build.ps1`
- Build only C++ (app + tests): `./build.ps1 -Target cpp`

Outputs:
- App: `nm-lib/app.exe`
- Tests: `nm-lib/bin/tests/tema*_*.exe`

Run an individual test (from repo root):
- `./nm-lib/bin/tests/tema3_iterative.exe`

### Option B: compile directly with g++
From `nm-lib/` (PowerShell):

`New-Item -ItemType Directory -Force -Path .\bin\tests | Out-Null`

`g++ -std=c++17 -g -O0 -Iinclude -Isrc -o .\bin\tests\tema1_rootfinding.exe .\tests\tema1_rootfinding.cpp src/core/*.cpp src/linear/*.cpp src/nonlinear/*.cpp src/utils/*.cpp`
`g++ -std=c++17 -g -O0 -Iinclude -Isrc -o .\bin\tests\tema2_gauss.exe .\tests\tema2_gauss.cpp src/core/*.cpp src/linear/*.cpp src/nonlinear/*.cpp src/utils/*.cpp`
`g++ -std=c++17 -g -O0 -Iinclude -Isrc -o .\bin\tests\tema3_iterative.exe .\tests\tema3_iterative.cpp src/core/*.cpp src/linear/*.cpp src/nonlinear/*.cpp src/utils/*.cpp`
`g++ -std=c++17 -g -O0 -Iinclude -Isrc -o .\bin\tests\tema4_newton_systems.exe .\tests\tema4_newton_systems.cpp src/core/*.cpp src/linear/*.cpp src/nonlinear/*.cpp src/utils/*.cpp`

## Webapp (dev)

From repo root:
- `cd webapp`
- `npm install`
- `npm run dev`
