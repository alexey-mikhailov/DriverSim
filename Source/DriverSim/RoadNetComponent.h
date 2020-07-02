// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "RoadNetComponent.generated.h"

class ULaneJoint;
class ULaneSegment;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DRIVERSIM_API URoadNetComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TArray<ULaneJoint*> Joints;

	UPROPERTY(BlueprintReadWrite)
	TArray<ULaneSegment*> Segments;

	UPROPERTY(Category = Editor, EditAnywhere, BlueprintReadWrite)
	float DefaultLaneWidth;

	UPROPERTY(Category = Editor, EditAnywhere, BlueprintReadWrite)
	float ArrowSize;

#if WITH_EDITORONLY_DATA
	class URoadNetVisualizer* Visualizer;
#endif

	// Sets default values for this component's properties
	URoadNetComponent();

	UFUNCTION(Category=Road, CallInEditor)
	void Init();

protected:
	virtual void OnRegister() override;

	// Called when the game starts
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
