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

#include <fstream>

#include <algorithm>
#include <functional>
#include <utility>

#include "CIR_SharedData.hh"

namespace CarbonIonRadiography {

SharedReconstructionData ReconstructionData::instance_;

SharedReconstructionData
ReconstructionData::instance() 
{
	return instance_;
}

SharedReconstructionData
ReconstructionData::instance( const char* file_clear, const char* file_object)  
{
	instance_ = SharedReconstructionData(new ReconstructionData( file_clear,
		file_object));

	return instance_;
}

ReconstructionData::ReconstructionData( const char*, const char*)
{
}

ReconstructionData::~ReconstructionData()
{
}

} // namespace CarbonIonRadiography
