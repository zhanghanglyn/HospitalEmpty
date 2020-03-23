#include "SerializeSystemNew.h"
#include "Misc/Paths.h"
#include "Base/HptGameInstance.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"
#include "Engine.h"
#include "Misc/DateTime.h"
//#include "GameBase/GroundObject/GroundObj.h"
//#include "GameBase/GroundObject/GroundGridMgrComponent.h"
#include "Serialize/SaveableActorInterface.h"
#include "Misc/FileHelper.h"
#include "Serialize/testObj/ChildSerializeObj.h"
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

	ActorRecord.ID = InObj->GetClass()->GetName() + "_" + ActorRecord.Name.ToString();//ActorRecord.Class + "_" + ActorRecord.Name.ToString();

	//将当前以保存ID记录
	CurrentFObjSerializeDataID.Add(ActorRecord.ID);

	/* 将该Actor上的所有继承自可保存接口的Obj保存 */
	CheckSavableProject(InObj, OutData, ActorRecord.RefurrenceList , ActorRecord.ArrayRefurrenceList, InSaveActor);

	//在输出列表中添加上当前项
	OutData.Add(ActorRecord.ID, ActorRecord);

}
#pragma optimize("",off)
void USerializeSystemNew::CheckSavableProject(UObject* InObj, TMap<FString, FObjSerializeData> &OutData, TArray<FRefurrenceData> &RefData ,
	TArray<FRefurrenceArrayData> &RefArrayData , const TArray<AActor *> InSaveActor)
{
	//用来存储输出的Ojb引用的list
	//TArray< FRefurrenceData> RefurrenceList;
	//用来存储输出的TArray引用的List
	//TArray<FRefurrenceArrayData> RefurrenceArrayList;

	//设置一下当前Obj的ID
	FString SerializeDataID = InObj->GetClass()->GetName() + "_" + InObj->GetName();//InObj->GetClass()->GetPathName() + "_" + InObj->GetName();

	UClass* ObjClass = InObj->GetClass();
	/* 遍历该Obj的所有属性 */
	for (TFieldIterator<UProperty> PropertyIterator(ObjClass); PropertyIterator; ++PropertyIterator)
	{
		UProperty* OneProperty = *PropertyIterator;
		/* 如果该Property是一个Object类型的 */
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
				FRefurrenceData TempData;
				FString RefurrenceSerializeDataID = subObject->GetClass()->GetName() + "_" + subObject->GetName();
				TempData.SerializeDataID = RefurrenceSerializeDataID;//SerializeDataID;
				TempData.PropertyName = PropertyName;
				RefData.Add(TempData);//*OneProperty->GetNameCPP());
			}
		}
		/* 20.3.23 如果是一个TArray类型的！ */
		else if (UArrayProperty* ArrayProperty = Cast<UArrayProperty>(OneProperty))
		{
			bool BCanSave = false;
			//test
			//void* StructPtr;
			void* ValuePtr = ArrayProperty->ContainerPtrToValuePtr<void>(InObj);
			// We need the helper to get to the items of the array            
			FScriptArrayHelper Helper(ArrayProperty, ValuePtr);
			for (int32 i = 0, n = Helper.Num(); i < n; ++i)
			{
				if (UObjectProperty* InnerProperty = Cast<UObjectProperty>(ArrayProperty->Inner))
				{
					UObject* Ojb = InnerProperty->GetPropertyValue(Helper.GetRawPtr(i));
					if (ISaveableActorInterface* AllSavableActor = Cast<ISaveableActorInterface>(Ojb))
					//if (AChildSerializeObj* AllSavableActor = Cast<AChildSerializeObj>(Ojb))
					{
						BCanSave = true;
						break;
					}
				}
			}

			TArray<UObject*> OuterObject = *ArrayProperty->ContainerPtrToValuePtr<TArray<UObject*>>(InObj);
			if (BCanSave && OuterObject.Num() > 0 )//&& OuterObject[0]->GetArchetype ->IsA<UObject>())
			{
				//if (ISaveableActorInterface* AllSavableActor = Cast<ISaveableActorInterface>(OuterObject[0]))
				{
					/* 先生成一个保存对象 */
					FRefurrenceArrayData TempData;
					TempData.PropertyName = *OneProperty->GetNameCPP();
					/* 为可以保存的部分，才进行保存的循环 */
					for (UObject* EachObj : OuterObject)
					{
						if (ISaveableActorInterface* SavableActor = Cast<ISaveableActorInterface>(EachObj))
						{
							if (!CheckObjInArray(EachObj, InSaveActor) && !CheckObjectBeSerialized(EachObj))
							{
								SaveObjToData(EachObj, OutData, InSaveActor);
							}
							//!然后，将该属性标记为需要外部进行设置！
							FString RefurrenceSerializeDataID = EachObj->GetClass()->GetName() + "_" + EachObj->GetName();
							TempData.SerializeListID.Add(RefurrenceSerializeDataID);
						}
					}
					RefArrayData.Add(TempData);
				}
			}
		}

	}
	//return RefurrenceList;
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
	FString Class = InObject->GetClass()->GetName();//GetPathName();
	
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
	TMap<FString, TArray<FRefurrenceArrayData>> RefurrenceArrayData;

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
					RefurrenceArrayData.Add(CurSerializeData.ID, CurSerializeData.ArrayRefurrenceList);
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
					RefurrenceArrayData.Add(CurSerializeData.ID, CurSerializeData.ArrayRefurrenceList);
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
			SavableActor->RePointRefurrence(Iterator->Value,RefurrenceData[Iterator->Key], RefurrenceArrayData[Iterator->Key], SerializeObjList);
		}
	}

	UE_LOG(LogTemp ,Warning , TEXT("Load Object OVer~!"));

	return true;
}