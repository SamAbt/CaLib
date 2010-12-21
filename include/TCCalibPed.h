// SVN Info: $Id$

/*************************************************************************
 * Author: Dominik Werthmueller
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TCCalibPed                                                           //
//                                                                      //
// Base pedestal calibration module class.                              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#ifndef TCCALIBPED_H
#define TCCALIBPED_H

#include "TCanvas.h"
#include "TH2.h"
#include "TLine.h"

#include "TCCalib.h"
#include "TCUtils.h"
#include "TCFileManager.h"


class TCCalibPed : public TCCalib
{

private:
    Int_t* fADC;                        // array of element ADC numbers
    TCFileManager* fFileManager;        // file manager
    Double_t fMean;                     // mean position
    TLine* fLine;                       // indicator line
    
    virtual void Init();
    virtual void Fit(Int_t elem);
    virtual void Calculate(Int_t elem);

    void ReadADC();

public:
    TCCalibPed() : TCCalib(), fADC(0), fFileManager(0), fMean(0), fLine(0) { }
    TCCalibPed(const Char_t* name, const Char_t* title, CalibData_t data,
               Int_t nElem);
    virtual ~TCCalibPed();

    ClassDef(TCCalibPed, 0) // Base pedestal calibration class
};


class TCCalibTAPSPedLG : public TCCalibPed
{

public:
    TCCalibTAPSPedLG() 
        : TCCalibPed("TAPS.Ped.LG", "TAPS LG pedestal calibration",
                     kCALIB_TAPS_LG_E0,
                     TCReadConfig::GetReader()->GetConfigInt("TAPS.Elements")) { }
    virtual ~TCCalibTAPSPedLG() { }
    
    ClassDef(TCCalibTAPSPedLG, 0) // TAPS LG pedestal calibration class
};


class TCCalibTAPSPedSG : public TCCalibPed
{

public:
    TCCalibTAPSPedSG() 
        : TCCalibPed("TAPS.Ped.SG", "TAPS SG pedestal calibration",
                     kCALIB_TAPS_SG_E0,
                     TCReadConfig::GetReader()->GetConfigInt("TAPS.Elements")) { }
    virtual ~TCCalibTAPSPedSG() { }
    
    ClassDef(TCCalibTAPSPedSG, 0) // TAPS SG pedestal calibration class
};


class TCCalibTAPSPedVETO : public TCCalibPed
{

public:
    TCCalibTAPSPedVETO() 
        : TCCalibPed("TAPS.Ped.VETO", "VETO pedestal calibration",
                     kCALIB_VETO_E0,
                     TCConfig::kMaxVETO) { }
    virtual ~TCCalibTAPSPedVETO() { }
    
    ClassDef(TCCalibTAPSPedVETO, 0) // VETO pedestal calibration class
};

#endif

