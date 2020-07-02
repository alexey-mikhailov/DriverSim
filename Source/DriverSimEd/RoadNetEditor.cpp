// Fill out your copyright notice in the Description page of Project Settings.


#include "RoadNetEditor.h"
#include "TempVisualizer.h"
#include "CommonStructs.h"
#include "RoadNetCommands.h"
#include "../DriverSim/RoadNetComponent.h"
#include "../DriverSim/LaneBranch.h"
#include "../DriverSim/LaneSegment.h"
#include "../DriverSim/LaneJoint.h"
#include "Materials/Material.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "RenderingThread.h"
#include "UObject/ConstructorHelpers.h"




FRoadNetEditor::FRoadNetEditor() : 
	Owner(nullptr),
	Visualizer(nullptr),
	SelectedJoint(nullptr),
	SelectedSegment(nullptr),
	SelectedBranch(nullptr), 
	bHasArrowsMesh(false),
	ArrowsMesh(),
	CmdList()
{
}

FRoadNetEditor::~FRoadNetEditor()
{
}

void FRoadNetEditor::OnRegister()
{
	FRoadNetCommands::Register();
	auto& Commands = FRoadNetCommands::Get();
	CmdList = MakeShared<FUICommandList>();
	CmdList->MapAction(Commands.Extrude,
					   FExecuteAction::CreateSP(this, &FRoadNetEditor::OnExtrude));
	CmdList->MapAction(Commands.DeleteJoint,
					   FExecuteAction::CreateSP(this, &FRoadNetEditor::OnDeleteJoint));
}

void FRoadNetEditor::DrawVisualization(const UActorComponent* Component, 
									   const FSceneView* View, 
									   FPrimitiveDrawInterface* PDI)
{
	Owner = (URoadNetComponent*)Component;

	if (Visualizer == nullptr)
	{
		Visualizer = NewObject<UTempVisualizer>(Owner, NAME_None, RF_Transactional | RF_TextExportTransient);
		Visualizer->Init(Owner);
		Visualizer->SetupAttachment(Owner);
		Visualizer->SetIsVisualizationComponent(true);
		Visualizer->CreationMethod = Owner->CreationMethod;
		Visualizer->RegisterComponentWithWorld(Owner->GetWorld());
	}

	if (Owner && Owner->IsA<URoadNetComponent>())
	{
		// Segments
		for (ULaneSegment* Segment : Owner->Segments)
		{
			if (Segment)
			{
				ULaneBranch* Start = Segment->Start;
				ULaneBranch* Finish = Segment->Finish;
				FVector StartA = Start->Joint->Position;
				FVector StartB = Start->Joint->Position + Start->Direction;
				FVector FinishA = Finish->Joint->Position;
				FVector FinishB = Finish->Joint->Position + Finish->Direction;

				float Length = (FinishA - StartA).Size() +
					(FinishB - FinishA).Size() +
					(FinishB - StartB).Size() +
					(FinishB - FinishA).Size();

				Length /= 2.f;
				int32 ArrowNum =
					FMath::RoundToInt(Length / Owner->ArrowSize);

				float Dt = 1.f / ArrowNum;

				FVector Pos0 = StartA;
				FVector Pos1 = StartA;
				float T, T2, T3, TRem, TRem2, TRem3;

				HSegmentProxy* Proxy = new HSegmentProxy(Owner, Segment);
				PDI->SetHitProxy(Proxy);

				for (int32 Index = 0; Index < ArrowNum; ++Index)
				{
					T = (Index + 1.0) / ArrowNum;
					T2 = T * T;
					T3 = T2 * T;
					TRem = 1 - T;
					TRem2 = TRem * TRem;
					TRem3 = TRem2 * TRem;

					Pos1 = TRem3 * StartA +
						3 * TRem2 * T * StartB +
						3 * TRem * T2 * FinishB +
						T3 * FinishA;

					PDI->DrawLine(Pos0, Pos1, FLinearColor::Green, SDPG_World);
					Pos0 = Pos1;
				}

				// TODO: Can I remove it?
				PDI->SetHitProxy(nullptr);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Segment was reset to nullptr. "));
			}
		}

		// Branch
		for (ULaneSegment* Segment : Owner->Segments)
		{
			if (Segment)
			{
				ULaneBranch* Start = Segment->Start;
				ULaneBranch* Finish = Segment->Finish;
				FVector StartA = Start->Joint->Position;
				FVector StartB = Start->Joint->Position + Start->Direction;
				FVector FinishA = Finish->Joint->Position;
				FVector FinishB = Finish->Joint->Position + Finish->Direction;

				HBranchProxy* Proxy;
				Proxy = new HBranchProxy(Owner, Start);
				PDI->SetHitProxy(Proxy);
				PDI->DrawLine(StartA, StartB, FLinearColor::Red, SDPG_Foreground);
				PDI->DrawPoint(StartB, FLinearColor::Red, 12, SDPG_Foreground);
				PDI->SetHitProxy(nullptr);

				Proxy = new HBranchProxy(Owner, Finish);
				PDI->SetHitProxy(Proxy);
				PDI->DrawLine(FinishA, FinishB, FLinearColor::Red, SDPG_Foreground);
				PDI->DrawPoint(FinishB, FLinearColor::Red, 12, SDPG_Foreground);
				PDI->SetHitProxy(nullptr);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Segment was reset to nullptr. "));
			}
		}

		// Joints
		for (ULaneJoint* Joint : Owner->Joints)
		{
			if (Joint)
			{
				HJointProxy* Proxy = new HJointProxy(Owner, Joint);
				PDI->SetHitProxy(Proxy);
				PDI->DrawPoint(Joint->Position, FLinearColor::Green, 16, SDPG_Foreground);
				PDI->SetHitProxy(nullptr);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Joint was reset to nullptr. "));
			}
		}

		// Road itself
		PDI->SetHitProxy(new HRoadNetProxy(Owner));
		PDI->DrawLine(Owner->GetComponentLocation() + FVector(10, 10, 0),
					  Owner->GetComponentLocation() + FVector(10, -10, 0), 
					  FLinearColor::White, SDPG_Foreground);
		PDI->DrawLine(Owner->GetComponentLocation() + FVector(10, -10, 0),
					  Owner->GetComponentLocation() + FVector(-10, -10, 0),
					  FLinearColor::White, SDPG_Foreground);
		PDI->DrawLine(Owner->GetComponentLocation() + FVector(-10, -10, 0),
					  Owner->GetComponentLocation() + FVector(-10, 10, 0),
					  FLinearColor::White, SDPG_Foreground);
		PDI->DrawLine(Owner->GetComponentLocation() + FVector(-10, 10, 0),
					  Owner->GetComponentLocation() + FVector(10, 10, 0),
					  FLinearColor::White, SDPG_Foreground);
		PDI->SetHitProxy(nullptr);
	}
}

