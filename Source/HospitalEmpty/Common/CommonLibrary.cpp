#include "CommonLibrary.h"
#include "Engine.h"
#include "UObject/ObjectMacros.h"
#if WITH_EDITOR
#include "Builders/CubeBuilder.h"
#include "BSPOps.h"
#endif

UCommonLibrary::UCommonLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FString UCommonLibrary::GetEnumValString(int EnumToGet, FString EnumType)
{
	UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, EnumType.GetCharArray().GetData(), true);//TEXT("EDecorationType"), true);
	FString CurShooterDataStr(EnumPtr->GetNameByValue((int)EnumToGet).ToString());//转化相应枚举字符串

	FString ClassName;

	CurShooterDataStr.Split("::", nullptr, &ClassName);

	return ClassName;
}

UObject* UCommonLibrary::GetOuterLevel(UObject* InObj)
{
	UObject* Top = (UObject*)InObj;
	for (;;)
	{
		UObject* CurrentOuter = Top->GetOuter();

		if (ULevel* OuterLevel = Cast<ULevel>(CurrentOuter))
		{
			return CurrentOuter;
		}

		/* 如果都没有找到Level */  
		if (!CurrentOuter)
		{
			return nullptr;
		}
		Top = CurrentOuter;
	}
}

#if WITH_EDITOR
#pragma optimize("",off)
AVolume* UCommonLibrary::CreateBoxVolume(ULevel* InLevel,UClass* InClass , FTransform SpawnTransform, const FName Name)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.OverrideLevel = InLevel;
	SpawnInfo.ObjectFlags = RF_Transient;
	SpawnInfo.Name = Name;

	AActor* CreateActor = InLevel->OwningWorld->SpawnActor(InClass, &SpawnTransform, SpawnInfo);
	if (!CreateActor)
	{
		UE_LOG(LogTemp, Warning, TEXT(" CreateBoxVolume SpawnActor Error!!! "));
		return nullptr;
	}

	//为创建出来的Volume建立笔刷
	AVolume* VolumeActor = CastChecked<AVolume>(CreateActor);
	if (VolumeActor != NULL)
	{
		UCubeBuilder* BrushBuilder = NewObject<UCubeBuilder>();
		
		VolumeActor->PreEditChange(NULL);

		VolumeActor->PolyFlags = 0;
		VolumeActor->Brush = NewObject<UModel>(VolumeActor, NAME_None, RF_Transactional);
		VolumeActor->Brush->Initialize(nullptr, true);
		VolumeActor->Brush->Polys = NewObject<UPolys>(VolumeActor->Brush, NAME_None, RF_Transactional);
		VolumeActor->GetBrushComponent()->Brush = VolumeActor->Brush;
		if (BrushBuilder != nullptr)
		{
			VolumeActor->BrushBuilder = DuplicateObject<UBrushBuilder>(BrushBuilder, VolumeActor);
		}

		BrushBuilder->Build(VolumeActor->GetWorld(), VolumeActor);

		FBSPOps::csgPrepMovingBrush(VolumeActor);

		// Set the texture on all polys to NULL.  This stops invisible textures
		// dependencies from being formed on volumes.
		if (VolumeActor->Brush)
		{
			for (int32 poly = 0; poly < VolumeActor->Brush->Polys->Element.Num(); ++poly)
			{
				FPoly* Poly = &(VolumeActor->Brush->Polys->Element[poly]);
				Poly->Material = NULL;
			}
		}

		VolumeActor->PostEditChange();
	}

	return VolumeActor;
}
#pragma optimize("",on)
#endif