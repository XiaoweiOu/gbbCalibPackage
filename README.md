# gbbCalibration Package

## Installation and setup
First time install:
```
git clone --recursive ssh://git@gitlab.cern.ch:7999/atlas-boosted-hbb/gbbCalibPackage.git
cd gbbCalibPackage
source ./setup.sh
cd build
cmake ../source
make
```
This will create the file structure and compile the code into the build directory.
To set up the environment for a previously installed package, run:
```
cd gbbCalibPackage
source ./setup.sh
```

## Running the code
This package contains the code to make templates from a flat ntuple and calculate scale factors from those templates. There is another package available for produce ntuples from DAODs, see https://gitlab.cern.ch/atlas-boosted-hbb/gbb-calibration-superproject.
There is also a twiki page: [twiki](https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/XbbScaleFactorsInGbbEvtR21)).

### General information
Configuration files are located `gbbCalibPackage/source/gbbCalibration/data/configs`
   * The histogram making code uses the `run_GbbTupleAna_*.cfg` files
   * The scale factor calculation code uses the `config_*_SF.cfg` files
   * Common configuration is stored in the `GlobalConfig.cfg` file
   * Check the `data/xsections_*.txt` file to be sure your MC channel(s) are listed
      * You can get cross-section information for samples using the getMetadata.py script provided in AthAnalysis
   * To keep a record of the configuration used to produce a specific set of scale factors, __commit your changes and create a tag__

### Producing histograms from ntuples
The histogram-making code lives in `gbbCalibPackage/source/gbbCalibration/histMaker` and can be run with with `run\_gbbTupleAna`. Batch submission scripts for several systems are located in `gbbCalibPackage/source/gbbCalibration/scripts`.
To run locally do:
```
run\_gbbTupleAna &lt;input&gt; &lt;root\_output&gt; &lt;tree\_name&gt; &lt;config\_file&gt;
```
If &lt;input&gt; ends in .txt it is assumed to be a list of root files (1 per line), otherwise it is assumed to be a root file.
The default config file for producing a full set of histograms is run\_GbbTupleAna\_Calib.cfg. The config file run\_GbbTupleAna\_Reweight.cfg can be used to produce 2D reweighting histograms for input into the calibration.

Making templates with qsub:
```
source scripts/NAFScripts/submit\_gbbTupleAna\_array.sh &lt;run\_type&gt; run\_GbbTupleAna\_Calib.cfg &lt;tag&gt;
```
&lt;run\_type&gt;: 0 -- nominal only, 1 -- nominal+systematics, 2 -- systematics only

After producing histograms from individual files or batches of files you will need to produce a single file consolidating all the histograms and weighting the appropriately. First hadd the histograms files by channel, the create a json file with the locations of your data and MC. We don't currently provide a script for this but example json files can be found in `source/gbbCalibration/data/examples/json`. To produce the consolidated histogram file run:
```
python gbbCalibration/python/makeCrossCheckInputsInclusiveLL\_AllSys.py &lt;root\_output&gt; &lt;json\_input&gt; [&lt;options&gt;]
```

### Producing scale factors and plots from histograms
The histogram-making code lives in `gbbCalibPackage/source/gbbCalibration/scaleFactorCalculator` and can be run with with `run\_calculateSF`.
```
run\_calculateSF &lt;root\_input&gt; config\_Calib\_SF.cfg &lt;output\_folder&gt;
```

### Producing scale factors with TRExFitter
`run\_calculateSF` automatically rebins the template histograms to reduce per-bin errors. A copy of this rebinning algorithm is provided in `rebin.py`. Scripts to produce TRExFitter config files and run them are located in `gbbCalibPackage/source/gbbCalibration/scripts`. To run them do:
```
python gbbCalibration/python/rebin.py &lt;trex\_input&gt; &lt;hist\_input&gt; -r [&lt;options&gt;]
source run_trex.sh &lt;output\_folder&gt; &lt;tex\_input&gt; --make
python gbbCalibration/python/plotting/makeTRExOutputPlots.py &lt;output\_folder&gt; [&lt;options&gt;]
```
