# ==============================================================================
# weaverR Dynamic Multi-Folder Flattening Script for Windows Build Pipelines
# ==============================================================================
# R only loads the files located directly in R/. This script copies the package
# to a staging folder and moves every R file from the R/ subfolders to R/.
param (
    [string]$PackageSrcDir,
    [string]$StageBuildDir,
    [ValidateSet("full", "server")][string]$Variant = "full"
)

$ErrorActionPreference = "Stop"

Write-Host "[CMake Target] Purging and syncing build staging folder..."
if (Test-Path $StageBuildDir) {
    Remove-Item -Path $StageBuildDir -Recurse -Force
}
Copy-Item -Path $PackageSrcDir -Destination $StageBuildDir -Recurse -Force

if ($Variant -eq "server") {
    foreach ($Rel in @("R/gui", "inst/gui")) {
        $Path = Join-Path $StageBuildDir $Rel
        if (Test-Path $Path) {
            Write-Host "[CMake Target] Variant 'server': removing $Rel"
            Remove-Item -Path $Path -Recurse -Force
        }
    }
}

$StageRDir = Join-Path $StageBuildDir "R"

if (Test-Path $StageRDir) {
    Write-Host "[CMake Target] Flattening nested R files from subdirectories..."

    # Gather all .R files that are not located directly in the root R/ folder
    $NestedRFiles = Get-ChildItem -Path $StageRDir -Recurse -Filter *.R | Where-Object { $_.DirectoryName -ne $StageRDir }

    # Safety check: two files with the same name would overwrite each other
    $AllRFiles = Get-ChildItem -Path $StageRDir -Recurse -Filter *.R
    $Duplicates = $AllRFiles | Group-Object Name | Where-Object { $_.Count -gt 1 }
    if ($Duplicates) {
        foreach ($Group in $Duplicates) {
            Write-Error ("Duplicate file name '{0}' found in:`n  {1}" -f $Group.Name, (($Group.Group | ForEach-Object { $_.FullName }) -join "`n  ")) -ErrorAction Continue
        }
        throw "Flattening aborted: rename the duplicated R files so every name is unique."
    }

    foreach ($File in $NestedRFiles) {
        $TargetDestination = Join-Path $StageRDir $File.Name
        Move-Item -Path $File.FullName -Destination $TargetDestination
    }

    # Purge now empty subfolders inside the staging folder to prevent R CMD compilation errors
    Get-ChildItem -Path $StageRDir -Directory | Remove-Item -Recurse -Force
    Write-Host "[CMake Target] Flattening operational cycle completed successfully."
} else {
    Write-Warning "[CMake Target] Active source R directory context was not found."
}
