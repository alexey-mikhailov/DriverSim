// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LaneSegment.generated.h"

class ULaneJoint;
class ULaneBranch;

UCLASS()
class DRIVERSIM_API ULaneSegment : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	ULaneBranch* Start;
	
	UPROPERTY(BlueprintReadWrite)
	ULaneBranch* Finish;

	ULaneSegment();
	~ULaneSegment();

	static ULaneSegment* MakeSegment(ULaneJoint* From,
									 ULaneJoint* To,
									 float Width);
};
