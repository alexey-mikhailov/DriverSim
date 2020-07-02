// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LaneBranch.generated.h"

class ULaneJoint;
class ULaneSegment;

UCLASS()
class DRIVERSIM_API ULaneBranch : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	ULaneSegment* Segment;
	
	UPROPERTY(BlueprintReadWrite)
	ULaneJoint* Joint;
	
	UPROPERTY(BlueprintReadWrite)
	float Width;
	
	UPROPERTY(BlueprintReadWrite)
	FVector Direction;

	ULaneBranch();
	~ULaneBranch();

	ULaneBranch* GetOtherSide() const;
};
