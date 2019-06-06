#!/usr/bin/env python

"""
gbbCalibrationPackage fullrun script
Author: Youyuan Wu, May 2019
Discription: 
This script runs the whole gbbCalibration procedures 
from begining to end, with multi-process speed up. 

To run this script in default 
the user must provide/modify the following
1.provide input txt files to run_gbbtupleAna in folder
"InDir" which is "gbbCalibPackage/run/InputTxt" by default

2.change the run_GbbTupleAna_Calib.cfg setting of the following:
JetPtEtaReweightingFile <path to gbbCalibPackage>/run/workspace/reweightedFile/reweighted_nmu_out.root
JetPtEtaReweightingFileInclusive <path to gbbCalibPackage>/run/workspace/reweightedFile/reweighted_mu_out.root

3. change config_Calib_SF.cfg setting of the following
InputFile   <path to gbbCalibPackage>/run/workspace/crossChecked/crosschecked-out.root

4. change the MaxProcesses variable below
"""

import subprocess
# global variables:
# --- directory setting:

# User Optional: change to SourceDir of the gbb package location
import os
thisPath = os.path.realpath(__file__) # path of this script
# path of gbbCalibPackage 
# if fullrun.py is moved around, SoureceDir must be changed accordingly
SourceDir = os.path.abspath(thisPath+"/../../../../../")


# User Required: put input txt files in this folder
# or change to a folder containing input txt files
InDir= SourceDir + "/run/InputTxt"

# User Optional: change to dir that this script use to output stuff
# Currently is set to in /run folder in gbbCalibPackage
BaseDir= SourceDir + "/run"+"/workspace"

OutDir= BaseDir + "/gbbAnaTupleOutFiles" # do not change
LogDir=BaseDir+"/logs" # do not change
JsonDir=BaseDir+"/jsons" # do not change
CrossCheckDir=BaseDir + "/crossChecked" # do not change
SfoutputDir=BaseDir +"/sfOutput" # do not change
ReweightedFileDir = BaseDir+ "/reweightedFile" # do not change

# User required: change calib config to match this.
# JetPtEtaReweightingFile  JetPtEtaReweightingFileInclusive
reweighted_mu_out_path = ReweightedFileDir + "/reweighted_mu_out.root"  
reweighted_nmu_out_path = ReweightedFileDir + "/reweighted_nmu_out.root"



# User Optional: changer config file you want to use for each
ConfigDir=SourceDir + "/source/gbbCalibration/data/configs"
ReweightConfigPath=ConfigDir + "/run_GbbTupleAna_Reweight.cfg"
CalibConfigPath=ConfigDir +"/run_GbbTupleAna_Calib.cfg"
SFconfigPath=ConfigDir + "/config_Calib_SF.cfg"

#--- working directory setting end

# ------parrellel setting-----
# User Required: Change this to set the number 
# of run_gbbtupleana in parallel
# set it less than 5 when running locally
# set it to be the num of cpu when running in 
# job manager (slurm), (32 cpus recommended)
MaxProcesses = 34
Processes = []
# ------parellel setting end

# User : adjust systematics
Systematics =["Nominal"]


def main():

   print "parallel :" + str(MaxProcesses)
   # Step 0: setup folders
   # run it if you don't have basedir setup
   setUp()

   # Step 1: run gbb reweight
   for sys in Systematics:
      runGbb("reweight",sys)

   # Step 1.1: hadd data files reweight
   haddHistInDir(OutDir+"/reweight/data");

   # Step 1.2: produce reweight json
   getjson("reweight")
   
   # Step 1.3: run reweighthisto.py script
   reweighthisto()
   
   # Step 2: run gbb calib
   for sys in Systematics:
      runGbb("calib",sys)
   
   # Step 2.1 : hadd calib data files
   haddHistInDir(OutDir+"/calib/data");
   
   # Step 2.2 : produce calib json file
   getjson("calib")

   # Step 2.3 : run crosscheck.py, produce .root file
   runCrossCheck()
   
   # Step 3: run SF calculator
   runSFCalculator()



