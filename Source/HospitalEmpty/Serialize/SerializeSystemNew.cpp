#include "SerializeSystemNew.h"
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

USerializeSystemNew* USerializeSystemNew::Get(const UObject* WorldContextObject)
{
	UWorld* MyWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (MyWorld)
	{
		UHptGameInstance* GameInstance = Cast<UHptGameInstance>(MyWorld->GetGameInstance());
		if (GameInstance)
		{
			USerializeSystemNew* Out = GameInstance->GetSerializeSystem();
			return Out;
		}
	}

	return nullptr;
}

USerializeSystemNew::USerializeSystemNew(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SavePath = *FString::Printf(TEXT("%s%s"), *FPaths::ProjectContentDir(), *SaveFileName);
}

bool USerializeSystemNew::SaveAllActorData(const UObject* WorldContextObject)
{
	/* 先把当前保存的ID清空 */
	CurrentFObjSerializeDataID.Empty();

	TArray<AActor*> SaveActors;
	UGameplayStatics::GetAllActorsWithInterface(WorldContextObject, USaveableActorInterface::StaticClass(), SaveActors);

	//将当前数据添加到保存结构中
	FGameSerializeData GameSaveSerializeData;
	GameSaveSerializeData.GameID = "SaveData1";
	GameSaveSerializeData.Timestamp = FDateTime::Now();

	for (AActor* InActor : SaveActors)
	{
		//最终需要保存的包含了所有序列化完成数据的Map
		TMap< FString, FObjSerializeData> TempData;
		SaveObjToData(Cast<UObject>(InActor) , TempData , SaveActors);
		
		GameSaveSerializeData.SerializeObj = TempData;
	}

	return SaveGameSerializeDataToFile(GameSaveSerializeData);
}

/* 需要递归去取Obj身上的可保存结构 */
void USerializeSystemNew::SaveObjToData(UObject* InObj, TMap< FString, FObjSerializeData> &OutData, const TArray<AActor*> InSaveActor)
{

	//先创建一个存储Actor的结构体
	FObjSerializeData ActorRecord;
	ActorRecord.Name = FName(*InObj->GetName());
	ActorRecord.Class = InObj->GetClass()->GetPathName();
	if (AActor* TempActor = Cast<AActor>(InObj))
		ActorRecord.ActorTransForm = TempActor->GetTransform();
	else
		ActorRecord.ActorTransForm = FTransform::Identity;

	//创建用来进行序列化的结构
	FMemoryWriter MemoryWriter(ActorRecord.SerializeData, true);
	FSaveOjbArchive Ar(MemoryWriter, true);
	InObj->Serialize(Ar);

	ActorRecord.ID = ActorRecord.Class + "_" + ActorRecord.Name.ToString();

	//将当前以保存ID记录
	CurrentFObjSerializeDataID.Add(ActorRecord.ID);

	/* 将该Actor上的所有继承自可保存接口的Obj保存 */
	ActorRecord.RefurrenceList = CheckSavableProject(InObj, OutData, InSaveActor);

	//在输出列表中添加上当前项
	OutData.Add(ActorRecord.ID, ActorRecord);

}
#pragma optimize("",off)
TArray<FRefurrenceData> USerializeSystemNew::CheckSavableProject(UObject* InObj, TMap< FString, FObjSerializeData> &OutData, const TArray<AActor *> InSaveActor)
{
	TArray< FRefurrenceData> RefurrenceList;

	//设置一下当前Obj的ID
	FString SerializeDataID = InObj->GetClass()->GetPathName() + "_" + InObj->GetName();

	UClass* ObjClass = InObj->GetClass();
	/* 遍历该Obj的所有属性 */
	for (TFieldIterator<UProperty> PropertyIterator(ObjClass); PropertyIterator; ++PropertyIterator)
	{
		UProperty* OneProperty = *PropertyIterator;
		if (UObjectProperty* ObjProperty = Cast<UObjectProperty>(OneProperty))
		{
			UObject* subObject = ObjProperty->GetObjectPropertyValue_InContainer(InObj);
			//如果既不在外部的需要序列化的Actor中，也不在当前已经序列化过的list中
			ISaveableActorInterface* SavableActor = Cast<ISaveableActorInterface>(subObject);
			if (SavableActor)
			{
				if(!CheckObjInArray(subObject, InSaveActor) && !CheckObjectBeSerialized(subObject))
				{
					SaveObjToData(subObject, OutData, InSaveActor);
				}
				//!然后，将该属性标记为需要外部进行设置！
				FName PropertyName = *OneProperty->GetNameCPP();
				//FString FullName = OneProperty->GetName();
				FRefurrenceData TempData;
				TempData.SerializeDataID = SerializeDataID;
				TempData.PropertyName = PropertyName;
				RefurrenceList.Add(TempData);//*OneProperty->GetNameCPP());
			}
		}
	}

	return RefurrenceList;
}
#pragma optimize("",on)
bool USerializeSystemNew::CheckObjInArray(UObject* InActor, const TArray<AActor*> InSaveActor)
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

