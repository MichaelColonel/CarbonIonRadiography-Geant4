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

#include <G4DataVector.hh>
#include <globals.hh>
#include <tr1/memory>
#include <boost/noncopyable.hpp>
#include <map>
#include <vector>

class TH2D;

namespace CarbonIonRadiography {

class ReconstructionData;
typedef std::tr1::shared_ptr<ReconstructionData> SharedReconstructionData;

class ReconstructionData : private boost::noncopyable  {
public:
	virtual ~ReconstructionData();
	// Get object instance only
	static SharedReconstructionData instance();
	// Make & Get instance
	static SharedReconstructionData instance( const char* file_clear,
		const char* file_object);
	TH2D* reconstruct( G4double x_min, G4double x_max,
		G4double y_min, G4double y_max, G4int bin_x, G4int bin_y);

private:
	ReconstructionData( const char* file_clear, const char* file_object);

	static SharedReconstructionData instance_;
};

} // namespace CarbonIonRadiography
