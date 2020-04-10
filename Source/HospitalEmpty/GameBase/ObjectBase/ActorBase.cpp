#include "GameBase/ObjectBase/ActorBase.h"
#include "Common/CommonLibrary.h"

AActorBase::AActorBase(FString InActorName) : Super()
{
	ActorName = InActorName;
}

void AActorBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	/* 4.10 在此取出该Actor的对应的Level，并且将该Level的名字存储在自身的StreamLevel中
	*/
	if (UObject* OuterLevel = UCommonLibrary::GetOuterLevel(this))
	{
		FString RealLevelName = OuterLevel->GetFullGroupName(true);
		StreamLevelName = RealLevelName;
	}
}