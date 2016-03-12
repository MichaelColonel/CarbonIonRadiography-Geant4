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

#include <G4Types.hh>

#include <vector>
#include <map>
#include <fstream>

#include "CIR_StripGeometry.hh"

namespace CarbonIonRadiography {

typedef std::vector<G4bool> HitsVector;
typedef std::vector<G4int> NumbersVector;

typedef std::map< StripGeometryType, HitsVector > StripsHitsMap;
typedef std::map< StripGeometryType, NumbersVector > StripsNumbersMap;

class HitsPositions;
typedef std::vector<HitsPositions> HitsPositionsVector;

class HitsPositions {

friend std::ostream& operator<<( std::ostream& s, const HitsPositions& obj);
friend std::istream& operator>>( std::istream& s, HitsPositions& obj);

friend class TrackCoordinates;

public:
	HitsPositions();
	HitsPositions(const HitsVector& calorimeter_hits);
	HitsPositions(const HitsPositions& src);
	virtual ~HitsPositions();
	HitsPositions& operator=(const HitsPositions& src);
	G4bool operator==(const HitsPositions& src) const;
	G4bool operator<(const HitsPositions& src) const;

	void add_plane_hits( StripGeometryType, const HitsVector& hits);
	void add_calorimeter_hits(const HitsVector& hits);

	G4bool calorimeter_empty() const;
	G4int calorimeter_position() const;

	static void save( const char* filename, const HitsPositionsVector&);
	static void load( const char* filename, HitsPositionsVector&);

private:
	HitsVector numbers_2_hits(StripGeometryType) const;
	NumbersVector hits_2_numbers(const HitsVector& hits) const;

	StripsNumbersMap strips_numbers_;
	HitsVector calorimeter_hits_;
};

} // namespace CarbonIonRadiography
