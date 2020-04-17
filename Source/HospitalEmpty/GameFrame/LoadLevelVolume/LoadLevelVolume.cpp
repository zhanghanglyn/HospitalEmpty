#include "LoadLevelVolume.h"
#include "Engine/CollisionProfile.h"
#include "Components/BrushComponent.h"
#include "GameFrame/SaveGameSystem/SaveGameSystem.h"
#include "Common/CommonLibrary.h"
#include "Sound/AudioVolume.h"

ALoadLevelVolume::ALoadLevelVolume(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//GetBrushComponent()->SetCollisionProfileName(UCollisionProfile::BlockAllDynamic_ProfileName);

	//如果将碰撞的绑定放在这里，那不是在编辑器模式下就可以测试了？
	OverlapBeginDele.Clear();
	OverlapBeginDele.BindUFunction(this, "OnTriggerOverlapBegin");
	GetBrushComponent()->OnComponentBeginOverlap.Add(OverlapBeginDele);

	OverlapEndDele.Clear();
	OverlapEndDele.BindUFunction(this, "OnTriggerOverlapEnd");
	GetBrushComponent()->OnComponentEndOverlap.Add(OverlapEndDele);
}

void ALoadLevelVolume::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	InitAroundLocation();
}

#pragma optimize("",off)
void ALoadLevelVolume::OnTriggerOverlapBegin(AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AActor* OverlapActor = OtherComp->GetOwner();
	if (!OverlapActor || !OverlapActor->GetRootComponent())
	{
		OverlapActor = Cast<AActor>(OtherComp);
		if (!OverlapActor || !OverlapActor->GetRootComponent())
			return;
	}
	if (!OverlapActor->Tags.Contains("player"))
		return;

	SetCurOverLapLocationType(OverlapActor->GetActorLocation());

	/* Begin的时候，应该UnLoad掉StreamLevel */
	TArray<FName> CurStreamLevelNames = GetStreamLevelNames(CurOverlapLocationType);

	USaveGameSystem::Get(this)->UnLoadStreamLevel(this, CurStreamLevelNames, this, "StreamLevelUnLoadedCallBack", nullptr);

}

void ALoadLevelVolume::OnTriggerOverlapEnd(AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AActor* OverlapActor = OtherComp->GetOwner();
	if (!OverlapActor || !OverlapActor->GetRootComponent())
	{
		OverlapActor = Cast<AActor>(OtherComp);
		if (!OverlapActor || !OverlapActor->GetRootComponent())
			return;
	}

	if (!OverlapActor->Tags.Contains("player"))
		return;

	SetCurOverLapLocationType(OverlapActor->GetActorLocation());

	TArray<FName> CurStreamLevelNames = GetStreamLevelNames(CurOverlapLocationType);

	USaveGameSystem::Get(this)->LoadStreamLevel(this, CurStreamLevelNames, this , "StreamLevelLoadedCallBack" , nullptr );
	
}

void ALoadLevelVolume::StreamLevelLoadedCallBack(UObject* InParam)
{
	UE_LOG(LogTemp, Warning, TEXT(" All the StreamLevelIsLoaded!!! "));
}
void ALoadLevelVolume::StreamLevelUnLoadedCallBack(UObject* InParam)
{
	UE_LOG(LogTemp, Warning, TEXT(" All the StreamLevelUnLoadedCallBack!!! "));
}


void ALoadLevelVolume::InitAroundLocation()
{
	FVector VolumeLocation = GetActorLocation();
	FBoxSphereBounds CurBounds = this->GetBounds();
	FBox CurBox = CurBounds.GetBox();
	
	LLBoundLocation.LeftX = VolumeLocation.X - CurBox.GetSize().X/2;
	LLBoundLocation.RightX = VolumeLocation.X + CurBox.GetSize().X / 2;
	LLBoundLocation.TopY = VolumeLocation.Y - CurBox.GetSize().Y / 2;
	LLBoundLocation.BottomY = VolumeLocation.Y + CurBox.GetSize().Y / 2;

	LLBoundLocation.Center = FVector2D( VolumeLocation.X , VolumeLocation.Y);

	//在此测试一下，能不能自己创建一个Volume 由于创建函数只是一个Editor的函数，所以，在正式创建之后应该保存在Level的序列化中，不用通过这个再次初始化
//#if WITH_EDITOR
//	if (!TestVolume)
//	{
//		if (UObject* OuterLevel = UCommonLibrary::GetOuterLevel(this))
//		{
//			FString RealLevelName = OuterLevel->GetFullGroupName(true);
//			UCommonLibrary::CreateBoxVolume(Cast<ULevel>(OuterLevel), AAudioVolume::StaticClass(), this->GetActorTransform(),
//				"Test_Wall!!!");
//		}
//	}
//	
//#endif



}
#pragma optimize("",on)

void ALoadLevelVolume::SetCurOverLapLocationType(FVector InLocation)
{
	FVector OtherActorLocation = InLocation;

	//判断在左侧还是右侧
	if (OtherActorLocation.X > LLBoundLocation.Center.X)  //Actor在右侧
	{
		if (OtherActorLocation.Y > LLBoundLocation.Center.Y)	//在上侧
		{
			//如果离右边更近
			if (FMath::Abs(OtherActorLocation.X - LLBoundLocation.RightX) < FMath::Abs(OtherActorLocation.Y - LLBoundLocation.TopY))
				CurOverlapLocationType = EOverlepLocationType::RIGHT;
			else
				CurOverlapLocationType = EOverlepLocationType::TOP;
		}
		else//在右下侧
		{
			//如果离右边更近
			if (FMath::Abs(OtherActorLocation.X - LLBoundLocation.RightX) < FMath::Abs(OtherActorLocation.Y - LLBoundLocation.BottomY))
				CurOverlapLocationType = EOverlepLocationType::RIGHT;
			else
				CurOverlapLocationType = EOverlepLocationType::BOTTOM;
		}
	}
	else  //Actor在左侧
	{
		if (OtherActorLocation.Y > LLBoundLocation.Center.Y)	//在上侧
		{
			//如果离左边更近
			if (FMath::Abs(OtherActorLocation.X - LLBoundLocation.LeftX) < FMath::Abs(OtherActorLocation.Y - LLBoundLocation.TopY))
				CurOverlapLocationType = EOverlepLocationType::LEFT;
			else
				CurOverlapLocationType = EOverlepLocationType::TOP;
		}
		else//在左下侧
		{
			//如果离左边更近
			if (FMath::Abs(OtherActorLocation.X - LLBoundLocation.LeftX) < FMath::Abs(OtherActorLocation.Y - LLBoundLocation.BottomY))
				CurOverlapLocationType = EOverlepLocationType::LEFT;
			else
				CurOverlapLocationType = EOverlepLocationType::BOTTOM;
		}
	}

}

TArray<FName> ALoadLevelVolume::GetStreamLevelNames(EOverlepLocationType InOverlepLocationType)
{
	switch (InOverlepLocationType)
	{
	case EOverlepLocationType::LEFT:
		return LeftStringLevelNames;
	case EOverlepLocationType::RIGHT:
		return RightStringLevelNames;
	case EOverlepLocationType::TOP:
		return TopStringLevelNames;
	case EOverlepLocationType::BOTTOM:
		return BottomStringLevelNames;
	default:
		break;
	}

	TArray<FName> Reusult;
	return Reusult;
}
