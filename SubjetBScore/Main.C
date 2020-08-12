#include "State.h"

int main(int argc,char **argv)
{
 //usage output
 if(argc!=3)
 {
  cout<<"Warn:missing enough variables!!"<<endl;
  cout<<"ntuple_file_name MC/data"<<endl;
  return 0;
 }

 //read in filelist
 ifstream infile;
 infile.open(argv[1],ios::in);
 int count=0;//recording the number of rootfile

 cout<<"********* GbbAna begin! *********"<<endl;

 GbbAna run(argv[2]);
 TString inputRoot;

 while(infile>>inputRoot)
 {
  count++;
  run.Init(inputRoot,count);
  run.Loop();
  run.EndFile(count);
 }
 run.Finish(count);

 cout<<"********* GbbAna finish! *********"<<endl;
 return 0;
}
