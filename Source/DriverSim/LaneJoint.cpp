// Fill out your copyright notice in the Description page of Project Settings.


#include "LaneJoint.h"
#include "LaneSegment.h"
#include "LaneBranch.h"

ULaneJoint::ULaneJoint() : 
	Position(0)
{
}

ULaneJoint::~ULaneJoint()
{
}

float ULaneJoint::GetMaxBranchWidth() const
{
	float Result = MIN_flt;
	for (ULaneBranch* Branch : Branches)
	{
		Result = FMath::Max(Result, Branch->Width);
	}

	return Result;
}

void ULaneJoint::Delete()
{
	for (ULaneBranch* Branch : Branches)
	{
		if (Branch == Branch->Segment->Start)
		{
			ULaneBranch* Other = Branch->Segment->Finish;
			Other->Joint->Branches.Remove(Other);
		}
		else if (Branch == Branch->Segment->Finish)
		{
			ULaneBranch* Other = Branch->Segment->Start;
			Other->Joint->Branches.Remove(Other);
		}
	}

	Branches.Empty();
}

