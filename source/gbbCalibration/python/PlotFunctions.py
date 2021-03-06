# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TAxisFunctions import *

##
## New plotting functions :
##
## FullFormatCanvasDefault(can,...)
## ConvertToDifferential()
## AddHistogram(can,hist)
## SetAxisLabels(can,xlabel,ylabel)
##
## GetLuminosityText(lumi)
## GetSqrtsText(sqrts)
## GetAtlasInternalText(status='Internal')
## DrawText(can,text,x1,y1,x2,y2,...)
## MakeLegend(can,x1,x2,y1,y2,...)
## FormatCanvasAxes(can,options...) - must be run AFTER the first histograms are added!
## SetupStyle()
## RatioCanvas(name,title,canw,canh,ratio_size_as_fraction)
## SetLeftMargin(can,margin)
## GetTopPad(can)
## GetBotPad(can)
## AddHistogramTop(can,hist)
## AddHistogramBot(can,hist)
## AddRatio(can,hist,ref_hist)
##
## # SetMarkerStyles() Coming soon!
## # SetFillStyles() Coming soon!
##

##
## This global list called "tobject_collector" collects TObjects (TH1, TGraph, TLegend, TLatex...)
## that are created in these functions. It is a way of preventing these objects from going out of
## scope (and thus being deleted) while requiring very little from the user.
##
## IMPORTANT NOTE! If you are using too much memory because of the proliferation of TObjects in this
## list, you can periodically delete them (after you have printed the canvas to pdf) by calling
## del tobject_collector[:]
## in your script. (tobject_collector is a global variable, so this single line is sufficient.)
##
global tobject_collector;
tobject_collector = []

##
## FullFormatCanvasDefault is a collection of functions for easy "1-step" plotting.
##
def FullFormatCanvasDefault(can,lumi=3.2,sqrts=13,additionaltext1='',additionaltext2='',additionaltext3='',additionaltext4='',additionaltext5='',preliminary=False,simulation=False,ignoreErrors=False, doLogx=False, doLogy=False) :
    FormatCanvasAxes(can,XTitleSize=32,YTitleSize=32,XLabelSize=30,YLabelSize=30)
    SetColors(can)

    stat = ''
    if simulation and not preliminary:
        stat = 'Simulation Internal'
    elif simulation:
        stat = 'Simulation Preliminary'
    elif not simulation and preliminary:
        stat = 'Preliminary'
    else:
        stat = 'Internal'

    text_lines = []
    text_lines += [GetAtlasInternalText(stat)]

    if sqrts and lumi : text_lines += [GetSqrtsText(sqrts)+', '+ GetLuminosityText(lumi)]
    elif sqrts : text_lines += [GetSqrtsText(sqrts)]
    elif lumi : text_lines += [GetLuminosityText(lumi)]

    if additionaltext1 : text_lines += [additionaltext1]
    if additionaltext2 : text_lines += [additionaltext2]
    if additionaltext3 : text_lines += [additionaltext3]
    if additionaltext4 : text_lines += [additionaltext4]
    if additionaltext5 : text_lines += [additionaltext5]


#    if can.GetPrimitive('pad_top') :
#        DrawText(can,text_lines,.35,.7,.65,.93,totalentries=4)
#        MakeLegend(can,0.35, 0.5, 0.65, 0.68,totalentries=3)
#    else :
#        DrawText(can,text_lines,.40,.73,.70,.94,totalentries=4)
#        MakeLegend(can,0.35, 0.5, 0.62, 0.7,totalentries=3)
    if can.GetPrimitive('pad_top') :
        if simulation:
            DrawText(can,text_lines,.10,.67,.65,.9,textsize=35,totalentries=4)
        else :
            DrawText(can,text_lines,.05,.67,.65,.9,textsize=35,totalentries=4)
#        MakeLegend(can,0.25, 0.47, 0.65, 0.65,totalentries=4)
        MakeLegend(can,0.55, 0.57, 0.92, 0.9,totalentries=8)
    else :
        if simulation:
            DrawText(can,text_lines,.1,.7,.50,.92,textsize=35,totalentries=4)
        else:
            DrawText(can,text_lines,.2,.7,.50,.92,textsize=35,totalentries=4)
    MakeLegend(can,0.65, 0.55, 0.92, 0.88,textsize=30,totalentries=4)

    if doLogx :
        can.SetLogx()
    if doLogy :
        can.SetLogy()
        #SetYaxisRanges(can,MinimumForLog(can),10);
    AutoFixAxes(can,ignoreErrors)
    return

