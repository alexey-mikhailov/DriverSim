// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ComponentVisualizer.h"
#include "../DriverSim/RoadNetComponent.h"

class ULaneJoint;
class ULaneSegment;
class ULaneBranch;

struct HSegmentProxy : HComponentVisProxy
{
	DECLARE_HIT_PROXY()

	ULaneSegment* Segment;

	HSegmentProxy(const URoadNetComponent* Owner,
				  ULaneSegment* Segment);
};

struct HJointProxy : HComponentVisProxy
{
	DECLARE_HIT_PROXY()

	ULaneJoint* Joint;

	HJointProxy(const URoadNetComponent* Owner, 
				ULaneJoint* Joint);
};

struct HBranchProxy : HComponentVisProxy
{
	DECLARE_HIT_PROXY()

	ULaneBranch* Branch;

	HBranchProxy(const URoadNetComponent* Owner,
				 ULaneBranch* Branch);
};

struct HRoadNetProxy : HComponentVisProxy
{
	DECLARE_HIT_PROXY()

	const URoadNetComponent* RoadNet;

	HRoadNetProxy(const URoadNetComponent* Owner);
};