bool FRoadNetEditor::VisProxyHandleClick(FEditorViewportClient* InViewportClient, HComponentVisProxy* VisProxy, const FViewportClick& Click)
{
	if (VisProxy && VisProxy->Component.IsValid())
	{
		// Select logic
		if (VisProxy->IsA(HJointProxy::StaticGetType()))
		{
			HJointProxy* Proxy = (HJointProxy*)VisProxy;
			SelectedJoint = Proxy->Joint;
			SelectedSegment = nullptr;
			SelectedBranch = nullptr;
			return true;
		}
		else if (VisProxy->IsA(HBranchProxy::StaticGetType()))
		{
			HBranchProxy* Proxy = (HBranchProxy*)VisProxy;
			SelectedJoint = nullptr;
			SelectedSegment = nullptr;
			SelectedBranch = Proxy->Branch;
			return true;
		}
		else if (VisProxy->IsA(HSegmentProxy::StaticGetType()))
		{
			HSegmentProxy* Proxy = (HSegmentProxy*)VisProxy;
			SelectedJoint = nullptr;
			SelectedSegment = Proxy->Segment;
			SelectedBranch = nullptr;
			return true;
		}
		else if (VisProxy->IsA(HRoadNetProxy::StaticGetType()))
		{
			UE_LOG(LogTemp, Display, TEXT("RoadNetProxy clicked"));
			return true;
		}
	}
	else
	{
		// Unselect logic
		SelectedJoint = nullptr;
		SelectedSegment = nullptr;
		SelectedBranch = nullptr;
		return false;
	}

	return false;
}

bool FRoadNetEditor::GetWidgetLocation(
	const FEditorViewportClient* ViewportClient, 
	FVector& OutLocation) const
{
	if (!Owner)
	{
		return false;
	}

	if (SelectedJoint)
	{
		OutLocation = SelectedJoint->Position;
		return true;
	}
	else if (SelectedBranch)
	{
		OutLocation = SelectedBranch->Joint->Position + 
					  SelectedBranch->Direction;
		return true;
	}
	
	return false;
}

