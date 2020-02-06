#include "GridSystemMgr.h"
#include "Engine/Engine.h"
#include "HospitalEmpty/Base/HptGameInstance.h"

UGridSystemMgr* UGridSystemMgr::Get(const UObject * WorldContextObject)
{
	UWorld* MyWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (MyWorld)
	{
		UHptGameInstance* GameInstance = Cast<UHptGameInstance>(MyWorld->GetGameInstance());
		if (GameInstance)
		{
			UGridSystemMgr* Out = GameInstance->GetGridSystemMgr();
			return Out;
		}
	}
	return nullptr;

}