##
## Convert a histogram to a differential histogram. Remember to change your y-axis label accordingly
## (e.g. events/GeV)
##
def ConvertToDifferential(hist) :
    hist.Scale(1,'width')
    return

##
## Add a TH1 or a TGraph to a canvas.
## If a RatioCanvas is specified as the canvas, then the histogram will be added to the top pad
## by default. (To specifically add a canvas to the bottom, do AddHistogram(GetBotPad(can),hist)
## This will *make a copy* of the histogram or graph, so that when you further manipulate the histogram
## in its canvas it will only affect the appearance in this one canvas. This way you
## can add the same histogram to multiple canvases and be able to manipulate the appearance of each
## instance separately.
##
def AddHistogram(can,hist,drawopt='pE1',inStack=False,markerSize=1) :
    if can.GetPrimitive('pad_top') :
        AddHistogram(can.GetPrimitive('pad_top'),hist,drawopt)
        return
    from ROOT import TH1,TGraph,THStack
    tmp = hist.Clone()
    is_graph = issubclass(type(hist),TGraph)
    is_stack = issubclass(type(hist),THStack)

    plot_exists = list(issubclass(type(a),TH1) for a in can.GetListOfPrimitives())
    plot_exists += list(issubclass(type(a),TGraph) for a in can.GetListOfPrimitives())
    plot_exists += list(issubclass(type(a),THStack) for a in can.GetListOfPrimitives())

    if (not is_graph) and (True in plot_exists) :
        drawopt += ' sames'
    if is_graph and not (True in plot_exists) :
        drawopt += ' a'
    if is_stack :
        tobject_collector.append(tmp)
        #drawopt += ' same'
        can.cd()
        tmp.Draw(drawopt)
        return

    tobject_collector.append(tmp)
    tmp.SetMarkerStyle(20)
    tmp.SetMarkerSize(markerSize)

    tmp.SetName('%s_%s'%(can.GetName(),hist.GetName()))
    if not inStack :
        can.cd()
        tmp.Draw(drawopt)
    return

##
## Set x- and y-axis labels. Do this *after* you have added your first histogram to the canvas.
##
def SetAxisLabels(can,xlabel,ylabel,yratiolabel='ratio') :
    if 'pad_top' in (a.GetName() for a in can.GetListOfPrimitives()) :
        SetAxisLabels(can.GetPrimitive('pad_bot'),xlabel,yratiolabel)
        SetAxisLabels(can.GetPrimitive('pad_top'),'',ylabel)
    for i in can.GetListOfPrimitives() :
        if hasattr(i,'GetXaxis') :
            i.GetXaxis().SetTitle(xlabel)
            i.GetYaxis().SetTitle(ylabel)
            break
    can.Modified()
    can.Update()
    return


def SetMarkerStyles(can,these_styles=[],these_sizes=[]) :
    if not these_styles :
        these_styles = [20 for i in xrange(30)]

    if not these_sizes :
        these_sizes = [1 for i in xrange(30)]

    the_primitives = can.GetListOfPrimitives()
    if can.GetPrimitive('pad_top') :
        the_primitives = can.GetPrimitive('pad_top').GetListOfPrimitives()

    style_count = 0
    for i in the_primitives :
        if hasattr(i,'SetMarkerColor') :
            i.SetMarkerStyle(these_styles[style_count])
            i.SetMarkerSize(these_sizes[style_count])
            #
            # Check if there is a bottom pad, with ratios...
            #
            if can.GetPrimitive('pad_bot') :
                original_name = i.GetName().replace('pad_top_','')
                j = can.GetPrimitive('pad_bot').GetPrimitive('pad_bot_%s_ratio'%(original_name))
                if j :
                    j.SetMarkerStyle(these_styles[style_count])
                    j.SetMarkerSize(these_sizes[style_count])
                    can.GetPrimitive('pad_bot').Modified()
                    can.GetPrimitive('pad_bot').Update()
            style_count += 1
        if style_count >= len(these_styles) :
            break

    can.Modified()
    can.Update()
    return