bool USerializeSystemNew::CheckObjectBeSerialized(UObject* InObject)
{
	FName Name = FName(*InObject->GetName());
	FString Class = InObject->GetClass()->GetPathName();
	
	FString CurId = Class +"_" + Name.ToString() ;
	if (CurrentFObjSerializeDataID.Contains(CurId))
	{
		return true;
	}
	return false;
}

bool USerializeSystemNew::SaveGameSerializeDataToFile(FGameSerializeData &InData)
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
/*                              Load相关                                */
/************************************************************************/
bool USerializeSystemNew::LoadActorData(const UObject* WorldContextObject, FString LoadPath)
{
	TArray<uint8> BinaryData;
	//先写死作为测试
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

	FGameSerializeData LoadGameData;
	FromBinary << LoadGameData;

	FromBinary.FlushCache();
	BinaryData.Empty();
	FromBinary.Close();

	//将数据载入完之后进行正式的生成

	//要将加载出来的所有包括Actor和Component和UObject之类的东西全都加入到这个列表中，用来进行每一个Actor反序列化时的指针查找。
	//Key为对应的ID
	TMap<FString, UObject* > SerializeObjList;
	TMap<FString, TArray<FRefurrenceData> > RefurrenceData;

	for (TMap< FString, FObjSerializeData>::TConstIterator Iterator(LoadGameData.SerializeObj); Iterator; ++Iterator)
	{
		FObjSerializeData CurSerializeData = (Iterator->Value);

		FVector SpawnPos = CurSerializeData.ActorTransForm.GetLocation();
		FRotator SpawnRot = CurSerializeData.ActorTransForm.Rotator();
		FActorSpawnParameters SpawnParam;
		SpawnParam.Name = CurSerializeData.Name;
		UClass* SpawnClass = FindObject<UClass>(ANY_PACKAGE, *CurSerializeData.Class);
		//是有可能找不到的！如果未加载的话
		if (SpawnClass == nullptr)
			SpawnClass = LoadObject<UClass>(NULL, *CurSerializeData.Class);
		//如果是Actor，则生成到场景中，如果不是，则使用NewObject生成
		if (SpawnClass)
		{
			//AActor* DefaultActor = SpawnClass->GetDefaultObject<AActor>();
			UObject *Ret = SpawnClass->GetDefaultObject();
			if ( !Ret->IsA(AActor::StaticClass()))
			{
				UWorld* CurWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
				if (SpawnClass && CurWorld)
				{
					UObject* Obj = NewObject<UObject>(CurWorld , SpawnClass);
					FMemoryReader MemoryReader(CurSerializeData.SerializeData, true);
					FSaveOjbArchive Ar(MemoryReader, true);
					Obj->Serialize(Ar);
					SerializeObjList.Add(CurSerializeData.ID, Obj);
					RefurrenceData.Add(CurSerializeData.ID, CurSerializeData.RefurrenceList);
				}
			}
			//如果是Actor
			else
			{
				UWorld* CurWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
				if (SpawnClass && CurWorld)
				{
					AActor* NewActor = CurWorld->SpawnActor(SpawnClass, &SpawnPos, &SpawnRot, SpawnParam);
					FMemoryReader MemoryReader(CurSerializeData.SerializeData, true);
					FSaveOjbArchive Ar(MemoryReader, true);
					NewActor->Serialize(Ar);
					NewActor->SetActorTransform(CurSerializeData.ActorTransForm);

					SerializeObjList.Add(CurSerializeData.ID, NewActor);
					RefurrenceData.Add(CurSerializeData.ID, CurSerializeData.RefurrenceList);
				}
			}
			
		}
	}

	//所有需要序列化的OBJ生成完毕之后，进行引用指针的重定向,所有能够保存的OBJ，都继承了保存接口
	for (TMap<FString, UObject* >::TConstIterator Iterator(SerializeObjList); Iterator; ++Iterator)
	{
		ISaveableActorInterface* SavableActor = Cast<ISaveableActorInterface>(Iterator->Value);
		if (SavableActor)
		{
			SavableActor->RePointRefurrence(RefurrenceData[Iterator->Key], SerializeObjList);
		}
	}

	return true;
}