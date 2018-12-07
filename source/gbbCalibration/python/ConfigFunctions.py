import string
import os
import re
from ROOT import TFile,TTree
import json

def GetPathsFromJSON(infile):
  dataPath = ''
  inclMCPaths = []
  mufiltMCPaths = []
  with open(infile, 'r') as f:
    tbl = json.load(f)
    if "BasePath" in tbl:
      if "Data" in tbl:
        dataPath = tbl["BasePath"]+tbl["Data"]
      if "MuFilteredMC" in tbl:
        muFiltMCPaths = [tbl["BasePath"]+path for path in tbl["MuFilteredMC"]]
      if "InclusiveMC" in tbl:
        inclMCPaths = [tbl["BasePath"]+path for path in tbl["InclusiveMC"]]
  return dataPath, muFiltMCPaths, inclMCPaths

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

def GetChannelWeights():
  filepath = GetDataFile("gbbCalibration/xsections.txt")
  if not filepath:
    print "Cross-sections file not found!"
    exit()
  tree = TTree("tree","")
  tree.ReadFile(filepath)
  output = {}
  for entry in tree:
    # Factor of 1e3 converts cross-section to pb
    output[entry.dataset_number] = entry.crossSection*1e3*entry.kFactor*entry.genFiltEff
  print "Loaded cross-section information"
  return output

def GetChannelNumber(filename):
  # Assume filename contains mc16_13TeV.(channel).(other info)
  channel = int(re.search(r'(?<=mc16_13TeV\.)[0-9]+',filename).group())
  if not channel:
    print "MC channel number for file",filename,"not found!"
    exit()
  return channel

def LoadGlobalConfig():
  filepath = GetDataFile("gbbCalibration/configs/GlobalConfig.cfg")
  if not filepath:
    print "Global config file not found!"
    exit()
  from ROOT import GlobalConfig
  return GlobalConfig(filepath)

class HistHelper:

  def __init__(self,):
    self.MapOfChannelWeights = GetChannelWeights()
    self.MapOfFiles = {}

  def __exit__(self, *exc):
    print "closing files"
    for openFile in self.MapOfFiles.itervalues():
      openFile.Close()

  def AddMCHists(self,histname, ListOfMCPaths):
    hist=None
    for path in ListOfMCPaths:
      file_curr = self.MapOfFiles.get(path)
      if not file_curr:
        print("Opening file",path)
        file_curr = TFile(path,"READ")
        self.MapOfFiles[path] = file_curr
      if file_curr.IsZombie():
        print("Cannot open file "+path)
        return None

      channel = GetChannelNumber(path)
      bookkeep_hist = file_curr.Get("Hist_BookKeeping") #Events in AOD is in Bin 3
      if self.MapOfChannelWeights[channel] == 0:
        print "missing channel: ",channel
        return None
      weight = self.MapOfChannelWeights[channel]/bookkeep_hist.GetBinContent(3)

      if not hist:
        hist = file_curr.Get(histname)
        if hist:
          hist.SetDirectory(0)
          hist.Scale(weight)
        else:
          print("Cannot find hist "+histname+" in file "+path)
      else :
        histTemp = file_curr.Get(histname)
        if histTemp:
          hist.Add(histTemp,weight)
        else:
          print("Cannot find hist "+histname+" in file "+path)
      #file_curr.Close()
    if not hist:
      print("Hist "+histname+" not found in any input MC files!")
    return hist
