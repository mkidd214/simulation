#include "PrimaryGeneratorAction.hh"
//#include "gamma.hh"
#include "G4Event.hh"
#include "G4GeneralParticleSource.hh"


/*void PrimaryGeneratorAction::generateEnergies(int nEvents) {
	//acceptance-rejection method
	//double* energies = new double[nEvents];
	int i = 0;
	
	//energySpectrum = new G4Double[nEvents];
	
	while (i < nEvents) {
		double x_lower = 0;
		double x_upper = 156.5;
		double y_lower = 0; 
		double y_upper = 0.012;
		double function = 0;
		std::uniform_real_distribution<double> unif(x_lower,x_upper);
		std::default_random_engine re;
		double xrand = unif(re);
		std::uniform_real_distribution<double> unif2(y_lower,y_upper);
		std::default_random_engine re2;
		double yrand = unif(re2);
		function = (pow(xrand,2)+pow((2*xrand*511),0.5))*(pow(156.5-xrand,2))*((xrand+511)/135000000000);
		
		if (function <= yrand) {
// 			//energySpectrum[i] = xrand;
			i++;
		}
	}
	
	//return energies;
}*/


PrimaryGeneratorAction::PrimaryGeneratorAction()
: G4VUserPrimaryGeneratorAction(),
  particleGun(0)
{
	G4cout << "Constructing PrimaryGeneratorAction\n";
  
  particleGun = new G4GeneralParticleSource();
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  //delete particleGun;
  if (particleGun) delete particleGun;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  particleGun->GeneratePrimaryVertex(anEvent);
//  G4cout << "Generated primary vertex\n";
}

