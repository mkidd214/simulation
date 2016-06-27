 //----- C++ source codes: main() function for visualization
 #ifdef G4VIS_USE
 #include "G4VisExecutive.hh"
 #endif

#include "G4RunManager.hh"
#include "G4UImanager.hh"

#include "DetectorConstruction.hh"

#include "PhysicsList.hh"
#include "PrimaryGeneratorAction.hh"

#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"
#include "TrackingAction.hh"

#include "Randomize.hh"
#include "time.h"

 int main(int argc,char** argv) {
	
	 //choose the Random engine
	CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine());
	//set random seed with system time
	G4long seed = time(NULL);
	CLHEP::HepRandom::setTheSeed(seed);
	 
	
	G4cout << "start of main \n";
	G4RunManager* runManager = new G4RunManager;
	G4cout << "Created runManager...\n";
	
	DetectorConstruction* detector = new DetectorConstruction;
	G4VUserPhysicsList* physics = new PhysicsList;
	G4cout << "Created physics list...\n";
	runManager->SetUserInitialization(detector);
	runManager->SetUserInitialization(physics);
	G4cout << "Initialized physics list...\n";
	
	G4VUserPrimaryGeneratorAction* gen_action = new PrimaryGeneratorAction;
	runManager->SetUserAction(gen_action);
	//gen_action->GeneratePrimaries();
	
	
	RunAction* runAction = new RunAction();
	runManager->SetUserAction(runAction);
	
  	EventAction* eventAction = new EventAction();
  	runManager->SetUserAction(eventAction);
	
  	SteppingAction* steppingAction = new SteppingAction(detector, eventAction);
	runManager->SetUserAction(steppingAction);

  	TrackingAction* trackingAction = new TrackingAction(runAction, eventAction);
	runManager->SetUserAction(trackingAction);
	
	runManager->Initialize();
	
	
	// Instantiation and initialization of the Visualization Manager
	#ifdef G4VIS_USE
	// visualization manager
	G4VisManager* visManager = new G4VisExecutive;
	visManager->Initialize();
	#endif

	detector->Construct();
	
	G4UImanager* UI = G4UImanager::GetUIpointer();
	G4String command = "/control/execute ";
	G4String fileName = argv[1];
	UI->ApplyCommand(command+fileName); 
	
	
	// Job termination
	#ifdef G4VIS_USE
	delete visManager;
	#endif

	delete runManager;
	return 0;
 }

 //----- end of C++