# set up folders
# you can do this manually in bash too.
def setUp():
            
   # create basedir
   createDir(BaseDir)
   createDir(OutDir)
   # inside OutDir
   # reweight dir
   reweightDir= OutDir + "/reweight"
   createDir(reweightDir);
   createDir(reweightDir + "/data")
   mudir = reweightDir + "/mu"
   nmudir = reweightDir + "/nmu"
   createDir(mudir)
   for sys in Systematics:
      createDir(mudir + "/" + sys)
      createDir(nmudir + "/" + sys)
   
   # calib dir
   calibDir= OutDir + "/calib"
   createDir(calibDir);
   createDir(calibDir + "/data")
   mudir = calibDir + "/mu"
   nmudir = calibDir + "/nmu"
   createDir(mudir)
   for sys in Systematics:
      createDir(mudir + "/" + sys)
      createDir(nmudir + "/" + sys)
   
   # createDir(InDir)  # optional
   # createDir(ConfigDir) # optional
   createDir(ReweightedFileDir)
   createDir(LogDir)
   createDir(JsonDir)
   createDir(CrossCheckDir)
   createDir(SfoutputDir)
   print "created base dir successfully"


# create dir, absolute path
def createDir(dirPath):
   import os
   #directory = os.path.dirname(dirPath)
   if os.path.exists(dirPath):
      #abort
      print ("dir already exists :" + dirPath)
      print ("please try again/ remove /change dirPath")
      exit(1)
             
   # create
   os.makedirs(dirPath)
   







# -- paralle methods
# start gbb with args
def StartNew(args):
   """ Start a new subprocess if there is work to do """
   global Processes

   # get logfile and write to it in subprocess
   outFilePath=args[1]
   outfilename= getFileNameFromPath(outFilePath);
   logPathName= getLogPath(outfilename) # xxx.root.log
   log = open(logPathName, 'w')

   #start subprocess and add it to queue
   from subprocess import Popen, PIPE
   proc = subprocess.Popen(["run_gbbTupleAna"]+args, stdout=log)
   print ("Started run_gbbTupleana on :" + outfilename)
   Processes.append(proc)

# run job if the process list has space
# arglist is a list of arguments; each for a gbb run
def CheckRunning(arglist):
   """ Check any running processes and start new ones if there are spare slots."""
   global Processes

   for p in range(len(Processes) - 1,-1,-1): # from last index to 0 
      # Check the processes in reverse order
      if Processes[p].poll() is not None: 
         # If the process hasn't finished will return None
         del Processes[p] 
         # Remove from list - this is why we needed reverse order
         

   # More to do and some spare slots
   while (len(Processes) < MaxProcesses):
      if(len(arglist) > 0):
         args= arglist.pop(0)
         StartNew(args)
      else:
         # no more jobs
         break



#----end parallel





def runSFCalculator():
   #run_calculateSF XXX/config_Calib_SF.cfg <output_Folder_name>
   
   SFoutputFolder=SfoutputDir+"/scalefactor-out"
   import shutil
   import os
   
   ## Try to remove tree if exists;
   ## if failed show an error using try...except on screen
   if(os.path.isdir(SFoutputFolder)):
      try:
         shutil.rmtree(SFoutputFolder)
      except OSError as e:
         print ("Error: %s - %s." % (e.filename, e.strerror))

   exe = SourceDir + "/source/gbbCalibration/python/makeCrossCheckInputsInclusiveLL_AllSys.py"
   
   
   # run the py script for reweight
   proc = subprocess.call(["run_calculateSF", SFconfigPath,
                           SFoutputFolder])
   #
   if (proc != 0):
      print "sf Calculator failed"
      exit(1)
   else:
      print "sf Calculator success"
   

