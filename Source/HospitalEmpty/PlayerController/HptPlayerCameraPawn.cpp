#include "HptPlayerCameraPawn.h"
#include "ActorBase.h"
#include "DecorationBase.h"
#include "HospitalEmpty/GridSystem/DecorationSystemMgr.h"
#include "GroundObject/GroundObj.h"
#include "Actor/GroundDefaultActor.h"
#include "Engine/Engine.h"
#include "FSM/FSMMgr.h"
#include "FSM/StatePlayerControl/StateIdle.h"
#include "Runtime/Engine/Public/Engine.h"
#include "FSM/StatePlayerControl/StatePreArrange.h"
#include "FSM/StatePlayerControl/StateArrange.h"
#include "FSM/StatePlayerControl/CreateWall/StateArrangeWall.h"

AHptPlayerCameraPawn::AHptPlayerCameraPawn(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void AHptPlayerCameraPawn::SetupPlayerInputComponent(UInputComponent* InInputComponent)
{
	check(InInputComponent);

	Super::SetupPlayerInputComponent(InInputComponent);


}

void AHptPlayerCameraPawn::OnMouseClickStart()
{
	Super::OnMouseClickStart();

	if (FSMMgr)
		FSMMgr->OnMouseClickStart();
	//家具系统管理器
	//if (DecorationSystemMgr)
	//	DecorationSystemMgr->OnMouseClickStart();

}

void AHptPlayerCameraPawn::OnMouseClickMove()
{
	Super::OnMouseClickMove();
	if (FSMMgr)
		FSMMgr->OnMouseClickMove();
}

void AHptPlayerCameraPawn::OnMouseClickEnd()
{
	Super::OnMouseClickEnd();
	if (FSMMgr)
		FSMMgr->OnMouseClickEnd();
}

void AHptPlayerCameraPawn::OnMouseHover()
{
	Super::OnMouseHover();

	//UE_LOG( LogTemp , Warning , TEXT("Hovering!!!!!") );

	//家具系统管理器
	//if (DecorationSystemMgr)
	//	DecorationSystemMgr->OnMouseHover();
	if (FSMMgr)
		FSMMgr->OnMouseHover();

}

void AHptPlayerCameraPawn::BeginPlay()
{
	Super::BeginPlay();
	//测试先创建一个家具

	DecorationSystemMgr = UDecorationSystemMgr::Get(this);
	DecorationSystemMgr->SetPlayerPawn(this);

	//创建一个玩家操作状态机
	InitFSM();
}

void AHptPlayerCameraPawn::Tick(float deltaSeconds)
{
	Super::Tick(deltaSeconds);

	if (FSMMgr)
		FSMMgr->UpdateState();
	
}

/************************************************************************/
/*                              状态机相关                              */
/************************************************************************/
#pragma optimize("",off)
void AHptPlayerCameraPawn::InitFSM()
{
	if (FSMMgr == nullptr)
		FSMMgr = NewObject<UFSMMgr>(this, TEXT("ControlFSMMgr"));

	/*** 创建多个状态State ***/
	/*创建一个普通状态*/
	UStateIdle* IdleState = NewObject< UStateIdle>(FSMMgr);
	IdleState->AddCondition(ETransConditionID::C_CREATEFURNITURE , EStateEnum::PRE_ARRANGE);
	//3.9 为Idle添加一个转换状态，当Idle状态点击了家具时，直接设置为可拖动
	IdleState->AddCondition(ETransConditionID::C_IdleClickToArrange, EStateEnum::ARRANGE);
	IdleState->SetPlayerPawn(this);
	IdleState->SetFSMMgr(FSMMgr);

	/*创建一个预布置状态*/
	UStatePreArrange* PreArrange = NewObject< UStatePreArrange>(FSMMgr);
	PreArrange->AddCondition(ETransConditionID::C_PreToIdle, EStateEnum::IDLE);
	PreArrange->AddCondition(ETransConditionID::C_CreateToArrange, EStateEnum::ARRANGE);
	PreArrange->AddCondition(ETransConditionID::C_CreateToWallArrange, EStateEnum::WALL_ARRANGE);
	PreArrange->SetDecorationSystemMgr(DecorationSystemMgr);
	PreArrange->SetPlayerPawn(this);
	PreArrange->SetFSMMgr(FSMMgr);

	/* 创建一个布置状态 */
	UStateArrange* StateArrange = NewObject< UStateArrange>(FSMMgr);
	StateArrange->AddCondition(ETransConditionID::C_PreToIdle, EStateEnum::IDLE);
	StateArrange->SetDecorationSystemMgr(DecorationSystemMgr);
	StateArrange->SetPlayerPawn(this);
	StateArrange->SetFSMMgr(FSMMgr);

	/* 创建布置墙的状态 */
	UStateArrangeWall* StateArrangeWall = NewObject< UStateArrangeWall>(FSMMgr);
	StateArrangeWall->AddCondition(ETransConditionID::C_PreToIdle, EStateEnum::IDLE);
	StateArrangeWall->SetDecorationSystemMgr(DecorationSystemMgr);
	StateArrangeWall->SetPlayerPawn(this);
	StateArrangeWall->SetFSMMgr(FSMMgr);

	FSMMgr->Init(EStateEnum::IDLE, IdleState);
	FSMMgr->AddState(EStateEnum::PRE_ARRANGE, PreArrange);
	FSMMgr->AddState(EStateEnum::ARRANGE, StateArrange);
	FSMMgr->AddState(EStateEnum::WALL_ARRANGE, StateArrangeWall);
}

void AHptPlayerCameraPawn::SetStatePreArrange(EDecorationType InDecorationType)
{
	UPreArrangeParam* TempParam = NewObject<UPreArrangeParam>(FSMMgr);
	TempParam->DecorationType = InDecorationType;
	FSMMgr->TransState(ETransConditionID::C_CREATEFURNITURE, TempParam);
	TempParam->RemoveFromRoot();
	TempParam = nullptr;
}
#pragma optimize("",on)


/************************************************************************/
/*                                                                      */
/************************************************************************/
#pragma optimize("",off)
AActorBase* AHptPlayerCameraPawn::GetMouseLocationInGround(FVector &GroundLocation) const
{
	UWorld* world = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull);

	if (world == nullptr)
		return nullptr;

	FVector WorldPos;
	FVector Dir;
	GetMouseWorldPos(WorldPos, Dir);

	FVector StartPos = WorldPos;
	FVector ForwardVector = CameraComponent->GetForwardVector();
	StartPos.Z = GetActorLocation().Z;
	TArray<FHitResult> temp_HitResult;

	FCollisionQueryParams CollisionParam(FName(TEXT("Combattrace")), true, NULL);
	CollisionParam.bTraceComplex = true;
	//CollisionParam.bReturnPhysicalMaterial = false;
	//CollisionParam.AddIgnoredActor(this);

	world->LineTraceMultiByObjectType(temp_HitResult, StartPos, StartPos + Dir * RayLength, FCollisionObjectQueryParams::AllObjects, CollisionParam);
	//DrawDebugLine(world, StartPos, StartPos + Dir * RayLength, FColor::Red, true, RayLength);

	//20.2.24 新添加默认地面，如果没有射到格子地面，则返回射到默认地面的值
	FVector HitDefaultLocation;
	AGroundDefaultActor* DefaultGround = nullptr;

	if (temp_HitResult.Num() > 0)
	{
		for (int i = 0; i < temp_HitResult.Num(); i++)
		{
			AActor* HitActor = temp_HitResult[i].GetActor();
			/* 把取得的Actor转化为地面 */
			if (AGroundObj* HitGround = Cast<AGroundObj>(HitActor))
			{
				GroundLocation = temp_HitResult[i].Location;

				return HitGround;
			}
			else if(AGroundDefaultActor* HitDefaultGround = Cast<AGroundDefaultActor>(HitActor))
			{
				HitDefaultLocation = temp_HitResult[i].Location;
				DefaultGround = HitDefaultGround;
			}
		}
	}

	GroundLocation = HitDefaultLocation;
	return DefaultGround;
}

