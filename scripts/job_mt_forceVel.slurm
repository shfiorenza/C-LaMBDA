#!/bin/bash

# === List of SBATCH arguments ===
#SBATCH --account=ucb-summit-smr	# allocations account
#SBATCH --partition=shas			# Type of node to run on
#SBATCH --qos=condo        		# quality of service/queue
#SBATCH --nodes=1					# number of cluster nodes, abbreviated by -N
#SBATCH --ntasks=1					# number of parallel process
#SBATCH --cpus-per-task=24
#SBATCH --time=48:00:00				# walltime, abbreviated by -t
#SBATCH --job-name=MTforce		 	# name of job
#SBATCH --output=run_mt_forceVel_%j.out   # name of the stdout redirection file
#SBATCH --error=run_mt_forceVel_%j.err	# name of the stderr redirection file

# === Purge all modules and load needed ones ===
module purge
module load gcc/8.2.0
module list

# === Run the program ===
export LD_LIBRARY_PATH=/projects/shfi4480/overlap_analysis/mgh_model/libs/gsl/lib:$LD_LIBRARY_PATH
echo "LD_LIBRARY_PATH set to ${LD_LIBRARY_PATH}"
echo "Current variables CC=${CC} and CXX=${CXX}"

bash scan_mt_forceVel.sh
