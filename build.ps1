$ErrorActionPreference = 'Stop'
$root = $PSScriptRoot
$compiler = (Get-Command g++ -ErrorAction Stop).Source
$common = @('-std=c++17', '-Wall', '-Wextra', '-Wpedantic', '-I', (Join-Path $root 'include'))
$sources = @(
    (Join-Path $root 'src/book.cpp'),
    (Join-Path $root 'src/storage.cpp'),
    (Join-Path $root 'src/library.cpp')
)
& $compiler @common @sources (Join-Path $root 'src/main.cpp') '-o' (Join-Path $root 'library_manager.exe')
if ($LASTEXITCODE -ne 0) { throw 'Main build failed.' }
& $compiler @common @sources (Join-Path $root 'tests/test_library.cpp') '-o' (Join-Path $root 'library_tests.exe')
if ($LASTEXITCODE -ne 0) { throw 'Test build failed.' }
& $compiler @common (Join-Path $root 'examples/syntax_lab.cpp') '-o' (Join-Path $root 'syntax_lab.exe')
if ($LASTEXITCODE -ne 0) { throw 'Syntax lab build failed.' }
Write-Host 'Built library_manager.exe, library_tests.exe and syntax_lab.exe'
