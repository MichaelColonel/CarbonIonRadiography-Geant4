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

#pragma once

#include <G4VUserDetectorConstruction.hh>

#include "CIR_Defines.hh"
#include <trec_strip_geometry.hh>
//#include "CIR_StripGeometry.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
class G4Material;
class G4Region;
class G4Box;

/// Detector construction class to define materials and geometry.

namespace CarbonIonRadiography {

class ParallelWorld;
class DetectorMessenger;

class DetectorConstruction : public G4VUserDetectorConstruction {

public:
	DetectorConstruction();
	virtual ~DetectorConstruction();

public:
	virtual G4VPhysicalVolume* Construct();
	virtual void ConstructSDandField();
	void UpdateGeometry();

private:
	void ConstructWorld();
	void ConstructSiliconPlane( G4int pos, const TREC::StripGeometryPair&);
	void ConstructCalorimeter(G4double offset_z);
	void ConstructSiliconDetectors();
	void ConstructObject();

	DetectorMessenger* detectorMessenger; // for future use
	ParallelWorld* parallelWorld;

	G4Box* worldBox;
	G4LogicalVolume* worldLogicalVolume;
	G4VPhysicalVolume* worldPhysicalVolume;
	G4Material* worldMaterial;

	G4Box* calorimeterBox;
	G4LogicalVolume* calorimeterLogicalVolume; 
	G4VPhysicalVolume* calorimeterPhysicalVolume;
	G4Material *calorimeterMaterial;
	G4Region *calorimeterRegion;

	std::vector<G4Box*> siliconBoxes;
	std::vector<G4LogicalVolume*> siliconLogicalVolumes; 
	std::vector<G4VPhysicalVolume*> siliconPhysicalVolumes;
	G4Material* siliconMaterial;
	std::vector<G4Region*> siliconRegions;

	G4double worldSizeX;
	G4double worldSizeY;
	G4double worldSizeZ;

	G4double calorimeterSizeX;
	G4double calorimeterSizeY;
	G4double calorimeterSizeZ;

	G4double siliconSizeX;
	G4double siliconSizeY;
	G4double siliconSizeZ;
};

} // namespace CarbonIonRadiography
