#include "SerializeSystem.h"
#include "Misc/Paths.h"
#include "Base/HptGameInstance.h"
#include "Serialization/MemoryWriter.h"
#include "Engine.h"
#include "Misc/DateTime.h"
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
		UE_LOG(  LogTemp , Warning , TEXT(" SaveFileTo Error!!! "));
		return false;
	}

	BinaryData.FlushCache();
	BinaryData.Empty();

	return true;
}
		