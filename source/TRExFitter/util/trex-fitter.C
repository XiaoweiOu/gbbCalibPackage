// Framework includes
#include "TRExFitter/Common.h"
#include "TRExFitter/ConfigParser.h"
#include "TRExFitter/ConfigReader.h"
#include "TRExFitter/ConfigReaderMulti.h"
#include "TRExFitter/MultiFit.h"
#include "TRExFitter/StatusLogbook.h"
#include "TRExFitter/TRExFit.h"

// RooStatsIncludes
#include "RooStats/RooStatsUtils.h"

// ATLAS stuff
#include "AtlasUtils/AtlasStyle.h"
#include "AtlasUtils/AtlasLabels.h"
#include "AtlasUtils/AtlasUtils.h"

// ROOT includes
#include "TH1.h"
#include "TSystem.h"

// c++ includes
#include <memory>
#include <string>
#include <fstream>

#define _STRINGIZE(x) #x
#define STRINGIZE(x) _STRINGIZE(x)

// -------------------------------------------------------
// -------------------------------------------------------

// trick to suppress the RooFit banner
// int doBanner(){ return 0; }

void FitExample(std::string opt="h",std::string configFile="config/myFit.config",std::string options=""){

    // pre-read the config just to extract the debug level
    std::string debugStr = ReadValueFromConfig(configFile,"DebugLevel");
    if(debugStr=="") WriteWarningStatus("", "Not able to pre-read the DebugLevel => keep the default (1).");
    else if(debugStr!="1") TRExFitter::DEBUGLEVEL = atoi(debugStr.c_str());

    // pre-read the logo option
    std::string logoStr = ReadValueFromConfig(configFile,"Logo");
    std::transform(logoStr.begin(), logoStr.end(), logoStr.begin(), ::toupper);
    if(logoStr=="TRUE"){
        std::string homeArea("$TREXFITTER_HOME");
#ifdef TREXFITTER_HOME
        homeArea = std::string(STRINGIZE(TREXFITTER_HOME));
#endif
        std::ifstream logoFile(gSystem->ExpandPathName((homeArea+"/logo.txt").c_str()));
        std::string str;
        std::string logo = "";
        while(getline(logoFile,str)){
            if(!logoFile.good()) break;
            logo+=str;
            logo+="\n";
        }
        std::cout << logo << std::endl;
    }

    if(TRExFitter::DEBUGLEVEL<=0)      gErrorIgnoreLevel = kError;
    else if(TRExFitter::DEBUGLEVEL<=1) gErrorIgnoreLevel = kWarning;

    // now can set ATLAS style
    if(TRExFitter::DEBUGLEVEL<=0) std::cout.setstate(std::ios_base::failbit);
    SetAtlasStyle();
    if(TRExFitter::DEBUGLEVEL<=0) std::cout.clear();

    RooStats::UseNLLOffset(true);

    // interpret opt
    bool readHistograms  = opt.find("h")!=std::string::npos;
    bool readNtuples     = opt.find("n")!=std::string::npos;
    bool rebinAndSmooth  = opt.find("b")!=std::string::npos;
    bool createWorkspace = opt.find("w")!=std::string::npos;
    bool doFit           = opt.find("f")!=std::string::npos;
    bool doRanking       = opt.find("r")!=std::string::npos;
    bool doLimit         = opt.find("l")!=std::string::npos;
    bool doSignificance  = opt.find("s")!=std::string::npos;
    bool drawPreFit      = opt.find("d")!=std::string::npos;
    bool drawPostFit     = opt.find("p")!=std::string::npos;
    bool drawSeparation  = opt.find("a")!=std::string::npos;
    bool groupedImpact   = opt.find("i")!=std::string::npos;
    bool doLHscan        = opt.find("x")!=std::string::npos;

    bool pruning = (createWorkspace || drawPreFit || drawPostFit); // ...

    if(!readNtuples && !rebinAndSmooth){
        TH1::AddDirectory(kFALSE); // FIXME: it would be nice to have a solution which works always
    }

    // multi-fit
    bool isMultiFit      = opt.find("m")!=std::string::npos;
    if(isMultiFit){
        std::unique_ptr<MultiFit> myMultiFit = std::make_unique<MultiFit>();
        ConfigReaderMulti confReaderMulti(myMultiFit.get());
        int sc = confReaderMulti.ReadFullConfig(configFile,opt,options) ;

        if (sc != 0){
            WriteErrorStatus("trex-fitter::FitExample", "Failed to read the config file for multifit.");
            exit(EXIT_FAILURE);
        }

        if(myMultiFit->fCombine){
            if(createWorkspace){
                std::cout << "Combining workspaces..." << std::endl;
                myMultiFit->SaveCombinedWS();
            }
            if(doFit){
                std::cout << "Fitting combining workspace..." << std::endl;
                myMultiFit->FitCombinedWS( myMultiFit->fFitType, myMultiFit->fDataName, false );
            }
            if(doLHscan){
                std::cout << "Doing likelihood scan for the combined workspace..." << std::endl;
                myMultiFit->FitCombinedWS( myMultiFit->fFitType, myMultiFit->fDataName, true );
            }
            if(doLimit){
                std::cout << "Getting combined limit..." << std::endl;
                myMultiFit->GetCombinedLimit( myMultiFit->fDataName );
            }
            if(doSignificance){
                std::cout << "Getting combined significance..." << std::endl;
                myMultiFit->GetCombinedSignificance( myMultiFit->fDataName );
            }
            if(doRanking){
                std::cout << "Getting combined ranking..." << std::endl;
                if(myMultiFit->fRankingOnly!="plot")  myMultiFit->ProduceNPRanking( myMultiFit->fRankingOnly );
                if(myMultiFit->fRankingOnly=="all" || myMultiFit->fRankingOnly=="plot")  myMultiFit->PlotNPRankingManager();
            }
            if(groupedImpact){
                std::cout << "Getting combined grouped systematic impact..." << std::endl;
                myMultiFit->fDoGroupedSystImpactTable = true;
                if(myMultiFit->fGroupedImpactCategory!="combine") myMultiFit->FitCombinedWS( myMultiFit->fFitType, myMultiFit->fDataName, false );
                else                                              myMultiFit->BuildGroupedImpactTable();
            }
        }
        //
        if(myMultiFit->fCompare){
            std::cout << "Comparing fits..." << std::endl;
            if(myMultiFit->fComparePulls){
                for(unsigned int i_cat=0;i_cat<myMultiFit->fNPCategories.size();i_cat++){
                    myMultiFit->ComparePulls(myMultiFit->fNPCategories[i_cat]);
                }
                myMultiFit->CompareNormFactors("");
            }
            if(myMultiFit->fPlotCombCorrMatrix) myMultiFit->PlotCombinedCorrelationMatrix();
            if(myMultiFit->fComparePOI)    myMultiFit->ComparePOI(myMultiFit->fPOI);
            if(myMultiFit->fCompareLimits) myMultiFit->CompareLimit();
        }
        //
        if(myMultiFit->fPlotSoverB){
            std::cout << "Comparing fits..." << std::endl;
            myMultiFit->PlotSummarySoverB();
        }
        //
        return;
    }

    // proceed if not multi-fit

    TRExFit *myFit = new TRExFit();

    // initialize config reader
    ConfigReader reader(myFit);

    // read the actual config
    int sc = reader.ReadFullConfig(configFile,opt,options);
    if(sc!=0){
        WriteErrorStatus("trex-fitter::FitExample", "Failed to read the config file.");
        exit(EXIT_FAILURE);
    }

    WriteInfoStatus("trex-fitter::FitExample", "Successfully read config file.");

    if (TRExFitter::DEBUGLEVEL < 2){
        gErrorIgnoreLevel = kError;
        RooMsgService::instance().setGlobalKillBelow(RooFit::WARNING);
    }

    // check compatibility between run option and config file
    if(readHistograms && myFit->fInputType!=TRExFit::HIST){
        WriteErrorStatus("trex-fitter::FitExample", "Option \"h\" asked but no HISTO InputType specified in the configuration file. Aborting.");
        return;
    }
    if(readNtuples && myFit->fInputType!=TRExFit::NTUP){
        WriteErrorStatus("trex-fitter::FitExample", "Option \"n\" asked but no NTUP InputType specified in the configuration file. Aborting.");
        return;
    }

    // -------------------------------------------------------
    myFit->PrintConfigSummary();

    if(readHistograms){
        std::cout << "Reading histograms..." << std::endl;
        myFit->CreateRootFiles();
        myFit->ReadHistograms();
        myFit->CorrectHistograms();
        myFit->MergeSystematics();
        myFit->CreateCustomAsimov();
        myFit->WriteHistos();
        if(TRExFitter::SYSTCONTROLPLOTS) myFit->DrawSystPlots();
        if(TRExFitter::SYSTDATAPLOT)     myFit->DrawSystPlotsSumSamples();
    }
    else if(readNtuples){
        std::cout << "Reading ntuples..." << std::endl;
        myFit->CreateRootFiles();
        myFit->ReadNtuples();
        myFit->CorrectHistograms();
        myFit->MergeSystematics();
        myFit->CreateCustomAsimov();
        myFit->WriteHistos();
        if(TRExFitter::SYSTCONTROLPLOTS) myFit->DrawSystPlots();
        if(TRExFitter::SYSTDATAPLOT)     myFit->DrawSystPlotsSumSamples();
    }
    else{
        if(drawPreFit || drawPostFit || createWorkspace || drawSeparation || rebinAndSmooth) myFit->ReadHistos();
    }

    if(rebinAndSmooth){
        std::cout << "Rebinning and smoothing..." << std::endl;
        bool udpate = myFit->fUpdate;
        myFit->fUpdate = true;
        myFit->CreateRootFiles();
        myFit->fUpdate = udpate;
        myFit->CorrectHistograms();
        myFit->MergeSystematics();
        myFit->CreateCustomAsimov();
        myFit->WriteHistos(false);
        if(TRExFitter::SYSTCONTROLPLOTS) myFit->DrawSystPlots();
        if(TRExFitter::SYSTDATAPLOT)     myFit->DrawSystPlotsSumSamples();
    }

    if(pruning){
        std::cout << "Applying systematics pruning..." << std::endl;
        myFit->SystPruning();
    }

    if(createWorkspace){
        std::cout << "Creating workspace..." << std::endl;
        myFit->SetLumiErr(0.);
        myFit->ToRooStat(true,true);
    }

    if(doFit){
        std::cout << "Fitting..." << std::endl;
        myFit->Fit(false);
        myFit->PlotFittedNP();
        myFit->PlotCorrelationMatrix();
    }
    if (doLHscan){
        std::cout << "Running LH scan only..." << std::endl;
        myFit->Fit(true);
    }
    if(doRanking){
        std::cout << "Doing ranking..." << std::endl;
        if(myFit->fRankingOnly!="plot")  myFit->ProduceNPRanking( myFit->fRankingOnly );
        if(myFit->fRankingOnly=="all" || myFit->fRankingOnly=="plot")  myFit->PlotNPRankingManager();
    }

    if(doLimit){
        std::cout << "Extracting limit..." << std::endl;
        myFit->GetLimit();
    }

    if(doSignificance){
        std::cout << "Extracting significance..." << std::endl;
        myFit->GetSignificance();
    }

    if(groupedImpact){
        std::cout << "Doing grouped systematics impact table..." << std::endl;
        myFit->fDoGroupedSystImpactTable = true;
        if(myFit->fGroupedImpactCategory!="combine") myFit->Fit(false);
        else                                         myFit->BuildGroupedImpactTable();
    }

    TRExPlot* prefit_plot = 0;
    TRExPlot* prefit_plot_valid = 0;
    if( drawPostFit && TRExFitter::PREFITONPOSTFIT ) {
        drawPreFit = true;
    }

    if(drawPreFit){
        std::cout << "Drawing pre-fit plots..." << std::endl;
        if(TRExFitter::OPTION["PrefitRatioMax"]==2){
            myFit->DrawAndSaveAll("prefit");
            if(myFit->fDoMergedPlot){
                if(myFit->fRegionGroups.size()==0)
                    myFit->DrawMergedPlot("prefit");
                for(unsigned int i_gr=0;i_gr<myFit->fRegionGroups.size();i_gr++){
                    myFit->DrawMergedPlot("prefit",myFit->fRegionGroups[i_gr]);
                }
            }
            if(myFit->fDoSummaryPlot){
                prefit_plot       = myFit->DrawSummary("log prefit");
                prefit_plot_valid = myFit->DrawSummary("log valid prefit");
            }
        }
        else{
            myFit->DrawAndSaveAll();
            if(myFit->fDoMergedPlot){
                if(myFit->fRegionGroups.size()==0)
                    myFit->DrawMergedPlot("");
                for(unsigned int i_gr=0;i_gr<myFit->fRegionGroups.size();i_gr++){
                    myFit->DrawMergedPlot("",myFit->fRegionGroups[i_gr]);
                }
            }
            if(myFit->fDoSummaryPlot){
                prefit_plot       = myFit->DrawSummary("log");
                prefit_plot_valid = myFit->DrawSummary("log valid");
            }
        }
        if(myFit->fDoTables){
            myFit->BuildYieldTable();
            for(unsigned int i_gr=0;i_gr<myFit->fRegionGroups.size();i_gr++){
                myFit->BuildYieldTable("",myFit->fRegionGroups[i_gr]);
            }
            myFit->PrintSystTables();
        }
        int nCols = 2;
        int nRows = 2;
        if(myFit->fNRegions>4){
            nCols = (int)sqrt(myFit->fNRegions);
            if(sqrt(myFit->fNRegions)>nCols) nCols++;
            nRows = (int)sqrt(myFit->fNRegions);
            if(nCols*nRows < myFit->fNRegions) nRows++;
        }
        if(myFit->fDoSignalRegionsPlot) myFit->DrawSignalRegionsPlot(nCols,nRows);
        if(myFit->fDoPieChartPlot)      myFit->DrawPieChartPlot("pre",nCols,nRows);
    }

    if(drawPostFit){
        std::cout << "Drawing post-fit plots..." << std::endl;
        myFit->DrawAndSaveAll("post");
        if(myFit->fDoMergedPlot){
            if(myFit->fRegionGroups.size()==0)
                myFit->DrawMergedPlot("post");
            for(unsigned int i_gr=0;i_gr<myFit->fRegionGroups.size();i_gr++){
                myFit->DrawMergedPlot("post",myFit->fRegionGroups[i_gr]);
            }
        }
        if(myFit->fDoSummaryPlot){
            myFit->DrawSummary("log post",      prefit_plot);
            myFit->DrawSummary("log post valid",prefit_plot_valid);
        }
        if(myFit->fDoTables){
            myFit->BuildYieldTable("post");
            for(unsigned int i_gr=0;i_gr<myFit->fRegionGroups.size();i_gr++){
                myFit->BuildYieldTable("post",myFit->fRegionGroups[i_gr]);
            }
            myFit->PrintSystTables("post");
        }
        int nCols = 2;
        int nRows = 2;
        if(myFit->fNRegions>4){
            nCols = (int)sqrt(myFit->fNRegions);
            if(sqrt(myFit->fNRegions)>nCols) nCols++;
            nRows = (int)sqrt(myFit->fNRegions);
            if(nCols*nRows < myFit->fNRegions) nRows++;
        }
        if(myFit->fDoPieChartPlot)      myFit->DrawPieChartPlot("post",nCols,nRows);
    }

    if(drawSeparation){
        std::cout << "Drawing separation plots..." << std::endl;
        myFit->DrawAndSaveSeparationPlots();
    }

    if(drawPreFit || drawPostFit || createWorkspace || drawSeparation || rebinAndSmooth) myFit->CloseInputFiles();
}

