#pragma once

#include "CoreMinimal.h"
#include "Components/PrimitiveComponent.h"
#include "TempVisualizer.generated.h"

class URoadNetComponent;

UCLASS(collapsecategories, hidecategories = Object, editinlinenew, MinimalAPI)
class UTempVisualizer : public UPrimitiveComponent
{
	GENERATED_BODY()

public:
	const URoadNetComponent* Component;

	UPROPERTY()
	UMaterial* Material;

	// TODO: Remove
	TArray<uint32> Indices;
	TArray<FVector> Vertices;
	TArray<FVector> Normals;

	UTempVisualizer();
	void Init(const URoadNetComponent* Owner);

	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;

protected:
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;

#if WITH_EDITOR
	virtual bool IgnoreBoundsForEditorFocus() const override { return true; }
#endif
};