bool FRoadNetEditor::HandleInputDelta(
	FEditorViewportClient* Client, 
	FViewport* Viewport, 
	FVector& DeltaTranslate, 
	FRotator& DeltaRotate, 
	FVector& DeltaScale)
{
	if (!Owner)
	{
		return false;
	}

	if (SelectedJoint)
	{
		SelectedJoint->Position += DeltaTranslate;

		// Now we have to rotate all the branches of this joint
		for (auto Branch : SelectedJoint->Branches)
		{
			Branch->Direction = DeltaRotate.Quaternion() * Branch->Direction;
			Branch->Direction *= DeltaScale + FVector::OneVector;
		}

		return true;
	}
	else if (SelectedBranch)
	{
		SelectedBranch->Direction += DeltaTranslate;
		SelectedBranch->Direction = DeltaRotate.Quaternion() * SelectedBranch->Direction;
		SelectedBranch->Direction *= DeltaScale + FVector::OneVector;

		return true;
	}

	return false;
}

TSharedPtr<SWidget> FRoadNetEditor::GenerateContextMenu() const
{
	if (SelectedJoint)
	{
		// Generate joint menu
		FMenuBuilder MenuBuilder(true, CmdList);
		auto& Commands = FRoadNetCommands::Get();
		MenuBuilder.AddMenuEntry(Commands.Extrude);
		MenuBuilder.AddMenuEntry(Commands.DeleteJoint);
		return MenuBuilder.MakeWidget();
	}
	else if (SelectedSegment)
	{
		// Generate segment menu

	}
	else if (SelectedBranch)
	{
		// Generate branch menu

	}

	return TSharedPtr<SWidget>();
}

void FRoadNetEditor::OnExtrude()
{
	if (!Owner)
	{
		UE_LOG(LogTemp, Error, TEXT("OnExtrude: Owner is nullptr. "));
		return;
	}

	if (!SelectedJoint)
	{
		UE_LOG(LogTemp, Error, TEXT("OnExtrude: Joint is not selected. "));
		return;
	}

	if (SelectedJoint->Branches.Num() == 0)
	{
		// Extrude 10 m along Y axis
		ULaneJoint* FinishJoint = NewObject<ULaneJoint>(Owner);
		FinishJoint->Position = SelectedJoint->Position + 
								1000 * FVector::ForwardVector;
		Owner->Joints.Add(FinishJoint);

		ULaneSegment* Segment = 
			ULaneSegment::MakeSegment(SelectedJoint, 
									  FinishJoint,
									  Owner->DefaultLaneWidth);
		Owner->Segments.Add(Segment);
	}
	else if (SelectedJoint->Branches.Num() == 1)
	{
		// Extrude along opposite branch
		FVector Direction = -SelectedJoint->Branches[0]->Direction;
		ULaneJoint* FinishJoint = NewObject<ULaneJoint>(Owner);
		FinishJoint->Position = SelectedJoint->Position + 3 * Direction;
		Owner->Joints.Add(FinishJoint);

		ULaneSegment* Segment =
			ULaneSegment::MakeSegment(SelectedJoint,
									  FinishJoint,
									  Owner->DefaultLaneWidth);
		Owner->Segments.Add(Segment);
	}
	else if (SelectedJoint->Branches.Num() > 1)
	{
		// Extrude 10 m as average branch
		int32 LastIndex = SelectedJoint->Branches.Num() - 1;

		// TODO: Implement via ULaneBranch::GetOtherSide()->Joint->Position
		FVector Direction = .5f * SelectedJoint->Branches[LastIndex]->GetOtherSide()->Joint->Position + 
							.5f * SelectedJoint->Branches[0]->GetOtherSide()->Joint->Position;

		Direction = Direction.GetSafeNormal();
		if (!Direction.Size())
		{
			Direction = FVector::RightVector;
		}

		ULaneJoint* FinishJoint = NewObject<ULaneJoint>(Owner);
		FinishJoint->Position = SelectedJoint->Position + 
								1000 * Direction;

		Owner->Joints.Add(FinishJoint);

		ULaneSegment* Segment =
			ULaneSegment::MakeSegment(SelectedJoint,
									  FinishJoint,
									  Owner->DefaultLaneWidth);
		
		Owner->Segments.Add(Segment);
	}
}

void FRoadNetEditor::OnDeleteJoint()
{
	if (Owner && SelectedJoint)
	{
		Owner->Joints.Remove(SelectedJoint);
		for (ULaneBranch* Branch : SelectedJoint->Branches)
		{
			Owner->Segments.Remove(Branch->Segment);
		}
		
		SelectedJoint->Delete();
	}
}
