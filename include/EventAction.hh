//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id$
// 
/// \file EventAction.hh
/// \brief Definition of the EventAction class

#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include <vector>

class RunAction;
class SteppingAction;
class TrackingAction;


class G4GenericMessenger;

/// Event action class
///

class EventAction : public G4UserEventAction
{
  public:
    EventAction();
    virtual ~EventAction();

    virtual void  BeginOfEventAction(const G4Event* event);
    virtual void    EndOfEventAction(const G4Event* event);
    
    void AddAbs(G4double de, G4double dl);
    void StorePrimaryInfo(G4double primaryEn,G4double primaryX,G4double primaryY,G4double primaryZ);
                     
    void SetNumPhot(G4int value);
    void SetDetPhot(G4int value);
    void SetKillPhot(G4int value);    
    void SetBackPhot(G4int value);        
    void SetIntPoint(G4double copy, G4double x1, G4double y1, G4double z1, G4int num);        
    bool GetPrimaryRxn();

  private:
    RunAction*  fRunAction;
    G4double  fPrimaryEn;   
    G4double  fPrimaryX;       
    G4double  fPrimaryY;
    G4double  fPrimaryZ;
    G4double  fEnergyAbs;
    G4double  fTrackLAbs; 
    G4double  fTrackPhot; 
    G4double  fTrackDet; 
    G4double  fTrackKilled; 
    G4double  fTrackBack;

    G4double 	  fCopy[10];
    G4double  fx0; 
    G4double  fy0; 
    G4double  fz0; 
	bool fPrimaryRxn;
                     
    G4int     fPrintModulo;
};

// inline functions

inline void EventAction::AddAbs(G4double de, G4double dl) {
  fEnergyAbs += de; 
  fTrackLAbs += dl;
}

inline void EventAction::StorePrimaryInfo(G4double primaryEn,G4double primaryX,G4double primaryY,G4double primaryZ) {
 fPrimaryEn = primaryEn;
 fPrimaryX = primaryX;
 fPrimaryY = primaryY;
 fPrimaryZ = primaryZ;
}

inline void EventAction::SetNumPhot(G4int value) {
  fTrackPhot += value;
//  G4cout<<"set number of OP created to "<<fTrackPhot<<"\n";
}

inline void EventAction::SetDetPhot(G4int detVal) {
  fTrackDet += detVal;
//  G4cout<<"set number of detected OP to "<<fTrackDet<<"\n";
}

inline void EventAction::SetKillPhot(G4int killVal) {
  fTrackKilled += killVal;
//  G4cout<<"set number of killed OP to "<<fTrackKilled<<"\n";
}

inline void EventAction::SetBackPhot(G4int backVal) {
  fTrackBack += backVal;
//  G4cout<<"set number of backscattered OP to "<<fTrackBack<<"\n";
}

inline void EventAction::SetIntPoint(G4double copyi, G4double xi, G4double yi, G4double zi, G4int num) {
  fCopy[num]=copyi;
  fx0 = xi;
  fy0 = yi;
  fz0 = zi;
}

                     
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

    
