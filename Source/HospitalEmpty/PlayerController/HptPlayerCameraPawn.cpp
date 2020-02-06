#include "HptPlayerCameraPawn.h"
#include "HospitalEmpty/GridSystem/GridSystemMgr.h"
#include "GroundObject/GroundObj.h"
#include "Engine/Engine.h"

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

	//测试函数，再场景中添加一个Decoration
	if (UGridSystemMgr* Mgr = UGridSystemMgr::Get(this))
	{
		if (UWorld* world = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull))
		{
			AGroundObj* ttt = world->SpawnActor<AGroundObj>(AGroundObj::StaticClass());
			if (ttt)
			{
				Mgr->CreateDecoration(FVector::ZeroVector, ttt, EDecorationType::DecorationBase);
			}
		}
	}

}

void AHptPlayerCameraPawn::OnMouseClickMove()
{
	Super::OnMouseClickMove();
}

void AHptPlayerCameraPawn::OnMouseClickEnd()
{
	Super::OnMouseClickEnd();
}

void AHptPlayerCameraPawn::BeginPlay()
{
	Super::BeginPlay();
	//测试先创建一个家具

}

void AHptPlayerCameraPawn::Tick(float deltaSeconds)
{
	Super::Tick(deltaSeconds);

	
}

#pragma optimize("",on)
