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

#include <G4UIdirectory.hh>
#include <G4UIcmdWithADoubleAndUnit.hh>
#include <G4UIcmdWithoutParameter.hh>
#include <G4SystemOfUnits.hh>

#include "CIR_EventAction.hh"
#include "CIR_EventActionMessenger.hh"

namespace CarbonIonRadiography {

EventActionMessenger::EventActionMessenger(EventAction* event)
	:
	event_action(event),
	energy_thres_dir(0),
	thres_calo_slice_cmd(0),
	thres_si_strips_cmd(0),
	update_cmd(0)
{
	// Threshold directory
	energy_thres_dir = new G4UIdirectory("/thres/");
	energy_thres_dir->SetGuidance("Command to change the threshold's directory");

	// Change calorimeter slice energy threshold command
	thres_calo_slice_cmd = new G4UIcmdWithADoubleAndUnit(
		"/thres/calorimeter_slice", this);

	thres_calo_slice_cmd->SetGuidance(
		"Energy threshold for a calorimeter slice");

	thres_calo_slice_cmd->SetParameterName(
		"CalorimeterSliceEnergyThreshold",
		false);

	thres_calo_slice_cmd->SetDefaultUnit("MeV");
	thres_calo_slice_cmd->SetUnitCandidates("keV MeV GeV"); 
	thres_calo_slice_cmd->AvailableForStates(G4State_Idle);

	// Change silicon strips detectors energy threshold command
	thres_si_strips_cmd = new G4UIcmdWithADoubleAndUnit(
		"/thres/silicon_strips", this);

	thres_si_strips_cmd->SetGuidance(
		"Energy threshold for silicon strips detectors");

	thres_si_strips_cmd->SetParameterName(
		"SiliconStripsEnergyThreshold",
		false);

	thres_si_strips_cmd->SetDefaultUnit("MeV");
	thres_si_strips_cmd->SetUnitCandidates("keV MeV GeV"); 
	thres_si_strips_cmd->AvailableForStates(G4State_Idle);
	
	// update
	update_cmd = new G4UIcmdWithoutParameter(
		"/thres/update",
		this);
	update_cmd->SetGuidance("Update energy threshold.");
	update_cmd->SetGuidance("This command MUST be applied before \"beamOn\" ");
	update_cmd->SetGuidance("if you changed threshold value(s).");
	update_cmd->AvailableForStates(G4State_Idle);
}

/////////////////////////////////////////////////////////////////////////////
EventActionMessenger::~EventActionMessenger()
{
	delete update_cmd;
	delete thres_calo_slice_cmd;
	delete thres_si_strips_cmd;
	delete energy_thres_dir;
}

/////////////////////////////////////////////////////////////////////////////
void
EventActionMessenger::SetNewValue( G4UIcommand* command, G4String newValue)
{
    if (command == thres_calo_slice_cmd) {
		G4double thres = G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(newValue);
		event_action->setCaloSliceThres(thres);
	}
    else if (command == thres_si_strips_cmd) {
		G4double thres = G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(newValue);
		event_action->setSiStripsThres(thres);
	}
	else if (command == update_cmd) {
		event_action->update();
	}
}

} // namespace CarbonIonRadiography