def runGbb(runtype,sys):
   InputTxtFileList = getInputTxtFileNameList()

   ArgsList=[] # each element is argslist to run one tuple
   for txtFileName in InputTxtFileList:
      ArgsList.append(getArgumentList(txtFileName,runtype,sys))

   
   #print argumentlist
   CheckRunning(ArgsList);
   
   while (len(Processes) > 0): # Some thing still going on.
      import time
      time.sleep(3) # wait 3 sec then check again
      CheckRunning(ArgsList)
      
   print runtype + "/" + sys + "/" + "Done!"


# run crosscheck python
def runCrossCheck():
   
   #python makeCrossCheckInputsInclusiveLL_AllSys.py <root_output> <json_input>
   
   crosscheckOutFilePath=CrossCheckDir + "/crosschecked-out.root"
   import os
   if os.path.exists(crosscheckOutFilePath):
      os.remove(crosscheckOutFilePath)# clean target
   
   exe = SourceDir + "/source/gbbCalibration/python/makeCrossCheckInputsInclusiveLL_AllSys.py"
   
   crosscheckJsonPath=JsonDir + "/calibed.json"
   
   # run the py script for reweight
   proc = subprocess.call(["python", exe, crosscheckOutFilePath,
                            crosscheckJsonPath])
   # todo>>>>
   if (proc != 0):
      print "crosscheck failed"
      exit(1)
   else:
      print "crosscheck success"
     



def haddHistInDir(dir):
   import glob, os
   os.chdir(dir)
   FilePaths=[]
   for rootfile in glob.glob("*.root"):
      FilePaths.append(dir+ "/"+rootfile)
      
   #print FilePaths
   
   targetfilepath=dir+"/mergeall.root"
   if os.path.exists(targetfilepath):
      os.remove(targetfilepath)# clean target
   proc = subprocess.call(["hadd"] + [targetfilepath] + FilePaths)
   if (proc !=0):
      print "files in " + dir + "hadd failed"
      exit(1)
   else:
      print "hadd dir: " + dir + " success"
  

def getjson(runtype):
   # reweightjson
   # get mu/nmu file names as two lists
   FileBaseDir=""
   if(runtype == "reweight"):
      FileBaseDir= OutDir+"/" + runtype
   elif(runtype =="calib"):
      FileBaseDir= OutDir+"/"+ runtype

   sys="Nominal"
   muDir=FileBaseDir + "/mu/" + sys
   nmuDir=FileBaseDir + "/nmu/"+sys
   
   import glob, os
   os.chdir(muDir)
   mufiles=[]
   for rootfile in glob.glob("*.root"):
      mufiles.append("/"+ rootfile)

   os.chdir(nmuDir)
   nmufiles=[]
   for rootfile in glob.glob("*.root"):
      nmufiles.append("/"+rootfile)

   #print mufiles
   #print nmufiles

   # some varibles not used 
   jxsecFile = "xsections_r21.txt",
   jBasePath = FileBaseDir
   jData = "data/mergeall.root",
   jMuBase =  "/nmu/",
   jIncBase = "mu/",
   # jSys = [sys]

   import json
   jres= {
      "xsecFile" : "xsections_r21.txt",
      "BasePath" : jBasePath,
      "Data" : "/data/mergeall.root",
      "MuBase" : "/nmu/",
      "IncBase" : "/mu/",
      "Sys" : Systematics,
      "MuFilteredMC" : nmufiles,
      "InclusiveMC" : mufiles
   }


   # write to file as json
   reweightJsonPath=BaseDir+"/jsons/" + runtype +"ed.json"
   import os
   if os.path.exists(reweightJsonPath):
      os.remove(reweightJsonPath)# clean target
   with open(reweightJsonPath, 'w') as outfile:
      json.dump(jres, outfile)
   
   print runtype + "json generated."


   

