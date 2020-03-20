#include "SerializeSystem.h"
#include "Misc/Paths.h"
#include "Base/HptGameInstance.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"
#include "Engine.h"
#include "Misc/DateTime.h"
#include "GroundObj.h"
#include "RTSMode/Public/GroundObject/GroundGridMgrComponent.h"
#include "Serialize/SaveableActorInterface.h"
#include "Misc/FileHelper.h"
#include "UObject/UnrealType.h"

USerializeSystem* USerializeSystem::Get(const UObject* WorldContextObject)
{
	UWorld* MyWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (MyWorld)
	{
		UHptGameInstance* GameInstance = Cast<UHptGameInstance>(MyWorld->GetGameInstance());
		if (GameInstance)
		{
			//USerializeSystem* Out = GameInstance->GetSerializeSystem();
			return nullptr;
		}
	}

	return nullptr;
}

USerializeSystem::USerializeSystem(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SavePath = *FString::Printf(TEXT("%s%s"), *FPaths::ProjectContentDir(), *SaveFileName);
}

bool USerializeSystem::SaveAllActorData(const UObject* WorldContextObject)
{
	TArray<AActor*> SaveActors;
	UGameplayStatics::GetAllActorsWithInterface(WorldContextObject, USaveableActorInterface::StaticClass(), SaveActors);
	//UGameplayStatics::GetAllActorsOfClass(WorldContextObject, AActorBase::StaticClass(), SaveActors);

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

		/* 将该Actor上的所有继承自可保存接口的Obj保存 */
		GetSavableObjAndSave(InActor , SaveActors);

		GameSaveSerializeData.SerializeActors.Add(ActorRecord);
	}

	return SaveGameSerializeDataToFile(GameSaveSerializeData);
}

#pragma optimize("",off)
/*	该函数，会用过UClass来找寻所有的UPROPERTY的Saveable对象
	@Warning!! 还需要注意的一点是，如果该Property，是准备序列化的Actor中的一部分，则不需要再额外进行序列化保存了。
*/
TArray<FPropertySerializeData> USerializeSystem::GetSavableObjAndSave(AActor* InActor , const TArray<AActor*> InSaveActor)
{
	TArray<FPropertySerializeData> ReturnData;

	UClass* ActorClass = InActor->GetClass();
	TMap<FString, FString> NativePropertyValues;
	/* 测试一下，如果遍历这个属性 */
	for (TFieldIterator<UProperty> PropertyIterator(ActorClass); PropertyIterator; ++PropertyIterator)
	{
		UProperty* PropertyT = *PropertyIterator;
		if (PropertyT)
		{
			//获取对象或结构里的属性值地址，需要自己转换成具体类型
			//void* propertyValuePtr = PropertyT->ContainerPtrToValuePtr<void*>(InActor);
			//UObject* Pointer = Cast<UObject>(propertyValuePtr);
			//包含对象引用的属性可以获得对象
			if (UObjectProperty* ObjProperty = Cast<UObjectProperty>(PropertyT))
			{
				UObject* subObject = ObjProperty->GetObjectPropertyValue_InContainer(InActor);

				ISaveableActorInterface* SavableActor = Cast<ISaveableActorInterface>(subObject);
				if (!CheckActorInArray(subObject , InSaveActor) && SavableActor)
				{
					/*FPropertySerializeData ComponentRecord;
					ComponentRecord.ComponentClass = subObject->GetClass()->GetName();
					ComponentRecord.ComponentName = FName(*subObject->GetName());
					ComponentRecord.OuterClass = InActor->GetClass()->GetName();
					ComponentRecord.OuterName = FName(*InActor->GetName());

					FMemoryWriter MemoryWriter(ComponentRecord.ComponentData, true);
					FSaveActorArchive Ar(MemoryWriter, true);
					subObject->Serialize(Ar);

					ReturnData.Add(ComponentRecord);*/
					int32 a = 1;
				}
			}
		}
	}
	return ReturnData;
}

bool USerializeSystem::CheckActorInArray(UObject* InActor, const TArray<AActor*> InSaveActor)
{
	for (AActor* Each : InSaveActor)
	{
		if (AActor* Actor = Cast<AActor>(InActor))
		{
			if (Actor == Each)
				return true;
		}
	}
	return false;
}
#pragma optimize("",on)

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
		return false;
		

	FMemoryReader FromBinary = FMemoryReader(BinaryData, true);
	FromBinary.Seek(0);

	FGameActorSerializeData LoadGameData;
	FromBinary << LoadGameData;

	FromBinary.FlushCache();
	BinaryData.Empty();
	FromBinary.Close();

	//应该是要将加载出来的所有包括Actor和Component和UObject之类的东西全都加入到这个列表中，用来进行每一个Actor反序列化时的指针查找。
	TMap<FName, UObject* > SerializeObjList;

	for (FActorSerializeData ActorData : LoadGameData.SerializeActors)
	{
		/* 生成的Actor */
		AActor* NewActor = nullptr;

		FVector SpawnPos = ActorData.ActorTransForm.GetLocation();
		FRotator SpawnRot = ActorData.ActorTransForm.Rotator();
		FActorSpawnParameters SpawnParam;
		SpawnParam.Name = ActorData.ActorName;
		UClass* SpawnClass = FindObject<UClass>(ANY_PACKAGE, *ActorData.ActorClass);
		//是有可能找不到的！如果未加载的话
		if (SpawnClass == nullptr)
			SpawnClass = LoadObject<UClass>(NULL, *ActorData.ActorClass);

		if (SpawnClass)
		{
			UWorld* CurWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
			if (SpawnClass && CurWorld)
			{
				NewActor = CurWorld->SpawnActor(SpawnClass, &SpawnPos, &SpawnRot, SpawnParam);
				FMemoryReader MemoryReader(ActorData.ActorData, true);
				FSaveActorArchive Ar(MemoryReader, true);
				NewActor->Serialize(Ar);
				NewActor->SetActorTransform(ActorData.ActorTransForm);

				SerializeObjList.Add(ActorData.ActorName, NewActor);
			}
		}

		/*
			生成该Actor对应的Property部分
		*/
		for (FPropertySerializeData PropertyData : ActorData.PropertyData)
		{
			
		}
	}

	return true;
}

#pragma optimize("",on)


/************************************************************************/
/*                                                                      */
/************************************************************************/