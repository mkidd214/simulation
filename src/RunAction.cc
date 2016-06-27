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
/// \file RunAction.cc
/// \brief Implementation of the RunAction class

#include "RunAction.hh"
#include "Analysis.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::RunAction()
 : G4UserRunAction()
{ 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::~RunAction()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::BeginOfRunAction(const G4Run* run)
{ 
  
  G4cout << "### Run " << run->GetRunID() << " start." << G4endl;

  //inform the runManager to save random number seed
  //G4RunManager::GetRunManager()->SetRandomNumberStore(true);
  
  // Book histograms, ntuple
  //
  
  // Create analysis manager
  // The choice of analysis technology is done via selectin of a namespace
  // in B4Analysis.hh
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  G4cout << "Using " << analysisManager->GetType() 
         << " analysis manager" << G4endl;
  
  // Open an output file
  //
  G4String fileName = "SLIMER";
/*  analysisManager->OpenFile(fileName);
  analysisManager->SetFirstHistoId(1);

 
  // Creating ntuple
  //
  analysisManager->CreateNtuple("SLIMER", "Data");
  analysisManager->CreateNtupleDColumn("PrimaryEn");
  analysisManager->CreateNtupleDColumn("PrimaryX");
  analysisManager->CreateNtupleDColumn("PrimaryY");
  analysisManager->CreateNtupleDColumn("PrimaryZ");
  analysisManager->CreateNtupleDColumn("Eabs");
  analysisManager->CreateNtupleDColumn("Labs");
  analysisManager->CreateNtupleDColumn("NphotonsCreated");
  analysisManager->CreateNtupleDColumn("NphotonsDetected");
  analysisManager->CreateNtupleDColumn("NphotonsKilled");
  analysisManager->CreateNtupleDColumn("BackScatter");
  analysisManager->CreateNtupleDColumn("InitialX");
  analysisManager->CreateNtupleDColumn("InitialY");
  analysisManager->CreateNtupleDColumn("InitialZ");
  analysisManager->CreateNtupleDColumn("CopyNoHit");
  analysisManager->FinishNtuple();
  */
  
  tree = new TTree("Data", "Title");
Int_t eventid;
tree->Branch("eventid",&eventid);
tree->Branch("LayerData",en_dep,"en_dep[20]/F");
for each event (id) do {
   eventid = evt->GetEventID();
   // set value in en_dep
   tree->Fill();
}
tree->Write();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::EndOfRunAction(const G4Run* aRun)
{
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

  analysisManager->Write();
  analysisManager->CloseFile();
  
  // complete cleanup
  //
  delete G4AnalysisManager::Instance();  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
