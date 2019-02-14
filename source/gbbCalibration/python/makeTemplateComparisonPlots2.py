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
parser.add_argument('infiles1', help="JSON file with paths for data and MC files. See README for format")
parser.add_argument('infiles2', help="JSON file with paths for data and MC files. See README for format")
parser.add_argument('in_tag1', help="Tag of input 1")
parser.add_argument('in_tag2', help="Tag of input 2")
args = parser.parse_args()

outfilename = args.outfile
pathData1, ListOfMCPaths1, ListOfInclusiveMCPaths1, xsecFile1 = config.GetPathsFromJSON(args.infiles1)
pathData2, ListOfMCPaths2, ListOfInclusiveMCPaths2, xsecFile2 = config.GetPathsFromJSON(args.infiles2)
tag1 = args.in_tag1
tag2 = args.in_tag2

Lumi = 36000.0 #in pb^-1

MyConfig = config.LoadGlobalConfig()
histHelper1 = config.HistHelper(xsecFile1)
histHelper2 = config.HistHelper(xsecFile2)

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


colors1=[ROOT.kBlue+1,ROOT.kAzure-4,ROOT.kCyan+3,ROOT.kGreen-9,ROOT.kOrange]
colors2=[ROOT.kBlue+2,ROOT.kAzure-3,ROOT.kCyan+4,ROOT.kGreen-8,ROOT.kOrange+1]
#colors = [ROOT.kBlue, ROOT.kBlack]
#SetColors(canvas,colors)


#loop over MC histograms
for var in ListOfVariables :
  histsMJ1 = []
  histsNMJ1 = []
  
  histsMJ2 = []
  histsNMJ2 = []


  for flavour in ListOfFlavourPairs:
    
    ListOfPaths1 = ListOfMCPaths1
    ListOfPaths2 = ListOfMCPaths2
    if flavour in ListOfInclusiveFlavourPairs:
      ListOfPaths1 = ListOfInclusiveMCPaths1
      ListOfPaths2 = ListOfInclusiveMCPaths2

    for jet in [ 'mj', 'nmj' ]:
      histname = MyConfig.GetMCHistName("Nom",islice,flavour,jet+var).Data()

      histMC1 = histHelper1.AddMCHists(histname,ListOfPaths1)
      histMC2 = histHelper2.AddMCHists(histname,ListOfPaths2)
      
      if histMC1:
        histMC1.SetName((flavour).Data()+' '+tag1)
        histMC1.SetLineWidth(3);
        #Normalize to Unity
        histMC1.Scale(1./histMC1.Integral())
        if jet is 'nmj':
          histsNMJ1.append(histMC1)
        elif jet is 'mj':
          histsMJ1.append(histMC1)
      else:
        print("Could not find "+histname+" in all input1 files!")
      
      if histMC2:
        histMC2.SetName((flavour).Data()+' '+tag2)
        histMC2.SetLineWidth(3);
        #Normalize to Unity
        histMC2.Scale(1./histMC2.Integral())
        if jet is 'nmj':
          histsNMJ2.append(histMC2)
        elif jet is 'mj':
          histsMJ2.append(histMC2)
      else:
        print("Could not find "+histname+" in all input2 files!")

  i = 0
  for flavour in ListOfFlavourPairs: 
    
    #colors = [colors1[i], colors2[i]]
    
    canvMJ = ROOT.TCanvas("c_mujet"+var+flavour.Data(),"",800,800);
    AddHistogram(canvMJ,histsMJ1[i],'hist')
    AddHistogram(canvMJ,histsMJ2[i],'hist')
    FullFormatCanvasDefault(canvMJ,36,simulation=True, doLogy=True)
    #SetColors(canvMJ,colors)
    SetAxisLabels(canvMJ,'Muon-associated Track Jet '+MapOfAxisLabels[var],'Normalised to Unity')
    canvMJ.Print(outfilename+'_'+islice+'_'+var+'_'+flavour.Data()+'.pdf(')
    
    canvNMJ = ROOT.TCanvas("c_nonmujet"+var+flavour.Data(),"",800,800);
    AddHistogram(canvNMJ,histsNMJ1[i],'hist')
    AddHistogram(canvNMJ,histsNMJ2[i],'hist')
    FullFormatCanvasDefault(canvNMJ,36,simulation=True, doLogy=True)
    #SetColors(canvNMJ,colors)
    SetAxisLabels(canvNMJ,'Non-Muon-associated Track Jet '+MapOfAxisLabels[var],'Normalised to Unity')
    canvNMJ.Print(outfilename+'_'+islice+'_'+var+'_'+flavour.Data()+'.pdf)')
    
    i=i+1
    
print "done"
