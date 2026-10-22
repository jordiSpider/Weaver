# embed_schemas.ps1
Param(
    [string]$SourceSchemaDir,
    [string]$StageBuildDir 
)

$ErrorActionPreference = "Stop"

# Destino normativo e internamente empaquetado de R (inst/)
$TargetSchemaDir = Join-Path (Join-Path $StageBuildDir "inst") "schema"

Write-Host "[Weaver Asset Embedder] Verifying absolute schema folder tracking..." -ForegroundColor Cyan

# 1. Validación de seguridad sobre la ruta absoluta inyectada por CMake
if (!(Test-Path $SourceSchemaDir)) {
    Write-Error "CRITICAL CMake Error: Source schema files workspace not found at: $SourceSchemaDir"
    Exit 1
}

# 2. Construcción del árbol de subcarpetas en el directorio de staging temporal
if (!(Test-Path $TargetSchemaDir)) {
    New-Item -ItemType Directory -Force -Path $TargetSchemaDir | Out-Null
    Write-Host "[Weaver Asset Embedder] Created environment deployment point: inst/schema" -ForegroundColor Gray
} else {
    # OPCIONAL: Limpieza preventiva de ficheros residuales viejos de compilaciones previas
    Remove-Item -Path (Join-Path $TargetSchemaDir "*") -Recurse -Force -ErrorAction SilentlyContinue
}

# 3. Copia recursiva directa de los esquemas hacia la envoltura estructural del paquete
Copy-Item -Path (Join-Path $SourceSchemaDir "*") -Destination $TargetSchemaDir -Recurse -Force

Write-Host "[Weaver Asset Embedder] Package infrastructure assets synchronized successfully." -ForegroundColor Green
Exit 0
