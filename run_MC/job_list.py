#!/usr/bin/env python

import os,sys,stat

def job_list():
    print "================== create job list ====================="

    run_command=[]

    for dirs in os.listdir("./filelist_MC/"):
     dir_name="./filelist_MC/"+dirs
     for file in os.listdir(dir_name):
      inputname=os.path.join(dir_name,file)
      resultname=file.split(".txt")[0]
      run_command.append("run_gbbTupleAna " +inputname+" ./result_MC/Nominal/"+dirs+"/"+resultname+".root FlavourTagging_Nominal run_GbbTupleAna_Calib.cfg")

    #print run_command
    run_cal=len(run_command)
    cal_list=0
    cal_file=0

    os.chdir("/home/ouxiaowei/gbbCalibPackage/run_MC")

    while run_cal>0:
     print run_cal
     cal_list=cal_list+1
     name="job_file_"+str(cal_list)
     if os.path.exists(name)==False:#mkdir dir
      os.makedirs(name)
     
     #write .sub file
     sub_file=open(name+"/"+name+".sub","w")
     sub_file.write("Universe   = vanilla\nExecutable = /home/ouxiaowei/gbbCalibPackage/run_MC/"+name+"/runjob.sh\nArguments  =\nLog        = /home/ouxiaowei/gbbCalibPackage/run_MC/"+name+"/example.log\nOutput     = /home/ouxiaowei/gbbCalibPackage/run_MC/"+name+"/example.out\nError      = /home/ouxiaowei/gbbCalibPackage/run_MC/"+name+"/example.err\nQueue")
     sub_file.close()

     #write runjob.sh file
     sh_file=open(name+"/runjob.sh","w")
     os.chmod(name+"/runjob.sh",stat.S_IRWXU+stat.S_IRGRP+stat.S_IXGRP+stat.S_IROTH+stat.S_IXOTH)
     sh_file.write("#!/bin/bash\nexport PATH=/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBaseExternals/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/bin/:$PATH\ncd /home/ouxiaowei/gbbCalibPackage\nsource ./setup.sh\necho \"!!! source complete !!!\"\ncd /home/ouxiaowei/gbbCalibPackage/run_MC\n")
      
     if 3<run_cal:
      min=3
     else:
      min=run_cal
     for i in range(min):
      sh_file.write(run_command[cal_file])
      sh_file.write('\n')
      cal_file=cal_file+1

     run_cal=run_cal-min
     sh_file.close()

def get_list():
 BasePath='/store/bl2/atlas/ouxiaowei/gbb_MC'
 WritePath='/home/ouxiaowei/gbbCalibPackage/run_MC/filelist_MC/'
 ResultPath='/home/ouxiaowei/gbbCalibPackage/run_MC/result_MC/Nominal/'
 dirlist=os.listdir(BasePath)

 for dir in dirlist:
  help_path=dir.split('.root')[0]
  result_path=os.path.join(ResultPath,help_path)#for looping MC result
  list_path=os.path.join(WritePath,help_path)#for write list

  if os.path.exists(list_path)==False:
   os.makedirs(list_path)
  if os.path.exists(result_path)==False:
   os.makedirs(result_path)

  mid_path=os.path.join(BasePath,dir)#for path to MC sample
  file_name=os.listdir(mid_path)
  length=len(file_name)
  cal_file=0
  cal_list=0
  while length>0:
   print(length)
   cal_list=cal_list+1
   write_file=open(os.path.join(list_path,"filelist_"+str(cal_list)+".txt"),"w")
   if 10<length:
    min=10
   else:
    min=length
   for i in range(min):
    write_file.write(os.path.join(mid_path,file_name[cal_file]))
    write_file.write('\n')
    cal_file=cal_file+1

   length=length-min
   write_file.close()

if __name__=="__main__":
 get_list()
 job_list()
