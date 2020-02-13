#include "DecorationSystemMgr.h"
#include "Engine/Engine.h"
#include "HospitalEmpty/Base/HptGameInstance.h"
#include "GroundObj.h"
#include "DecorationBase.h"
#include "HospitalEmpty/PlayerController/HptPlayerCameraPawn.h"
#include "HospitalEmpty/Common/CommonLibrary.h"

UDecorationSystemMgr* UDecorationSystemMgr::Get(const UObject * WorldContextObject)
{
	UWorld* MyWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (MyWorld)
	{
		UHptGameInstance* GameInstance = Cast<UHptGameInstance>(MyWorld->GetGameInstance());
		if (GameInstance)
		{
			UDecorationSystemMgr* Out = GameInstance->GetDecorationSystemMgr();
			return Out;
		}
	}
	return nullptr;

}

void UDecorationSystemMgr::SetPlayerPawn(AHptPlayerCameraPawn* InPlayerPawn)
{
	PlayerPawn = InPlayerPawn;
}

ADecorationBase* UDecorationSystemMgr::CreateDecoration(FVector Location, AGroundObj* GridGround, EDecorationType DecorationType)
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

				Decoration->SetActorLocation(Location);

				/* 为生成的家具设置地面等相关 */
				Decoration->SetGround(GridGround);
				//Decoration->UpdateGridByGround();
				Decoration->MoveTo(Location);

				return Decoration;
			}
		}
	}

	return nullptr;
}

/************************************************************************/
/*                            点击相关                                   */
/************************************************************************/ 

/*
首先去检测，是否点击到了格子地面，如果点击到了格子地面，则生成一个家具
之后会使用状态机?看看是否有必要把  
暂时都生成Base家具测试
*/
void UDecorationSystemMgr::OnMouseClickStart()
{
	if (PlayerPawn == nullptr)
		return;

	//如果当前处于空闲状态，则在对应位置生成一个家具
	if (CurControlType == ControlType::None)
	{
		FVector CreateLocation;
		CurGridGround = PlayerPawn->GetMouseLocationInGround(CreateLocation);
		if(CurGridGround != nullptr)
		{
			CreateLocation.Z += 1;
			CurDecoration = CreateDecoration(CreateLocation, CurGridGround, EDecorationType::DecorationBase);
			CurControlType = ControlType::ArrangeDecoration;
		}	
	}
	//放置家具
	else if (CurControlType == ControlType::ArrangeDecoration)
	{
		if (!CurGridGround->SaveCurDecoration(CurDecoration))
		{
			CurControlType = ControlType::None;
			CurDecoration = nullptr;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Block is Grid Block！！！！！！！！"));

		}
			
	}
		
}

void UDecorationSystemMgr::OnMouseHover()
{
	if (PlayerPawn == nullptr)
	{
		return;
	}

	//如果当前属于布置家具状态
	if (CurControlType == ControlType::ArrangeDecoration)
	{
		if (CurDecoration)
		{

			FVector PreLocation = CurDecoration->GetActorLocation();
			FVector MoveToLocation = LastDecorationLocation;
			PlayerPawn->GetMouseLocationInGround(MoveToLocation);
			//CurDecoration->SetActorLocation(FVector(MoveToLocation.X, MoveToLocation.Y, PreLocation.Z));
			CurDecoration->MoveTo(FVector(MoveToLocation.X, MoveToLocation.Y, PreLocation.Z));
			LastDecorationLocation = MoveToLocation;
		}
	}
}