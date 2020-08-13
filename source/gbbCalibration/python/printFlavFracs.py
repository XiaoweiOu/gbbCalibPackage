import ROOT
import ConfigFunctions as config
import argparse

parser = argparse.ArgumentParser(description='Print table of pre-fit flavour fractions')
parser.add_argument('infile', help="input file")
parser.add_argument('--var', default="mjmeanSd0", help="variable to use [default: mjmeanSd0]")
parser.add_argument('--bin', default="Incl", help="pT bin to check [default: Incl]")
parser.add_argument('--sys', default="Nom", help="systematic to check [default: Nom]")
parser.add_argument('--nomerge', action="store_true", help="flag to check unmerged flavours")
args = parser.parse_args()

ROOT.gROOT.SetBatch(True)
from ROOT import TFile

infile = TFile(args.infile,"READ")
if infile.IsZombie():
  print("Cannot open file "+path)
  exit()

MyConfig = config.LoadGlobalConfig()

FatJetFlavours = MyConfig.GetFlavourGroups()
if args.nomerge:
  FatJetFlavours = MyConfig.GetFatJetFlavours()
# All the normalization should have been done already
# just loop over the flavours and check the ratios
FlavFracs = {}
FlavFracsWgt = {}
total = 0
totalWgt = 0
for flav in FatJetFlavours:
  histname = MyConfig.GetMCHistName(args.sys,args.bin,flav,args.var).Data()

  hist = infile.Get(histname)
  if not hist:
    print("Hist "+histname+" not found!")
    exit()

  FlavFracs[flav.Data()] = hist.GetEntries()
  total += hist.GetEntries()
  FlavFracsWgt[flav.Data()] = hist.GetSumOfWeights()
  totalWgt += hist.GetSumOfWeights()

histname = MyConfig.GetDataHistName(args.bin,args.var).Data()
hist = infile.Get(histname)
if not hist:
  print("Hist "+histname+" not found!")
  exit()
data = hist.GetEntries()

print('pT bin = {}'.format(args.bin))
print('syst = {}'.format(args.sys))
print('')
print('----------------')
print('{}'.format('Pre-fit Fracs'))
print('-------------')
for flav in sorted(FlavFracsWgt.keys(), key=lambda x: FlavFracsWgt[x], reverse=True):
  print('{:2} | {:>8.3f}'.format(flav,100*FlavFracsWgt[flav]/totalWgt))
print('----------------')
print('')
print('Data/MC ratio = {}'.format(data/totalWgt))
print('----------------')
print('{}'.format('Pre-fit NEntries'))
print('----------------')
for flav in sorted(FlavFracs.keys(), key=lambda x: FlavFracs[x], reverse=True):
  print('{:2}    | {:>8.2f}'.format(flav,FlavFracs[flav]))
print('-------------')
infile.Close()