AActorBase* AHptPlayerCameraPawn::GetMouseHitDecoration(FHitResult &InHitResult , FVector &GroundLocation) const
{
	UWorld* world = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull);

	if (world == nullptr)
		return nullptr;

	FVector WorldPos;
	FVector Dir;
	GetMouseWorldPos(WorldPos, Dir);

	FVector StartPos = WorldPos;
	StartPos.Z = GetActorLocation().Z;
	TArray<FHitResult> temp_HitResult;

	FCollisionQueryParams CollisionParam(FName(TEXT("Combattrace")), true, NULL);
	CollisionParam.bTraceComplex = true;

	world->LineTraceMultiByObjectType(temp_HitResult, StartPos, StartPos + Dir * RayLength, FCollisionObjectQueryParams::AllObjects, CollisionParam);

	AActorBase* ResultHitDecoration = nullptr;

	if (temp_HitResult.Num() > 0)
	{
		for (int i = 0; i < temp_HitResult.Num(); i++)
		{
			AActor* HitActor = temp_HitResult[i].GetActor();
			/* 把取得的Actor转化为家具 */
			if (ADecorationBase* HitDecoration = Cast<ADecorationBase>(HitActor))
			{
				InHitResult = temp_HitResult[i];

				ResultHitDecoration = HitDecoration;
			}
			/* 把取得的Actor转化为地面 */
			if (AGroundObj* HitGround = Cast<AGroundObj>(HitActor))
			{
				GroundLocation = temp_HitResult[i].Location;
			}

		}
	}

	return ResultHitDecoration;

}

#pragma optimize("",on)