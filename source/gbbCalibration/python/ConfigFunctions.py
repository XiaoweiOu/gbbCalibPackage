import string
import os
import re
from ROOT import TFile,TTree
import json


#-----------------------------------------------
def GetPathsFromJSON(infile):
  dataPath = ''
  inclMCPaths = []
  muFiltMCPaths = []
  with open(infile, 'r') as f:
    tbl = json.load(f)
    print "ConfigFunctions >> json file imported"
    if "BasePath" in tbl:
      if "Data" in tbl:
        dataPath = tbl["BasePath"]+tbl["Data"]
      for syspath in tbl["Sys"]:
        if "MuFilteredMC" in tbl:
          muFiltMCPaths = muFiltMCPaths+[tbl["BasePath"]+tbl["MuBase"]+syspath+path for path in tbl["MuFilteredMC"]]
        if "InclusiveMC" in tbl:
          inclMCPaths = inclMCPaths+[tbl["BasePath"]+tbl["IncBase"]+syspath+path for path in tbl["InclusiveMC"]]
    if "xsecFile" in tbl:
      xsecFile = tbl["xsecFile"]
      #for syspath in tbl["Sys"]:
      #  if "MuFilteredMC" in tbl:
      #    muFiltMCPaths = [tbl["BasePath"]+tbl["MuBase"]+syspath+path for path in tbl["MuFilteredMC"]]
      #  if "InclusiveMC" in tbl:
      #    inclMCPaths = [tbl["BasePath"]+tbl["IncBase"]+syspath+path for path in tbl["InclusiveMC"]]
  return dataPath, muFiltMCPaths, inclMCPaths, xsecFile


#-----------------------------------------------
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


#-----------------------------------------------
def GetChannelWeights(xsecfile):
  filepath = GetDataFile("gbbCalibration/"+xsecfile)
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


#-----------------------------------------------
def GetChannelNumber(filename):
  # Assume filename contains _13TeV.(channel).(other info)
  result = re.search(r'(?<=mc1(\d)_13TeV\.)[0-9]+',filename)
  if result:
    channel = int(result.group())
    if channel:
      return channel

  print "MC channel number for file",filename,"not found!"
  return -1


#-----------------------------------------------
def LoadGlobalConfig():
  filepath = GetDataFile("gbbCalibration/configs/GlobalConfig.cfg")
  if not filepath:
    print "Global config file not found!"
    exit()
  from ROOT import GlobalConfig
  return GlobalConfig(filepath)

#===============================================
class HistHelper:

  #-----------------------------------
  def __init__(self,xsecfile):
    self.xsecfile = xsecfile
    self.MapOfChannelWeights = GetChannelWeights(self.xsecfile)
    self.MapOfFiles = {}

  #-----------------------------------
  def __exit__(self, *exc):
    print "closing files"
    for openFile in self.MapOfFiles.itervalues():
      openFile.Close()

  #-----------------------------------
  def AddMCHists(self,histname, ListOfMCPaths):
    hist=None
    for path in ListOfMCPaths:
      file_curr = self.MapOfFiles.get(path)
      if not file_curr:
        print("Opening file",path)
        file_curr = TFile(path,"READ")
        self.MapOfFiles[path] = file_curr
      if file_curr.IsZombie():
        print("Cannot open MC file "+path)
        return None

      channel = GetChannelNumber(path)
      bookkeep_hist = file_curr.Get("Hist_BookKeeping") #Events in AOD is in Bin 3
      if self.MapOfChannelWeights[channel] == 0:
        print "missing channel: ",channel
        return None
      if not bookkeep_hist.GetBinContent(3):
        weight = 0;
        print "Warning: nevt in bookKeeping for tuple = 0, event weight set to 0 --- file: ",path
      else:
        weight = self.MapOfChannelWeights[channel]/bookkeep_hist.GetBinContent(3)


      if not hist:
        hist = file_curr.Get(histname)
        if hist:
          hist.SetDirectory(0)
          hist.Scale(weight)
        #else:
          #print("Cannot find hist "+histname+" in file "+path)
      else :
        histTemp = file_curr.Get(histname)
        if histTemp:
          hist.Add(histTemp,weight)
        #else:
          #print("Cannot find hist "+histname+" in file "+path)
      #file_curr.Close()
    if not hist:
      print("Hist "+histname+" not found in any input MC files!")
    return hist
