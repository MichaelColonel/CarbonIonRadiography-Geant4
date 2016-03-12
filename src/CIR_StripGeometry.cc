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

#include "CIR_GlobalStrings.hh"

#include "CIR_StripGeometry.hh"

namespace {

const G4String Silicon("Silicon");
const G4String Log("Log");
const G4String Phys("Phys");
const G4String Parallel("Parallel");
const G4String Division("Division");
const G4String Sensitive("Sensitive");
const G4String Detector("Detector");

} // namespace

namespace CarbonIonRadiography {

StripGeometryMap
StripGeometry::create()
{
	StripGeometryMap map;
	
	map[MSD_Y1] = StripGeometryPair( strip_geometry_[0], create(MSD_Y1));
	map[MSD_X1] = StripGeometryPair( strip_geometry_[1], create(MSD_X1));
	map[MSD_Y2] = StripGeometryPair( strip_geometry_[2], create(MSD_Y2));
	map[MSD_X2] = StripGeometryPair( strip_geometry_[3], create(MSD_X2));
	map[MSD_Y3] = StripGeometryPair( strip_geometry_[4], create(MSD_Y3));
	map[MSD_X3] = StripGeometryPair( strip_geometry_[5], create(MSD_X3));
	map[MSD__U] = StripGeometryPair( strip_geometry_[6], create(MSD__U));
	map[MSD__V] = StripGeometryPair( strip_geometry_[7], create(MSD__V));

	return map;
}

const StripGeometry*
StripGeometry::strip_geometry(StripGeometryType type)
{
	const StripGeometry* geom = 0;

	switch (type) {
	case MSD_Y1:
		geom = &strip_geometry_[0];
		break;
	case MSD_X1:
		geom = &strip_geometry_[1];
		break;
	case MSD_Y2:
		geom = &strip_geometry_[2];
		break;
	case MSD_X2:
		geom = &strip_geometry_[3];
		break;
	case MSD_Y3:
		geom = &strip_geometry_[4];
		break;
	case MSD_X3:
		geom = &strip_geometry_[5];
		break;
	case MSD__U:
		geom = &strip_geometry_[6];
		break;
	case MSD__V:
		geom = &strip_geometry_[7];
		break;
	case MSD_ER: // error - no value
	default:
		break;
	}

	return geom;
}

StripNamesMap
StripGeometry::create_names()
{
	StripNamesMap map;

	map[MSD_Y1] = create(MSD_Y1);
	map[MSD_X1] = create(MSD_X1);
	map[MSD_Y2] = create(MSD_Y2);
	map[MSD_X2] = create(MSD_X2);
	map[MSD_Y3] = create(MSD_Y3);
	map[MSD_X3] = create(MSD_X3);
	map[MSD__U] = create(MSD__U);
	map[MSD__V] = create(MSD__V);

	return map;
}

StripGeometryNames
StripGeometry::create(StripGeometryType type)
{
	StripGeometryNames names;
	G4String value;
	switch (type) {
	case MSD_Y1:
		value = G4String("Y1");
		break;
	case MSD_X1:
		value = G4String("X1");
		break;
	case MSD_Y2:
		value = G4String("Y2");
		break;
	case MSD_X2:
		value = G4String("X2");
		break;
	case MSD_Y3:
		value = G4String("Y3");
		break;
	case MSD_X3:
		value = G4String("X3");
		break;
	case MSD__U:
		value = G4String("U");
		break;
	case MSD__V:
		value = G4String("V");
		break;
	case MSD_ER: // error - no value
	default:
		break;
	}
	
	if (!value.empty())  {
		names.body_name = Silicon + value;
		names.logical_name = Silicon + value + Log;
		names.physical_name = Silicon + value + Phys;
		names.parallel_body_name = Silicon + value + Parallel;
		names.parallel_logical_name = Silicon + value + Log + Parallel;
		names.parallel_physical_name = Silicon + value + Phys + Parallel;
		names.body_devision_name = Silicon + value + Division + Parallel;
		names.logical_devision_name = Silicon + value + Log + Division + Parallel;
		names.physical_devision_name = Silicon + value + Phys + Division + Parallel;
		names.logical_sensitive_name = names.logical_devision_name;
		names.functional_detector_name = Detector + value;
		names.sensitive_detector_name = Sensitive + Detector + value;
	}
	return names;
}

} // namespace CarbonIonRadiography
