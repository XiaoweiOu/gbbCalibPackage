## systematics cross-check

## bb-tagging calibration

Aim is to derive scale factors and uncertainies for double-b-tagged jets.
Input: Ntuples from CxAOD_FlavourTagging framework
Steps:
   1. run_gbbTupleAna.exe <infile> <outfile> <treename> <configname>
      Used to apply final event selection and produce files with histograms.
      Different modes available based on config file:
        produce Sd0 templates for fit
        produce inputs for systematics cross-check
        produce inputs for calibration
      Need to hadd output files
        also run python script for syst. cross-check inputs
   2. run_calculateSF.exe <configname>
      Used to run Sd0 template fit and calculate scale factors
      Input is file from previous step
