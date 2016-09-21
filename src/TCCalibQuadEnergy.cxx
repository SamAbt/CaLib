/*************************************************************************
 * Author: Dominik Werthmueller, Thomas Strub
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TCCalibQuadEnergy                                                    //
//                                                                      //
// Base quadratic energy correction module class.                       //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#include "TH2.h"
#include "TF1.h"
#include "TCLine.h"
#include "TCanvas.h"
#include "TMath.h"

#include "TCCalibQuadEnergy.h"
#include "TCMySQLManager.h"
#include "TCFileManager.h"
#include "TCUtils.h"

ClassImp(TCCalibQuadEnergy)

//______________________________________________________________________________
TCCalibQuadEnergy::TCCalibQuadEnergy(const Char_t* name, const Char_t* title, const Char_t* data,
                                     Int_t nElem)
    : TCCalib(name, title, data, nElem)
{
    // Constructor.

    // init members
    fPar0 = 0;
    fPar1 = 0;
    fMainHisto2 = 0;
    fMainHisto3 = 0;
    fFitHisto1b = 0;
    fFitHisto2 = 0;
    fFitHisto3 = 0;
    fFitFunc1b = 0;
    fPi0Pos = 0;
    fEtaPos = 0;
    fPi0MeanE = 0;
    fEtaMeanE = 0;
    fLinePi0 = 0;
    fLineEta = 0;
    fLineMeanEPi0 = 0;
    fLineMeanEEta = 0;
    fPi0PosHisto = 0;
    fEtaPosHisto = 0;
}

//______________________________________________________________________________
TCCalibQuadEnergy::~TCCalibQuadEnergy()
{
    // Destructor.

    if (fPar0) delete [] fPar0;
    if (fPar1) delete [] fPar1;
    if (fMainHisto2) delete fMainHisto2;
    if (fMainHisto3) delete fMainHisto3;
    if (fFitHisto1b) delete fFitHisto1b;
    if (fFitHisto2) delete fFitHisto2;
    if (fFitHisto3) delete fFitHisto3;
    if (fFitFunc1b) delete fFitFunc1b;
    if (fLinePi0) delete fLinePi0;
    if (fLineEta) delete fLineEta;
    if (fLineMeanEPi0) delete fLineMeanEPi0;
    if (fLineMeanEEta) delete fLineMeanEEta;
    if (fPi0PosHisto) delete fPi0PosHisto;
    if (fEtaPosHisto) delete fEtaPosHisto;
}

//______________________________________________________________________________
void TCCalibQuadEnergy::Init()
{
    // Init the module.

    Char_t tmp[256];

    // init members
    fPar0 = new Double_t[fNelem];
    fPar1 = new Double_t[fNelem];
    fFitHisto1b = 0;
    fFitHisto2 = 0;
    fFitHisto3 = 0;
    fFitFunc1b = 0;
    fPi0Pos = 0;
    fEtaPos = 0;
    fPi0MeanE = 0;
    fEtaMeanE = 0;
    fLinePi0 = new TCLine();
    fLineEta = new TCLine();
    fLineMeanEPi0 = new TCLine();
    fLineMeanEEta = new TCLine();

    // configure lines
    fLinePi0->SetLineColor(4);
    fLinePi0->SetLineWidth(3);
    fLineEta->SetLineColor(4);
    fLineEta->SetLineWidth(3);
    fLineMeanEPi0->SetLineColor(4);
    fLineMeanEPi0->SetLineWidth(3);
    fLineMeanEEta->SetLineColor(4);
    fLineMeanEEta->SetLineWidth(3);

    // get main histogram name
    sprintf(tmp, "%s.Histo.Fit.Name", GetName());
    if (!TCReadConfig::GetReader()->GetConfig(tmp))
    {
        Error("Init", "Histogram name was not found in configuration!");
        return;
    }
    else fHistoName = *TCReadConfig::GetReader()->GetConfig(tmp);

    // get mean pi0 energy histogram name
    TString hMeanPi0Name;
    sprintf(tmp, "%s.Histo.MeanE.Pi0.Name", GetName());
    if (!TCReadConfig::GetReader()->GetConfig(tmp))
    {
        Error("Init", "Histogram name was not found in configuration!");
        return;
    }
    else hMeanPi0Name = *TCReadConfig::GetReader()->GetConfig(tmp);

    // get mean eta energy histogram name
    TString hMeanEtaName;
    sprintf(tmp, "%s.Histo.MeanE.Eta.Name", GetName());
    if (!TCReadConfig::GetReader()->GetConfig(tmp))
    {
        Error("Init", "Histogram name was not found in configuration!");
        return;
    }
    else hMeanEtaName = *TCReadConfig::GetReader()->GetConfig(tmp);

    // read old parameters (only from first set)
    if (this->InheritsFrom("TCCalibCBQuadEnergy"))
    {
        TCMySQLManager::GetManager()->ReadParameters("Data.CB.Energy.Quad.Par0", fCalibration.Data(), fSet[0], fPar0, fNelem);
        TCMySQLManager::GetManager()->ReadParameters("Data.CB.Energy.Quad.Par1", fCalibration.Data(), fSet[0], fPar1, fNelem);
    }
    else if (this->InheritsFrom("TCCalibTAPSQuadEnergy"))
    {
        TCMySQLManager::GetManager()->ReadParameters("Data.TAPS.Energy.Quad.Par0", fCalibration.Data(), fSet[0], fPar0, fNelem);
        TCMySQLManager::GetManager()->ReadParameters("Data.TAPS.Energy.Quad.Par1", fCalibration.Data(), fSet[0], fPar1, fNelem);
    }

    // sum up all files contained in this runset
    TCFileManager f(fData, fCalibration.Data(), fNset, fSet);

    // get the main calibration histogram
    fMainHisto = f.GetHistogram(fHistoName.Data());
    if (!fMainHisto)
    {
        Error("Init", "Main histogram does not exist!\n");
        return;
    }

    // get the pi0 mean energy histogram
    fMainHisto2 = (TH2*) f.GetHistogram(hMeanPi0Name.Data());
    if (!fMainHisto2)
    {
        Error("Init", "Pi0 mean energy histogram does not exist!\n");
        return;
    }

    // get the eta mean energy histogram
    fMainHisto3 = (TH2*) f.GetHistogram(hMeanEtaName.Data());
    if (!fMainHisto3)
    {
        Error("Init", "Eta mean energy histogram does not exist!\n");
        return;
    }

    // create the pi0 overview histogram
    fPi0PosHisto = new TH1F("Pi0 position overview", ";Element;#pi^{0} peak position [MeV]", fNelem, 0, fNelem);
    fPi0PosHisto->SetMarkerStyle(2);
    fPi0PosHisto->SetMarkerColor(4);
    sprintf(tmp, "%s.Histo.Overview.Pi0", GetName());
    TCUtils::FormatHistogram(fPi0PosHisto, tmp);

    // create the eta overview histogram
    fEtaPosHisto = new TH1F("Eta position overview", ";Element;#eta peak position [MeV]", fNelem, 0, fNelem);
    fEtaPosHisto->SetMarkerStyle(2);
    fEtaPosHisto->SetMarkerColor(4);
    sprintf(tmp, "%s.Histo.Overview.Eta", GetName());
    TCUtils::FormatHistogram(fEtaPosHisto, tmp);

    // prepare fit histogram canvas
    fCanvasFit->Divide(1, 4, 0.001, 0.001);

    // draw the overview histograms
    fCanvasResult->Divide(1, 2, 0.001, 0.001);
    fCanvasResult->cd(1);
    fPi0PosHisto->Draw("P");
    fCanvasResult->cd(2);
    fEtaPosHisto->Draw("P");
}

//______________________________________________________________________________
void TCCalibQuadEnergy::Fit(Int_t elem)
{
    // Perform the fit of the element 'elem'.

    Char_t tmp[256];

    // get the 2g invariant mass histograms
    sprintf(tmp, "ProjHisto_%d", elem);
    TH2* h2 = (TH2*) fMainHisto;
    if (fFitHisto) delete fFitHisto;
    if (fFitHisto1b) delete fFitHisto1b;
    fFitHisto = (TH1*) h2->ProjectionX(tmp, elem+1, elem+1, "e");
    sprintf(tmp, "ProjHisto_%db", elem);
    fFitHisto1b = (TH1*) fFitHisto->Clone(tmp);
    sprintf(tmp, "%s.Histo.Fit.Pi0.IM", GetName());
    TCUtils::FormatHistogram(fFitHisto, tmp);
    sprintf(tmp, "%s.Histo.Fit.Eta.IM", GetName());
    TCUtils::FormatHistogram(fFitHisto1b, tmp);

    // get pi0 mean energy projection
    sprintf(tmp, "ProjHistoMeanPi0_%d", elem);
    h2 = (TH2*) fMainHisto2;
    if (fFitHisto2) delete fFitHisto2;
    fFitHisto2 = h2->ProjectionX(tmp, elem+1, elem+1, "e");
    sprintf(tmp, "%s.Histo.Fit.Pi0.MeanE", GetName());
    TCUtils::FormatHistogram(fFitHisto2, tmp);

    // get eta mean energy projection
    sprintf(tmp, "ProjHistoMeanEta_%d", elem);
    h2 = (TH2*) fMainHisto3;
    if (fFitHisto3) delete fFitHisto3;
    fFitHisto3 = h2->ProjectionX(tmp, elem+1, elem+1, "e");
    sprintf(tmp, "%s.Histo.Fit.Eta.MeanE", GetName());
    TCUtils::FormatHistogram(fFitHisto3, tmp);

    // draw pi0
    fCanvasFit->cd(1);
    fFitHisto->SetFillColor(35);
    fFitHisto->Draw("hist");

    // draw eta
    fCanvasFit->cd(2);
    fFitHisto1b->SetFillColor(35);
    fFitHisto1b->Draw("hist");

    // draw pi0 mean energy
    fCanvasFit->cd(3);
    fFitHisto2->SetFillColor(35);
    fFitHisto2->Draw("hist");

    // draw eta mean energy
    fCanvasFit->cd(4);
    fFitHisto3->SetFillColor(35);
    fFitHisto3->Draw("hist");

    // check for sufficient statistics
    if (fFitHisto->GetEntries())
    {
        // delete old functions
        if (fFitFunc) delete fFitFunc;
        if (fFitFunc1b) delete fFitFunc1b;

        // create pi0 fitting function
        sprintf(tmp, "fPi0_%i", elem);
        fFitFunc = new TF1(tmp, "gaus(0)+pol2(3)", 100, 170);
        fFitFunc->SetLineColor(2);

        // create eta fitting function
        sprintf(tmp, "fEta_%i", elem);
        fFitFunc1b = new TF1(tmp, "gaus(0)+pol3(3)", 450, 650);
        fFitFunc1b->SetLineColor(2);

        // get x-axis range
        Double_t xmin = fFitHisto1b->GetXaxis()->GetBinCenter(fFitHisto1b->GetXaxis()->GetFirst());
        Double_t xmax = fFitHisto1b->GetXaxis()->GetBinCenter(fFitHisto1b->GetXaxis()->GetLast());

        // set new range & get the peak position of eta
        fFitHisto1b->GetXaxis()->SetRangeUser(500, 600);
        Double_t fMaxEta = fFitHisto1b->GetBinCenter(fFitHisto1b->GetMaximumBin());
        fFitHisto1b->GetXaxis()->SetRangeUser(xmin, xmax);

        // init peak positions
        fPi0Pos = 135.;
        fEtaPos = fMaxEta;

        if (fIsReFit)
        {
            fPi0Pos = fLinePi0->GetPos();
            fEtaPos = fLineEta->GetPos();
        }

        // configure fitting functions
        // pi0
        fFitFunc->SetParameters(fFitHisto->GetMaximum(), fPi0Pos, 10, 1, 1, 1);
        fFitFunc->SetParLimits(0, 0.1*fFitHisto->GetMaximum(), 1.5*fFitHisto->GetMaximum());
        fFitFunc->SetParLimits(1, fPi0Pos - 15., fPi0Pos+15.);
        fFitFunc->SetParLimits(2, 2, 40);

        // eta
        fFitFunc1b->SetParameters(fFitHisto1b->GetMaximum(), fEtaPos, 15, 1, 1, 1, 0.1);
        fFitFunc1b->SetParLimits(0, 0.1*fFitHisto1b->GetMaximum(), 1.5*fFitHisto1b->GetMaximum());
        fFitFunc1b->SetParLimits(1, fEtaPos - 30, fEtaPos + 30);
        fFitFunc1b->SetParLimits(2, 1, 50);
        //fFitFunc1b->SetParLimits(3, 0, 100);
        //fFitFunc1b->SetParLimits(4, -1, 0);
        //fFitFunc1b->SetParLimits(5, -1, 0);//0, 50

        // set strict 3% limits for refitting
        if (fIsReFit)
        {
            fFitFunc->SetParLimits(1, (1 - 0.03)*fPi0Pos, (1 + 0.03)*fPi0Pos);
            fFitFunc1b->SetParLimits(1, (1 - 0.03)*fEtaPos, (1 + 0.03)*fEtaPos);
        }

        // fit peaks
        for (Int_t i = 0; i < 10; i++)
            if (!fFitHisto->Fit(fFitFunc, "RBQ0")) break;
        for (Int_t i = 0; i < 10; i++)
            if (!fFitHisto1b->Fit(fFitFunc1b, "RBQ0")) break;

        // get results
        fPi0Pos = fFitFunc->GetParameter(1);
        fEtaPos = fFitFunc1b->GetParameter(1);
        fPi0MeanE = fFitHisto2->GetMean();
        fEtaMeanE = fFitHisto3->GetMean();

        // check if mass is in normal range
        if (!fIsReFit)
        {
            if (fPi0Pos < 80 || fPi0Pos > 200) fPi0Pos = 135;
            if (fEtaPos < 450 || fEtaPos > 650) fEtaPos = 547;
        }

        // set indicator lines
        fLinePi0->SetPos(fPi0Pos);
        fLineEta->SetPos(fEtaPos);

        // set lines
        fLineMeanEPi0->SetPos(fPi0MeanE);
        fLineMeanEEta->SetPos(fEtaMeanE);

        // draw pi0
        fCanvasFit->cd(1);
        if (fFitFunc) fFitFunc->Draw("same");
        fLinePi0->Draw();

        // draw eta
        fCanvasFit->cd(2);
        if (fFitFunc1b) fFitFunc1b->Draw("same");
        fLineEta->Draw();

        // draw pi0 mean energy
        fCanvasFit->cd(3);
        fLineMeanEPi0->Draw();

        // draw eta mean energy
        fCanvasFit->cd(4);
        fLineMeanEEta->Draw();
    }

    // update canvas
    fCanvasFit->Update();

    // update overview
    if (elem % 20 == 0)
    {
        fCanvasResult->cd(1);
        fPi0PosHisto->Draw("E1");
        fCanvasResult->cd(2);
        fEtaPosHisto->Draw("E1");
        fCanvasResult->Update();
    }
}

//______________________________________________________________________________
void TCCalibQuadEnergy::Calculate(Int_t elem)
{
    // Calculate the new value of the element 'elem'.

    Bool_t no_corr = kFALSE;

    // check if fit was performed
    if (fFitHisto->GetEntries())
    {
        // check if pi0 line position was modified by hand
        if (fLinePi0->GetPos() != fPi0Pos) fPi0Pos = fLinePi0->GetPos();
        if (fLineMeanEPi0->GetPos() != fPi0MeanE) fPi0MeanE = fLineMeanEPi0->GetPos();

        // check if etaline position was modified by hand
        if (fLineEta->GetPos() != fEtaPos) fEtaPos = fLineEta->GetPos();
        if (fLineMeanEEta->GetPos() != fEtaMeanE) fEtaMeanE = fLineMeanEEta->GetPos();

        // calculate quadratic correction factors
        Double_t mean_E_ratio = fEtaMeanE / fPi0MeanE;
        Double_t pion_im_ratio = TCConfig::kPi0Mass / fPi0Pos;
        Double_t eta_im_ratio = TCConfig::kEtaMass / fEtaPos;
        fPar0[elem] = (eta_im_ratio - mean_E_ratio*pion_im_ratio) / (1. - mean_E_ratio);
        fPar1[elem] = (pion_im_ratio - fPar0[elem]) / fPi0MeanE;

        // check values
        if (TMath::IsNaN(fPar0[elem]) || TMath::IsNaN(fPar1[elem]))
        {
            fPar0[elem] = 1;
            fPar1[elem] = 0;
            no_corr = kTRUE;
        }

        // update overview histograms
        fPi0PosHisto->SetBinContent(elem+1, fPi0Pos);
        fPi0PosHisto->SetBinError(elem+1, 0.0000001);
        fEtaPosHisto->SetBinContent(elem+1, fEtaPos);
        fEtaPosHisto->SetBinError(elem+1, 0.0000001);
    }
    else
    {
        fPar0[elem] = 1;
        fPar1[elem] = 0;
        no_corr = kTRUE;
    }

    // user information
    printf("Element: %03d    Pi0 Pos.: %6.2f    Pi0 ME: %6.2f    "
           "Eta Pos.: %6.2f    Eta ME: %6.2f    Par0: %12.8f    Par1: %e",
           elem, fPi0Pos, fPi0MeanE, fEtaPos, fEtaMeanE, fPar0[elem], fPar1[elem]);
    if (no_corr) printf("    -> no correction");
    if (this->InheritsFrom("TCCalibCBQuadEnergy"))
    {
        if (TCUtils::IsCBHole(elem)) printf(" (hole)");
    }
    printf("\n");
}

//______________________________________________________________________________
void TCCalibQuadEnergy::PrintValues()
{
    // Print out the old and new values for all elements.

    // loop over elements
    for (Int_t i = 0; i < fNelem; i++)
    {
        printf("Element: %03d    Par0: %12.8f    "
               "Par1: %12.8f\n",
               i, fPar0[i], fPar1[i]);
    }
}

//______________________________________________________________________________
void TCCalibQuadEnergy::WriteValues()
{
    // Write the obtained calibration values to the database.

    // write values to database
    for (Int_t i = 0; i < fNset; i++)
    {
        if (this->InheritsFrom("TCCalibCBQuadEnergy"))
        {
            TCMySQLManager::GetManager()->WriteParameters("Data.CB.Energy.Quad.Par0", fCalibration.Data(), fSet[i], fPar0, fNelem);
            TCMySQLManager::GetManager()->WriteParameters("Data.CB.Energy.Quad.Par1", fCalibration.Data(), fSet[i], fPar1, fNelem);
        }
        else if (this->InheritsFrom("TCCalibTAPSQuadEnergy"))
        {
            TCMySQLManager::GetManager()->WriteParameters("Data.TAPS.Energy.Quad.Par0", fCalibration.Data(), fSet[i], fPar0, fNelem);
            TCMySQLManager::GetManager()->WriteParameters("Data.TAPS.Energy.Quad.Par1", fCalibration.Data(), fSet[i], fPar1, fNelem);
        }
    }

    // save overview canvas
    SaveCanvas(fCanvasResult, "Overview");
}