def SetFillStyles(marker_styles=[],marker_sizes=[]) :
    return

##
## Set colors. A default color list is provided, though you can provide your own list.
## Do this *after all of the histograms* have been added to the canvas.
## If you give this function a RatioCanvas, it will make histograms and their corresponding
## ratio histograms the same color.
##
def SetColors(can,these_colors=[]) :
    if not these_colors :
        from ROOT import kBlack,kRed,kBlue,kAzure,kGreen,kMagenta,kCyan,kOrange,kGray,kYellow
        these_colors = [kBlack+0,kRed+1,kAzure-2,kGreen+1,kMagenta+1,kCyan+1,kOrange+1
                        ,kBlack+2,kRed+3,kBlue+3,kGreen+3,kMagenta+3,kCyan+3,kOrange+3
                        ,kGray,kRed-7,kBlue-7,kGreen-7,kMagenta-7,kCyan-7,kOrange-7
                        ,kYellow+2,kRed-5,kBlue-5,kGreen-5,kMagenta-5,kCyan-5,kOrange-5
                        ,21,22,23,24,25,26,27,28,29,30
                        ,21,22,23,24,25,26,27,28,29,30
                        ,21,22,23,24,25,26,27,28,29,30
                        ]

    the_primitives = can.GetListOfPrimitives()
    if can.GetPrimitive('pad_top') :
        the_primitives = can.GetPrimitive('pad_top').GetListOfPrimitives()

    color_count = 0
    for i in the_primitives :
        if hasattr(i,'SetLineColor') and hasattr(i,'SetMarkerColor') :
            i.SetLineColor(these_colors[color_count])
            i.SetMarkerColor(these_colors[color_count])
            i.SetFillColor(0)
            #
            # Check if there is a bottom pad, with ratios...
            #
            if can.GetPrimitive('pad_bot') :
                original_name = i.GetName().replace('pad_top_','')
                j = can.GetPrimitive('pad_bot').GetPrimitive('pad_bot_%s_ratio'%(original_name))
                if j :
                    j.SetLineColor(these_colors[color_count])
                    j.SetMarkerColor(these_colors[color_count])
                    j.SetFillColor(0)
                    can.GetPrimitive('pad_bot').Modified()
                    can.GetPrimitive('pad_bot').Update()
            color_count += 1
        if color_count >= len(these_colors) :
            break

    can.Modified()
    can.Update()
    return

##
## Set line style when drawing with option hist. A default style list is provided, though you can provide your own list.
## Do this *after all of the histograms* have been added to the canvas.
## If you give this function a RatioCanvas, it will make histograms and their corresponding
## ratio histograms the same style.
##

def SetLineStyles(can,these_styles=[],width=2) :
    if not these_styles :
        these_styles = [1,9,7,3]

    the_primitives = can.GetListOfPrimitives()
    if can.GetPrimitive('pad_top') :
        the_primitives = can.GetPrimitive('pad_top').GetListOfPrimitives()

    style_count = 0
    for i in the_primitives :
        if hasattr(i,'SetLineStyle') and hasattr(i,'SetLineWidth') :
            i.SetLineStyle(these_styles[style_count])
            i.SetLineWidth(width)
            #
            # Check if there is a bottom pad, with ratios...
            #
            if can.GetPrimitive('pad_bot') :
                original_name = i.GetName().replace('pad_top_','')
                j = can.GetPrimitive('pad_bot').GetPrimitive('pad_bot_%s_ratio'%(original_name))
                if j :
                    j.SetLineStyle(these_styles[style_count])
                    j.SetLineWidth(width)
                    can.GetPrimitive('pad_bot').Modified()
                    can.GetPrimitive('pad_bot').Update()
            style_count += 1
        if style_count >= len(these_styles) :
            break

    can.Modified()
    can.Update()
    return




##
## Draw luminosity on your plot. Give it the lumi and sqrts.
## The x and y coordinates are the fractional distances, with the origin at the bottom left.
##
def GetLuminosityText(lumi=3.2) :
    #return '#lower[-0.2]{#scale[0.60]{#int}}Ldt = %1.1f fb^{-1}'%(lumi)
    return '%1.1f fb^{-1}'%(lumi)

