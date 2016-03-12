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

#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4Region.hh>
#include <G4PVPlacement.hh>
#include <G4PVReplica.hh>
#include <G4ThreeVector.hh>
#include <G4SystemOfUnits.hh>
#include <G4SDManager.hh>

#include <G4MultiFunctionalDetector.hh>
#include <G4PSEnergyDeposit.hh>
#include <G4SDManager.hh>
#include <G4VisAttributes.hh>

#include "CIR_GlobalStrings.hh"
#include "CIR_Defines.hh"
//#include "CIR_StripGeometry.hh"

#include "CIR_ParallelWorld.hh"

namespace {

G4int strips = CIR_NUMBER_OF_STRIPS_PER_SILICON;
G4double si_x = CIR_SIZE_SILICON * CLHEP::mm / 2.0; // half size
G4double si_y = si_x;  // half size
G4double pitch_x = si_x / strips;  // half size
G4double pitch_y = pitch_x;  // half size
G4double si_z = CIR_SIZE_SILICON_THICKNESS * CLHEP::um / 2.0;  // half size

G4double calo_x = CIR_SIZE_CALORIMETER * CLHEP::mm / 2.0;  // half size
G4double calo_y = calo_x;  // half size
G4double calo_z = CIR_SIZE_CALORIMETER_THICKNESS * CLHEP::mm / 2.0; // half size
G4double calo_slice_z = CIR_SIZE_CALORIMETER_SLICE_THICKNESS * CLHEP::um / 2.0; // half size
G4int calo_slices = calo_z / calo_slice_z;

} // namespace

