/// \file B1DetectorConstruction.cc
/// \brief Implementation of the B1DetectorConstruction class

#include "DetectorConstruction.hh"
//#include "SteppingAction.hh"
   // use of stepping action to set the accounting volume

#include "G4RunManager.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4Tubs.hh"
#include "G4Isotope.hh"
#include "G4Element.hh"
#include "G4Material.hh"
#include "G4UnitsTable.hh"

#include "G4OpticalSurface.hh"
#include "G4MaterialPropertyVector.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"

DetectorConstruction::DetectorConstruction()
: G4VUserDetectorConstruction()
{ }

DetectorConstruction::~DetectorConstruction()
{ }

G4VPhysicalVolume* DetectorConstruction::Construct()
{	
	//Option to switch on/off checking of volumes overlaps
	G4bool checkOverlaps = true;

// ------------- Materials -------------

	G4double a, z, density;
	G4int nelements;

	// Air
	// 
	G4Element* N = new G4Element("Nitrogen","N",z=7,a=14.01*g/mole);
	G4Element* O = new G4Element("Oxygen","O",z=8,a=16.00*g/mole);

	G4Material* Air = new G4Material("Air",density=1.29*mg/cm3,nelements=2);
	Air->AddElement(N,70.*perCent);
	Air->AddElement(O,30.*perCent);

	// CsI
	// 
	G4Element* Cs = new G4Element("Cesium","Cs",z=55,a=132.91*g/mole);
	G4Element* I = new G4Element("Iodine","I",z=53,a=126.90*g/mole);

	G4Material* CesiumIodide = new G4Material("CesiumIodide",density=4.51*g/cm3,nelements=2);
	CesiumIodide->AddElement(Cs,1);
	CesiumIodide->AddElement(I,1);
	
	// Al
	//
	G4Element* Al = new G4Element("Aluminum","Al",z=13,a=26.98*g/mole);
	
	G4Material* Aluminum = new G4Material("Aluminum",density=2.70*g/cm3,nelements=1);
	Aluminum->AddElement(Al,1);
	
// ------------ Generate & Add Material Properties Table ------------

	const G4int nEntries = 5;
	
	G4double PhotonEnergy[nEntries] = {2.761*eV,2.485*eV,2.258*eV,2.071*eV,1.911*eV};

// Cesium iodide
	
	G4double RefractiveIndexCsI[nEntries] = {1.79,1.79,1.79,1.79,1.79};
	G4double AbsorptionCsI[nEntries] = {39.0*m,39.0*m,39.0*m,39.0*m,39.0*m};
	G4double ScintilFast[nEntries] = {1.00,1.00,1.00,1.00,1.00};
	G4double ScintilSlow[nEntries] = {0.01,1.00,2.00,3.00,4.00};
	
	G4MaterialPropertiesTable* mptCsI = new G4MaterialPropertiesTable();
	
	mptCsI->AddProperty("RINDEX",PhotonEnergy,RefractiveIndexCsI,nEntries)->SetSpline(true);
	mptCsI->AddProperty("ABSLENGTH",PhotonEnergy,AbsorptionCsI,nEntries)->SetSpline(true);
	mptCsI->AddProperty("FASTCOMPONENT",PhotonEnergy,ScintilFast,nEntries)->SetSpline(true);
	mptCsI->AddProperty("SLOWCOMPONENT",PhotonEnergy,ScintilSlow,nEntries)->SetSpline(true);
	
	mptCsI->AddConstProperty("SCINTILLATIONYIELD",0.01/keV);  //SHOULD BE 54 // used this to turn off OP for a while
	mptCsI->AddConstProperty("RESOLUTIONSCALE",1.0);
	mptCsI->AddConstProperty("FASTTIMECONSTANT",1.*ns);
	mptCsI->AddConstProperty("SLOWTIMECONSTANT",100.*ns);
	mptCsI->AddConstProperty("YIELDRATIO",0.45);
	
	// Energy and MIE stuff??
	
	CesiumIodide->SetMaterialPropertiesTable(mptCsI);
	
// Air
	
	G4double RefractiveIndexAir[nEntries] = {1.00,1.00,1.00,1.00,1.00};
	
	G4MaterialPropertiesTable* mptAir = new G4MaterialPropertiesTable();
	mptAir->AddProperty("RINDEX",PhotonEnergy,RefractiveIndexAir,nEntries);
	
	Air->SetMaterialPropertiesTable(mptAir);
	
// ------------- Volumes --------------
	
// The world
	
	G4double worldx = 100.0*cm;
	G4double worldy = 100.0*cm;
	G4double worldz = 100.0*cm;
	
	G4Box* solidWorld = new G4Box("World",worldx,worldy,worldz);
	
	G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld,Air,"World",0,0,0);
	
	G4VPhysicalVolume* physWorld = new G4PVPlacement(0,G4ThreeVector(),logicWorld,"World",0,false,0);
	
	
