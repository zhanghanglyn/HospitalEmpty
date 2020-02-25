#include "DecorationSystemMgr.h"
#include "Engine/Engine.h"
#include "HospitalEmpty/Base/HptGameInstance.h"
#include "GroundObj.h"
#include "Util/StateEnum.h"
#include "DecorationBase.h"
#include "HospitalEmpty/PlayerController/HptPlayerCameraPawn.h"
#include "HospitalEmpty/Common/CommonLibrary.h"
#include "Actor/GroundDefaultActor.h"

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

ADecorationBase* UDecorationSystemMgr::CreateDecoration(FVector Location, AActorBase* GridGround, EDecorationType DecorationType , bool BSetGroundData)
{
	FString DecorationName = FCommonLibrary::GetEnumValString((int)DecorationType, "EDecorationType");
	//组合路径位置 Blueprint'/Game/XXX/NAME/BP_NAME.BP_NAME_C'
	FString BPPath = DecorationBPPath + DecorationName + "/BP_" + DecorationName + ".BP_" + DecorationName + "_C'";
	
	//加载出对应的蓝图类后，生成Actor
	UClass* DecorationClass = LoadClass<ADecorationBase>(NULL, BPPath.GetCharArray().GetData());
	if (DecorationClass)
	{
		UWorld* MyWorld = GEngine->GetWorldFromContextObject(GridGround, EGetWorldErrorMode::LogAndReturnNull);
		if (MyWorld)
		{
			if (ADecorationBase* Decoration = MyWorld->SpawnActor<ADecorationBase>(DecorationClass))
			{

				Decoration->SetActorLocation(Location);

				/* 为生成的家具设置地面等相关 */
				if (BSetGroundData == true)
				{
					if (AGroundObj* GroundObj = Cast<AGroundObj>(GridGround))
					{
						Decoration->SetGround(GroundObj);
						//Decoration->UpdateGridByGround();
						Decoration->MoveTo(Location);
					}
				}
				
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
20.2.25 全部都会提成状态
*/
void UDecorationSystemMgr::OnMouseClickStart()
{
	if (PlayerPawn == nullptr)
		return;

	//如果当前处于空闲状态，则在对应位置生成一个家具
	if (CurControlType == ControlType::None)
	{
		FVector CreateLocation;
		AActorBase* ResultGround = PlayerPawn->GetMouseLocationInGround(CreateLocation);
		CurGridGround = Cast<AGroundObj>(ResultGround);
		if(CurGridGround != nullptr)
		{
			CreateLocation.Z += 1;
			CurDecoration = CreateDecoration(CreateLocation, CurGridGround, EDecorationType::DecorationBase);
			CurControlType = ControlType::PreArrange;
		}	
		//如果在默认地面上
		else
		{
			AGroundDefaultActor* DefaultGround = Cast<AGroundDefaultActor>(ResultGround);
			CreateLocation.Z += 3;
			CurDecoration = CreateDecoration(CreateLocation, DefaultGround, EDecorationType::DecorationBase , false);
			CurControlType = ControlType::PreArrange;
		}
	}
	//如果在预放置中（没有放置与地面上）
	else if (CurControlType == ControlType::PreArrange)
	{
		UE_LOG(LogTemp, Warning, TEXT("Not In The Ground"));
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

	if (CurControlType == ControlType::PreArrange)
	{
		if (CurDecoration)
		{
			FVector PreLocation = CurDecoration->GetActorLocation();
			FVector MoveToLocation = LastDecorationLocation;
			//如果射到了地面，则直接将家具布置到场景中
			AActorBase* ResultGround = PlayerPawn->GetMouseLocationInGround(MoveToLocation);
			AGroundObj* HitGround = Cast<AGroundObj>(ResultGround);
			if (HitGround != nullptr)
			{
				CurDecoration->SetActorLocation(FVector(MoveToLocation.X, MoveToLocation.Y, PreLocation.Z));

				/* 为生成的家具设置地面等相关 */
				CurDecoration->SetGround(HitGround);
				CurGridGround = HitGround;
				CurDecoration->MoveTo(MoveToLocation);
				CurControlType = ControlType::ArrangeDecoration;
			}
			else
			{
				CurDecoration->SetActorLocation(FVector(MoveToLocation.X, MoveToLocation.Y, PreLocation.Z));
			}
			LastDecorationLocation = MoveToLocation;
		}
	}
	//如果当前属于布置家具状态
	else if (CurControlType == ControlType::ArrangeDecoration)
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