def GetSqrtsText(sqrts=13) :
    return '#sqrt{s} = %d TeV'%(sqrts)

def GetAtlasInternalText(status='Internal') :
    return '#font[72]{ATLAS} #font[42]{%s}'%(status)

##
## Draw some additional text on your plot, in the form of a TLegend (easier to manage)
## The x and y coordinates are the fractional distances, with the origin at the bottom left.
## Specify multi-lines by specifing a list ['line1','line2','line3'] instead of a string 'single line'.
##
def DrawText(can,text='text',x1=.2,y1=.84,x2=.5,y2=.9,angle=0,align='',textsize=18,totalentries=0) :
    can.cd()
    if can.GetPrimitive('pad_top') :
        can.GetPrimitive('pad_top').cd()
    from ROOT import TLegend
    leg = TLegend(x1,y1,x2,y2)
    leg.SetName('text')
    tobject_collector.append(leg)
    leg.SetTextSize(textsize)
    leg.SetTextFont(43)
    leg.SetBorderSize(0)
    leg.SetFillStyle(0)
    if type(text) == type('') :
        text = [text]
    total = 0
    for i in text :
        leg.AddEntry(0,i,'')
        total += 1
    for i in range(100) :
        if totalentries == 0 : break
        if total >= totalentries : break
        leg.AddEntry(None,'','')
        total += 1
    leg.Draw()
    can.Modified()
    can.Update()
    return

##
## The MakeLegend function looks for any TH1 or TGraph you added to your canvas, and puts them
## in a legend in the order that you added them to a canvas.
## The entry label is taken from the title of the TH1 or TGraph. *Be sure to set the title
## of your TH1 or TGraph *before* you add it to the canvas.*
## The x and y coordinates are the fractional distances, with the origin at the bottom left.
##
def MakeLegend(can,x1=.8,y1=.8,x2=.9,y2=.9,textsize=18,ncolumns=1,totalentries=0,option='f') :
    from ROOT import TLegend,TH1,gStyle,TGraph,THStack
    if can.GetPrimitive('pad_top') :
        MakeLegend(can.GetPrimitive('pad_top'),x1,y1,x2,y2,textsize,ncolumns,totalentries,option)
        return
    is_th1 = list(issubclass(type(i),TH1) for i in can.GetListOfPrimitives())
    is_tgr = list(issubclass(type(i),TGraph) for i in can.GetListOfPrimitives())
    is_ths = list(issubclass(type(i),THStack) for i in can.GetListOfPrimitives())
    if True in is_ths : return # currently skip the legend for THStacks!
    if not (True in is_th1+is_tgr+is_ths) :
        print 'Error: trying to make legend from canvas with 0 plots. Will do nothing.'
        return
    #
    # if a previous version exists from this function, delete it
    #
    if can.GetPrimitive('legend') :
        can.GetPrimitive('legend').Delete()
    leg = TLegend(x1,y1,x2,y2)
    leg.SetName('legend')
    tobject_collector.append(leg)
    leg.SetTextFont(43)
    leg.SetTextSize(textsize)
    leg.SetTextFont(43)
    leg.SetBorderSize(0)
    leg.SetFillStyle(0)
    leg.SetNColumns(ncolumns)
    #
    # Add by TH1 GetTitle()
    #
    the_primitives = can.GetListOfPrimitives()
    if can.GetPrimitive('pad_top') :
        the_primitives = can.GetPrimitive('pad_top').GetListOfPrimitives()

    if type(option) == type('') :
        option = [option]*100

    total = 0
    for i in the_primitives :
        drawopt = i.GetDrawOption()
        if 'Data' in i.GetName() :
            drawopt = 'lep'
        #drawopt = 'lep'
        else :
            drawopt = 'l'
        if issubclass(type(i),TH1) or issubclass(type(i),TGraph) :
            leg.AddEntry(i,i.GetName().replace(can.GetName()+'_',''),drawopt) # plef
            #print i.GetName().replace(can.GetName()+'_','')
            total += 1
        elif issubclass(type(i),THStack) :
            for stackedHists in i.GetHists() :
                #print stackedHists.GetName()
                leg.AddEntry(stackedHists,stackedHists.GetName().replace(can.GetName()+'_',''),drawopt) # plef
                #print stackedHists.GetName().replace(can.GetName()+'_','')
                total += 1


    #
    # Add empty entries to ensure a standard layout
    #
    for i in range(100) :
        if totalentries == 0 : break
        if total >= totalentries : break
        leg.AddEntry(None,'','')
        total += 1

    # recipe for making roughly square boxes
    h = leg.GetY2()-leg.GetY1()
    w = leg.GetX2()-leg.GetX1()
    leg.SetMargin(leg.GetNColumns()*h/float(leg.GetNRows()*w))
    can.cd()
    if can.GetPrimitive('pad_top') :
        can.GetPrimitive('pad_top').cd()
    leg.Draw()
    can.Modified()
    can.Update()
    return

