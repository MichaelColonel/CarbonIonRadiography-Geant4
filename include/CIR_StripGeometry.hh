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

#include <map>
#include <G4String.hh>
#include <G4Types.hh>
#include "CIR_Defines.hh"

namespace CarbonIonRadiography {

// Micro strips detector plane type
enum StripGeometryType {
	MSD_ER, // error - no plane
	MSD_Y1,
	MSD_X1,
	MSD_Y2,
	MSD_X2,
	MSD_Y3,
	MSD_X3,
	MSD__U,
	MSD__V
};

struct StripGeometry;
struct StripGeometryNames;

typedef std::pair< StripGeometry, StripGeometryNames> StripGeometryPair;
typedef std::map< StripGeometryType, StripGeometryNames> StripNamesMap;
typedef std::map< StripGeometryType, StripGeometryPair> StripGeometryMap;

// Micro strips detector plane geometry names for Geant4
struct StripGeometryNames {
	// mass geometry names
	G4String body_name;
	G4String logical_name;
	G4String physical_name;
	// parallel world names
	G4String parallel_body_name;
	G4String parallel_logical_name;
	G4String parallel_physical_name;
	G4String body_devision_name;
	G4String logical_devision_name;
	G4String physical_devision_name;
	G4String logical_sensitive_name;
	G4String functional_detector_name;
	G4String sensitive_detector_name;
};

// Micro strips detector plane geometry parameters
const struct StripGeometry {
	static G4int index(StripGeometryType); // get plane index from type
	static StripGeometryType index(G4int); // get plane type from index
	static StripGeometryMap create();
	static StripGeometryNames create(StripGeometryType);
	static StripNamesMap create_names();
	static const StripGeometry* strip_geometry(StripGeometryType);
	static G4double sigma_geometry(StripGeometryType);

	G4double z; // position z (um)
	G4double angle; // angle of the detector (degree)
	G4double offset; // pitch offset (um) -- reserved
	G4double angle_diff; // angle corrections (degree) -- reserved
	G4double phi_diff; // reserved
	G4double sigma; // sigma multiple scattering + alingment (um) -- reserved
	G4double t; // detector thickness (um)
	G4double x; // half size of square detector (um)
	G4int strips; // number of strips
	G4double pitch; // pitch size (um)
	G4double dx; // detector offset (um) -- reserved
} strip_geometry_[CIR_NUMBER_OF_SILICON_DETECTORS] = {
	{  -52000., 180.0, 0.0, 0.0, 0.0, 0.0, 300., 30000., 300, 200., 0. },
	{  -50000.,  90.0, 0.0, 0.0, 0.0, 0.0, 300., 30000., 300, 200., 0. },
	{  298000., 180.0, 0.0, 0.0, 0.0, 0.0, 300., 30000., 300, 200., 0. },
	{  300000.,  90.0, 0.0, 0.0, 0.0, 0.0, 300., 30000., 300, 200., 0. },
	{ 1300000., 180.0, 0.0, 0.0, 0.0, 0.0, 300., 30000., 300, 200., 0. },
	{ 1302000.,  90.0, 0.0, 0.0, 0.0, 0.0, 300., 30000., 300, 200., 0. },
	{ 1320000., -10.5, 0.0, 0.0, 0.0, 0.0, 300., 30000., 300, 200., 0. },
	{ 1322000.,  10.5, 0.0, 0.0, 0.0, 0.0, 300., 30000., 300, 200., 0. }
};

inline
G4int
StripGeometry::index(StripGeometryType type)
{
	G4int pos = -1;

	switch (type) {
	case MSD_Y1:
		pos = 0;
		break;
	case MSD_X1:
		pos = 1;
		break;
	case MSD_Y2:
		pos = 2;
		break;
	case MSD_X2:
		pos = 3;
		break;
	case MSD_Y3:
		pos = 4;
		break;
	case MSD_X3:
		pos = 5;
		break;
	case MSD__U:
		pos = 6;
		break;
	case MSD__V:
		pos = 7;
		break;
	case MSD_ER:
	default:
		break;
	}
	return pos;
}

inline
StripGeometryType
StripGeometry::index(G4int pos)
{
	StripGeometryType type = MSD_ER;

	switch (pos) {
	case 0:
		type = MSD_Y1;
		break;
	case 1:
		type = MSD_X1;
		break;
	case 2:
		type = MSD_Y2;
		break;
	case 3:
		type = MSD_X2;
		break;
	case 4:
		type = MSD_Y3;
		break;
	case 5:
		type = MSD_X3;
		break;
	case 6:
		type = MSD__U;
		break;
	case 7:
		type = MSD__V;
		break;
	default:
		break;
	}
	return type;
}

} // namespace CarbonIonRadiography
