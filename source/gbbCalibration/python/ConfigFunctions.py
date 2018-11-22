import sys
import string
import os
from ROOT import TTree

def GetDataFile(name):
  if os.path.exists(name):
    print "found",name
    return name
  data_paths = string.split(os.environ['DATAPATH'], os.pathsep)
  found = False
  for path in data_paths:
    if os.path.exists(os.path.join(path,name)):
      found = True
      break
  if found:
    print "found",name,"in",path
    return os.path.join(path,name)
  else:
    print name,"not found"
    return ""

def MakeCrossSectionTable():
  filepath = GetDataFile("gbbCalibration/xsections.txt")
  if not filepath:
    print "Cross-sections file not found!"
    exit()
  tree = TTree("tree","")
  tree.ReadFile(filepath)
  output = {}
  for entry in tree:
    output[entry.dataset_number] = {'xSec': entry.crossSection, 'kFac': entry.kFactor, 'fEff': entry.genFiltEff}
  print "Loaded cross-section information"
  return output

def LoadGlobalConfig():
  filepath = GetDataFile("gbbCalibration/configs/binning.cfg")
  if not filepath:
    print "Global config file not found!"
    exit()
  from ROOT import BinConfig
  return BinConfig(filepath)