namespace CarbonIonRadiography {

ParallelWorld::ParallelWorld(const G4String& worldName)
	:
	G4VUserParallelWorld(worldName),
	siliconYPitchSizeX(si_x), // half size
	siliconYPitchSizeY(pitch_y), // half size
	siliconYPitchSizeZ(si_z), // half size
	numberOfSiliconStripsAlongX(strips),
	numberOfSiliconStripsAlongY(strips),
	calorimeterSizeX(calo_x), // half size
	calorimeterSizeY(calo_y), // half size
	calorimeterSizeZ(calo_z), // half size
	sizeOfCalorimeterVoxelAlongX(calo_x), // half size
	sizeOfCalorimeterVoxelAlongY(calo_y), // half size
	sizeOfCalorimeterVoxelAlongZ(calo_slice_z), // half size
	numberOfCalorimeterVoxelsAlongZ(calo_slices),
	ghostWorld(0)
{
}

ParallelWorld::~ParallelWorld()
{
}

void
ParallelWorld::Construct()
{
	// World
	ghostWorld = GetWorld();
	TREC::StripGeometryMap map = TREC::StripGeometry::create();

	for ( TREC::StripGeometryMap::iterator it = map.begin();
		it != map.end(); ++it) {

		G4int pos = TREC::StripGeometry::index(it->first);
		TREC::StripGeometryPair& pair = it->second;

		ConstructSiliconPlane( pos, pair);
	}

	const TREC::StripGeometry* V = TREC::StripGeometry::get(TREC::MSD__V);

	ConstructCalorimeter(V->z + calo_z + 10 * CLHEP::mm);
}

void
ParallelWorld::ConstructSiliconPlane( G4int, const TREC::StripGeometryPair& pair)
{
	const TREC::StripGeometry& geom = pair.first;
	const TREC::StripGeometryNames& names = pair.second;

	// Phantom Geometry 
	G4Box* siliconParallel = new G4Box(
		names.parallel_body_name,
		si_x, 
		si_y,
		si_z);

	G4LogicalVolume* siliconLogParallel = new G4LogicalVolume(
		siliconParallel,
		0,
		names.parallel_logical_name);

	G4RotationMatrix* matrix = new G4RotationMatrix();
	matrix->rotateZ(geom.angle);

	G4VPhysicalVolume* siliconPhysParallel = new G4PVPlacement(
		matrix,
		G4ThreeVector( 0, 0, geom.z),
		names.parallel_physical_name,
		siliconLogParallel,
		ghostWorld,
		false,
		0);

	G4Box* siliconDivisionParallel = new G4Box(
		names.body_devision_name,
		siliconYPitchSizeX, // 3 cm
		siliconYPitchSizeY, // 100 um
		siliconYPitchSizeZ); // 150 um
 
	G4LogicalVolume* siliconLogDivisionParallel = new G4LogicalVolume(
		siliconDivisionParallel,
		0,
		names.logical_devision_name); // Sensitive Logical Detector Name

	new G4PVReplica(
		names.physical_devision_name,
		siliconLogDivisionParallel,
		siliconPhysParallel,
		kYAxis,
		numberOfSiliconStripsAlongY,
		pitch_y * 2); // pitch size
}

void
ParallelWorld::ConstructCalorimeter(G4double offset_z)
{
	// Phantom Geometry 
	G4Box* CalorimeterBoxParallel = new G4Box(
		CalorimeterParallelStr, 
		calo_x, // half size 
		calo_y, // half size 
		calo_z); // half size

	G4LogicalVolume* CalorimeterLogParallel = new G4LogicalVolume(
		CalorimeterBoxParallel,
		0,
		CalorimeterLogParallelStr);

	G4VPhysicalVolume* CalorimeterPhysParallel = new G4PVPlacement(
		0,
		G4ThreeVector( 0, 0, offset_z),
		CalorimeterPhysParallelStr,
		CalorimeterLogParallel,
		ghostWorld,
		false,
		0);

	G4Box* CalorimeterBoxDivisionParallel = new G4Box(
		CalorimeterDivisionParallelStr,
		sizeOfCalorimeterVoxelAlongX, // half size
		sizeOfCalorimeterVoxelAlongY, // half size 
		sizeOfCalorimeterVoxelAlongZ); // half size
 
	G4LogicalVolume* CalorimeterLogDivisionParallel = new G4LogicalVolume(
		CalorimeterBoxDivisionParallel,
		0,
		CalorimeterLogDivisionParallelStr // Sensitive Logical Calorimemter Name
		);

	new G4PVReplica(
		CalorimeterPhysDivisionParallelStr,
		CalorimeterLogDivisionParallel,
		CalorimeterPhysParallel,
		kZAxis,
		numberOfCalorimeterVoxelsAlongZ, // number of slices
		sizeOfCalorimeterVoxelAlongZ * 2.); // size of a slice along Z axis
}

void
ParallelWorld::ConstructSD()
{
	G4SDManager* sensitiveDetectorManager = G4SDManager::GetSDMpointer();

	TREC::StripGeometryMap map = TREC::StripGeometry::create();
	// Sensitive detectors for silicon planes
	for ( TREC::StripGeometryMap::iterator it = map.begin();
		it != map.end(); ++it) {
		TREC::StripGeometryPair& pair = it->second;
		TREC::StripGeometryNames& names = pair.second;

		std::string& fuctional = names.functional_detector_name;
		std::string& sensitive = names.sensitive_detector_name;
		std::string& logical = names.logical_devision_name;
//		std::string& logical = names.parallel_logical_name;

		G4MultiFunctionalDetector* detector = new G4MultiFunctionalDetector(fuctional);
		G4VPrimitiveScorer* primitive = new G4PSEnergyDeposit("EnergyDeposit");
		detector->RegisterPrimitive(primitive);

		if (!sensitiveDetectorManager->FindSensitiveDetector( sensitive, true)) {
			G4cout << "Registering new DetectorSD \"" << sensitive << "\""<< G4endl;
			sensitiveDetectorManager->AddNewDetector(detector);
		}
		SetSensitiveDetector( logical, detector);
	}

	// Sensitive detector for PS calorimeter
	G4String sensitive = G4String(CalorimeterSensitiveDetectorStr);
	G4MultiFunctionalDetector* detector = new G4MultiFunctionalDetector(CalorimeterStr);

	G4VPrimitiveScorer* primitive = new G4PSEnergyDeposit("EnergyDeposit");
	detector->RegisterPrimitive(primitive);

	if (!sensitiveDetectorManager->FindSensitiveDetector( sensitive, true)) {
		G4cout << "Registering new DetectorSD \"" << sensitive << "\""<< G4endl;
		sensitiveDetectorManager->AddNewDetector(detector);
	}
	SetSensitiveDetector( CalorimeterLogDivisionParallelStr, detector);
}

} // namespace CarbonIonRadiography
