#include "TempVisualizer.h"
#include "../DriverSim/RoadNetComponent.h"
#include "../DriverSim/LaneJoint.h"
#include "../DriverSim/LaneBranch.h"
#include "../DriverSim/LaneSegment.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/CollisionProfile.h"
#include "PrimitiveSceneProxy.h"
#include "Materials/Material.h"
#include "DynamicMeshBuilder.h"
#include "CommonStructs.h"

class FRoadNetSceneProxy final : public FPrimitiveSceneProxy
{
	// TODO: Remove
	TArray<FDynamicMeshVertex> Vertices;
	TArray<uint32> Indices;

	UPROPERTY()
	UMaterial* Material;

	const URoadNetComponent* Component;
	const UTempVisualizer* Visualizer;

public:
	FRoadNetSceneProxy(const UTempVisualizer* InVisualizer)
		: FPrimitiveSceneProxy(InVisualizer),
		Indices(InVisualizer->Indices),
		Material(InVisualizer->Material)
	{
		UE_LOG(LogTemp, Display, TEXT("FRoadNetSceneProxy::ctor called. "));

		Visualizer = InVisualizer;
		Component = Visualizer->Component;
		bWillEverBeLit = false;

		for (auto VIdx = 0; VIdx < Visualizer->Vertices.Num(); ++VIdx)
		{
			auto V = FDynamicMeshVertex(Visualizer->GetComponentLocation() + Visualizer->Vertices[VIdx]);
			V.TangentZ = Visualizer->Normals[VIdx];
			Vertices.Add(V);
		}
	}

	virtual ~FRoadNetSceneProxy() {}

	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override
	{
		FPrimitiveViewRelevance Result;
		Result.bDrawRelevance = true;
		Result.bDynamicRelevance = true;
		Result.bNormalTranslucencyRelevance = true;
		return Result;
	}