// -------------------------------------------------------
// -------------------------------------------------------
// main function
// -------------------------------------------------------
// -------------------------------------------------------

int main(int argc, char **argv){
    std::ifstream in;
    std::string version;
    std::string homeArea("$TREXFITTER_HOME");
#ifdef TREXFITTER_HOME
    homeArea = std::string(STRINGIZE(TREXFITTER_HOME));
#endif
    in.open(gSystem->ExpandPathName((homeArea+"/version.txt").c_str()));
    std::getline(in,version);
    in.close();
    std::cout << "\033[1m" << version << " -- Developed by Michele Pinamonti, Loic Valery, Alexander Held, Tomas Dado\033[0m" << std::endl;
    std::cout << "                    No rights reserved, feel free to use and modify it ;)" << std::endl;

    if (argc == 2){
        WriteErrorStatus("trex-fitter::main", "You provided 1 parameter, but only 0, 2 or 3 parameters are allowed. Please check this.");
        return 1;
    }

    if (argc > 4) {
        WriteWarningStatus("trex-fitter::main", "You provided " + std::to_string(argc-1) + " parameters, but only 0, 2 or 3 parameters are allowed.");
        WriteWarningStatus("trex-fitter::main", "Ignoring parameters after third parameter.");
    }

    std::string opt="h";
    std::string config="config/myFit.config";
    std::string options="";

    if(argc>1) opt     = argv[1];
    if(argc>2) config  = argv[2];
    if(argc>3) options = argv[3];

    // call the function
    FitExample(opt,config,options);

    return 0;
}
