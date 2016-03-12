/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

#include <G4Element.hh>
#include <G4Material.hh>
#include <G4MaterialTable.hh>
#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4Sphere.hh>
#include <G4LogicalVolume.hh>
#include <G4VPhysicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4PVReplica.hh>
#include <G4SubtractionSolid.hh>
#include <G4GeometryManager.hh>
#include <G4RunManager.hh>
#include <G4ProductionCuts.hh>

#include <G4UserLimits.hh>
#include <G4UnitsTable.hh>

#include <G4NistManager.hh>

#include <G4VisAttributes.hh>
#include <G4Colour.hh>

#include <G4ios.hh>

#include <cstdio>

#include "CIR_GlobalStrings.hh"
#include "CIR_ParallelWorld.hh"
//#include "CIR_StripGeometry.hh"
#include "CIR_DetectorConstruction.hh"

#define UNIFORM_BOX_SIZE 15.0

namespace {

const G4double world_x = CIR_SIZE_WORLD * CLHEP::mm / 2.0;
const G4double world_y = CIR_SIZE_WORLD * CLHEP::mm / 2.0;
const G4double world_z = CIR_SIZE_WORLD * CLHEP::mm / 2.0;

const G4double calo_x = CIR_SIZE_CALORIMETER * CLHEP::mm / 2.0;
const G4double calo_y = CIR_SIZE_CALORIMETER * CLHEP::mm / 2.0;
const G4double calo_z = CIR_SIZE_CALORIMETER_THICKNESS * CLHEP::mm / 2.0;

const G4double si_x = CIR_SIZE_SILICON * CLHEP::mm / 2.0;
const G4double si_y = CIR_SIZE_SILICON * CLHEP::mm / 2.0;
const G4double si_z = CIR_SIZE_SILICON_THICKNESS * CLHEP::um / 2.0;

} // namespace

