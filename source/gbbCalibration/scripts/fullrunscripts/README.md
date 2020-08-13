fullrun.py Documentation

fullrun.py runs the gbbcalibration entire workflow.
(fullrun.py can run locally or be submitted to SLURM)

The purpose of this script is to achieve gbbCalibration code automation.
That it takes input txt files for run_GbbtupleAna in one folder, and output scalefactor and histograms in along the way.

By default, user supply the input txt files(everything for one full run, data and mc ) in `gbbCalibPackage/run/InputTxt`. fullrun.py will output everything that gbbCalibration package outputs in `gbbCalibPackage/run/workspace`



WorkFlow:

1. Setup the working envirnment as specified in gbbCalibration package.(section Installation and setup)

Make sure `run_gbbTupleAna` can execute in command line.

2. Modify the configs as follows:

* change (in data/config folder) the `run_GbbTupleAna_Calib.cfg` setting of the following:
~~~~
JetPtEtaReweightingFile <path to gbbCalibPackage>/gbbCalibPackage/run/workspace/reweightedFile/reweighted_nmu_out.root
JetPtEtaReweightingFileInclusive <path to gbbCalibPackage>/gbbCalibPackage/run/workspace/reweightedFile/reweighted_mu_out.root
~~~~
* change (in data/config folder) `config_Calib_SF.cfg` setting of the following:
~~~~
InputFile   <path to gbbCalibPackage>/gbbCalibPackage/run/workspace/crossChecked/crosschecked-out.root
~~~~

3. Put input txt files (all of them for a full run, data and mc) to in `gbbCalibPackage/run/InputTxt` (you may need to create this folder)

4. remove directory `gbbCalibPackage/run/workspace` if it exist. it will be created and be filled with outputs.

5. adjust  `MaxProcesses` global variable in fullrun.py.
MaxProcesses is the number of run_gbbTupleAna to run in parallel.

For example:
`MaxProcesses= 32` means 32 run_gbbTupleAna is running in parallel and each is processing one .txt input.

If fullrun.py runs on local machine, set MaxProcesses less than 5, or the number recommended by your machine admin. If fullrun.py runs on a batch system (like SLURM) MaxProcesses should be set to the number of cpu you set for the batch job.(More on Batch below) 

6. `python fullrun.py` starts the job.


Batch System (SLURM) Usage and Example (filename batchfullrun.slr):
~~~~
#!/bin/bash -l
#SBATCH --account=<optinal>
#SBATCH --time=<suspenstion time limit>
#SBATCH --time-min=<time guaranteed to execute>
#SBATCH --qos=<shared or regular>
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=<32 or appropriate, match the MaxProcesses in fullrun.py>
#SBATCH --constraint=haswell
#SBATCH -J gbbCalibration
#SBATCH --image=centos:centos7 --export=ALL

srun python <absolute path to this script>
# for exampe srun python /global/???/gbbCalibration/scripts/fullrunscripts/fullrun.py
~~~~
Sbatch Comments: 

fullrun.py is a single multi-process parallel task. Thus --ntasks=1. --cpus-per-task= 32 is usual because 1 node with 32 cpu is quite easy to acquire in job manager.

Make sure fullrun.py runs correctly locally, then this job can be run in the same environment as does locally. --export=ALL means copy the local environment when the job is submitted. 

Submit job run in command line:
~~~~
sbatch ./batchfullrun.slr
~~~~

Remarks to User:

1. In the main function of fullrun.py, comment out steps that you don't want to run, or add exit(0) after the desired step finish.

2. You can run in main() function step 0, `setUp()` once and then comment it out, then run fullrun.py again will use the same gbbCalibration/run/workspace folder.

3. Systematics is set in global variable Systematics, by default only nominal is used.

4. Json files are produced/dumped in workspace/jsons
Json with systematics have never been tested, modification is needed in getjson function. (it assumes nominal is always used)

5. If `fullrun.py` gets moved around in the gbbCalibPackage (i.e. the location changed), the global variable `SourceDir` must be adjusted accordingly. 


Remarks to Developers:

fullrun.py was originally written for local run only. So there can be possible improvements on batch system.
* The multi-processes are handled manually, but in a batch system it is better for job manager to decide the number of processes to run at the same time. TODO: One can convert each python subprocess of run_gbbtupleana to a srun command in .slr submit script, then the batch system can be tuned to have hundreds of run_gbbtuplana running in parallel across multiple nodes. (You can also submit one single run_gbbtupleana as a batch job, but the overhead image creating is wasteful and the queue time is long) This gives the ultimate speed up in production

* The reweightedFiles and scalefactor-calculator input file should be taken as an argument to run_gbbtupleana and scalefactorcalculator respectively. Then changing the config set can be eliminatted.




 
