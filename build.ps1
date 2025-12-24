param(
  [ValidateSet('all','cpp','webapp')]
  [string]$Target = 'all',

  [ValidateSet('Debug','Release')]
  [string]$Config = 'Debug'
)

$ErrorActionPreference = 'Stop'

function Invoke-Step([string]$Title, [scriptblock]$Body)
{
  Write-Host "`n==> $Title" -ForegroundColor Cyan
  & $Body
}

function Require-Command([string]$Name)
{
  if (-not (Get-Command $Name -ErrorAction SilentlyContinue))
  {
    throw "Missing required command: $Name"
  }
}

$repoRoot = Split-Path -Parent $MyInvocation.MyCommand.Path

$cppFlags = @('-std=c++17')
if ($Config -eq 'Debug')
{
  $cppFlags += @('-g','-O0')
}
else
{
  $cppFlags += @('-O2')
}

if ($Target -in @('all','cpp'))
{
  Invoke-Step "Build C++ app + tests ($Config)" {
    Require-Command 'g++'

    Push-Location (Join-Path $repoRoot 'nm-lib')
    try
    {
      if (-not (Test-Path '.\bin\tests'))
      {
        New-Item -ItemType Directory -Force -Path '.\bin\tests' | Out-Null
      }

      & g++ @cppFlags -Iinclude -Isrc -o app.exe main.cpp src/core/*.cpp src/linear/*.cpp src/nonlinear/*.cpp src/utils/*.cpp

      & g++ @cppFlags -Iinclude -Isrc -o .\bin\tests\tema1_rootfinding.exe .\tests\tema1_rootfinding.cpp src/core/*.cpp src/linear/*.cpp src/nonlinear/*.cpp src/utils/*.cpp
      & g++ @cppFlags -Iinclude -Isrc -o .\bin\tests\tema2_gauss.exe .\tests\tema2_gauss.cpp src/core/*.cpp src/linear/*.cpp src/nonlinear/*.cpp src/utils/*.cpp
      & g++ @cppFlags -Iinclude -Isrc -o .\bin\tests\tema3_iterative.exe .\tests\tema3_iterative.cpp src/core/*.cpp src/linear/*.cpp src/nonlinear/*.cpp src/utils/*.cpp
      & g++ @cppFlags -Iinclude -Isrc -o .\bin\tests\tema4_newton_systems.exe .\tests\tema4_newton_systems.cpp src/core/*.cpp src/linear/*.cpp src/nonlinear/*.cpp src/utils/*.cpp
    }
    finally
    {
      Pop-Location
    }
  }
}

if ($Target -in @('all','webapp'))
{
  Invoke-Step "Build webapp" {
    Require-Command 'npm'

    Push-Location (Join-Path $repoRoot 'webapp')
    try
    {
      if (-not (Test-Path '.\node_modules'))
      {
        & npm install
      }

      & npm run build
    }
    finally
    {
      Pop-Location
    }
  }
}

Write-Host "`nDone." -ForegroundColor Green
