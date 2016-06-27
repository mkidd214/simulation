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
/// \file SteppingAction.cc
/// \brief Implementation of the SteppingAction class

#include "SteppingAction.hh"
#include "EventAction.hh"
#include "DetectorConstruction.hh"
#include "G4TrackingManager.hh"
#include "G4SteppingManager.hh"

#include "G4Step.hh"
#include "G4RunManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::SteppingAction(DetectorConstruction* detectorConstruction, EventAction* eventAction)
  : G4UserSteppingAction(),
    fDetConstruction(detectorConstruction),
    fEventAction(eventAction)
{
	
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::~SteppingAction()
{ 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SteppingAction::UserSteppingAction(const G4Step* step)
{
	G4SteppingManager *fSteppingManager = new G4SteppingManager();
	
	// Collect energy and track length step by step
	//	G4cout<<"Step number:  "<<nstep<<"\t";
  	// get volume of the current step
  	
  	G4VPhysicalVolume* volume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
  	G4Track *track = step->GetTrack();
  	
//  G4cout<<"takes place in volume "<<volume->GetName()<<"\t";
//	G4cout<<"and is a "<<track->GetDefinition()->GetPDGEncoding()<<"\t"<<track->GetDefinition()->GetParticleName()<<"\n";
//	G4cout<<"Current Step number "<<track->GetCurrentStepNumber()<<"\n";

	isPrimary = fEventAction->GetPrimaryRxn();  //this is passed from EventAction.  
				//If the primary is a particle instead of a nucleus, the energy is recorded there.
				//For a nuclear beta decay, we record the primary energy of the outgoing electron here.
				
	G4int partID = track->GetDefinition()->GetPDGEncoding(); //we don't want the outgoing nucleus or anti-nu
	G4int parentID = track->GetParentID(); //we only want electrons from the initial decay, not ones created by ionization later
	G4int stepNo = track->GetCurrentStepNumber(); //we only want the first step energy, "primary" energy.

	if(isPrimary==false && partID < 100 &&  stepNo == 1 && partID != 0 && partID != -12 && parentID == 1)
	//If the primary is a nuclear decay, the particle ID is not a nucleus, an optical photon, or an anti-nu,
	// and it was created by the radioactive decay (parentID = 1), then...
	{	//we get the kinetic energy of the electron
	G4double primaryEn = step->GetPreStepPoint()->GetKineticEnergy() / keV;
	fEventAction->StorePrimaryEn(primaryEn);//passes KE of electron to event action where it can be stored in the ntuple
	}


// if it's an optical photon, they can go on forever.  We want to kill some of these.  
// Also killing daughter nuclei Np237 and N14 so that they don't fly off and collide with the CsI.  
// Need to confine them to a material....

  	if ( step->GetTrack()->GetDefinition()->GetPDGEncoding() == 0. || track->GetDefinition()->GetPDGEncoding() == 1000932371 || track->GetDefinition()->GetPDGEncoding() == 1000070141 ||track->GetDefinition()->GetPDGEncoding() == -12 || track->GetDefinition()->GetPDGEncoding() == 1000090040) {//if it's an optical photon
    	n++;

  		G4Track* track = step->GetTrack();
  		if (n > 100 && step->GetTrack()->GetDefinition()->GetPDGEncoding() == 0.) 
  		{  //this one kills OPs that have reached 100 steps
			track->SetTrackStatus(fStopAndKill);  
			n = 0;
		}
		else
		{  //this one kills nuclei and anit-neutrinos.  We don't care about them.
			track->SetTrackStatus(fStopAndKill);  
			n = 0;
		}
		//in both cases, n is reset to zero after killed.  	
	}


	//if it's not an optical photon and is in the CsI, we want to know the total energy deposited and the total path length  
  	if(volume->GetName() == "CsI" && step->GetTrack()->GetDefinition()->GetPDGEncoding() != 0.)
  	{
  		// energy deposit
  		G4double edep = step->GetTotalEnergyDeposit();
  
  		// step length
  		G4double stepLength = 0.;
  		stepLength = step->GetStepLength();

    	fEventAction->AddAbs(edep,stepLength); //this sends the delta energy and delta step to
    										   //event action to be added up.  The total will 
    										   //be stored in the ntuple.
	}
	
	if(track->GetDefinition()->GetPDGEncoding() == 11 && track->GetKineticEnergy() == 0) {
		track->SetTrackStatus(fStopAndKill);
	}
	
	
	//G4cout << track->GetKineticEnergy() / keV << " ";
	//G4cout << track->GetDefinition()->GetParticleName();
	//if(track->GetTrackStatus() == fStopAndKill) G4cout << " has been killed!\n";
	//else G4cout << "\n";
	
	
	nstep++;
	if(track->GetTrackStatus()==2)nstep = 0;  //reset nstep if the track is killed
}

void SteppingAction::GetSecondariesOfCurrentStep(G4SteppingManager *fSteppingManager) {
	
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
