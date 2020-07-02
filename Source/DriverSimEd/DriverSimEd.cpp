#include "DriverSimEd.h"
#include "Modules/ModuleManager.h"
#include "Modules/ModuleInterface.h"
#include "UnrealEdGlobals.h"
#include "Editor/UnrealEdEngine.h"
#include "RoadNetEditor.h"
#include "../DriverSim/RoadNetComponent.h"

IMPLEMENT_GAME_MODULE ( FDriverSimEditorModule, DriverSimEd );

void FDriverSimEditorModule::StartupModule()
{
	if (GUnrealEd)
	{
		TSharedPtr<FRoadNetEditor> Comp = MakeShared<FRoadNetEditor>();

		if (Comp.IsValid())
		{
			GUnrealEd->RegisterComponentVisualizer(URoadNetComponent::StaticClass()->GetFName(), Comp);
			Comp->OnRegister();
		}
	}
}

void FDriverSimEditorModule::ShutdownModule()
{
	if (GUnrealEd)
	{
		GUnrealEd->UnregisterComponentVisualizer(URoadNetComponent::StaticClass()->GetFName());
	}
}