##
## Format the axes of your canvas or RatioCanvas, including axis labels, sizes, offsets.
## Call this *after* one or more histograms have been added to the canvas.
##
def FormatCanvasAxes(can
                     ,XTitleSize   = 22
                     ,XTitleOffset = 0.98
                     ,XTitleFont   = 43
                     ,XLabelSize   = 22
                     ,XLabelOffset = 0.002
                     ,XLabelFont   = 43

                     ,YTitleSize   = 22
                     ,YTitleOffset = 1.75
                     ,YTitleFont   = 43
                     ,YLabelSize   = 22
                     ,YLabelOffset = 0.006
                     ,YLabelFont   = 43
                     ,YNDiv = [10,5,0]

                     ,ZTitleSize   = 22
                     ,ZTitleOffset = 0.85
                     ,ZTitleFont   = 43
                     ,ZLabelSize   = 22
                     ,ZLabelOffset = 0.002
                     ,ZLabelFont   = 43
                     ) :

    if can.GetPrimitive('pad_top') :
        FormatCanvasAxes(can.GetPrimitive('pad_top'),XLabelOffset=0.1
                         ,XTitleSize=XTitleSize,XTitleOffset=XTitleOffset,XTitleFont=XTitleFont
                         ,XLabelSize=XLabelSize,XLabelFont=XLabelFont
                         ,YTitleSize=YTitleSize,YTitleOffset=YTitleOffset,YTitleFont=YTitleFont
                         ,YLabelSize=YLabelSize,YLabelOffset=YLabelOffset,YLabelFont=YLabelFont
                         ,YNDiv=YNDiv
                         ,ZTitleSize=ZTitleSize,ZTitleOffset=ZTitleOffset,ZTitleFont=ZTitleFont
                         ,ZLabelSize=ZLabelSize,ZLabelOffset=ZLabelOffset,ZLabelFont=ZLabelFont
                         )
    if can.GetPrimitive('pad_bot') :
        FormatCanvasAxes(can.GetPrimitive('pad_bot'),YLabelOffset=0.009
                         ,XTitleSize=XTitleSize,XTitleOffset=XTitleOffset,XTitleFont=XTitleFont
                         ,XLabelSize=XLabelSize,XLabelOffset=XLabelOffset,XLabelFont=XLabelFont
                         ,YTitleSize=YTitleSize,YTitleOffset=YTitleOffset,YTitleFont=YTitleFont
                         ,YLabelSize=YLabelSize,YLabelFont=YLabelFont
                         ,YNDiv = [5,5,0]
                         ,ZTitleSize=ZTitleSize,ZTitleOffset=ZTitleOffset,ZTitleFont=ZTitleFont
                         ,ZLabelSize=ZLabelSize,ZLabelOffset=ZLabelOffset,ZLabelFont=ZLabelFont
                         )

    for i in can.GetListOfPrimitives() :
        if not hasattr(i,'GetXaxis') :
            continue
        i.GetXaxis().SetTitleSize  (XTitleSize  )
        i.GetXaxis().SetTitleOffset(XTitleOffset/float(can.GetHNDC()))
        i.GetXaxis().SetTitleFont  (XTitleFont  )
        i.GetXaxis().SetLabelSize  (XLabelSize  )
        i.GetXaxis().SetLabelOffset(XLabelOffset/float(can.GetHNDC()))
        i.GetXaxis().SetLabelFont  (XLabelFont  )

        i.GetXaxis().SetTickLength(0.02/float(can.GetHNDC()))

        i.GetYaxis().SetTitleSize  (YTitleSize  )
        i.GetYaxis().SetTitleOffset(YTitleOffset)
        i.GetYaxis().SetTitleFont  (YTitleFont  )
        i.GetYaxis().SetLabelSize  (YLabelSize  )
        i.GetYaxis().SetLabelOffset(YLabelOffset)
        i.GetYaxis().SetLabelFont  (YLabelFont  )
        i.GetYaxis().SetNdivisions (YNDiv[0],YNDiv[1],YNDiv[2])

        if not hasattr(i,'GetZaxis') :
            continue
        i.GetZaxis().SetTitleSize  (ZTitleSize  )
        i.GetZaxis().SetTitleOffset(ZTitleOffset)
        i.GetZaxis().SetTitleFont  (ZTitleFont  )
        i.GetZaxis().SetLabelSize  (ZLabelSize  )
        i.GetZaxis().SetLabelOffset(ZLabelOffset)
        i.GetZaxis().SetLabelFont  (ZLabelFont  )

        break

    can.Modified()
    can.Update()
    return

