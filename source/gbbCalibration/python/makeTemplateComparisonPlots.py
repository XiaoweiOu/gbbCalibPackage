import ROOT
from PlotFunctions import *
from TAxisFunctions import *
import ConfigFunctions as config
import argparse

ROOT.gROOT.SetBatch(True)
from ROOT import TCanvas,TPad,TH2,TColor

#ROOT.gStyle.SetOptStat(0)
SetupStyle()

#slice:
islice="Incl"
#islice="mjpt_g100l200_nmjpt_g200l300"

parser = argparse.ArgumentParser(description='Make reweight histograms.')
parser.add_argument('outfile', help="Prefix of output files")
parser.add_argument('infiles', help="JSON file with paths for data and MC files. See README for format")
args = parser.parse_args()

outfilename = args.outfile
pathData, ListOfMCPaths, ListOfInclusiveMCPaths , xsecFile = config.GetPathsFromJSON(args.infiles)

Lumi = 36000.0 #in pb^-1

MyConfig = config.LoadGlobalConfig()
histHelper = config.HistHelper(xsecFile)

ListOfSystematics = [ ROOT.TString("Nom") ] #MyConfig.GetSystematics() 
ListOfFlavourPairs = MyConfig.GetFlavourPairs()
ListOfInclusiveFlavourPairs = [ 'LL' ]

ListOfVariables = [ 'meanSd0', 'maxSd0', 'subSd0', 'thirdSd0' ]
#ListOfVariables = [ 'MV2c10' ]

MapOfAxisLabels = { 'meanSd0' :'#LT s_{d0} #GT',
                    'maxSd0'  :'Leading s_{d0}', 
                    'subSd0'  :'Sub-leading s_{d0}',
                    'thirdSd0':'Third-leading s_{d0}',
                    #'MV2c10'  :'MV2c10 Score'    
                  }
                    

colors=[ROOT.kBlue+1,ROOT.kAzure-4,ROOT.kCyan+3,ROOT.kGreen-9,ROOT.kOrange]
#colors = [ROOT.kBlue, ROOT.kBlack]
#SetColors(canvas,colors)

#loop over MC histograms
for var in ListOfVariables :
  histsMJ = []
  histsNMJ = [] 
  
  for flavour in ListOfFlavourPairs:
    ListOfPaths = ListOfMCPaths
    if flavour in ListOfInclusiveFlavourPairs:
      ListOfPaths = ListOfInclusiveMCPaths

    for jet in [ 'mj', 'nmj' ]:
      histname = MyConfig.GetMCHistName("Nom",islice,flavour,jet+var).Data()

      histMC = histHelper.AddMCHists(histname,ListOfPaths)
      if histMC:
        histMC.SetName(flavour.Data())
        histMC.SetLineWidth(3);
        #Normalize to Unity
        histMC.Scale(1./histMC.Integral())
        if jet is 'nmj':
          histsNMJ.append(histMC)
        elif jet is 'mj':
          histsMJ.append(histMC)
      else:
        print("Could not find "+histname+" in all input files!")
  
  #make Canvas
  canvMJ = ROOT.TCanvas("c_mu"+var,"",800,800);
  canvNMJ = ROOT.TCanvas("c_nonmu"+var,"",800,800);

  for hist in histsMJ:
    AddHistogram(canvMJ,hist,'hist')
  FullFormatCanvasDefault(canvMJ,36,simulation=True, doLogy=True)
  SetColors(canvMJ,colors)
  SetAxisLabels(canvMJ,'Muon-associated Track Jet '+MapOfAxisLabels[var],'Normalised to Unity')
  canvMJ.Print(outfilename+'_'+islice+'_'+var+'.pdf(')

  for hist in histsNMJ:
    AddHistogram(canvNMJ,hist,'hist')
  FullFormatCanvasDefault(canvNMJ,36,simulation=True, doLogy=True)
  SetColors(canvNMJ,colors)
  SetAxisLabels(canvNMJ,'Non-Muon-associated Track Jet '+MapOfAxisLabels[var],'Normalised to Unity')
  canvNMJ.Print(outfilename+'_'+islice+'_'+var+'.pdf)')

print "done"
