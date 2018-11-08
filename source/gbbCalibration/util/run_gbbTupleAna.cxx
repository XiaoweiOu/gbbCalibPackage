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
       << " -c <config> (-l list | -f file)\n"
       << "       -c       <config> file\n"
       << "       -l       file containing list of inputs\n"
       << "       -f       input file\n"
       << "       -o       output file name\n"
       << "       -h       Print out this message"
       << endl;
}

/*---------------------------------------------------------------------------*/
int main(int argc,char *argv[]){
  int c;
  std::string config;
  std::string infile;
  std::string inlist;
  std::string outfile;

  while ((c = getopt(argc, argv, "hc:f:l:o:")) != -1) {
    switch (c) {
      case 'c':
        config = optarg;
        break;
      case 'f':
        infile = optarg;
        break;
      case 'l':
        inlist = optarg;
        break;
      case 'o':
        outfile = optarg;
        break;
      case 'h':
      default:
        Usage(basename(argv[0]));
        return 0;
    }
  }
  // Need a config file and either an input file or input list (not both)
  bool useList = !inlist.empty();
  if ( config.empty() || outfile.empty() || (!useList && infile.empty()) || (useList && !infile.empty()) ) {
    std::cerr << "[ERROR] (" << basename(argv[0]);
    std::cerr << ") missing arguments: -c <config>, -o <output name> and one of -l <list> or -f <file> required." << std::endl;
    Usage(basename(argv[0]));
    return 1;
  }

  std::vector<TString> infiles;
  if (useList) { // Pass list of input files to GbbTupleAna
    ifstream fs(inlist);
    if (!fs.is_open()) {
      std::cerr << "[ERROR] (" << basename(argv[0]);
      std::cerr << ") could not open list of input files: " << inlist.c_str() << std::endl;
      return 1;
    }
    // Assume 1 file per line, no extra formatting
    for (std::string line; std::getline(fs, line); ) infiles.push_back(line.c_str());

  } else { // Pass single input file to GbbTupleAna
    infiles.push_back(infile);
  }

  GbbTupleAna ana(infiles, outfile, config);
  ana.Loop();
  ana.Finalize();
  return 0;
}