##
## Setup general style.
##
def SetupStyle() :
    from ROOT import gROOT,TStyle,TColor
    mystyle = TStyle("mystyle","mystyle")
    mystyle.SetStatColor(0)
    mystyle.SetTitleColor(0)
    mystyle.SetCanvasColor(0)
    mystyle.SetPadColor(0)
    mystyle.SetPadBorderMode(0)
    mystyle.SetCanvasBorderMode(0)
    mystyle.SetFrameBorderMode(0)
    mystyle.SetOptStat(0)
    mystyle.SetStatH(0.3)
    mystyle.SetStatW(0.3)
    mystyle.SetTitleColor(1)
    mystyle.SetTitleFillColor(0)
    mystyle.SetTitleBorderSize(0)
    mystyle.SetHistLineWidth(2)
    #mystyle.SetLineWidth(2) # no
    mystyle.SetFrameFillColor(0)
    mystyle.SetOptTitle(0)
    mystyle.SetPaintTextFormat('4.1f ')
    mystyle.SetEndErrorSize(3)

    mystyle.SetPadTickX(1)
    mystyle.SetPadTickY(1)

    mystyle.SetPadTopMargin(0.05)
    mystyle.SetPadRightMargin(0.1)
    mystyle.SetPadBottomMargin(0.11)
    mystyle.SetPadLeftMargin(0.16)

    mystyle.SetMarkerStyle(20)
    mystyle.SetMarkerSize(1.2)

    #
    # NOTE that in ROOT rendering the font size is slightly smaller than in pdf viewers!
    # The effect is about 2 points (i.e. 18 vs 20 font)
    #
    # all axes
    mystyle.SetTitleSize  (22   ,'xyz')
    mystyle.SetTitleFont  (43   ,'xyz')
    mystyle.SetLabelSize  (22   ,'xyz')
    mystyle.SetLabelFont  (43   ,'xyz')

    # x axis
    mystyle.SetTitleXOffset(1.0)
    mystyle.SetLabelOffset(0.002,'x')

    # y axis
    mystyle.SetTitleOffset(1.75 ,'y')
    mystyle.SetLabelOffset(0.002,'y')

    # printing plots

    gROOT.SetStyle("mystyle")

    return

