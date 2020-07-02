// Fill out your copyright notice in the Description page of Project Settings.


#include "RoadNetComponent.h"
#include "LaneJoint.h"
#include "Materials/Material.h"
#include "UnrealEdGlobals.h"

// Sets default values for this component's properties
URoadNetComponent::URoadNetComponent() : 
	DefaultLaneWidth(350.f),
	ArrowSize(100)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

void URoadNetComponent::Init()
{
	if (!Joints.Num())
	{
		ULaneJoint* Joint = NewObject<ULaneJoint>(this);
		Joints.Add(Joint);
		UE_LOG(LogTemp, Display, TEXT("RoadNet: Default joint created. "));
	}
}

void URoadNetComponent::OnRegister()
{
#if WITH_EDITORONLY_DATA
	if (AActor * MyOwner = GetOwner())
	{
		//if (Visualizer == nullptr)
		//{
		//	Visualizer = NewObject<URoadNetVisualizer>(MyOwner, NAME_None, RF_Transactional | RF_TextExportTransient);
		//	Visualizer->Init(this);
		//	Visualizer->SetupAttachment(this);
		//	Visualizer->SetIsVisualizationComponent(true);
		//	Visualizer->CreationMethod = CreationMethod;
		//	Visualizer->RegisterComponentWithWorld(GetWorld());
		//}
	}

#endif

	Super::OnRegister();
}

// Called when the game starts
void URoadNetComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void URoadNetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

