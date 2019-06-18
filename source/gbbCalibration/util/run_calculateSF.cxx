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

  TString inputname;
  TString configname;
  TString outputdir;

  if (argc < 3) {
    std::cout << "Too few arguments given. Run as:" << std::endl;
    std::cout << " run_calculateSF <InputFile> <ConfigFile> ";
    std::cout << "[OutputDir (default: ScaleFactors)]" << std::endl;
    return 1;

  } else if(argc==3) {
    inputname=argv[1];
    configname=argv[2];
    outputdir="./ScaleFactors";

  } else if(argc==4) {
    inputname=argv[1];
    configname=argv[2];
    outputdir=argv[3];
  }

  std::cout<<"Input File: " << inputname << std::endl;
  std::cout<<"Config File: " << configname << std::endl;
  std::cout<<"Output in: " << outputdir << std::endl << std::endl;

  // Making output dirrectory
  DIR* dir = opendir(outputdir);
  if (dir) {
    // Directory exists
    std::cout << "Output Directory Exists. Will overwrite." << std::endl;
    closedir(dir);

  } else if (ENOENT == errno) {
    // Directory does not exist.

    TString outputsf = outputdir + "/SF/";
    TString outputnf = outputdir + "/NF/";
    TString outputctrl = outputdir + "/ctrl_plots/";
    TString outputctrlt = outputdir + "/ctrl_plots/toys/";

    bool error = false;
    error = createdir(outputdir);
    if (error) return 1;
    error = createdir(outputsf);
    if (error) return 1;
    error = createdir(outputnf);
    if (error) return 1;
    error = createdir(outputctrl);
    if (error) return 1;
    error = createdir(outputctrlt);
    if (error) return 1;
  } else {
        // opendir() failed for some other reason.
        std::cout << "Failed to check if directory exists, exiting.." << std::endl;
        return 1;
  }

  std::cout << "Starting SF Calculation... " << std::endl << std::endl;
  std::cout << "------------------------------------------------------" << std::endl << std::endl;

  ScaleFactorCalculator sf_calculator(inputname,configname,outputdir);

}

bool createdir(TString outputdir) {

  std::cout << "Creating Output Dirrectory " << outputdir;
  const int dir_err = mkdir(outputdir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  if (-1 == dir_err)
  {
    printf("Error creating directory! exiting.. n");
    return true;
  }
  else return false;

}
