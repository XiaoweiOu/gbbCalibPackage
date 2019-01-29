#include "histMaker/GbbTupleAna.h"
#include "TString.h"
#include <string>
#include <iostream>
#include <fstream>
#include <unistd.h>

/*---------------------------------------------------------------------------*/
void Usage(const string& name)
/*---------------------------------------------------------------------------*/
{
  cerr << "Usage: "
       << name
       << " <infile> <outfile> <treename> <configfile>\n"
       << " If infile ends in .txt it is assumed to be a list\n"
       << " of root files (one per line).\n"
       << endl;
}

/*---------------------------------------------------------------------------*/
int main(int argc,char *argv[]){

  if (argc < 5) {
    std::cerr << "[ERROR] (" << basename(argv[0]);
    std::cerr << ") missing arguments: <infile> <outfile> <treename> <configfile> required." << std::endl;
    Usage(basename(argv[0]));
    return 1;
  }
  TString infile = argv[1];
  TString outfile = argv[2];
  TString treename = argv[3];
  TString config = argv[4];
  bool useList = infile.EndsWith(".txt");

  std::vector<TString> infiles = std::vector<TString>();
  if (useList) { // Pass list of input files to GbbTupleAna
    ifstream fs(infile.Data());
    if (!fs.is_open()) {
      std::cerr << "[ERROR] (" << basename(argv[0]);
      std::cerr << ") could not open list of input files: " << infile.Data() << std::endl;
      return 1;
    }
    // Assume 1 file per line, no extra formatting
    for (std::string line; std::getline(fs, line); ) infiles.push_back(line.c_str());

  } else { // Pass single input file to GbbTupleAna
    infiles.push_back(infile);
  }

  GbbTupleAna ana(infiles, outfile, treename, config);
  ana.Loop();
  ana.Finalize();
  return 0;
}
