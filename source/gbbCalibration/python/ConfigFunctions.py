import string
import os
import re
from ROOT import TFile,TTree
import json


#-----------------------------------------------
def getKey(infile,key):
  # get TObject from ROOT file
  obj = infile.Get(key)
  if not obj:
    print('cannot find '+key+' in file '+infile.GetName())
    exit()
  return obj

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
  data_paths = string.split(os.environ['GBB_BUILD_DIR'], os.pathsep)
  found = False
  for path in data_paths:
    path = os.path.join(path,'gbbCalibration/data')
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
  filepath = GetDataFile(xsecfile)
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
  from ROOT import GlobalConfig
  return GlobalConfig('GlobalConfig.cfg')

#-----------------------------------------------
def GetLumi(year):
  # luminosity values from https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/GoodRunListsForAnalysisRun2 and may change slightly if data is reprocessed
  # values used should match the GRL and lumicalc file used to generate the ntuples
  if year == "2015" :
    return 3219.56 # in /pb
  elif year == "2016" :
    return 32988.1 # in /pb
  elif year == "2015+2016" :
    return 3219.56 + 32988.1 # in /pb
  elif year == "2017" :
    return 44307.4 # in /pb
  elif year == "2018" :
    return 58450.1 # in /pb
  else:
    print('Year {} not found'.format(year))
    return 0.

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
