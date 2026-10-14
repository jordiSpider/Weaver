#!/bin/bash
#SBATCH --job-name=weaver_ga
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=8    # El motor leerá estos 8 núcleos automáticamente mediante availableCores()
#SBATCH --mem=16G
#SBATCH --output=weaver_optimisation_%j.out

module load R/4.3.1 # Carga el entorno de R de tu clúster

Rscript run_slurm_optimisation.R