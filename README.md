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
The histogram-making code lives in `gbbCalibPackage/source/gbbCalibration/histMaker` and can be run with with `run_gbbTupleAna`. Batch submission scripts for several systems are located in `gbbCalibPackage/source/gbbCalibration/scripts`.
To run locally do:
```
run_gbbTupleAna <input> <root_output> <tree_name> <config_file>
```
If &lt;input&gt; ends in .txt it is assumed to be a list of root files (1 per line), otherwise it is assumed to be a root file.
The default config file for producing a full set of histograms is run\_GbbTupleAna\_Calib.cfg. The config file run\_GbbTupleAna\_Reweight.cfg can be used to produce 2D reweighting histograms for input into the calibration.

Making templates with qsub:
```
source scripts/NAFScripts/submit_gbbTupleAna_array.sh <run_type> run_GbbTupleAna_Calib.cfg <tag>
```
&lt;run\_type&gt;: 0 -- nominal only, 1 -- nominal+systematics, 2 -- systematics only

After producing histograms from individual files or batches of files you will need to produce a single file consolidating all the histograms and weighting the appropriately. First hadd the histograms files by channel, the create a json file with the locations of your data and MC. We don't currently provide a script for this but example json files can be found in `source/gbbCalibration/data/examples/json`. To produce the consolidated histogram file run:
```
python gbbCalibration/python/makeCrossCheckInputsInclusiveLL_AllSys.py <root_output> <json_input> [<options>]
```

### Producing scale factors and plots from histograms
The histogram-making code lives in `gbbCalibPackage/source/gbbCalibration/scaleFactorCalculator` and can be run with with `run_calculateSF`.
```
run_calculateSF <root_input> config_Calib_SF.cfg <output_folder>
```

### Producing scale factors with TRExFitter
The consolidated histogram file output by `gbbCalibration/python/makeCrossCheckInputsInclusiveLL_AllSys.py` can be used to calculate SFs with TRExFitter. In this case, however, there are a few extra steps to process the input and output correctly. `run_calculateSF` automatically rebins histograms to reduce statistical fluctuations in the templates. A version of this rebinning is provided as a standalone script called `rebin.py`. The output of this rebinning script is then given to `run_trex.sh`, which will create a TRExFitter config file for each bin and then fit them in sequence. TRExFitter will automatically produce some plots, such as correlation matrices, ranking and pull plots.
Another script, `makePostfitPlots.py`, is provided to read the fit results and make plots that aren't made automatically.
```
python gbbCalibration/python/rebin.py <trex_input> <hist_input> [<options>]
run_trex.sh -o <output_folder> -i <tex_input> [<options>]
python gbbCalibration/python/plotting/makePostfitPlots.py <input_folder> [<options>]
```

A config file can be given to `run_trex.sh` to make it easier to control the options provided. One such config file is provided here: `data/configs/trex_Calib_SF.cfg`. Other notable options include the `--nosys` option, which tells these scripts to produce and plot nominal-only fits, and the `--bins` option which can be set to `fatjet` to run the fit in bins of large-R jet pT.

#### Rebin.py options
```
usage: rebin.py [-h] [--nosys] [--stat STAT] [--force FORCE] [--hist HIST]
                [--split]
                outfile infile

Make reweight histograms.

positional arguments:
  outfile        Name of output ROOT file
  infile         Name of input ROOT file

optional arguments:
  -h, --help     show this help message and exit
  --nosys        Rebin only nominal histograms
  --stat STAT    Stat threshold (err/N) for rebinning [default: 0.25]
  --force FORCE  Force rebinning by at least n [default: 1]
  --hist HIST    Changes which hists are checked for stat threshold. Options
                 are 'data', 'MC', or any of the flavour-pair labels [default:
                 BB].
  --split        Rebin negative tail separately (merging from right to left)
```

#### run\_trex.sh options
```
Usage: run_trex.sh -i <input> -o <output> [-h] [-e <args>]
       -o <output> Name of directory to store TRExFitter results
       -i <input>  Name of histogram file for TRExFitter input
       -h          Print this help message
       -c          Name of config file (located in data/configs)
       -e          Additional arguments to be passed directly
                   to generate_trex_config.py
```
`run_trex.sh` uses another script, `generate_trex_config.py`, to write TRExFitter config files. The options for this script are:
```
usage: generate_trex_config.py [-h] [--bins BINS] [--year YEAR] [--asimov]
                               [--noMCstat] [--fitSF] [--cfg CFG]
                               [--debug DEBUG] [--nosys]
                               output

Write config files used to run TRExFitter.

positional arguments:
  output         Name of output directory

optional arguments:
  -h, --help     show this help message and exit
  --bins BINS    Use trkjet bins, fatjet bins or no bins (incl) [default:
                 trkjet]
  --year YEAR    Year determines luminosity to normalize to [default:
                 2015+2016]
  --asimov       Do Asimov fit rather than fitting to data
  --noMCstat     Don't use MC stat errors in fit for comparison to old
                 framework
  --fitSF        Fit scale factor directly (rather than only correcting flavor
                 fractions)
  --cfg CFG      Name of extra config file (located in
                 gbbCalibration/data/configs)
  --debug DEBUG  Debug level for TRexFitter [default: 2]
  --nosys        Run nominal-only fit
```
Note that options provided in the config file (if any) will override options provided directly to the script.

#### makePostfitPlots.py options
```
usage: makePostfitPlots.py [-h] [--bins BINS] [--year YEAR] [--debug]
                              [--plots PLOTS [PLOTS ...]]
                              input

Make fit result histograms.

positional arguments:
  input                 Folder containing TRExFitter results

optional arguments:
  -h, --help            show this help message and exit
  --bins BINS           Use trkjet bins, fatjet bins or no bins (incl)
                        [default: trkjet]
  --year YEAR           Year determines luminosity to normalize to [default:
                        2015+2016]
  --debug               Add extra printouts
  --plots PLOTS [PLOTS ...]
                        List of plots to make. Options include
                        'SF','NF','Tmpl','Kine',...
```
