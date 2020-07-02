// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SceneView.h"
#include "SceneManagement.h"
#include "ComponentVisualizer.h"
#include "EditorViewportClient.h"

class ULaneJoint;
class ULaneSegment;
class ULaneBranch;
class URoadNetComponent;
class UTempVisualizer;

/**
 * 
 */
class DRIVERSIMED_API FRoadNetEditor : public FComponentVisualizer
{
public:
	FRoadNetEditor();
	~FRoadNetEditor();

	void OnRegister();

	virtual void DrawVisualization(const UActorComponent* Component,
								   const FSceneView* View,
								   FPrimitiveDrawInterface* PDI) override;
	
	virtual bool VisProxyHandleClick(FEditorViewportClient* InViewportClient,
									 HComponentVisProxy* VisProxy,
									 const FViewportClick& Click) override;

	virtual bool GetWidgetLocation(const FEditorViewportClient* ViewportClient,
								   FVector& OutLocation) const override;

	virtual bool HandleInputDelta(FEditorViewportClient* Client,
								  FViewport* Viewport,
								  FVector& DeltaTranslate,
								  FRotator& DeltalRotate,
								  FVector& DeltaScale) override;

	virtual TSharedPtr<SWidget> GenerateContextMenu() const override;

protected:
	virtual void OnExtrude();
	virtual void OnDeleteJoint();

	UPROPERTY()
	URoadNetComponent* Owner;

	UPROPERTY()
	UTempVisualizer* Visualizer;

	ULaneJoint* SelectedJoint;
	ULaneSegment* SelectedSegment;
	ULaneBranch* SelectedBranch;
	bool bHasArrowsMesh;
	FMeshBatch ArrowsMesh;
	TSharedPtr<FUICommandList> CmdList;
};