def reweighthisto():

   #python makeReweightingHistos.py <root_output> <json_input> <filter_type>
   
   import os
   if os.path.exists(reweighted_mu_out_path):
      os.remove(reweighted_mu_out_path)# clean target
   if os.path.exists(reweighted_nmu_out_path):
      os.remove(reweighted_nmu_out_path)# clean target
   
   exe = SourceDir+ "/source/gbbCalibration/python/makeReweightHistos.py"
   reweightJsonPath=JsonDir+ "/reweighted.json"
   # run the py script for reweight
   proc1 = subprocess.call(["python", exe,reweighted_mu_out_path,
                            reweightJsonPath,"1"])
   proc2 = subprocess.call(["python", exe,reweighted_nmu_out_path,
                            reweightJsonPath,"0"])
   # todo>>>>
   if (proc1 !=0 or proc2!=0):
      print "reweight histo failed"
      exit(1)
   else:
      print "reweight histo success"
      










#----get rungbbtupleana arguments -----



# arguments to pass to one rungbbana run
def getArgumentList(txtFileName,runtype,sys):
   #name without extension
   import os
   tupleName= os.path.splitext(txtFileName)[0]
   inFilePathName = InDir + "/" + txtFileName
 
   outfilename=getRootOutFileName(tupleName,runtype)
   configPathName=getConfigPath(runtype)
   outFilePathName=getOutFilePath(outfilename,runtype,sys)
   #todo
   if(sys == "Nominal"):
      sys="FlavourTagging_Nominal"
   arguments=[inFilePathName,outFilePathName,sys,configPathName]
   return arguments

# get list of txt input from Indir
def getInputTxtFileNameList():
   txtList=[] 
   # for each txt file in in dir run gbb
   import glob, os
   os.chdir(InDir)
   for infile in glob.glob("*.txt"):
      txtList.append(infile)
   return txtList

# return the filename without extension
# inputTxtName is the name without path
def getTupleName(inputTxtName):
   import os
   return os.path.splitext(inputTxtName)[0]

def getFileNameFromPath(PathName):
   import os
   head, tail = os.path.split(PathName)
   return tail

# return the name of out .root file
def getRootOutFileName(tupleName,runtype):
   outfilename=""
   if(runtype == "reweight"):
      outfilename= tupleName +"-reweighted.root"
   elif (runtype == "calib"):
      outfilename= tupleName +"-calibed.root"
   else:
      print "not recognized runtype" + runtype
      exit(1)
   return outfilename


def getOutFilePath(outfilename, runtype, sys):
   #classifiy data/mu/nmu
   foldername=""
   if("data" in outfilename):
      foldername= "data"
   elif("mc" in outfilename):
      if("TeV.361" in outfilename):
         foldername="mu/"+sys
      elif("TeV.427" in outfilename):
         foldername="nmu" +"/"+sys

   outFilePath=""
   if(runtype == "reweight"):
      outFilePath=OutDir+"/"+runtype+"/" + foldername + "/" + outfilename
   elif (runtype == "calib"):
      outFilePath=OutDir+"/"+runtype + "/"+ foldername +"/" + outfilename
   else:
      print "not recognized runtype" + runtype
      exit(1)
   return outFilePath


def getConfigPath(runtype):
   configPathName=""
   if(runtype == "reweight"):
      configPathName=ReweightConfigPath
   elif (runtype == "calib"):
      configPathName= CalibConfigPath
   else:
      print "not recognized runtype" + runtype
      exit(1)
   return configPathName


def getLogPath(outfilename):
   logfilename= outfilename+".log"
   return LogDir + "/" + logfilename


# not used
def runGbbLocal(infilePathName, outfilePathName, 
                treename, configPathName):
  arguments=[infilePathName,outfilePathName,treename,configPathName]
  #print arguments
  subprocess.call(["run_gbbTupleAna"]+arguments);


    
  
if __name__== "__main__":
    main()