namespace CarbonIonRadiography {

DetectorConstruction::DetectorConstruction()
	:
	detectorMessenger(0),
	parallelWorld(0),
	worldBox(0),
	worldLogicalVolume(0),
	worldPhysicalVolume(0),
	worldMaterial(0),
	calorimeterBox(0),
	calorimeterLogicalVolume(0), 
	calorimeterPhysicalVolume(0),
	calorimeterMaterial(0),
	calorimeterRegion(0),
	siliconBoxes(CIR_NUMBER_OF_SILICON_DETECTORS),
	siliconLogicalVolumes(CIR_NUMBER_OF_SILICON_DETECTORS),
	siliconPhysicalVolumes(CIR_NUMBER_OF_SILICON_DETECTORS),
	siliconMaterial(0),
	siliconRegions(CIR_NUMBER_OF_SILICON_DETECTORS),
	worldSizeX(world_x), // half size
	worldSizeY(world_y), // half size
	worldSizeZ(world_z), // half size
	calorimeterSizeX(calo_x), // half size
	calorimeterSizeY(calo_y), // half size
	calorimeterSizeZ(calo_z), // half size
	siliconSizeX(si_x), // half size
	siliconSizeY(si_y), // half size
	siliconSizeZ(si_z) // half size
{
	// parallel world for phantom
	parallelWorld = new ParallelWorld(ParallelWorldStr);

	RegisterParallelWorld(parallelWorld);
}

DetectorConstruction::~DetectorConstruction()
{
	for ( std::vector<G4Region*>::iterator iter = siliconRegions.begin();
		iter != siliconRegions.end(); ++iter) {
		delete *iter;
	}
}

G4VPhysicalVolume*
DetectorConstruction::Construct()
{
	//--------- Material definition ---------
	G4NistManager* man = G4NistManager::Instance();
	man->SetVerbose(0);
	worldMaterial = man->FindOrBuildMaterial("G4_Galactic");
//	worldMaterial = man->FindOrBuildMaterial("G4_AIR");
	siliconMaterial = man->FindOrBuildMaterial("G4_Si");
	calorimeterMaterial = man->FindOrBuildMaterial("G4_POLYSTYRENE");

	ConstructWorld();
	ConstructSiliconDetectors();
	ConstructObject();

	const TREC::StripGeometry* V = TREC::StripGeometry::get(TREC::MSD__V);
	ConstructCalorimeter(V->z + calorimeterSizeZ + 10 * CLHEP::mm);

	return worldPhysicalVolume;
}

void
DetectorConstruction::ConstructWorld()
{
	//---------------- Volume definition
	worldBox = new G4Box( WorldStr, worldSizeX, worldSizeY, worldSizeZ); 
	worldLogicalVolume = new G4LogicalVolume( worldBox, worldMaterial, WorldLogStr);
	worldPhysicalVolume = new G4PVPlacement(
		0, 
		G4ThreeVector(),
		WorldPhysStr,
		worldLogicalVolume,
		0,
		false,
		0);

	worldLogicalVolume->SetVisAttributes(G4VisAttributes::Invisible);
/*
	G4VisAttributes* attr = new G4VisAttributes(G4Colour( 1.0, 1.0, 1.0));
	attr->SetVisibility(true);
	attr->SetForceWireframe(true);
	worldLogicalVolume->SetVisAttributes(attr);
*/
}

void
DetectorConstruction::ConstructSiliconDetectors()
{
	TREC::StripGeometryMap map = TREC::StripGeometry::create();
	for ( TREC::StripGeometryMap::iterator it = map.begin();
		it != map.end(); ++it) {
		G4int pos = TREC::StripGeometry::index(it->first);
		TREC::StripGeometryPair& pair = it->second;

		ConstructSiliconPlane( pos, pair);
	}
}

void
DetectorConstruction::ConstructSiliconPlane( G4int pos,
	const TREC::StripGeometryPair& pair)
{
	const TREC::StripGeometry& geom = pair.first;
	const TREC::StripGeometryNames& names = pair.second;

	//---------------- Volume definition --- Silicon plane
	siliconBoxes[pos] = new G4Box( names.body_name,
		siliconSizeX, siliconSizeY, siliconSizeZ);

	siliconLogicalVolumes[pos] = new G4LogicalVolume(
		siliconBoxes[pos],
		siliconMaterial,
		names.logical_name);

	G4RotationMatrix* matrix = new G4RotationMatrix();
	matrix->rotateZ(geom.angle);
//	siliconPhysParallel->SetRotation(matrix);

	siliconPhysicalVolumes[pos] = new G4PVPlacement(
		matrix,
		G4ThreeVector( 0, 0, geom.z),
		names.physical_name,
		siliconLogicalVolumes[pos],
		worldPhysicalVolume,
		false,
		0);

	G4VisAttributes* attr = new G4VisAttributes(G4Colour::Green());
	attr->SetVisibility(true);
	attr->SetForceWireframe(true);
	siliconLogicalVolumes[pos]->SetVisAttributes(attr);

/*
	// **************
	// Cut per Region
	// **************
  
	// A smaller cut is fixed in the calorimeter to calculate
	// the energy deposit with the required accuracy

	if (!siliconRegions[pos]) {
		siliconRegions[pos] = new G4Region(names.logical_name);
		siliconLogicalVolumes[pos]->SetRegion(siliconRegions[pos]);
		siliconRegions[pos]->AddRootLogicalVolume(siliconLogicalVolumes[pos]);

		G4double cut = 150. * CLHEP::um;
		G4ProductionCuts* cuts = new G4ProductionCuts;
		cuts->SetProductionCut( cut, G4ProductionCuts::GetIndex("gamma"));
		cuts->SetProductionCut( cut, G4ProductionCuts::GetIndex("e-"));
		cuts->SetProductionCut( cut, G4ProductionCuts::GetIndex("e+"));
		siliconRegions[pos]->SetProductionCuts(cuts);
    }
*/
}
/*
void
DetectorConstruction::ConstructObject()
{
	//--------- Material definition ---------
	G4NistManager* man = G4NistManager::Instance();
	man->SetVerbose(0);
	G4Material* waterMaterial = man->FindOrBuildMaterial("G4_WATER");
	G4Material* objMaterial = man->FindOrBuildMaterial("G4_MUSCLE_STRIATED_ICRU");

	//---------------- Volume definition ------- Objects

	G4Box* WatObj = new G4Box( "WaterObj", 3 * CLHEP::cm, 3 * CLHEP::cm, 12.5 * CLHEP::cm);
	G4Box* MusObj = new G4Box( "MuscleObj", 1.5 * CLHEP::cm, 1.5 * CLHEP::cm, 0.5 * CLHEP::cm);
 
	G4SubtractionSolid* WatSolid = new G4SubtractionSolid( "Solid",
		WatObj, MusObj);

	G4LogicalVolume* MusLogicalVolume = new G4LogicalVolume(
		MusObj, objMaterial, "MuscleLog");

	G4LogicalVolume* WatLogicalVolume = new G4LogicalVolume(
		WatSolid, waterMaterial, "WatSolidLog");

	G4PVPlacement* WatPhysicalVolume = new G4PVPlacement(
		0,
		G4ThreeVector( 0.0, 0.0, 80.0 * CLHEP::cm),
		"SolidSpherePhys",
		WatLogicalVolume,
		worldPhysicalVolume,
		false,
		0);

	new G4PVPlacement(
		0,
		G4ThreeVector( 0.0 * CLHEP::cm, 0.0 * CLHEP::cm, 0.0 * CLHEP::cm),
		"SolidTubePhys",
		MusLogicalVolume,
		WatPhysicalVolume,
		false,
		0);

	G4VisAttributes* attr = new G4VisAttributes(G4Colour::Blue());
	attr->SetVisibility(true);
	attr->SetForceWireframe(true);
	WatLogicalVolume->SetVisAttributes(attr);

	G4VisAttributes* attr1 = new G4VisAttributes(G4Colour::Cyan());
	attr1->SetVisibility(true);
	attr1->SetForceWireframe(true);
	MusLogicalVolume->SetVisAttributes(attr1);
}
*/
/*
void
DetectorConstruction::ConstructObject()
{
	//--------- Material definition ---------
	G4NistManager* man = G4NistManager::Instance();
	man->SetVerbose(0);
	G4Material* waterMaterial = man->FindOrBuildMaterial("G4_WATER");
	G4Material* pmmaMaterial = man->FindOrBuildMaterial("G4_PLEXIGLASS");
	G4Material* kaptonMaterial = man->FindOrBuildMaterial("G4_KAPTON");

	//---------------- Volume definition ------- Objects

	G4Sphere* Sphere = new G4Sphere( "Sphere",
		0.5 * CLHEP::cm, 
		2.0 * CLHEP::cm,
		0. * CLHEP::deg, 
		360. * CLHEP::deg, 
		0. * CLHEP::deg, 
		180. * CLHEP::deg);
 
	G4Tubs* Tube = new G4Tubs( "Object",
		0.25 * CLHEP::cm,
		0.5 * CLHEP::cm,
		0.5 * CLHEP::cm,
		0 * CLHEP::deg,
		360 * CLHEP::deg
	);

	G4Tubs* TubeInside = new G4Tubs( "ObjectInside",
		0 * CLHEP::mm,
		0.7 * CLHEP::mm,
		0.2 * CLHEP::cm,
		0 * CLHEP::deg,
		360 * CLHEP::deg
	);

	G4SubtractionSolid* solidSphere = new G4SubtractionSolid( "SolidSphere",
		Sphere, Tube, 0, G4ThreeVector( 0.8 * CLHEP::cm, 0.8 * CLHEP::cm, 0 * CLHEP::cm));

	G4SubtractionSolid* solidTube = new G4SubtractionSolid( "SolidTube",
		Tube, TubeInside, 0, G4ThreeVector( 0.25 * CLHEP::cm, 0.25 * CLHEP::cm, 0 * CLHEP::cm));

	G4LogicalVolume* TubeLogicalVolume = new G4LogicalVolume(
		solidTube, waterMaterial, "SolidTubeLog");

	G4LogicalVolume* SphereLogicalVolume = new G4LogicalVolume(
		solidSphere, pmmaMaterial, "SolidSphereLog");

	G4PVPlacement* SpherePhysicalVolume = new G4PVPlacement(
		0,
		G4ThreeVector( 0.0, 0.0, 80.0 * CLHEP::cm),
		"SolidSpherePhys",
		SphereLogicalVolume,
		worldPhysicalVolume,
		false,
		0);

	G4PVPlacement* TubePhysicalVolume = new G4PVPlacement(
		0,
		G4ThreeVector( 0.8 * CLHEP::cm, 0.8 * CLHEP::cm, 0 * CLHEP::cm),
		"SolidTubePhys",
		TubeLogicalVolume,
		SpherePhysicalVolume,
		false,
		0);

	G4LogicalVolume* TubeInsideLogicalVolume = new G4LogicalVolume(
		TubeInside, kaptonMaterial, "TubeInsideLog");

	G4PVPlacement* TubeInsidePhysicalVolume = new G4PVPlacement(
		0,
		G4ThreeVector( 0.25 * CLHEP::cm, 0.25 * CLHEP::cm, 0. * CLHEP::cm),
		"TubeInsidePhys",
		TubeInsideLogicalVolume,
		TubePhysicalVolume,
		false,
		0);

	G4VisAttributes* attr = new G4VisAttributes(G4Colour::Blue());
	attr->SetVisibility(true);
	attr->SetForceWireframe(true);
	TubeLogicalVolume->SetVisAttributes(attr);

	G4VisAttributes* attr1 = new G4VisAttributes(G4Colour::Cyan());
	attr1->SetVisibility(true);
	attr1->SetForceWireframe(true);
	TubeInsideLogicalVolume->SetVisAttributes(attr1);

	G4VisAttributes* attr2 = new G4VisAttributes(G4Colour::Yellow());
	attr2->SetVisibility(true);
	attr2->SetForceWireframe(true);
	SphereLogicalVolume->SetVisAttributes(attr2);
}
*/
/*
void
DetectorConstruction::ConstructObject()
{
	//---------------- Volume definition ------- Objects
	G4Box* box = new G4Box( "Object",
		calorimeterSizeX, calorimeterSizeY, UNIFORM_BOX_SIZE * CLHEP::cm);
 
	G4LogicalVolume* boxLog = new G4LogicalVolume( box,
		calorimeterMaterial, "ObjectLog");

	new G4PVPlacement(
		0,
		G4ThreeVector( 0, 0, 80 * CLHEP::cm),
		"ObjectPhys",
		boxLog,
		worldPhysicalVolume,
		false,
		0);


	G4VisAttributes* attr = new G4VisAttributes(G4Colour::Blue());
	attr->SetVisibility(true);
	attr->SetForceWireframe(true);
	boxLog->SetVisAttributes(attr);
}
*/

void
DetectorConstruction::ConstructObject()
{
	//--------- Material definition ---------
	G4NistManager* man = G4NistManager::Instance();
	man->SetVerbose(0);
	G4Material* pmmaMaterial = man->FindOrBuildMaterial("G4_PLEXIGLASS");
	G4Material* airMaterial = man->FindOrBuildMaterial("G4_AIR");
	G4Material* lungMaterial = man->FindOrBuildMaterial("G4_LUNG_ICRP");
	G4Material* boneMaterial = man->FindOrBuildMaterial("G4_BONE_CORTICAL_ICRP");
//	G4Material* boneMaterial = man->FindOrBuildMaterial("G4_Pb");


	//---------------- Volume definition ------- Objects

	G4Tubs* TubePMMA = new G4Tubs( "TubePMMA",
		0. * CLHEP::cm,
		2.5 * CLHEP::cm,
		2.0 * CLHEP::cm,
		0 * CLHEP::deg,
		360 * CLHEP::deg
	);

	G4Tubs* TubeAIR1 = new G4Tubs( "TubeINSIDE",
		0. * CLHEP::cm,
		0.5 * CLHEP::cm,
		2.0 * CLHEP::cm,
		0 * CLHEP::deg,
		360 * CLHEP::deg
	);
	G4VSolid* TubeAIR2 = TubeAIR1->Clone();
	G4VSolid* TubeLUNG = TubeAIR1->Clone();
	G4VSolid* TubeBONE = TubeAIR1->Clone();
/*
	G4SubtractionSolid* sub1 = new G4SubtractionSolid( "Sub1",
		TubePMMA, TubeAIR1, 0, G4ThreeVector( -1.25 * CLHEP::cm, 0.0 * CLHEP::cm, 0.0 * CLHEP::cm));

	G4SubtractionSolid* sub2 = new G4SubtractionSolid( "Sub2",
		TubePMMA, TubeAIR2, 0, G4ThreeVector( 1.25 * CLHEP::cm, 0.0 * CLHEP::cm, 0.0 * CLHEP::cm));

	G4SubtractionSolid* sub3 = new G4SubtractionSolid( "Sub3",
		TubePMMA, TubeLUNG, 0, G4ThreeVector( 0.0 * CLHEP::cm, -1.75 * CLHEP::cm, 0.0 * CLHEP::cm));

	G4SubtractionSolid* sub4 = new G4SubtractionSolid( "Sub4",
		TubePMMA, TubeBONE, 0, G4ThreeVector( 0.0 * CLHEP::cm, 1.75 * CLHEP::cm, 0.0 * CLHEP::cm));

	G4LogicalVolume* pmmaLogicalVolume = new G4LogicalVolume(
		sub4, pmmaMaterial, "PMMALog");
*/

	G4LogicalVolume* pmmaLogicalVolume = new G4LogicalVolume(
		TubePMMA, pmmaMaterial, "PMMALog");

	G4LogicalVolume* air1LogicalVolume = new G4LogicalVolume(
		TubeAIR1, airMaterial, "AIR1Log");

	G4LogicalVolume* air2LogicalVolume = new G4LogicalVolume(
		TubeAIR2, airMaterial, "AIR2Log");

	G4LogicalVolume* lungLogicalVolume = new G4LogicalVolume(
		TubeLUNG, lungMaterial, "LUNGLog");

	G4LogicalVolume* boneLogicalVolume = new G4LogicalVolume(
		TubeBONE, boneMaterial, "BONELog");

	G4PVPlacement* pmmaPhysicalVolume = new G4PVPlacement(
		0,
		G4ThreeVector( 0.0, 0.0, 80.0 * CLHEP::cm),
		"PMMAPhys",
		pmmaLogicalVolume,
		worldPhysicalVolume,
		false,
		0);

	G4PVPlacement* air1PhysicalVolume = new G4PVPlacement(
		0,
		G4ThreeVector( -1.25 * CLHEP::cm, 0.0 * CLHEP::cm, 0.0 * CLHEP::cm),
		"AIR1Phys",
		air1LogicalVolume,
		pmmaPhysicalVolume,
		false,
		0);

	G4PVPlacement* air2PhysicalVolume = new G4PVPlacement(
		0,
		G4ThreeVector( 1.25 * CLHEP::cm, 0.0 * CLHEP::cm, 0.0 * CLHEP::cm),
		"AIR2Phys",
		air2LogicalVolume,
		pmmaPhysicalVolume,
		false,
		0);

	G4PVPlacement* lungPhysicalVolume = new G4PVPlacement(
		0,
		G4ThreeVector( 0.0 * CLHEP::cm, -1.75 * CLHEP::cm, 0.0 * CLHEP::cm),
		"LUNGPhys",
		lungLogicalVolume,
		pmmaPhysicalVolume,
		false,
		0);

	G4PVPlacement* bonePhysicalVolume = new G4PVPlacement(
		0,
		G4ThreeVector( 0.0 * CLHEP::cm, 1.75 * CLHEP::cm, 0.0 * CLHEP::cm),
		"BONEPhys",
		boneLogicalVolume,
		pmmaPhysicalVolume,
		false,
		0);

	G4RotationMatrix* matrix = new G4RotationMatrix();
	matrix->rotateX(90.0 * CLHEP::deg);
	matrix->rotateZ(21.0 * CLHEP::deg);
	pmmaPhysicalVolume->SetRotation(matrix);

	G4VisAttributes* attr = new G4VisAttributes(G4Colour::Blue());
	attr->SetVisibility(true);
	attr->SetForceWireframe(true);
	pmmaLogicalVolume->SetVisAttributes(attr);

	G4VisAttributes* attr1 = new G4VisAttributes(G4Colour::Cyan());
	attr1->SetVisibility(true);
	attr1->SetForceWireframe(true);
	air1LogicalVolume->SetVisAttributes(attr1);
	air2LogicalVolume->SetVisAttributes(attr1);

	G4VisAttributes* attr2 = new G4VisAttributes(G4Colour::Yellow());
	attr2->SetVisibility(true);
	attr2->SetForceWireframe(true);
	lungLogicalVolume->SetVisAttributes(attr2);


	G4VisAttributes* attr3 = new G4VisAttributes(G4Colour::Red());
	attr3->SetVisibility(true);
	attr3->SetForceWireframe(true);
	boneLogicalVolume->SetVisAttributes(attr3);
}

void
DetectorConstruction::ConstructCalorimeter(G4double offset_z)
{
	//---------------- Volume definition ------ Calorimeter
	calorimeterBox = new G4Box( CalorimeterStr,
		calorimeterSizeX, calorimeterSizeY, calorimeterSizeZ);
 
	calorimeterLogicalVolume = new G4LogicalVolume( calorimeterBox,
		calorimeterMaterial, CalorimeterLogStr);

	calorimeterPhysicalVolume = new G4PVPlacement(
		0,
		G4ThreeVector( 0, 0, offset_z),
		CalorimeterPhysStr,
		calorimeterLogicalVolume,
		worldPhysicalVolume,
		false,
		0);

	G4VisAttributes* attr = new G4VisAttributes(G4Colour::Blue());
	attr->SetVisibility(true);
	attr->SetForceWireframe(true);
	calorimeterLogicalVolume->SetVisAttributes(attr);

	// **************
	// Cut per Region
	// **************
  
	// A smaller cut is fixed in the calorimeter to calculate
	// the energy deposit with the required accuracy
	if (!calorimeterRegion) {
		calorimeterRegion = new G4Region(CalorimeterLogStr);
		calorimeterLogicalVolume->SetRegion(calorimeterRegion);
		calorimeterRegion->AddRootLogicalVolume(calorimeterLogicalVolume);

		G4double cut = 1.5 * CLHEP::mm;
		G4ProductionCuts* cuts = new G4ProductionCuts;
		cuts->SetProductionCut( cut, G4ProductionCuts::GetIndex("gamma"));
		cuts->SetProductionCut( cut, G4ProductionCuts::GetIndex("e-"));
		cuts->SetProductionCut( cut, G4ProductionCuts::GetIndex("e+"));
		calorimeterRegion->SetProductionCuts(cuts);
    }
}

void
DetectorConstruction::ConstructSDandField()
{
}

void
DetectorConstruction::UpdateGeometry()
{
	G4cout << "Update" << G4endl;
}

} // namespace CarbonIonRadiography