##
## Call this if you want a TCanvas especially prepared for ratio plots. It creates two
## sub-pads, "pad_top" and "pad_bot", and the rest of the functions in this file will
## specifically look for this type of configuration and act accordingly. See also the special
## functions GetTopPad(can) and GetBotPad(can) if you want to manipulate the sub-pads yourself.
## To add histograms to the top pad, do AddHistogram(can,hist) or AddHistogramTop(can,hist)
## To add histograms to the bot pad, do AddHistogramBot(can,hist).
## To add a histogram to the top pad, and its ratio with a reference histogram to the bottom pad,
## do AddRatio(can,hist,ref_hist,'B') (the B is for binomial errors).
##
def RatioCanvas(canvas_name,canvas_title,canw=500,canh=600,ratio_size_as_fraction=0.35,drawopt='pE1',doLogy=False) :
    from ROOT import TCanvas,TPad
    c = TCanvas(canvas_name,canvas_title,canw,canh)
    c.cd()
    top = TPad("pad_top", "This is the top pad",0.0,ratio_size_as_fraction,1.0,1.0)
    top.SetBottomMargin(0.02/float(top.GetHNDC()))
    top.SetTopMargin   (0.04/float(top.GetHNDC()))
    top.SetRightMargin (0.05 )
    top.SetLeftMargin  (0.16 )
    top.SetFillColor(0)
    if doLogy :
        top.SetLogy()
    top.Draw(drawopt)
    tobject_collector.append(top)

    c.cd()
    bot = TPad("pad_bot", "This is the bottom pad",0.0,0.0,1.0,ratio_size_as_fraction)
    bot.SetBottomMargin(0.09/float(bot.GetHNDC()))
    bot.SetTopMargin   (0.02/float(bot.GetHNDC()))
    bot.SetRightMargin (0.05)
    bot.SetLeftMargin  (0.16)
    bot.SetFillColor(0)
    bot.Draw(drawopt)
    tobject_collector.append(bot)

    return c

##
## Set the left margin - useful for the RatioCanvas in particular (since it will handle sub-pads)
##
def SetLeftMargin(can,margin) :
    if can.GetPrimitive('pad_top') :
        SetLeftMargin(can.GetPrimitive('pad_top'),margin)
    if can.GetPrimitive('pad_bot') :
        SetLeftMargin(can.GetPrimitive('pad_bot'),margin)
    can.SetLeftMargin(margin)
    can.Modified()
    can.Update()
    return

##
## Set the right margin - useful for the RatioCanvas in particular (since it will handle sub-pads)
##
def SetRightMargin(can,margin) :
    if can.GetPrimitive('pad_top') :
        SetRightMargin(can.GetPrimitive('pad_top'),margin)
    if can.GetPrimitive('pad_bot') :
        SetRightMargin(can.GetPrimitive('pad_bot'),margin)
    can.SetRightMargin(margin)
    can.Modified()
    can.Update()
    return

##
## Return a pointer to the top pad of a RatioCanvas
##
def GetTopPad(can) :
    return can.GetPrimitive('pad_top')

##
## Return a pointer to the bottom pad of a RatioCanvas
##
def GetBotPad(can) :
    return can.GetPrimitive('pad_bot')

##
## Add a TH1 or TGraph to the top pad of a RatioCanvas
##
def AddHistogramTop(can,hist,drawopt='pE1') :
    AddHistogram(can.GetPrimitive('pad_top'),hist,drawopt)
    return

##
## Add a TH1 or TGraph to the bottom pad of a RatioCanvas
##
def AddHistogramBot(can,hist,drawopt='pE1') :
    AddHistogram(can.GetPrimitive('pad_bot'),hist,drawopt)
    return

##
## Adds a histogram to the top pad of a RatioCanvas, and a ratio (dividing by some reference
## histogram ref_hist) to the bottom pad of the RatioCanvas. Specify the division type
## by the "divide" option ("B" for binomial, "" for uncorrelated histograms)
##
def AddRatio(can,hist,ref_hist,divide='',drawopt='pE1') :
    from ROOT import TH1
    TH1.SetDefaultSumw2(True)
    ratioplot = hist.Clone()
    ratioplot.SetName(hist.GetName()+'_ratio')
    ratioplot.Divide(hist,ref_hist,1.,1.,divide)
    #AddHistogram(can.GetPrimitive('pad_top'),hist,drawopt)
    AddHistogram(can.GetPrimitive('pad_bot'),ratioplot,drawopt)
    return

##
##
## Draw a line in the ratio plot
##
##

def DrawLineRatio(can,x1,y1,x2,y2) :
    from ROOT import TLine
    ratiopad=GetBotPad(can)
    line=TLine(x1,y1,x2,y2)
    line.SetLineStyle(2)
    ratiopad.cd()
    line.Draw("same")
    return
