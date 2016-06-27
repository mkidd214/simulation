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
  	
  	G4int detPhot = 0;
  	G4int killedPhot = 0;
  	G4int backPhot = 0;
  	
//  	if(nstep == 0)copyNo.clear();
  	
  	G4VPhysicalVolume* volume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
  	G4Track *track = step->GetTrack();
  	

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
	G4double primaryX = step->GetPreStepPoint()->GetPosition().x();
	G4double primaryY = step->GetPreStepPoint()->GetPosition().y();
	G4double primaryZ = step->GetPreStepPoint()->GetPosition().z();	
	fEventAction->StorePrimaryInfo(primaryEn, primaryX, primaryY, primaryZ);//passes KE of electron to event action where it can be stored in the ntuple
	}

// if it's an optical photon, they can go on forever.  We want to kill some of these.  
// Also killing daughter nuclei Np237 and N14 so that they don't fly off and collide with the CsI.  
// Need to confine them to a material....

  	if ( step->GetTrack()->GetDefinition()->GetPDGEncoding() == 0 || track->GetDefinition()->GetPDGEncoding() == 1000932371 || track->GetDefinition()->GetPDGEncoding() == 1000070141 ||track->GetDefinition()->GetPDGEncoding() == -12) {//if it's an optical photon
    	n++;
  		G4Track* track = step->GetTrack();
  		if (n > 100 && step->GetTrack()->GetDefinition()->GetPDGEncoding() == 0) 
  		{  //this one kills OPs that have reached n steps
			track->SetTrackStatus(fStopAndKill);  
//			G4cout<<"Killed optical photon\n";
			n = 0;
			killedPhot++;
			fEventAction->SetKillPhot(killedPhot);
		}
		else if(step->GetTrack()->GetDefinition()->GetPDGEncoding() != 0)
		{  //this one kills nuclei and anti-neutrinos.  We don't care about them.
			track->SetTrackStatus(fStopAndKill);  
			n = 0;
		}
		//in both cases, n is reset to zero after killed.  	
	}

//G4cout<<"volume "<<volume->GetName()<<"\n";
//G4cout<<"copy "<<step->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber()<<"\n";
	//if it's not an optical photon and is in the CsI, we want to know the total energy deposited and the total path length  
  	if(volume->GetName() == "CsI" && step->GetTrack()->GetDefinition()->GetPDGEncoding() != 0.)
  	{
  		// energy deposit
  		G4double edep = step->GetTotalEnergyDeposit()/keV;
  
  		// step length
  		G4double stepLength = 0.;
  		stepLength = step->GetStepLength();

    	fEventAction->AddAbs(edep,stepLength); //this sends the delta energy and delta step to
    										   //event action to be added up.  The total will 
    										   //be stored in the ntuple.
	}
	
	G4double x0, y0, z0;
	G4double copyNo[10];
	G4int counter;
	if(nstep == 0)counter = 0;
	
	if(track->GetNextVolume())
	{	
		if(track->GetNextVolume()->GetName() == "camera" && step->GetTrack()->GetDefinition()->GetPDGEncoding() == 0)
		//if there's an optical photon in the camera, we want to count it as detected.
		{
		detPhot++;
		fEventAction->SetDetPhot(detPhot);
		}
		
		if(track->GetDefinition()->GetPDGEncoding() == 11 || track->GetDefinition()->GetPDGEncoding() == 22 || track->GetDefinition()->GetPDGEncoding() == 1000020040)
		{
			if(volume->GetName() == "World" && track->GetNextVolume()->GetName() == "CsI")
			{
				copyNo[counter]=step->GetPostStepPoint()->GetTouchableHandle()->GetCopyNumber();
				G4cout<<"copy no "<<copyNo[counter]<<"\n";
				x0 = track->GetPosition().x()/mm;
				y0 = track->GetPosition().y()/mm;
				z0 = track->GetPosition().z()/mm;
				//G4cout<<"x = "<<x0<<"\ty = "<<y0<<"\tx = "<<z0<<"\n";
				fEventAction->SetIntPoint(copyNo[counter], x0, y0, z0, counter);
			}
		}

	}
	else if(!track->GetNextVolume() && step->GetTrack()->GetDefinition()->GetPDGEncoding() == 0)
	{
		if(step->GetPreStepPoint()->GetMomentumDirection().z()>0)
		{
		backPhot++;
		fEventAction->SetBackPhot(backPhot);
		}
	}

	nstep++;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......