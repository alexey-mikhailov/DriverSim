// Fill out your copyright notice in the Description page of Project Settings.


#include "LaneSegment.h"
#include "LaneBranch.h"
#include "LaneJoint.h"

ULaneSegment::ULaneSegment()
{
}

ULaneSegment::~ULaneSegment()
{
}

ULaneSegment* ULaneSegment::MakeSegment(ULaneJoint* From,
										ULaneJoint* To,
										float Width)
{
	ULaneBranch* Start = NewObject<ULaneBranch>(From);
	ULaneBranch* Finish = NewObject<ULaneBranch>(To);
	Start->Joint = From;
	Finish->Joint = To;
	From->Branches.Add(Start);
	To->Branches.Add(Finish);
	Start->Direction = (To->Position - From->Position) / 3.f;
	Finish->Direction = (From->Position - To->Position) / 3.f;
	Start->Width = Finish->Width = Width;


	ULaneSegment* Result = NewObject<ULaneSegment>(Start);
	Result->Start = Start;
	Result->Finish = Finish;
	Start->Segment = Result;
	Finish->Segment = Result;
	return Result;
}