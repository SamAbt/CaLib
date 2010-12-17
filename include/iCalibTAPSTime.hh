/*************************************************************************
 * Author: Irakli Keshelashvili, Dominik Werthmueller
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// iCalibTAPSTime                                                       //
//                                                                      //
// Calibration module for the TAPS time.                                //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#ifndef ICALIBTAPSTIME_HH
#define ICALIBTAPSTIME_HH

#include "TCanvas.h"
#include "TH2.h"
#include "TLine.h"

#include "iCalib.hh"
#include "iFileManager.hh"


class iCalibTAPSTime : public iCalib
{

private:
    Double_t* fTimeGain;                // TAPS TDC gain
    Double_t fMean;                     // mean time position
    TLine* fLine;                       // indicator line
    
    virtual void Init();
    virtual void Fit(Int_t elem);
    virtual void Calculate(Int_t elem);

public:
    iCalibTAPSTime();
    virtual ~iCalibTAPSTime();

    ClassDef(iCalibTAPSTime, 0)   // TAPS time calibration
};

#endif

