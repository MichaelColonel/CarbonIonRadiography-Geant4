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

#ifdef G4MULTITHREADED
#include <G4MTRunManager.hh>
#else
#include <G4RunManager.hh>
#endif

#include <G4UImanager.hh>
#include <G4PhysListFactory.hh>
#include <G4StepLimiterPhysics.hh>
#include <G4ParallelWorldPhysics.hh>

#ifdef G4VIS_USE
#include <G4VisExecutive.hh>
#endif

#ifdef G4UI_USE
#include <G4UIExecutive.hh>
#endif

#include "CIR_GlobalStrings.hh"
#include "CIR_PhysicsList.hh"
#include "CIR_DetectorConstruction.hh"
#include "CIR_ActionInitialization.hh"
#include "CIR_TrackReconstruction.hh"
#include "CIR_SharedData.hh"

using CarbonIonRadiography::DetectorConstruction;
using CarbonIonRadiography::PhysicsList;
using CarbonIonRadiography::ActionInitialization;
using CarbonIonRadiography::ParallelWorldPhysicsStr;
using CarbonIonRadiography::TrackReconstruction;
using CarbonIonRadiography::FullTracksVector;
using CarbonIonRadiography::MainTracksVector;

int main( int argc, char** argv)
{
	// construct the default run manager
#ifdef G4MULTITHREADED
	int nof_threads = 0;
	if (argc != 1)
		nof_threads = atoi(argv[1]);

	G4MTRunManager* runManager = new G4MTRunManager;
	runManager->SetNumberOfThreads(nof_threads);
#else
	G4RunManager* runManager = new G4RunManager;
#endif

	// set mandatory initialization classes 
	runManager->SetUserInitialization(new DetectorConstruction);
/*
	G4PhysListFactory factory;
	G4VModularPhysicsList* physicsList = factory.GetReferencePhysList("QGSP_FTFP_BERT_EMY");
	physicsList->RegisterPhysics(new G4StepLimiterPhysics());
	physicsList->RegisterPhysics(new G4ParallelWorldPhysics( PhantomReadOutGeometryStr, true));
	runManager->SetUserInitialization(physicsList);
*/
//	BraggPeakData::instance( "all.dat", "primary.dat");

	PhysicsList* physicsList = new PhysicsList;
	physicsList->AddPhysicsList("QGSP_BIC");
	physicsList->RegisterPhysics(new G4StepLimiterPhysics());
	physicsList->RegisterPhysics(new G4ParallelWorldPhysics( ParallelWorldPhysicsStr, true));

	runManager->SetUserInitialization(physicsList);

	// set mandatory user action class
	runManager->SetUserInitialization(new ActionInitialization());

	// initialize G4 kernel
	runManager->Initialize();

#ifdef G4VIS_USE
	// Initialize visualization
	// G4VisManager* visManager = new G4VisExecutive;

	// G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
	G4VisManager* visManager = new G4VisExecutive("Quiet");
	visManager->Initialize();
#endif

	// Get the pointer to the User Interface manager
	G4UImanager* UImanager = G4UImanager::GetUIpointer();
/*
	FullTracksVector clear_full, object_full;
	MainTracksVector clear_main, object_main;

	TrackReconstruction::load( "clear_full_tracks.dat", clear_full);
	TrackReconstruction::load( "clear_main_tracks.dat", clear_main);
	TrackReconstruction::load( "objects_full_tracks.dat", object_full);
	TrackReconstruction::load( "objects_main_tracks.dat", object_main);

	TrackReconstruction rec( object_main, object_full);
	rec.reconstruct( clear_full, "image.root");
*/
	if (argc != 1) { // batch mode
		G4String command = "/control/execute ";
		G4String fileName = argv[2];
		UImanager->ApplyCommand(command + fileName);
	}
	else {  // interactive mode : define UI session
#ifdef G4UI_USE
		G4UIExecutive* ui = new G4UIExecutive( argc, argv);
#ifdef G4VIS_USE
		UImanager->ApplyCommand("/control/execute cir_visu.mac"); 
#else
		UImanager->ApplyCommand("/control/execute cir_novisu.mac"); 
#endif
	if (ui->IsGUI())
		UImanager->ApplyCommand("/control/execute cir_visu.mac");
		ui->SessionStart();
		delete ui;
#endif
	}

#ifdef G4VIS_USE
	delete visManager;
#endif
	delete runManager;

	return 0;
}
