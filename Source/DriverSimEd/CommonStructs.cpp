// Fill out your copyright notice in the Description page of Project Settings.


#include "CommonStructs.h"

IMPLEMENT_HIT_PROXY(HJointProxy, HComponentVisProxy)
IMPLEMENT_HIT_PROXY(HBranchProxy, HComponentVisProxy)
IMPLEMENT_HIT_PROXY(HSegmentProxy, HComponentVisProxy)
IMPLEMENT_HIT_PROXY(HRoadNetProxy, HComponentVisProxy)

HSegmentProxy::HSegmentProxy(const URoadNetComponent* Owner, 
							 ULaneSegment* Segment) :
	HComponentVisProxy(Owner),
	Segment(Segment)
{
}

HJointProxy::HJointProxy(const URoadNetComponent* Owner, 
						 ULaneJoint* Joint) :
	HComponentVisProxy(Owner),
	Joint(Joint)
{
}

HBranchProxy::HBranchProxy(const URoadNetComponent* Owner, 
						   ULaneBranch* Branch) :
	HComponentVisProxy(Owner),
	Branch(Branch)
{
}

HRoadNetProxy::HRoadNetProxy(const URoadNetComponent* Owner) :
	HComponentVisProxy(Owner),
	RoadNet(Owner)
{
}


