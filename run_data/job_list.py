#!/usr/bin/env python

import os,sys,stat

def job_list():
    print "================== create job list ====================="

    run_command=[]

    for file in os.listdir("./filelist_data/"):
     inputname="./filelist_data/"+file
     resultname=file.split(".txt")[0]
     run_command.append("run_gbbTupleAna " +inputname+" ./result_data/"+resultname+".root FlavourTagging_Nominal run_GbbTupleAna_Calib.cfg")

    #print run_command
    run_cal=len(run_command)
    cal_list=0
    cal_file=0

    os.chdir("/home/ouxiaowei/gbbCalibPackage/run_data")

    while run_cal>0:
     print run_cal
     cal_list=cal_list+1
     name="job_file_"+str(cal_list)
     if os.path.exists(name)==False:#mkdir dir
      os.makedirs(name)
     
     #write .sub file
     sub_file=open(name+"/"+name+".sub","w")
     sub_file.write("Universe   = vanilla\nExecutable = /home/ouxiaowei/gbbCalibPackage/run_data/"+name+"/runjob.sh\nArguments  =\nLog        = /home/ouxiaowei/gbbCalibPackage/run_data/"+name+"/example.log\nOutput     = /home/ouxiaowei/gbbCalibPackage/run_data/"+name+"/example.out\nError      = /home/ouxiaowei/gbbCalibPackage/run_data/"+name+"/example.err\nQueue")
     sub_file.close()

     #write runjob.sh file
     sh_file=open(name+"/runjob.sh","w")
     os.chmod(name+"/runjob.sh",stat.S_IRWXU+stat.S_IRGRP+stat.S_IXGRP+stat.S_IROTH+stat.S_IXOTH)
     sh_file.write("#!/bin/bash\nexport PATH=/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBaseExternals/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/bin/:$PATH\ncd /home/ouxiaowei/gbbCalibPackage\nsource ./setup.sh\necho \"!!! source complete !!!\"\ncd /home/ouxiaowei/gbbCalibPackage/run_data\n")
      
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

if __name__=="__main__":
 job_list()
