fullrun.py Documentation

fullrun.py runs the gbbcalibration entire workflow.
(fullrun.py can run locally or be submitted to SLURM)

The purpose of this script is to achieve gbbCalibration code automation.
That it takes input txt files for run_GbbtupleAna in one folder, and output scalefactor and histograms in along the way.

By default, user supply the input txt files(everything for one full run, data and mc ) in `gbbCalibPackage/run/InputTxt`. fullrun.py will output everything that gbbCalibration package outputs in `gbbCalibPackage/run/workspace`



WorkFlow:

1. Setup the working envirnment as specified in gbbCalibration package.(section Installation and setup)

Make sure `run_gbbTupleAna` can execute in command line.

2.Modify the configs as follows:

2.1 change (in data/config folder) the `run_GbbTupleAna_Calib.cfg` setting of the following:
`JetPtEtaReweightingFile <path to gbbCalibPackage>/run/workspace/reweightedFile/reweighted_nmu_out.root
JetPtEtaReweightingFileInclusive <path to gbbCalibPackage>/run/workspace/reweightedFile/reweighted_mu_out.root`

2.2 change (in data/config folder) `config_Calib_SF.cfg` setting of the following
`InputFile   <path to gbbCalibPackage>/run/workspace/crossChecked/crosschecked-out.root`

3. Put input txt files (all of them for a full run, data and mc) to in `gbbCalibPackage/run/InputTxt`

4. remove directory `gbbCalibPackage/run/workspace` if it exist. it will be created and be filled with outputs.

5. adjust  `MaxProcesses` global variable in fullrun.py
MaxProcesses is the number of run_gbbTupleAna to run in parallel.

For example:
`MaxProcesses= 32` means 32 run_gbbTupleAna is running in parallel and each is processing one .txt input.

If fullrun.py runs on local machine, set MaxProcesses less than 5, or the number recommended by your machine admin. If fullrun.py runs on a batch system (like SLURM) MaxProcesses should be set to the number of cpu you set for the batch job.(More on Batch below) 

6. `python fullrun.py` starts the job.


Remarks:

1. In the main function, comment out steps that you don't want to run, or add exit(0) after the desired step finish.

2. You can run step 0, setUp() once and then comment it out, then run fullrun.py again will use the same gbbCalibration/run/workspace folder.

3. Systematics is set in global variable Systematics, by default only nominal is used.

4. Json files are produced/dumped in workspace/jsons
Json with systematics have never been tested, modification is needed in getjson function. (it assumes nominal is always used)






 
