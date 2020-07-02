// Fill out your copyright notice in the Description page of Project Settings.


#include "RoadNetCommands.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "RoadNet"

FRoadNetCommands::FRoadNetCommands() 
	: TCommands<FRoadNetCommands>(
		TEXT("RoadNet"), 
		LOCTEXT("RoadNet", "Road Net"), 
		NAME_None, 
		FEditorStyle::GetStyleSetName())
{
}

void FRoadNetCommands::RegisterCommands()
{
	UI_COMMAND(Extrude,
			   "Extrude Segment",
			   "Extrude new segment",
			   EUserInterfaceActionType::Button,
			   FInputGesture());

	UI_COMMAND(DeleteJoint,
			   "Delete Joint", 
			   "Delete selected joint", 
			   EUserInterfaceActionType::Button, 
			   FInputGesture());
}

#undef LOCTEXT_NAMESPACE
