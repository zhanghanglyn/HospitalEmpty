#include "GridSystemMgr.h"
#include "Engine/Engine.h"
#include "HospitalEmpty/Base/HptGameInstance.h"
#include "GroundObj.h"
#include "DecorationBase.h"
#include "HospitalEmpty/Common/CommonLibrary.h"

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

ADecorationBase* UGridSystemMgr::CreateDecoration(FVector Location, AGroundObj* GridGround, EDecorationType DecorationType)
{
	FString DecorationName = FCommonLibrary::GetEnumValString((int)DecorationType, "EDecorationType");
	//组合路径位置 Blueprint'/Game/XXX/NAME/BP_NAME.BP_NAME_C'
	FString BPPath = DecorationBPPath + DecorationName + "/BP_" + DecorationName + ".BP_" + DecorationName + "_C'";
	
	//加载出对应的蓝图类后，生成Actor
	UClass* DecorationClass = LoadClass<ADecorationBase>(NULL, BPPath.GetCharArray().GetData());
	if (DecorationClass)
	{
		//ADecorationBase* Decoration = NewObject<ADecorationBase>(GridGround, Decoration->StaticClass());
		UWorld* MyWorld = GEngine->GetWorldFromContextObject(GridGround, EGetWorldErrorMode::LogAndReturnNull);
		if (MyWorld)
		{
			if (ADecorationBase* Decoration = MyWorld->SpawnActor<ADecorationBase>(DecorationClass))
			{
				return Decoration;
			}
		}
	}

	return nullptr;
}
