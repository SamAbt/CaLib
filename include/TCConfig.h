// SVN Info: $Id$

/*************************************************************************
 * Author: Dominik Werthmueller
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TCConfig                                                             //
//                                                                      //
// CaLib configuration namespace                                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#ifndef TCCONFIG_H
#define TCCONFIG_H

#include "Rtypes.h"


// calibrationd data enumeration
// NOTE: This enum has to be synchronized with TCConfig::kCalibDataTableLengths,
//       TCConfig::fgCalibDataNames and TCConfig::fgCalibDataTableNames!
enum ECalibData
{
    // empty element
    kCALIB_EMPTY = 0,
    
    // target position
    kCALIB_TARGET_POS,

    // tagger data
    kCALIB_TAGG_T0,

    // CB data
    kCALIB_CB_T0, 
    kCALIB_CB_WALK0, kCALIB_CB_WALK1, kCALIB_CB_WALK2, kCALIB_CB_WALK3,
    kCALIB_CB_E1, 
    kCALIB_CB_EQUAD0, kCALIB_CB_EQUAD1,

    // TAPS data
    kCALIB_TAPS_T0, kCALIB_TAPS_T1, 
    kCALIB_TAPS_LG_E0, kCALIB_TAPS_LG_E1,
    kCALIB_TAPS_SG_E0, kCALIB_TAPS_SG_E1, 
    kCALIB_TAPS_EQUAD0, kCALIB_TAPS_EQUAD1,
    kCALIB_TAPS_LED1, kCALIB_TAPS_LED2,

    // PID data
    kCALIB_PID_PHI, 
    kCALIB_PID_T0, 
    kCALIB_PID_E0, kCALIB_PID_E1, 
    kCALIB_PID_DROOP0, kCALIB_PID_DROOP1, kCALIB_PID_DROOP2, kCALIB_PID_DROOP3,

    // Veto data
    kCALIB_VETO_T0, kCALIB_VETO_T1, 
    kCALIB_VETO_E0, kCALIB_VETO_E1,
};
typedef ECalibData CalibData_t;


// calibrationd type enumeration
// NOTE: This enum has to be synchronized with TCConfig::kCalibTypeNames,
//       TCConfig::fgCalibTypeNData and TCConfig::fgCalibTypeData!
enum ECalibType
{
    // empty element
    kCALIB_TYPE_EMPTY = 0,
    
    // target position
    kCALIB_TYPE_TARGET_POS,

    // tagger data
    kCALIB_TYPE_TAGG_TIME,

    // CB data
    kCALIB_TYPE_CB_TIME, 
    kCALIB_TYPE_CB_WALK,
    kCALIB_TYPE_CB_ENERGY, 
    kCALIB_TYPE_CB_EQUAD,

    // TAPS data
    kCALIB_TYPE_TAPS_TIME, 
    kCALIB_TYPE_TAPS_LG_PED,
    kCALIB_TYPE_TAPS_LG_ENERGY,
    kCALIB_TYPE_TAPS_SG_PED, 
    kCALIB_TYPE_TAPS_SG_ENERGY, 
    kCALIB_TYPE_TAPS_EQUAD,
    kCALIB_TYPE_TAPS_LED1, kCALIB_TYPE_TAPS_LED2,

    // PID data
    kCALIB_TYPE_PID_PHI, 
    kCALIB_TYPE_PID_T0, 
    kCALIB_TYPE_PID_ENERGY, 
    kCALIB_TYPE_PID_DROOP,

    // Veto data
    kCALIB_TYPE_VETO_TIME, 
    kCALIB_TYPE_VETO_PED, 
    kCALIB_TYPE_VETO_ENERGY,
};
typedef ECalibType CalibType_t;


// detector enumeration
enum ECalibDetector
{
    kDETECTOR_NODET = 0,
    kDETECTOR_TAGG,
    kDETECTOR_CB,
    kDETECTOR_TAPS,
    kDETECTOR_PID,
    kDETECTOR_VETO
};
typedef ECalibDetector CalibDetector_t;


namespace TCConfig
{   
    // detector elements
    extern const Int_t kMaxTargPos;
    extern const Int_t kMaxCrystal;
    extern const Int_t kMaxCB;
    extern const Int_t kMaxTAPS;
    extern const Int_t kMaxPID;
    extern const Int_t kMaxVeto;
    extern const Int_t kMaxTAGGER;
    
    // maximum theta bins for TAPS quadratic energy correction
    extern const Int_t kMaxTAPSThetaBins;
    
    // database format definitions
    extern const Int_t kCalibNData;
    extern const Char_t* kCalibDataNames[];
    extern const Char_t* kCalibDataTableNames[];
    extern const Int_t kCalibDataTableLengths[];
    extern const Int_t kCalibNType;
    extern const Char_t* kCalibTypeNames[];
    extern const Int_t kCalibTypeNData[];
    extern const CalibData_t kCalibTypeData[][4];
    extern const Char_t* kCalibMainTableName;
    extern const Char_t* kCalibMainTableFormat; 
    extern const Char_t* kCalibDataTableHeader;
    extern const Char_t* kCalibDataTableSettings;
     
    // version numbers etc.
    extern const Char_t kCaLibVersion[];
    extern const Int_t kContainerFormatVersion;
    extern const Char_t kCaLibDumpName[];

    // constants
    extern const Double_t kPi0Mass;
    extern const Double_t kEtaMass;
}

#endif

