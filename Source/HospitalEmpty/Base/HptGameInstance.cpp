#include "HptGameInstance.h"
#include "HospitalEmpty/GridSystem/DecorationSystemMgr.h"
#include "UMG/UMGManager.h"
#include "Serialize/SerializeSystem.h"

UHptGameInstance* UHptGameInstance::GetInstance(const UObject* WorldContextObject)
{
	UWorld* MyWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (MyWorld)
	{
		UHptGameInstance* GameInstance = Cast<UHptGameInstance>(MyWorld->GetGameInstance());
		if (GameInstance)
			return GameInstance;
	}

	return nullptr;
}

void UHptGameInstance::Init()
{
	if (DecorationSystemMgr == nullptr)
		DecorationSystemMgr = NewObject<UDecorationSystemMgr>(this, TEXT("DecorationSystemMgr"));

	if(UMGManger == nullptr)
		UMGManger = NewObject<UUMGManager>(this, TEXT("UMGManager"));

	if(SerializeSystem == nullptr)
		SerializeSystem = NewObject<USerializeSystem>(this, TEXT("SerializeSystem"));
}

void UHptGameInstance::Shutdown()
{

}
