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

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4GenericMessenger.hh"
#include "G4UnitsTable.hh"

#include "Randomize.hh"
#include <iomanip>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::EventAction()
 : G4UserEventAction(),
   fEnergyAbs(0.),
   fTrackLAbs(0.),
   fPrintModulo(1)
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
  fEnergyAbs = 0. / keV;
  fTrackLAbs = 0. / keV;
  
  G4PrimaryVertex* primaryVertex = evt->GetPrimaryVertex();
  G4PrimaryParticle* primaryParticle = primaryVertex->GetPrimary();
  G4double ke = primaryParticle->GetKineticEnergy();
  G4int pID = primaryParticle->GetPDGcode();

  if(pID == 22 || pID == 11 || pID == 1000020040)	
  //if the primary is a gamma ray, electron, or alpha, we want to store that energy as primary energy
  {
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	analysisManager->FillNtupleDColumn(0, ke);
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

if(fPrimaryRxn == false)
	{
		analysisManager->FillNtupleDColumn(0, fPrimaryEn);
		//we only fill this here if we are interested in the secondary's energies.  
		//These are passed from SteppingAction.  
		//Otherwise it's filled in BeginOfEventAction()
	}
	fEnergyAbs = fEnergyAbs*1000;
  analysisManager->FillNtupleDColumn(1, fEnergyAbs);
  analysisManager->FillNtupleDColumn(2, fTrackLAbs);

  analysisManager->AddNtupleRow();  
    
}

bool EventAction::GetPrimaryRxn()
{
return fPrimaryRxn;
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
