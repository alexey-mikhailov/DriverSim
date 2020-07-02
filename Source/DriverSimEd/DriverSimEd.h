#pragma once
#include "CoreMinimal.h"

class FDriverSimEditorModule : public IModuleInterface
{
public:
       virtual void StartupModule() override;
       virtual void ShutdownModule() override;
};