	void BuildMesh(FDynamicMeshBuilder& MeshBuilder, 
				   FPrimitiveDrawInterface* PDI) const
	{
		TArray<FDynamicMeshVertex> VV;
		TArray<uint32> II;

		uint32 VIdx = 0U;
		for (ULaneJoint* Joint : Component->Joints)
		{
			uint32 SegmentCount = 16;
			float Radius = .5f * Joint->GetMaxBranchWidth();
			for (uint32 Index = 0; Index < SegmentCount; ++Index)
			{
				auto V = FDynamicMeshVertex(Joint->Position);
				V.TangentZ = FVector::UpVector;
				VV.Add(V);
				II.Add(VIdx++);

				float Angle1 = FMath::DegreesToRadians((float)Index / SegmentCount * 360.f);
				float Angle2 = FMath::DegreesToRadians((Index + 1.f) / SegmentCount * 360.f);
				FVector Pos1 = Joint->Position + Radius * FVector(FMath::Cos(Angle1), 
																  FMath::Sin(Angle1), 
																  .0f);
				FVector Pos2 = Joint->Position + Radius * FVector(FMath::Cos(Angle2), 
																  FMath::Sin(Angle2), 
																  .0f);
				V = FDynamicMeshVertex(Pos1);
				V.TangentZ = FVector::UpVector;
				VV.Add(V);
				II.Add(VIdx++);

				V = FDynamicMeshVertex(Pos2);
				V.TangentZ = FVector::UpVector;
				VV.Add(V);
				II.Add(VIdx++);
			}
		}

		for (ULaneSegment* Segment : Component->Segments)
		{
			if (Segment)
			{
				ULaneBranch* Start = Segment->Start;
				ULaneBranch* Finish = Segment->Finish;
				FVector StartA = Start->Joint->Position;
				FVector StartB = Start->Joint->Position + Start->Direction;
				FVector FinishA = Finish->Joint->Position;
				FVector FinishB = Finish->Joint->Position + Finish->Direction;

				float T, T2, T3, TRem, TRem2, TRem3;

				const int32 IterationCount = 2 * (FinishA - StartA).Size();
				const int32 ApprCount = IterationCount - 1;
				TArray<FVector> ErpPositions;
				TArray<float> ErpTravelVals;
				TArray<float> ErpNormalVals;
				ErpPositions.Init(FVector(), IterationCount);
				ErpTravelVals.Init(0.f, IterationCount);
				ErpNormalVals.Init(0.f, IterationCount);

				float LastTravelValue = 0.f;
				float StartMargin = .6f * Start->Joint->GetMaxBranchWidth();
				float FinishMargin = .6f * Finish->Joint->GetMaxBranchWidth();
				float StartMisfire = 0.f;
				float FinishMisfire = 0.f;
				int32 StartMisfireIndex = 0;
				int32 FinishMisfireIndex = 0;
				bool bIsFinishFound = false;
				FVector LastPosition = StartA;
				for (int32 Index = 0; Index < IterationCount; ++Index)
				{
					T = (float)Index / ApprCount;
					T2 = T * T;
					T3 = T2 * T;
					TRem = 1 - T;
					TRem2 = TRem * TRem;
					TRem3 = TRem2 * TRem;

					FVector Pos = TRem3 * StartA +
						3 * TRem2 * T * StartB +
						3 * TRem * T2 * FinishB +
						T3 * FinishA;
					
					float ApprLength = (Pos - LastPosition).Size();
					float Travel = LastTravelValue + ApprLength;
					ErpPositions[Index] = Pos;
					ErpTravelVals[Index] = Travel;
					ErpNormalVals[Index] = T;
					
					if ((Pos - StartA).Size() < StartMargin)
					{
						StartMisfire = Travel;
						StartMisfireIndex = Index;
					}
					
					if (!bIsFinishFound && (Pos - FinishA).Size() < FinishMargin)
					{
						FinishMisfire = Travel;
						FinishMisfireIndex = Index;
						bIsFinishFound = true;
					}

					LastPosition = Pos;
					LastTravelValue = Travel;
				}

				float Length = FinishMisfire - StartMisfire;
				int32 ArrowNum =
					FMath::RoundToInt(Length / Component->ArrowSize);

				float Dl = Length / ArrowNum;

				float TargetTravel = StartMisfire;
				float NearestDist = MAX_FLT;
				FVector NearestPos = StartA;
				float NearestWidth = Start->Width;
				TArray<FVector> ArrowNodes;
				TArray<float> ArrowWidths;
				for (int32 Index = StartMisfireIndex; Index <= FinishMisfireIndex; ++Index)
				{
					float Dist = FMath::Abs(TargetTravel - ErpTravelVals[Index]);

					if (Dist < NearestDist)
					{
						NearestDist = Dist;
						NearestPos = ErpPositions[Index];
						NearestWidth = Start->Width + ErpNormalVals[Index] * (Finish->Width - Start->Width);
					}
					else
					{
						ArrowNodes.Add(NearestPos);
						ArrowWidths.Add(NearestWidth);

						TargetTravel += Dl;
						NearestDist = MAX_FLT;
					}
				}

				ArrowNodes.Add(ErpPositions[FinishMisfireIndex]);
				ArrowWidths.Add(Start->Width + ErpNormalVals[FinishMisfireIndex] * (Finish->Width - Start->Width));

				FQuat TurnRight = FRotator(0.f, 90.f, 0.f).Quaternion();
				FVector Pos0 = ArrowNodes.Num() ? ArrowNodes[0] : StartA;
				FVector Pos1 = StartA;

				//HSegmentProxy* Proxy = new HSegmentProxy(Component, Segment);
				//PDI->SetHitProxy(Proxy);

				int32 ArrowNodesNum = ArrowNodes.Num();
				for (int32 Index = 1; Index < ArrowNodesNum; ++Index)
				{
					Pos1 = ArrowNodes[Index];

					//PDI->DrawLine(Pos0, Pos1, FLinearColor::Green, SDPG_World);
					auto V = FDynamicMeshVertex(Pos1);
					V.TangentZ = FVector::UpVector;
					VV.Add(V);
					II.Add(VIdx++);

					//PDI->DrawPoint(V.Position, FLinearColor::Red, 8, SDPG_Foreground);

					FVector Offset = TurnRight.RotateVector(Pos1 - Pos0);
					Offset = FVector::VectorPlaneProject(Offset, FVector::UpVector).GetSafeNormal();
					Offset = .5 * Offset * ArrowWidths[Index];
					V = FDynamicMeshVertex(Pos0 + Offset);
					V.TangentZ = FVector::UpVector;
					VV.Add(V);
					II.Add(VIdx++);

					//PDI->DrawPoint(V.Position, FLinearColor::Red, 8, SDPG_Foreground);

					V = FDynamicMeshVertex(Pos0 - Offset);
					V.TangentZ = FVector::UpVector;
					VV.Add(V);
					II.Add(VIdx++);

					//PDI->DrawPoint(V.Position, FLinearColor::Red, 8, SDPG_Foreground);

					Pos0 = Pos1;
				}

				// TODO: Can I remove it?
				//PDI->SetHitProxy(nullptr);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Segment was reset to nullptr. "));
			}
		}

		MeshBuilder.AddVertices(VV);
		MeshBuilder.AddTriangles(II);
	}

	virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views,
										const FSceneViewFamily& ViewFamily,
										uint32 VisibilityMap,
										FMeshElementCollector& Collector) const override
	{
		if (IsSelected())
		{
			for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ++ViewIndex)
			{
				auto PDI = Collector.GetPDI(ViewIndex);

				FDynamicMeshBuilder MeshBuilder(Views[ViewIndex]->GetFeatureLevel());
				if (Vertices.Num() == 0)
				{
					continue;
				}

				uint8 DPG = GetDepthPriorityGroup(Views[ViewIndex]);
				BuildMesh(MeshBuilder, PDI);
				MeshBuilder.GetMesh(FMatrix::Identity,
									new FColoredMaterialRenderProxy(Material->GetRenderProxy(),
																	FLinearColor::Green),
									DPG,
									true,
									true,
									false,
									ViewIndex,
									Collector,
									new HRoadNetProxy(Component));
			}
		}
	}

	virtual uint32 GetMemoryFootprint(void) const override { return(sizeof(*this) + GetAllocatedSize()); }
	uint32 GetAllocatedSize(void) const { return(FPrimitiveSceneProxy::GetAllocatedSize()); }

	SIZE_T GetTypeHash() const override
	{
		static size_t UniquePointer;
		return reinterpret_cast<size_t>(&UniquePointer);
	}
};

UTempVisualizer::UTempVisualizer() :
	Component(nullptr)
{
	bUseEditorCompositing = true;
	bHiddenInGame = true;
	SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	SetGenerateOverlapEvents(false);

	// Material
	static ConstructorHelpers::FObjectFinder<UMaterial> MatFinder(TEXT("Material'/Game/MAT_RoadNet'"));

	if (MatFinder.Object != nullptr)
	{
		Material = (UMaterial*)MatFinder.Object;
	}

	// Geometry
	const auto SegNum = 32U;
	const auto Radius = 200.f;
	const auto Length = 300.f;
	const auto Aperture = FMath::Atan(Radius / Length);

	for (auto SegIdx = 0U; SegIdx < SegNum; ++SegIdx)
	{
		auto Angle = (float)SegIdx / (float)SegNum * 2 * PI;
		auto X = FMath::Cos(Angle);
		auto Z = FMath::Sin(Angle) * FMath::Cos(Aperture);
		auto Y = FMath::Sin(Aperture);

		Vertices.Add(FVector(0, Length, 0));
		Vertices.Add(FVector(Radius * X, 0, Radius * Z));
		Normals.Add(FVector(X, Y, Z));
		Normals.Add(FVector(X, Y, Z));
	}

	auto LastIdx = SegNum - 1;
	for (auto SegIdx = 0; SegIdx < SegNum; ++SegIdx)
	{
		// Side
		Indices.Add(2 * SegIdx);
		Indices.Add(2 * SegIdx + 1);
		Indices.Add(SegIdx == LastIdx ? 1 : 2 * SegIdx + 3);

		Indices.Add(2 * SegIdx);
		Indices.Add(SegIdx == LastIdx ? 1 : 2 * SegIdx + 3);
		Indices.Add(SegIdx == LastIdx ? 0 : 2 * SegIdx + 2);
	}
}

void UTempVisualizer::Init(const URoadNetComponent* InComponent)
{
	Component = InComponent;
}

FPrimitiveSceneProxy* UTempVisualizer::CreateSceneProxy()
{
	return new FRoadNetSceneProxy(this);
}

FBoxSphereBounds UTempVisualizer::CalcBounds(const FTransform& LocalToWorld) const
{
	if (Component)
	{
		float Extent = 0.f;
		for (auto Joint : Component->Joints)
		{
			Extent = FMath::Max(Joint->Position.Size(), Extent);
		}

		// TODO: Fix it with joints
		return FBoxSphereBounds(FVector::ZeroVector, FVector::OneVector * Extent, Extent);
	}
	else
	{
		return FBoxSphereBounds();
	}
}
