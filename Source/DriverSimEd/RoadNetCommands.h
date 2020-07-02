// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"

/**
 * 
 */
class DRIVERSIMED_API FRoadNetCommands : public	TCommands<FRoadNetCommands>
{
public:
	FRoadNetCommands();
	virtual void RegisterCommands() override;

	TSharedPtr<FUICommandInfo> Extrude;
	TSharedPtr<FUICommandInfo> DeleteJoint;
};
