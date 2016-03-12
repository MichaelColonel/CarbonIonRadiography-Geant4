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

#include "CIR_PrimaryGeneratorAction.hh"
#include "CIR_RunAction.hh"
#include "CIR_EventAction.hh"
#include "CIR_TrackingAction.hh"
#include "CIR_SteppingAction.hh"
#include "CIR_StackingAction.hh"

#include "CIR_ActionInitialization.hh"


namespace CarbonIonRadiography {

ActionInitialization::ActionInitialization()
{
}

ActionInitialization::~ActionInitialization()
{
}

void
ActionInitialization::BuildForMaster() const
{
	EventAction* eventAction = new EventAction;
	SetUserAction(new RunAction(eventAction));
}

void
ActionInitialization::Build() const
{
	EventAction* eventAction = new EventAction;
	SetUserAction(new PrimaryGeneratorAction);
	SetUserAction(new RunAction(eventAction));
	SetUserAction(eventAction);
	SetUserAction(new StackingAction);
	SetUserAction(new SteppingAction(eventAction));
	SetUserAction(new TrackingAction);
}

} // namespace CarbonIonRadiography
