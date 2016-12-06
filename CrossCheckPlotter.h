/*
 * Plotter.h
 *
 *  Created on: April 12, 2016
 *      Author: rjacobs
 */

#ifndef CROSSCHECKPLOTTER_H_
#define CROSSCHECKPLOTTER_H_

#include "Plotter.h"
#include "TString.h"
#include "SampleInfo.h"

class CrossCheckPlotter : public Plotter {

public:
	CrossCheckPlotter();
	CrossCheckPlotter(TString in_dir, SampleInfo* info,TString name="");
	virtual ~CrossCheckPlotter();
	
	void makeCrossCheckFitTemplates(std::vector<TString>& hist_names_even, std::vector<TString>& hist_names_odd,TFile* outfile, TString sys_ext="Nom", bool doRescale=false);
	void makeTriggerTurnOnPlot(TString hist_name_num, TString hist_name_denom);
        //void printTemplateCorrelations(std::vector<TString>& hist_names );
	void makeBBEffPlot(TString type); //type = "muojet", "nonmuojet"
	void makeBinBBEffPlot(TString bin); //bin= "bin1" - "bin4"                                                                                                                                                                    
	void makeTemplateEffPlot(TString type); //type = "muojet", "nonmuojet"	
	MultiHistogramStack readInMultiHistStack_unweighted(std::vector<TString>& mc_hist_list, std::vector<TString>& data_hist_list);
	void makeMCStatsTestFile(std::vector<TString>&, std::vector<TString>&, TFile*);

	void makePileupComparisonPlot(std::vector<TString>& mc_hist_list);


};

#endif /* CROSSCHECKPLOTTER_H_ */
