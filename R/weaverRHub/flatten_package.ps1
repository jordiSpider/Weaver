# ==============================================================================
# weaverRHub Dynamic Multi-Folder Flattening Script for Windows Build Pipelines
# ==============================================================================
param (
    [string]$PackageSrcDir,
    [string]$StageBuildDir
)

$ErrorActionPreference = "Stop"

Write-Host "[CMake Target] Purging and syncing build staging folder..."
if (Test-Path $StageBuildDir) {
    Remove-Item -Path $StageBuildDir -Recurse -Force
}
Copy-Item -Path $PackageSrcDir -Destination $StageBuildDir -Recurse -Force

$StageRDir = Join-Path $StageBuildDir "R"

if (Test-Path $StageRDir) {
    Write-Host "[CMake Target] Flattening nested R files from subdirectories..."
    
    # Gather all .R files that are not located directly in the root R/ folder
    $NestedRFiles = Get-ChildItem -Path $StageRDir -Recurse -Filter *.R | Where-Object { $_.DirectoryName -ne $StageRDir }
    
    foreach ($File in $NestedRFiles) {
        $TargetDestination = Join-Path $StageRDir $File.Name
        Move-Item -Path $File.FullName -Destination $TargetDestination -Force
    }
    
    # Purge now empty subfolders inside the staging folder to prevent R CMD compilation errors
    Get-ChildItem -Path $StageRDir -Directory | Remove-Item -Recurse -Force
    Write-Host "[CMake Target] Flattening operational cycle completed successfully."
} else {
    Write-Warning "[CMake Target] Active source R directory context was not found."
}