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
/// \file EventAction.cc
/// \brief Implementation of the EventAction class

#include "EventAction.hh"
#include "RunAction.hh"
#include "Analysis.hh"
#include "SteppingAction.hh"
#include "TrackingAction.hh"

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4GenericMessenger.hh"
#include "G4UnitsTable.hh"

#include "Randomize.hh"
#include <iomanip>
#include <vector>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::EventAction()
 : G4UserEventAction(),
   fEnergyAbs(0.),
   fTrackLAbs(0.), fTrackPhot(0)
{
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::~EventAction()
{

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::BeginOfEventAction(const G4Event* evt)
{  

  G4int eventID = evt->GetEventID();
  if ( eventID % 10000 == 0 )  
  { 
    G4cout << "\n---> Begin of event: " << eventID << G4endl;
  }

  // initialisation per event
  fPrimaryEn = 0.;
  fEnergyAbs = 0.;
  fTrackLAbs = 0.;
  fTrackPhot = 0;
  fTrackDet = 0;
  fTrackKilled = 0;
  fTrackBack = 0;
  fx0 = 0.;
  fy0 = 0.;
  fz0 = 0.;
  for(int i = 0; i<10; i++) fCopy[i] = 0;
  
  G4PrimaryVertex* primaryVertex = evt->GetPrimaryVertex();
  G4PrimaryParticle* primaryParticle = primaryVertex->GetPrimary();
  G4double ke = primaryParticle->GetKineticEnergy();
  G4int pID = primaryParticle->GetPDGcode();
  G4ThreeVector pLoc = primaryVertex->GetPosition();

  if(pID == 22 || pID == 11 || pID == 1000020040)	
  //if the primary is a gamma ray, electron, or alpha, we want to store that energy as primary energy
  {
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	analysisManager->FillNtupleDColumn(0, ke);
	analysisManager->FillNtupleDColumn(1, pLoc.x());
	analysisManager->FillNtupleDColumn(2, pLoc.y());
	analysisManager->FillNtupleDColumn(3, pLoc.z());		
  	fPrimaryRxn = true;
  }
  else  fPrimaryRxn = false; 
  //if it's a nucleus, we'll want the decay products as our "primary" and we'll
  //get those energies elsewhere (in stepping action)

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::EndOfEventAction(const G4Event* evt)
{
  // get analysis manager
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  
  // fill ntuple

//G4cout<<"wrote number of OP created "<<fTrackPhot<<"\n";
//G4cout<<"wrote number of OP detected "<<fTrackDet<<"\n";
//G4cout<<"wrote number of OP killed "<<fTrackKilled<<"\n";
//G4cout<<"wrote number of OP backscattered "<<fTrackBack<<"\n";
if(fPrimaryRxn == false)
	{
		analysisManager->FillNtupleDColumn(0, fPrimaryEn);
		analysisManager->FillNtupleDColumn(1, fPrimaryX);
		analysisManager->FillNtupleDColumn(2, fPrimaryY);
		analysisManager->FillNtupleDColumn(3, fPrimaryZ);
		//we only fill this here if we are interested in the secondary's energies.  
		//These are passed from SteppingAction.  
		//Otherwise it's filled in BeginOfEventAction()
	}
  analysisManager->FillNtupleDColumn(4, fEnergyAbs);
  analysisManager->FillNtupleDColumn(5, fTrackLAbs);
  analysisManager->FillNtupleDColumn(6, fTrackPhot);
  analysisManager->FillNtupleDColumn(7, fTrackDet);
  analysisManager->FillNtupleDColumn(8, fTrackKilled);
  analysisManager->FillNtupleDColumn(9, fTrackBack);
  analysisManager->FillNtupleDColumn(10, fx0);  
  analysisManager->FillNtupleDColumn(11, fy0);  
  analysisManager->FillNtupleDColumn(12, fz0);  
  analysisManager->FillNtupleDColumn(13, *fCopy);  
  analysisManager->AddNtupleRow();  
    
}

bool EventAction::GetPrimaryRxn()
{
return fPrimaryRxn;
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
