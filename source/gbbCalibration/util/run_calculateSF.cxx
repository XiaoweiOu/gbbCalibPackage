/*
 * Main for ScaleFactorCalculator
 *
 */
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include "scaleFactorCalculator/ScaleFactorCalculator.h"

bool createdir(TString);

int main(int argc, const char* argv[] ){

  std::cout << "******************************************" << std::endl;
  std::cout << "Initialising SF Calculation" << std::endl << std::endl;

  TString configname;
  TString outputdir;

  if(argc==1)
  {
    
    configname="./configs/testconfig.cfg";
    outputdir="./ScaleFactors_test"; 
    std::cout<<"No arguments, run on test configuration!"<<std::endl;
    
  }
  else if(argc==2)
  {
    
    configname=argv[1];
    outputdir="./ScaleFactors"; 

  }
  else if(argc==3)
  {
    
    configname=argv[1];
    outputdir=argv[2]; 

  }

  std::cout<<"Config File: " << configname << std::endl;
  std::cout<<"Output in: " << outputdir << std::endl << std::endl;

  
  // Making output dirrectory

  DIR* dir = opendir(outputdir);
  if (dir)
  {
        // Directory exists
        std::cout << "Output Dirrectory Exists. Will overwrite." << std::endl;
        closedir(dir);
  }
  else if (ENOENT == errno)
  {
        // Directory does not exist.

        TString outputsf = outputdir + "/SF/"; 
        TString outputctrl = outputdir + "/ctrl_plots/"; 
        TString outputctrlt = outputdir + "/ctrl_plots/toys/"; 
        
        bool error = false;
        error = createdir(outputdir);
        if (error) return 1;    
        error = createdir(outputsf);     
        if (error) return 1;    
        error = createdir(outputctrl);     
        if (error) return 1;    
        error = createdir(outputctrlt);     
        if (error) return 1;    
  }
  else
  {
        // opendir() failed for some other reason.
        std::cout << "Failed to check if dirrectory exists, exiting.." << std::endl;
        return 1;
  }
  
  std::cout << "Starting SF Calculation... " << std::endl << std::endl;
  std::cout << "------------------------------------------------------" << std::endl << std::endl;

  ScaleFactorCalculator sf_calculator(configname,outputdir);

}


bool createdir(TString outputdir)
{

  std::cout << "Creating Output Dirrectory " << outputdir;  
  const int dir_err = mkdir(outputdir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  if (-1 == dir_err)
  {
    printf("Error creating directory! exiting.. n");
    return true;
  }
  else return false;

}