// The cylinders
	
	G4double innerRadius = 0.0*um;
	G4double outerRadius = 10*um;
//	G4double hz = 45*um;
	G4double hz = 150*um;
	G4double startAngle = 0.0*deg;
	G4double spanningAngle = 360.0*deg;
	
	G4Tubs* solidCylinder = new G4Tubs("Cylinder",innerRadius,outerRadius,hz,startAngle,spanningAngle);
	
	G4LogicalVolume* logicCylinder = new G4LogicalVolume(solidCylinder,CesiumIodide,"Cylinder");
	
	G4VPhysicalVolume* physCylinder = new G4PVPlacement(0,G4ThreeVector(),logicCylinder,"Cylinder",0,false,0,checkOverlaps);
	
	G4ThreeVector position = G4ThreeVector();
	new G4PVPlacement(0,position,logicCylinder,"cylinder",logicWorld,false,0);
	
	//do these loops for all the cylinders
	for (int i = -5; i < 5; i++) {
		for (int j = -5; j < 5; j++) {
			position = G4ThreeVector(20*i*um,20*j*um,0.0);
			new G4PVPlacement(0,position,logicCylinder,"CsI",logicWorld,false,0);
		}
	}
	
// The camera
	
	G4double cameraInner = 0.0*mm;
	G4double cameraOuter = 1.0*mm;
	G4double camerahz = 20*um;
	G4double cameraStartAngle = 0.0*deg;
	G4double cameraSpanningAngle = 360.0*deg;
	
	G4Tubs* solidCamera = new G4Tubs("Camera",cameraInner,cameraOuter,camerahz,cameraStartAngle,cameraSpanningAngle);
	
	G4LogicalVolume* logicCamera = new G4LogicalVolume(solidCamera,Aluminum,"Camera");
	
	G4VPhysicalVolume* physCamera = new G4PVPlacement(0,G4ThreeVector(),logicCamera,"Camera",0,false,0,checkOverlaps);
	
	G4ThreeVector cameraPosition = G4ThreeVector(0,0,-1.0*mm);
	new G4PVPlacement(0,cameraPosition,logicCamera,"camera",logicWorld,false,0);
	
	
// ------------- Surfaces --------------

	G4OpticalSurface* OpCsISurface = new G4OpticalSurface("CsISurface");
	OpCsISurface->SetType(dielectric_dielectric);
	OpCsISurface->SetFinish(polished);
	OpCsISurface->SetModel(glisur);
	
	new G4LogicalBorderSurface("CsISurface",physCylinder,physWorld,OpCsISurface);
	// maybe try it with G4LogicalSkinSurface
	
// Generate & Add Material Properties Table attached to the optical surface
	
	G4double RefractiveIndex[nEntries] = {1.79,1.79,1.79,1.79,1.79};
	G4double Backscatter[nEntries] = {0.0,0.0,0.0,0.0,0.0};
	
	G4MaterialPropertiesTable* mptSurface = new G4MaterialPropertiesTable();
	
	mptSurface->AddProperty("RINDEX",PhotonEnergy,RefractiveIndex,nEntries);
	mptSurface->AddProperty("BACKSCATTERCONSTANT",PhotonEnergy,Backscatter,nEntries);
	
	OpCsISurface->SetMaterialPropertiesTable(mptSurface);
	
	
        return physWorld;             
}