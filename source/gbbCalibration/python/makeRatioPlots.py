import ROOT
from PlotFunctions import *
from TAxisFunctions import *
import ConfigFunctions as config
import argparse

ROOT.gROOT.SetBatch(True)
from ROOT import TCanvas,TPad,TH2,TColor

#ROOT.gStyle.SetOptStat(0)
SetupStyle()

parser = argparse.ArgumentParser(description='Make ratio plots.')
parser.add_argument('outfile', help="Prefix of output files")
parser.add_argument('infiles', help="JSON file with paths for data and MC files. See README for format")
args = parser.parse_args()

outfilename = args.outfile
pathData, ListOfMCPaths, ListOfInclusiveMCPaths, xsecFile = config.GetPathsFromJSON(args.infiles)

Lumi = 32988.1 + 3219.56 #in pb^-1
if 'data17' in pathData:
  Lumi = 44307.4 #in pb^-1

MyConfig = config.LoadGlobalConfig()
histHelper = config.HistHelper(xsecFile)

ListOfSystematics = [ ROOT.TString("Nom") ] #MyConfig.GetSystematics() 
ListOfFlavourPairs = MyConfig.GetFlavourPairs()
ListOfInclusiveFlavourPairs = [ 'LL' ]

#make list of histograms
ListOfVariables = [ 'maxSd0',
                    'maxSd0_dR',
                    'subSd0',
                    'thirdSd0',
                    'meanSd0_sd0',
                    
                    'maxSd0_pt',
                    'subSd0_pt',
                    'thirdSd0_pt',
                    'meanSd0',
                    
                    'maxd0',
                    'subd0',
                    'thirdd0',
                    'meand0_sd0',
                    
                    'maxd0_pt',
                    'subd0_pt',
                    'thirdd0_pt',
                    'meand0_pt',
                    
                    'maxd0err',
                    'subd0err',
                    'thirdd0err',
                    
                    'maxd0err_pt',
                    'subd0err_pt',
                    'thirdd0err_pt',
                  ]
#MapOfAxisLabels = { 'maxSd0':     'leading s_{d0} (|s_{d0}| sorting)',
#                    'maxSd0_dR',
#                    'subSd0':     'sub-leading s_{d0} (|s_{d0}| sorting)',
#                    'thirdSd0':   'third s_{d0} (|s_{d0}| sorting)',
#                    'meanSd0_sd0':'#LT s_{d0} #GT (|s_{d0}| sorting)',
#                    
#                    'maxSd0_pt':  'leading s_{d0} (p_{T} sorting)',
#                    'subSd0_pt':  'sub-leading s_{d0} (p_{T} sorting)',
#                    'thirdSd0_pt':'third s_{d0} (p_{T} sorting)',
#                    'meanSd0':    '#LT s_{d0} #GT (p_{T} sorting)',
#                    
#                    'maxd0':     'leading d_{0} (|s_{d0}| sorting)',
#                    'subd0':     'sub-leading d_{0} (|s_{d0}| sorting)',
#                    'thirdd0':   'third d_{0} (|s_{d0}| sorting)',
#                    'meand0_sd0':'#LT d_{0} #GT (|s_{d0}| sorting)',
#                    
#                    'maxd0_pt':  'leading d_{0} (p_{T} sorting)',
#                    'subd0_pt':  'sub-leading d_{0} (p_{T} sorting)',
#                    'thirdd0_pt':'third d_{0} (p_{T} sorting)',
#                    'meand0_pt': '#LT d_{0} #GT (p_{T} sorting)',
#                    
#                    'maxd0err',
#                    'subd0err',
#                    'thirdd0err',
#                    
#                    'maxd0err_pt',
#                    'subd0err_pt',
#                    'thirdd0err_pt',
#                  }

colors = [ROOT.kBlue, ROOT.kBlack]
#SetColors(canvas,colors)

canv = RatioCanvas('c',"",800,800,0.3,'pE1',doLogy=True)
canv.Print(outfilename+'.pdf[')

outfile=ROOT.TFile("test.root","RECREATE")

#loop over MC histograms
for var in ListOfVariables :
  for jet in [ 'mj', 'nmj' ]:
    histname = MyConfig.GetMCHistName("Nom","Incl","Data",jet+var).Data()

    file_curr = ROOT.TFile(pathData,"READ")
    if not file_curr:
      print("Cannot open file "+path)
      exit()
    histData = file_curr.Get(histname)
    if histData:
      histData.SetDirectory(0)
      #histData.Scale(1/histData.Integral())
      outfile.cd()
      histData.Write()
      if 'data17' in pathData:
        histData.SetName("data17")
      else:
        histData.SetName("data15+16")
    else:
      print("Cannot find hist "+histname+" in file "+pathData)
      continue

    histMC = None
    for flavour in ListOfFlavourPairs:
      ListOfPaths = ListOfMCPaths
      if flavour in ListOfInclusiveFlavourPairs:
        ListOfPaths = ListOfInclusiveMCPaths

      histname = MyConfig.GetMCHistName("Nom","Incl",flavour,jet+var).Data()
      histTemp = histHelper.AddMCHists(histname,ListOfPaths)
      if histTemp:
        histTemp.Scale(Lumi)
        if not histMC:
          histMC = histTemp
        else:
          histMC.Add(histTemp)
        outfile.cd()
        histTemp.Write()
      else:
        print("Could not find "+histname+" in all input files!")
        continue

    if histMC:
      histMC.SetLineWidth(3);
      #histMC.SetAxisRange(-40,80,'X')
      #histMC.SetAxisRange(0,2500,'X')
      histMC.SetName("Pythia8 MC")
    else:
      print("Cannot find hist "+histname+" in MC files")
      continue

    #canv.SetLogy()
    histData.Scale(1/histData.Integral())
    histMC.Scale(1/histMC.Integral())
    AddHistogram(canv,histMC,'hist')
    AddHistogram(canv,histData,'e')
    AddRatio(canv,histData,histMC)
    #SetAxisLabels(canv,histData.GetXaxis().GetTitle(),'Events')
    SetAxisLabels(canv,histData.GetXaxis().GetTitle(),'Arbitrary Units')
    #SetAxisLabels(canv,var,'A.U.')
    FullFormatCanvasDefault(canv,lumi='')
    SetColors(canv,colors)
    #SetYaxisRanges(canv,10e-4,1)
    SetYaxisRangesRatio(canv,0.5,1.5)
    canv.Print(outfilename+'.pdf')
    #if jet == 'mj': 
    #  canv.Print(outfilename+'_'+var+'.pdf(')
    #elif jet == 'nmj':
    #  canv.Print(outfilename+'_'+var+'.pdf)')
    #else:
    #  print("Unknown jet "+jet)
    #  exit()
    canv.Clear('D')
canv.Print(outfilename+'.pdf]')
