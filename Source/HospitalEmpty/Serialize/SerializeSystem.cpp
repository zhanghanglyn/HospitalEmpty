#include "SerializeSystem.h"
#include "Misc/Paths.h"
#include "Base/HptGameInstance.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"
#include "Engine.h"
#include "Misc/DateTime.h"
#include "GroundObj.h"
#include "RTSMode/Public/GroundObject/GroundGridMgrComponent.h"
#include "Misc/FileHelper.h"

USerializeSystem* USerializeSystem::Get(const UObject* WorldContextObject)
{
	UWorld* MyWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (MyWorld)
	{
		UHptGameInstance* GameInstance = Cast<UHptGameInstance>(MyWorld->GetGameInstance());
		if (GameInstance)
		{
			USerializeSystem* Out = GameInstance->GetSerializeSystem();
			return Out;
		}
	}

	return nullptr;
}

USerializeSystem::USerializeSystem(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SavePath = *FString::Printf(TEXT("%s%s"), *FPaths::ProjectContentDir(), *SaveFileName);
}

bool USerializeSystem::SaveActorData(class AActor* InActor)
{
	//先创建一个存储Actor的结构体
	FActorSerializeData ActorRecord;
	ActorRecord.ActorName = FName(*InActor->GetName());
	FString TestSee = InActor->GetClass()->GetName();
	ActorRecord.ActorClass = InActor->GetClass()->GetPathName();
	ActorRecord.ActorTransForm = InActor->GetTransform();

	//创建用来进行序列化的结构
	FMemoryWriter MemoryWriter( ActorRecord.ActorData, true);
	FSaveActorArchive Ar(MemoryWriter,true);
	InActor->Serialize(Ar);

	//将当前数据添加到保存结构中
	FGameActorSerializeData GameSaveSerializeData;
	GameSaveSerializeData.SerializeActors.Add(ActorRecord);
	GameSaveSerializeData.GameID = "1";
	GameSaveSerializeData.Timestamp = FDateTime::Now();

	return SaveGameSerializeDataToFile(GameSaveSerializeData);
}

bool USerializeSystem::SaveAllActorData(const UObject* WorldContextObject)
{
	TArray<AActor*> SaveActors;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, AActorBase::StaticClass(), SaveActors);

	//将当前数据添加到保存结构中
	FGameActorSerializeData GameSaveSerializeData;
	//GameSaveSerializeData.SerializeActors.Add(ActorRecord);
	GameSaveSerializeData.GameID = "SaveData1";
	GameSaveSerializeData.Timestamp = FDateTime::Now();

	for (AActor* InActor : SaveActors)
	{
		//先创建一个存储Actor的结构体
		FActorSerializeData ActorRecord;
		ActorRecord.ActorName = FName(*InActor->GetName());
		FString TestSee = InActor->GetClass()->GetName();
		ActorRecord.ActorClass = InActor->GetClass()->GetPathName();
		ActorRecord.ActorTransForm = InActor->GetTransform();

		//创建用来进行序列化的结构
		FMemoryWriter MemoryWriter(ActorRecord.ActorData, true);
		FSaveActorArchive Ar(MemoryWriter, true);
		InActor->Serialize(Ar);

		GameSaveSerializeData.SerializeActors.Add(ActorRecord);
	}

	return SaveGameSerializeDataToFile(GameSaveSerializeData);
}


bool USerializeSystem::SaveGameSerializeDataToFile(FGameActorSerializeData &InData)
{
	FBufferArchive BinaryData;
	BinaryData << InData;

	if (FFileHelper::SaveArrayToFile(BinaryData, *SavePath))
	{
		UE_LOG(LogTemp, Warning, TEXT(" SaveFileTo Over~~ "));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT(" SaveFileTo Error!!! "));
		return false;
	}

	BinaryData.FlushCache();
	BinaryData.Empty();

	return true;
}

/************************************************************************/
/*                             Load     相关                            */
/************************************************************************/

#pragma optimize("",off)
bool USerializeSystem::LoadActorData(const UObject* WorldContextObject,FString LoadPath)
{
	TArray<uint8> BinaryData;
	if (FFileHelper::LoadFileToArray(BinaryData, *SavePath))
	{
		UE_LOG(LogTemp, Warning, TEXT(" LoadFileToArray Over~ "));
	}
	else
	{
		return false;
	}

	FMemoryReader FromBinary = FMemoryReader(BinaryData, true);
	FromBinary.Seek(0);

	FGameActorSerializeData LoadGameData;
	FromBinary << LoadGameData;

	FromBinary.FlushCache();
	BinaryData.Empty();
	FromBinary.Close();

	for (FActorSerializeData ActorData : LoadGameData.SerializeActors)
	{
		FVector SpawnPos = ActorData.ActorTransForm.GetLocation();
		FRotator SpawnRot = ActorData.ActorTransForm.Rotator();
		FActorSpawnParameters SpawnParam;
		SpawnParam.Name = ActorData.ActorName;
		UClass* SpawnClass = FindObject<UClass>(ANY_PACKAGE, *ActorData.ActorClass);
		//是有可能找不到的！如果未加载的话
		if (SpawnClass == nullptr)
			SpawnClass = LoadObject<UClass>(NULL, *ActorData.ActorClass);

		UWorld* CurWorld = GEngine->GetWorldFromContextObject(WorldContextObject,EGetWorldErrorMode::LogAndReturnNull);
		if (SpawnClass && CurWorld)
		{
			AActor* NewActor = CurWorld->SpawnActor(SpawnClass, &SpawnPos, &SpawnRot, SpawnParam);
			FMemoryReader MemoryReader(ActorData.ActorData, true);
			FSaveActorArchive Ar(MemoryReader,true);
			NewActor->Serialize(Ar);
			NewActor->SetActorTransform(ActorData.ActorTransForm);
		}
	}

	//20.3.19

	return true;
}

#pragma optimize("",on)