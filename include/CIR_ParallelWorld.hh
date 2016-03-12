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

#include <G4VUserParallelWorld.hh>

//#include "CIR_StripGeometry.hh"
#include <trec_strip_geometry.hh>

class G4VPhysicalVolume;

namespace CarbonIonRadiography {

class ParallelWorld : public G4VUserParallelWorld {
public:
	ParallelWorld(const G4String& name);
	virtual ~ParallelWorld();
	virtual void Construct();
	virtual void ConstructSD();
	void UpdateGeometry();

private:

	void ConstructSiliconPlane( G4int pos, const TREC::StripGeometryPair&);
	void ConstructCalorimeter(G4double offset);

	G4double siliconYPitchSizeX; // full size
	G4double siliconYPitchSizeY; // full size
	G4double siliconYPitchSizeZ; // full size

	G4int numberOfSiliconStripsAlongX;
	G4int numberOfSiliconStripsAlongY;

	G4double calorimeterSizeX; // full size
	G4double calorimeterSizeY; // full size
	G4double calorimeterSizeZ; // full size

	G4double sizeOfCalorimeterVoxelAlongX; // full size
	G4double sizeOfCalorimeterVoxelAlongY; // full size
	G4double sizeOfCalorimeterVoxelAlongZ; // full size

	G4int numberOfCalorimeterVoxelsAlongZ;
	G4VPhysicalVolume* ghostWorld;
};

} // namespace CarbonIonRadiography
