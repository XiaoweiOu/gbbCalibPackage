fullrun.py runs the gbbcalibration entire workflow. The flow can be adjusted in main function. (fullrun.py can run locally or be submitted to SLURM)

You need to have/set a workspace folder that fullrun.py dumps all the output. This set as the BaseDir.

Make sure you set the configPaths to your desired configfiles. Set your reweighted files in run_GbbTupleAna_Calib.cfg to be "reweighted_mu_out.root" and "reweighted_nmu_out.root" (absolute path?)

It is parallelised, and the number of parallel run_gbbtupleAna can be adjusted at maxnumberofprocess global variable.

batchfullrun.slr is the SLURM job script that is submitted to run fullrun.py. Make sure the number of core per node matches the parallel setting in fullrun.py. fullrun.py is 1 parallel progam with multi-process.

You need to setup the environment(ROOT, gbbCalibration package) that fullrun.py can run locally. Then SLURM can run in the same environment.

run_submit.sh is the submit command/script that add this job to job queue.

 
