#include "TestObject.h"
#include "Engine/World.h"
#include "UMG/Widgets/CreateWidget.h"
#include "Base/HptGameInstance.h"

ATestObject::ATestObject(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void ATestObject::LoadUI()
{
	UHptGameInstance* Instance = UHptGameInstance::GetInstance(this);
	UUMGManager* UMGManager = Instance->GetUMGManager();

	UWorld* MyWorld = GetWorld();

	CreateWidget = UMGManager->CreateScreenWidget( MyWorld , CreateUIPath);
	UCreateWidget* Widget = Cast<UCreateWidget>(CreateWidget);
}
