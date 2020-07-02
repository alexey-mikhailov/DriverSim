// Fill out your copyright notice in the Description page of Project Settings.


#include "LaneBranch.h"
#include "LaneSegment.h"

ULaneBranch::ULaneBranch()
{
}

ULaneBranch::~ULaneBranch()
{
}

ULaneBranch* ULaneBranch::GetOtherSide() const
{
	if (Segment->Start == this)
	{
		return Segment->Finish;
	}
	else
	{
		return Segment->Start;
	}
}
