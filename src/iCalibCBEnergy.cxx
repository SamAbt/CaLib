/*************************************************************************
 * Author: Irakli Keshelashvili, Dominik Werthmueller
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// iCalibCBEnergy                                                       //
//                                                                      //
// Calibration module for the CB energy.                                //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#include "iCalibCBEnergy.hh"

ClassImp(iCalibCBEnergy)


//______________________________________________________________________________
iCalibCBEnergy::iCalibCBEnergy(Int_t set)
    : iCalib("CB.Energy", "CB energy calibration",
             ECALIB_CB_E1, set, iConfig::kMaxCB)
{
    // Constructor.
    
    // init members
    fPi0IMOld = new Double_t[fNelem];
    fPi0IMNew = new Double_t[fNelem];
    fLine = new TLine();

    // get histogram name
    if (!iConfig::GetRC()->GetConfig("CB.Energy.Histo.Name"))
    {
        Error("iCalibCBEnergy", "Histogram name was not found in configuration!");
        return;
    }
    else fHistoName = *iConfig::GetRC()->GetConfig("CB.Energy.Histo.Name");
    
    // read old parameters
    iMySQLManager m;
    m.ReadParameters(fSet, fData, fOldVal, fNelem);
    //m.ReadParameters(fSet, ECALIB_CB_PI0IM, fPi0IMOld, fNelem);

    // sum up all files contained in this runset
    iFileManager f(fSet, fData);
    
    // get the main calibration histogram
    fMainHisto = f.GetHistogram(fHistoName.Data());
    if (!fMainHisto)
    {
        Error("iCalibCBEnergy", "Main histogram does not exist!\n");
        gSystem->Exit(0);
    }
    
    // create the overview histogram
    fOverviewHisto = new TH1F("Overview", ";Element;2#gamma inv. mass [MeV]", 720, 0, 720);
    fOverviewHisto->SetMarkerStyle(28);
    fOverviewHisto->SetMarkerColor(4);
    
    // get parameters from configuration file
    Double_t low = iConfig::GetRC()->GetConfigDouble("CB.Energy.Overview.Y.Min");
    Double_t upp = iConfig::GetRC()->GetConfigDouble("CB.Energy.Overview.Y.Max");
    
    // ajust overview histogram
    if (low || upp) fOverviewHisto->GetYaxis()->SetRangeUser(low, upp);
}

//______________________________________________________________________________
iCalibCBEnergy::~iCalibCBEnergy()
{
    // Destructor. 
    
    if (fPi0IMOld) delete [] fPi0IMOld;
    if (fPi0IMNew) delete [] fPi0IMNew;
    if (fLine) delete fLine;
}

//______________________________________________________________________________
void iCalibCBEnergy::CustomizeGUI()
{
    // Customize the GUI of this calibration module.

    // draw main histogram
    fCanvasFit->cd(1)->SetLogz();
    fMainHisto->Draw("colz");

    // draw the overview histogram
    fCanvasResult->cd();
    fOverviewHisto->Draw("P");
}

//______________________________________________________________________________
void iCalibCBEnergy::Fit(Int_t elem)
{
    // Perform the fit of the element 'elem'.
    
    Char_t tmp[256];
    
    // create histogram projection for this element
    sprintf(tmp, "ProjHisto_%i", elem);
    TH2* h2 = (TH2*) fMainHisto;
    if (fFitHisto) delete fFitHisto;
    fFitHisto = (TH1D*) h2->ProjectionX(tmp, elem+1, elem+1);
    
    // check for sufficient statistics
    if (fFitHisto->GetEntries())
    {
        // delete old function
        if (fFitFunc) delete fFitFunc;
        sprintf(tmp, "fEnergy_%i", elem);
        fFitFunc = new TF1(tmp, "pol1+gaus(2)");
        fFitFunc->SetLineColor(2);
        
        // estimate peak position
        fPi0IMNew[elem] = fFitHisto->GetBinCenter(fFitHisto->GetMaximumBin());
        if (fPi0IMNew[elem] < 100 || fPi0IMNew[elem] > 160) fPi0IMNew[elem] = 135;

        // estimate background
        Double_t bgPar0, bgPar1;
        iUtils::FindBackground(fFitHisto, fPi0IMNew[elem], 50, 50, &bgPar0, &bgPar1);
        
        // configure fitting function
        fFitFunc->SetRange(fPi0IMNew[elem] - 70, fPi0IMNew[elem] + 50);
        fFitFunc->SetLineColor(2);
        fFitFunc->SetParameters(bgPar0, bgPar1, fFitHisto->GetMaximum(), fPi0IMNew[elem], 9.);
        fFitFunc->SetParLimits(4, 5., 150); // sigma
        fFitHisto->Fit(fFitFunc, "+R0Q");

        // final results
        fPi0IMNew[elem] = fFitFunc->GetParameter(3); 

        // draw mean indicator line
        fLine->SetVertical();
        fLine->SetLineColor(4);
        fLine->SetLineWidth(3);
        fLine->SetY1(0);
        fLine->SetY2(fFitHisto->GetMaximum() + 20);
        
        // check if mass is in normal range
        if (fPi0IMNew[elem] < 80 || fPi0IMNew[elem] > 200) fPi0IMNew[elem] = 135;
        
        // set indicator line
        fLine->SetX1(fPi0IMNew[elem]);
        fLine->SetX2(fPi0IMNew[elem]);
    }

    // draw histogram
    fFitHisto->SetFillColor(35);
    fFitHisto->GetXaxis()->SetRangeUser(0, 500);
    fCanvasFit->cd(2);
    fFitHisto->Draw();
    
    // draw fitting function
    if (fFitFunc) fFitFunc->Draw("same");
    
    // draw indicator line
    fLine->Draw();
    
    // update canvas
    fCanvasFit->Update();
    
    // update overview
    if (elem % 20 == 0)
    {
        fCanvasResult->cd();
        fOverviewHisto->Draw("E1");
        fCanvasResult->Update();
    }   
}

//______________________________________________________________________________
void iCalibCBEnergy::Calculate(Int_t elem)
{
    // Calculate the new value of the element 'elem'.
    
    Bool_t unchanged = kFALSE;

    // check if fit was performed
    if (fFitHisto->GetEntries())
    {
        // check if line position was modified by hand
        if (fLine->GetX1() != fPi0IMNew[elem]) fPi0IMNew[elem] = fLine->GetX1();
        
        // set new pi0 position
        fPi0IMNew[elem] = fPi0IMNew[elem];

        // calculate the new offset
        fNewVal[elem] = fOldVal[elem] * (iConfig::kPi0Mass / fPi0IMNew[elem]);
    
        // if new value is negative take old
        if (fNewVal[elem] < 0) 
        {
            fNewVal[elem] = fOldVal[elem];
            unchanged = kTRUE;
        }

        // update overview histogram
        fOverviewHisto->SetBinContent(elem+1, fPi0IMNew[elem]);
        fOverviewHisto->SetBinError(elem+1, 0.1);
    }
    else
    {   
        // do not change old value
        fNewVal[elem] = fOldVal[elem];
        unchanged = kTRUE;
    }

    // user information
    printf("Element: %03d    Pi0: %12.8f    "
           "old gain: %12.8f    new gain: %12.8f",
           elem, fPi0IMNew[elem], fOldVal[elem], fNewVal[elem]);
    if (unchanged) printf("    -> unchanged");
    printf("\n");
}   
