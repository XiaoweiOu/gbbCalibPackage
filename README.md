# gbbCalibration Package for R21

## Installation and setup
Checking out the package with `git clone ssh://git@gitlab.cern.ch:7999/aemerman/gbbCalibPackage.git` will set up the full file structure.
Run `source setup.sh` from the main package directory to set the environment. This script assumes you have the following lines in your .bashrc:
```
export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
alias setupATLAS='source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh'
```
With the AnalysisBase release set up, compile with `cd build && cmake ../source && make`.
After first-time compilation, you will need to source the `build/${AnalysisBase_PLATFORM}/setup.sh` script that is created. Afterwards this will be done as part of the `setup.sh` script.

To change AnalysisBase releases, remove the `.asetup.save` file from the `source` directory and set up your new release there. The `setup.sh` script will then use that automatically.

NB: There is a .gitignore file in the `build` directory. If you need to clean the `build` directory (e.g. while changing releases or to fix compilation problems) please put this back or be very careful not to add build files to the repository.

## Running the code
Run the full scale factor calculation chain takes several steps and will depend on your batch submission system. There are a few scripts provided for different systems (not yet working). Generally speaking though, the steps are as follows:
1. Set the configuration files in `source/gbbCalibration/data/configs`
   * The histogram making code uses the `run_GbbTupleAna_*.cfg` files
   * The scale factor calculation code uses the `config_*_SF.cfg` files
   * Everything uses the `GlobalConfig.cfg` file
   * Check the `data/xsections.txt` file to be sure your MC channel(s) are listed
   * __Commit your changes and create a tag for future reference__
2. Making reweighting histograms from FTNtupCalib input
   * run\_gbbTupleAna &lt;input&gt; &lt;root\_output&gt; gbbCalibration/configs/run\_GbbTupleAna\_Reweight.cfg
      * If the <input> file ends in .txt it is assumed to be list
      * Some input lists are provided in the `datasets` directory
      * Do separately for data and each MC channel
      * Will use the systematics list from `GlobalConfig.cfg`
   * Create a json file with the locations of the &lt;output&gt; files
   * python makeReweightingHistos.py &lt;root\_output&gt; &lt;json\_input&gt;
      * Run from the `source/gbbCalibration/python` directory
3. Making template histograms from FTNtupCalib input
   * run\_gbbTupleAna &lt;input&gt; &lt;root\_output&gt; gbbCalibration/configs/run\_GbbTupleAna\_Calib.cfg
   * Create a json file with the locations of the &lt;output&gt; files
   * python makeCrossCheckInputsInclusiveLL\_AllSys.py &lt;root\_output&gt; &lt;json\_input&gt;
      * Run from the `source/gbbCalibration/python` directory
      * Will use the systematics list from `GlobalConfig.cfg`
4. Calculate scale factors
   * run\_calculateSF ../source/gbbCalibration/data/configs/config\_Calib\_SF.cfg &lt;output\_Folder\_name&gt;
      * TODO: provide input file as separate argument

## Package structure
This repository is meant to contain everything required to generate scale factors from flat FTNtupCalib tuples. The tuples are generated with the CxAOD\_FlavourTagging packages (see Migle's very detailed [twiki](https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/XbbScaleFactorsInGbbEvtR21)).

The code itself sits in the `source/gbbCalibration` directory and is split into several subdirectories. 
### histMaker
`histMaker` contains a [MakeClass](https://root.cern.ch/doc/master/classTTree.html#ac4ceaf4ae0b87412acf94093043cc2de)-based framework for generating histograms from a flat ntuple. The main body of the code is in GbbTupleAna.
### scaleFactorCalculator
`scaleFactorCalculator` contains the fitting code for generating scale factors from a set of histograms, as well as generating validation plots. The process is controlled in the ScaleFactorCalculator class, and the fitting is done in TMinuit from the FitData and Fitter classes.
### python
The `python` directory has a set of scripts for generating the reweight and scale-factor-calculation input files, as well as scripts for making various validation and cross-check plots.
### helpers
This directory is meant to hold functions used by multiple steps of the chain. The most important class here is GlobalConfig, which sets the systematics, kinematic regions and flavour-pair bins used in the calculation. It also contains binning information, and histogram naming conventions. This class reads the config file `data/configs/GlobalConfig.cfg`.
In addition, `helpers/atlasstyle` has macros for drawing ATLAS-style plots.

The package also contains a `datasets` directory to store lists of the latest tuple inputs. I am considering moving the `scripts` directory from gbbCalibration here, and adding a `tests` directory for CI tests.
