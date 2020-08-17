#!/usr/bin/env python

import os,sys,stat

def job_list(varlist):
    print "================== create job list ====================="

    run_command=[]

    for dirs in os.listdir("./filelist_MC/"):
     dir_name="./filelist_MC/"+dirs
     for file in os.listdir(dir_name):
      inputname=os.path.join(dir_name,file)
      resultname=file.split(".txt")[0] 
      for var in varlist:
       run_command.append("run_gbbTupleAna " +inputname+" ./result_MC/"+var+"/"+dirs+"/"+resultname+".root FlavourTagging_"+var+" run_GbbTupleAna_Calib.cfg")

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
      
     if 15<run_cal:
      min=15
     else:
      min=run_cal
     for i in range(min):
      sh_file.write(run_command[cal_file])
      sh_file.write('\n')
      cal_file=cal_file+1

     run_cal=run_cal-min
     sh_file.close()

def get_list(varlist):
 BasePath='/store/bl2/atlas/ouxiaowei/gbb_MC'
 WritePath='/home/ouxiaowei/gbbCalibPackage/run_MC/filelist_MC/'
 dirlist=os.listdir(BasePath)

 for dir in dirlist:
  help_path=dir.split('.root')[0]
  list_path=os.path.join(WritePath,help_path)#for write list

  #create directory for looping MC result
  for var in varlist:
   ResultPath=os.path.join('/home/ouxiaowei/gbbCalibPackage/run_MC/result_MC/',var)
   result_path=os.path.join(ResultPath,help_path)
   if os.path.exists(result_path)==False:
    os.makedirs(result_path)

  if os.path.exists(list_path)==False:
   os.makedirs(list_path)

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
 syslist=["JET_EffectiveNP_R10_6restTerm__1up", "JET_EffectiveNP_R10_6restTerm__1down", "JET_EtaIntercalibration_Modelling__1up", "JET_EtaIntercalibration_Modelling__1down", "JET_EtaIntercalibration_NonClosure_2018data__1up", "JET_EtaIntercalibration_NonClosure_2018data__1down", "JET_EtaIntercalibration_R10_TotalStat__1up", "JET_EtaIntercalibration_R10_TotalStat__1down", "JET_EffectiveNP_R10_1__1up", "JET_EffectiveNP_R10_1__1down", "JET_EffectiveNP_R10_2__1up", "JET_EffectiveNP_R10_2__1down", "JET_EffectiveNP_R10_3__1up", "JET_EffectiveNP_R10_3__1down", "JET_EffectiveNP_R10_4__1up", "JET_EffectiveNP_R10_4__1down", "JET_EffectiveNP_R10_5__1up", "JET_EffectiveNP_R10_5__1down", "JET_Flavor_Composition__1up", "JET_Flavor_Composition__1down", "JET_Flavor_Response__1up", "JET_Flavor_Response__1down", "MUON_ID__1up", "MUON_ID__1down", "MUON_SAGITTA_RESBIAS__1up", "MUON_SAGITTA_RESBIAS__1down", "MUON_SCALE__1up", "MUON_SCALE__1down", "MUON_EFF_TTVA_SYS__1up", "MUON_EFF_TTVA_SYS__1down", "TRK_RES_D0_MEAS__1up", "TRK_RES_D0_MEAS__1down", "TRK_RES_Z0_MEAS__1up", "TRK_RES_Z0_MEAS__1down", "TRK_RES_D0_DEAD", "TRK_RES_Z0_DEAD", "TRK_BIAS_D0_WM", "TRK_BIAS_QOVERP_SAGITTA_WM", "TRK_BIAS_Z0_WM", "TRK_EFF_LOOSE_GLOBAL", "TRK_EFF_LOOSE_IBL", "TRK_EFF_LOOSE_PHYSMODEL", "TRK_EFF_LOOSE_PP0", "TRK_FAKE_RATE_LOOSE", "TRK_EFF_LOOSE_TIDE", "TRK_FAKE_RATE_LOOSE_TIDE"]

 if len(sys.argv)!=2:
  print "ERROR:the number of parameter is invalid!!!"
  print "usage:nom / nom+sys / sys"
  exit(1)

 do_sys=sys.argv[1]
 varlist=[]
 if do_sys=="nom":
  print "------ Nominal ------"
  varlist.append("Nominal")
 if do_sys=="nom+sys":
  print "------ Nominal+sys ------"
  varlist=syslist
  varlist.append("Nominal")
 if do_sys=="sys":
  print "------ sys ------"
  varlist=syslist

 print "------ varlist ------"
 print varlist
 print

 get_list(varlist)
 job_list(varlist)
