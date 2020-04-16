#include "LoadLevelVolume.h"
#include "Engine/CollisionProfile.h"
#include "Components/BrushComponent.h"
#include "GameFrame/SaveGameSystem/SaveGameSystem.h"

ALoadLevelVolume::ALoadLevelVolume(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//GetBrushComponent()->SetCollisionProfileName(UCollisionProfile::BlockAllDynamic_ProfileName);

	//�������ײ�İ󶨷�������ǲ����ڱ༭��ģʽ�¾Ϳ��Բ����ˣ�
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

	SetCurOverLapLocationType(OverlapActor->GetActorLocation());

	/* Begin��ʱ��Ӧ��UnLoad�� */
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
}
#pragma optimize("",on)

void ALoadLevelVolume::SetCurOverLapLocationType(FVector InLocation)
{
	FVector OtherActorLocation = InLocation;

	//�ж�����໹���Ҳ�
	if (OtherActorLocation.X > LLBoundLocation.Center.X)  //Actor���Ҳ�
	{
		if (OtherActorLocation.Y > LLBoundLocation.Center.Y)	//���ϲ�
		{
			//������ұ߸���
			if (FMath::Abs(OtherActorLocation.X - LLBoundLocation.RightX) < FMath::Abs(OtherActorLocation.Y - LLBoundLocation.TopY))
				CurOverlapLocationType = EOverlepLocationType::RIGHT;
			else
				CurOverlapLocationType = EOverlepLocationType::TOP;
		}
		else//�����²�
		{
			//������ұ߸���
			if (FMath::Abs(OtherActorLocation.X - LLBoundLocation.RightX) < FMath::Abs(OtherActorLocation.Y - LLBoundLocation.BottomY))
				CurOverlapLocationType = EOverlepLocationType::RIGHT;
			else
				CurOverlapLocationType = EOverlepLocationType::BOTTOM;
		}
	}
	else  //Actor�����
	{
		if (OtherActorLocation.Y > LLBoundLocation.Center.Y)	//���ϲ�
		{
			//�������߸���
			if (FMath::Abs(OtherActorLocation.X - LLBoundLocation.LeftX) < FMath::Abs(OtherActorLocation.Y - LLBoundLocation.TopY))
				CurOverlapLocationType = EOverlepLocationType::LEFT;
			else
				CurOverlapLocationType = EOverlepLocationType::TOP;
		}
		else//�����²�
		{
			//�������߸���
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
