// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LaneJoint.generated.h"

class ULaneBranch;

UCLASS()
class DRIVERSIM_API ULaneJoint : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	FVector Position;

	UPROPERTY(BlueprintReadWrite)
	TArray<ULaneBranch*> Branches;

	ULaneJoint();
	~ULaneJoint();

	float GetMaxBranchWidth() const;

	void Delete();
};
