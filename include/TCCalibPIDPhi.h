/*************************************************************************
 * Authors: Dominik Werthmueller, Thomas Strub
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TCCalibPIDPhi                                                        //
//                                                                      //
// Calibration module for the PID phi angle.                            //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#ifndef TCCALIBPIDPHI_H
#define TCCALIBPIDPHI_H

#include "TCCalib.h"

class TCLine;
class TCanvas;
class TH1;
class TF1;

class TCCalibPIDPhi : public TCCalib
{

private:
    Double_t fMean;                     // mean time position
    TCLine* fLine;                      // indicator line
    TCanvas* fCanvasResult2;            // second result canvas
    TH1* fOverviewHisto2;               // second overview histogram
    TF1* fFitFunc2;                     // second fitting function

    virtual void Init();
    virtual void Fit(Int_t elem);
    virtual void Calculate(Int_t elem);

    TH1* GetMappedHistogram(TH1* histo);

public:
    TCCalibPIDPhi();
    virtual ~TCCalibPIDPhi();

    virtual void WriteValues();

    ClassDef(TCCalibPIDPhi, 0) // PID phi calibration
};

#endif

