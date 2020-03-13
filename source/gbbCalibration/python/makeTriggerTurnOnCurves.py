import argparse

parser = argparse.ArgumentParser(description='Make reweight histograms.')
parser.add_argument('-o','--outfile', help="Name of output ROOT file")
parser.add_argument('-i','--infiles', help="JSON file with paths for data and MC files. See README for format")
args = parser.parse_args()

from ROOT import gROOT, TFile
import ConfigFunctions as config

pathData, ListOfMCPaths, ListOfInclusiveMCPaths, xsecFile = config.GetPathsFromJSON(args.infiles)
histHelper = config.HistHelper(xsecFile)

ListOfTriggerHists = [
  'trigjet_pt_passHLT_j150',
  'trigjet_pt_passHLT_j175',
  'trigjet_pt_passHLT_j200',
  'trigjet_pt_passHLT_j260',
  'trigjet_pt_passHLT_j300',
  'trigjet_pt_passHLT_j320',
  'trigjet_pt_passHLT_j360',
  'trigjet_pt_passHLT_j380',
  'trigjet_pt_passHLT_j400',
  'trigjet_pt_passHLT_j420',
  'trigjet_pt_passHLT_j260_a10_lcw_L1J75',
  'trigjet_pt_passHLT_j260_a10_lcw_subjes_L1J75',
  'trigjet_pt_passHLT_j260_a10t_lcw_jes_L1J75',
  'trigjet_pt_passHLT_j400_a10_lcw_L1J100',
  'trigjet_pt_passHLT_j420_a10_lcw_L1J100',
  'trigjet_pt_passHLT_j420_a10t_lcw_jes_35smcINF_L1J100',
  'trigjet_pt_passHLT_j420_a10t_lcw_jes_40smcINF_L1J100',
  'trigjet_pt_passHLT_j440_a10_lcw_subjes_L1J100',
  'trigjet_pt_passHLT_j440_a10t_lcw_jes_L1J100',
  'trigjet_pt_passHLT_j460_a10_lcw_subjes_L1J100',
  'trigjet_pt_passHLT_j460_a10t_lcw_jes_L1J100',
  'trigjet_pt_passHLT_2j330_a10t_lcw_jes_35smcINF_L1J100',
  'trigjet_pt_passHLT_2j330_a10t_lcw_jes_40smcINF_L1J100',
  'trigjet_pt_passHLT_mu20_iloose_L1MU15',
  'trigjet_pt_passHLT_mu24_ivarloose',
  'trigjet_pt_passHLT_mu24_ivarloose_L1MU15',
  'trigjet_pt_passHLT_mu26_ivarmedium',

  'mu_pt_passHLT_mu20_iloose_L1MU15',
  'mu_pt_passHLT_mu24_ivarloose',
  'mu_pt_passHLT_mu24_ivarloose_L1MU15',
  'mu_pt_passHLT_mu26_ivarmedium',

  'fatjet_pt_passHLT_j260_a10_lcw_L1J75',
  'fatjet_pt_passHLT_j260_a10_lcw_subjes_L1J75',
  'fatjet_pt_passHLT_j260_a10t_lcw_jes_L1J75',
  'fatjet_pt_passHLT_j400_a10_lcw_L1J100',
  'fatjet_pt_passHLT_j420_a10_lcw_L1J100',
  'fatjet_pt_passHLT_j440_a10_lcw_subjes_L1J100',
  'fatjet_pt_passHLT_j440_a10t_lcw_jes_L1J100',
  'fatjet_pt_passHLT_j460_a10_lcw_subjes_L1J100',
  'fatjet_pt_passHLT_j460_a10t_lcw_jes_L1J100',

  'fatjet_pt_passHLT_j420_a10t_lcw_jes_35smcINF_L1J100',
  'fatjet_pt_passHLT_j420_a10t_lcw_jes_40smcINF_L1J100',
  'fatjet_m_passHLT_j420_a10t_lcw_jes_35smcINF_L1J100',
  'fatjet_m_passHLT_j420_a10t_lcw_jes_40smcINF_L1J100',

  'fatjet_pt1_passHLT_2j330_a10t_lcw_jes_35smcINF_L1J100',
  'fatjet_pt1_passHLT_2j330_a10t_lcw_jes_40smcINF_L1J100',
  'fatjet_pt2_passHLT_2j330_a10t_lcw_jes_35smcINF_L1J100',
  'fatjet_pt2_passHLT_2j330_a10t_lcw_jes_40smcINF_L1J100',

  'fatjet_m1_passHLT_2j330_a10t_lcw_jes_35smcINF_L1J100',
  'fatjet_m1_passHLT_2j330_a10t_lcw_jes_40smcINF_L1J100',
  'fatjet_m2_passHLT_2j330_a10t_lcw_jes_35smcINF_L1J100',
  'fatjet_m2_passHLT_2j330_a10t_lcw_jes_40smcINF_L1J100']

#ListOfPrescales = [ 7.378e-5, 5.479e-4, 7.633e-4, 3.945e-3, 2.453e-2, 2.514e-1, 1.0]

#open output file
outfile = TFile(args.outfile, "RECREATE")

#loop over MC histograms
for hname in ListOfTriggerHists:
  if 'trigjet_pt' in hname:
    basename = 'trigjet_pt_noHLTreq'
  elif 'mu_pt' in hname:
    basename = 'mu_pt_noHLTreq'
  elif 'fatjet_pt' in hname:
    basename = 'fatjet_pt_noHLTreq'
    if 'smcINF' in hname: # trigger with mass cut
      basename = 'fatjet_pt_noHLTreq_m50'
      if 'pt1' in hname: # di-jet trigger
        basename = 'fatjet_pt1_noHLTreq_m50'
      elif 'pt2' in hname: # di-jet trigger
        basename = 'fatjet_pt2_noHLTreq_m50'
  elif 'fatjet_m' in hname:
    basename = 'fatjet_m_noHLTreq_pt450'
    if 'm1' in hname: # di-jet trigger
      basename = 'fatjet_m1_noHLTreq_pt350'
    elif 'm2' in hname: # di-jet trigger
      basename = 'fatjet_m2_noHLTreq_pt350'
  else:
    print('Unknown base histogram for '+hname)
    continue

  hist1 = histHelper.AddMCHists(hname, ListOfInclusiveMCPaths['Nominal/'])
  if not hist1:
    continue
  num = hist1.GetCumulative(False)
  hist2 = histHelper.AddMCHists(basename, ListOfInclusiveMCPaths['Nominal/'])
  if not hist2:
    exit()
  denom = hist2.GetCumulative(False)

  ratio = num.Clone('trigTurnOn_'+hname)
  ratio.Divide(denom)
  outfile.cd()
  ratio.Write()

  for i in range(1, ratio.GetNbinsX()-2):
    if ratio.GetBinContent(i) >= 0.99:
      print("{} fully efficient at {:.1f} GeV".format(hname,ratio.GetBinLowEdge(i)))
      break

outfile.Close()
