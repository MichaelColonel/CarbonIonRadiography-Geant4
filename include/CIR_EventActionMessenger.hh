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

#include <G4UImessenger.hh>
#include <globals.hh>

class G4UIdirectory;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithoutParameter; 

namespace CarbonIonRadiography {

class EventAction;

class EventActionMessenger : public G4UImessenger {
public:
	EventActionMessenger(EventAction*);
	virtual ~EventActionMessenger();
	void SetNewValue( G4UIcommand*, G4String);

private:
	EventAction* event_action;

	G4UIdirectory* energy_thres_dir;
	G4UIcmdWithADoubleAndUnit* thres_calo_slice_cmd;
	G4UIcmdWithADoubleAndUnit* thres_si_strips_cmd;
	G4UIcmdWithoutParameter* update_cmd;
};

} // namespace CarbonIonRadiography
