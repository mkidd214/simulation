#ifndef PhysicsList_hh
#define PhysicsList_hh

//#include "G4VUserPhysicsList.hh"
//#include "globals.hh"
#include "G4VModularPhysicsList.hh"

class PhysicsList: public G4VModularPhysicsList
{
  public:
  PhysicsList();

  virtual ~PhysicsList();
  // Construct particle and physics process
  void ConstructParticle();
  void ConstructProcess();
  virtual  void SetCuts();
};


#endif