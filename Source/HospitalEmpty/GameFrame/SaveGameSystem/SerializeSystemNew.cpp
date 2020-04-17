#include "SerializeSystemNew.h"
#include "Misc/Paths.h"
#include "Base/HptGameInstance.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"
#include "Engine.h"
#include "Misc/DateTime.h"
#include "GameFrame/SaveGameSystem/SaveableActorInterface.h"
#include "Misc/FileHelper.h"
#include "GameFrame/SaveGameSystem/testObj/ChildSerializeObj.h"
#include "UObject/UnrealType.h"
#include "Engine/Level.h"
//#include "AssetRegistryModule.h"
//#include "AssetData.h"

FString USerializeSystemNew::OUTER_TYPE_WORLD = "WORLD";
FString USerializeSystemNew::OUTER_TYPE_LEVEL = "LEVEL";


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

bool USerializeSystemNew::SaveAllActorData(const UObject* WorldContextObject , FString GameID, FGameSerializeData &OutData)
{
	/* 先把当前保存的ID清空 */
	CurrentFObjSerializeDataID.Empty();

	/* 4.12 思考是否需要考虑其显示问题？其实不用，没有显示的地图根本就不会加载，既然不会加载，也不需要判断，所有显示出来的保存即可 */
	TArray<AActor*> SaveActors;
	UGameplayStatics::GetAllActorsWithInterface(WorldContextObject, USaveableActorInterface::StaticClass(), SaveActors);

	//将当前数据添加到保存结构中
	FGameSerializeData GameSaveSerializeData;
	GameSaveSerializeData.GameID = *GameID;//"SaveData1";
	GameSaveSerializeData.Timestamp = FDateTime::Now();

	/* 3.27 存一下当前的level名字 */
	UWorld* MyWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (MyWorld)
	{
		ULevel* CurrentLevel = MyWorld->GetCurrentLevel();
		if (CurrentLevel)
			GameSaveSerializeData.LevelName = MyWorld->GetName();//CurrentLevel->GetName();

		GameSaveSerializeData.LevelPackageName = *(MyWorld->GetName());
	}

	/* 4.12 修改为，先对所有的Actor根据StreamLevel进行分类！因为Actor上面的那些Mgr等，有些Outer并不属于Level，所以并没有StreamLevelName */
	TMap< FString, TArray<AActor*>> OrderedActor;
	for (AActor* InActor : SaveActors)
	{
		if (ISaveableActorInterface* SaveAbleInterface = Cast<ISaveableActorInterface>(InActor))
		{
			FString StreamLevelName = SaveAbleInterface->GetStreamLevelName(InActor);
			//如果已有该StreamLevelName
			if (OrderedActor.Contains(StreamLevelName))
			{
				OrderedActor[StreamLevelName].Add(InActor);
			}
			else
			{
				TArray<AActor*> TempActor;
				TempActor.Add(InActor);
				OrderedActor.Add(StreamLevelName,TempActor);
				//4.13 将当前显示的StreamLevelName加入保存
				GameSaveSerializeData.ShowStreamLevelNames.Add(StreamLevelName);
			}
		}
	}

	/* 4.12 修改为根据StreamLevelName进行加载 */
	for (TMap< FString, TArray<AActor*>>::TConstIterator Iter = OrderedActor.CreateConstIterator(); Iter; ++Iter)
	{
		/* 如果不包含对应的StreamLevelName 添加一个 */
		if (!GameSaveSerializeData.SerializeObjByMap.Contains(Iter->Key))
		{
			TMap< FString, FObjSerializeData> TempSerializeData;
			GameSaveSerializeData.SerializeObjByMap.Add(Iter->Key, TempSerializeData);
		}
		for (AActor* InActor : Iter->Value)
		{
			//最终需要保存的包含了所有序列化完成数据的Map
			SaveObjToData(Cast<UObject>(InActor) , GameSaveSerializeData.SerializeObjByMap[Iter->Key], SaveActors);
		}
	}

	OutData = GameSaveSerializeData;

	return SaveGameSerializeDataToFile(GameSaveSerializeData , GameID);
}
//#pragma optimize("",off)
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
	//20.3.23 将GetClass->GetName换成
	ActorRecord.ID = InObj->GetClass()->GetName() + "_" + InObj->GetOuter()->GetName() + "_" + ActorRecord.Name.ToString();//ActorRecord.Class + "_" + ActorRecord.Name.ToString();
	/* 记录Outer */
	UObject* OjbOuter = InObj->GetOuter();
	if (UWorld* world = Cast<UWorld>(OjbOuter))
		ActorRecord.OuterType = OUTER_TYPE_WORLD;
	else if (ULevel* level = Cast<ULevel>(OjbOuter))
	{
		//4.9 可以获取正式的Level的Name！如果是streamLevel就会返回StreamLevel的Name , 而GetName永远都只返回Level最上层的那个
		FString RealLevelName = level->GetFullGroupName(true);
		ActorRecord.OuterType = OUTER_TYPE_LEVEL;
		ActorRecord.OuterID = OjbOuter->GetName();

		if (ISaveableActorInterface* SaveableObj = Cast<ISaveableActorInterface>(InObj))
		{
			FString StreamLevelName = SaveableObj->GetStreamLevelName(InObj);
			ActorRecord.StreamLevelName = StreamLevelName;
		}
	}
	else
	{
		ActorRecord.OuterID = OjbOuter->GetClass()->GetName() + "_" + OjbOuter->GetOuter()->GetName() + "_" + OjbOuter->GetName();
	}
	

	//将当前以保存ID记录
	CurrentFObjSerializeDataID.Add(ActorRecord.ID);

	/* 将该Actor上的所有继承自可保存接口的Obj保存 */
	CheckSavableProject(InObj, OutData, ActorRecord.RefurrenceList , ActorRecord.ArrayRefurrenceList, ActorRecord.MapRefurrenceList, InSaveActor);

	//在输出列表中添加上当前项
	OutData.Add(ActorRecord.ID, ActorRecord);

}

void USerializeSystemNew::CheckSavableProject(UObject* InObj, TMap<FString, FObjSerializeData> &OutData, TArray<FRefurrenceData> &RefData ,
	TArray<FRefurrenceArrayData> &RefArrayData , TArray<FRefurrenceMapData> &RefMapData, const TArray<AActor *> InSaveActor)
{
	//设置一下当前Obj的ID
	FString SerializeDataID = InObj->GetClass()->GetName() + "_" + InObj->GetOuter()->GetName() + "_" + InObj->GetName();//InObj->GetClass()->GetPathName() + "_" + InObj->GetName();

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
				FString RefurrenceSerializeDataID = subObject->GetClass()->GetName() + "_" + subObject->GetOuter()->GetName() + "_" + subObject->GetName();
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
							FString RefurrenceSerializeDataID = EachObj->GetClass()->GetName() + "_" + EachObj->GetOuter()->GetName() + "_" + EachObj->GetName();
							TempData.SerializeListID.Add(RefurrenceSerializeDataID);
						}
					}
					RefArrayData.Add(TempData);
				}
			}
		}
		/* 20.3.24 如果是一个Map类型的 */
		else if (UMapProperty* MapProperty = Cast<UMapProperty>(OneProperty))
		{
			bool BCanSave = false;
			//test
			void* ValuePtr = MapProperty->ContainerPtrToValuePtr<void>(InObj);
			// We need the helper to get to the items of the array            
			FScriptMapHelper Helper(MapProperty, ValuePtr);

			FRefurrenceMapData TempData;
			TempData.PropertyName = *OneProperty->GetNameCPP();

			//如果Value的值是可以保存的Object,循环遍历每一个并保存
			if (UObjectProperty* InnerProperty = Cast<UObjectProperty>(MapProperty->ValueProp))
			{
				for (int32 i = 0, n = Helper.Num(); i < n; ++i)
				{
					UObject* Ojb = InnerProperty->GetPropertyValue(Helper.GetValuePtr(i));
					if (ISaveableActorInterface* AllSavableActor = Cast<ISaveableActorInterface>(Ojb))
					{
						//FRefurrenceMapData TempData;
						//TempData.PropertyName = *OneProperty->GetNameCPP();
						/* 为可以保存的部分，才进行保存的循环 */
						
						if (ISaveableActorInterface* SavableActor = Cast<ISaveableActorInterface>(Ojb))
						{
							if (!CheckObjInArray(Ojb, InSaveActor) && !CheckObjectBeSerialized(Ojb))
							{
								SaveObjToData(Ojb, OutData, InSaveActor);
							}
							//!然后，将该属性标记为需要外部进行设置！
							FString RefurrenceSerializeDataID = Ojb->GetClass()->GetName() + "_" + Ojb->GetOuter()->GetName() + "_" + Ojb->GetName();
							TempData.SerializeListID.Add(RefurrenceSerializeDataID);
						}
						
						//RefMapData.Add(TempData);
					}
				}
			}
			RefMapData.Add(TempData);
		}
	}
	//return RefurrenceList;
}
//#pragma optimize("",on)
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
	
	FString CurId = Class + "_" + InObject->GetOuter()->GetName() +"_" + Name.ToString() ;
	if (CurrentFObjSerializeDataID.Contains(CurId))
	{
		return true;
	}
	return false;
}

bool USerializeSystemNew::SaveGameSerializeDataToFile(FGameSerializeData &InData , FString GameID)
{
	FBufferArchive BinaryData;
	BinaryData << InData;

	FString DataSavePath = SavePath + GameID;

	if (FFileHelper::SaveArrayToFile(BinaryData, *DataSavePath))// *SavePath))
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
/*
	20.3.26 修改添加Outer部分，如果不为Actor，则需要使用Outer来进行NewObject,如果Outer尚未创建，则新New一个Outer放置
	也有可能出现，需要的Outer也需要其他Outer的情况，使用函数遍历。

*/
bool USerializeSystemNew::LoadActorData(const UObject* WorldContextObject, FString GameID , FString StreamLevelName)
{
	FString DataLoadPath = SavePath + GameID;

	TArray<uint8> BinaryData;
	//先写死作为测试
	if (FFileHelper::LoadFileToArray(BinaryData, *DataLoadPath))
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
	TMap<FString, TArray<FRefurrenceMapData>> RefurrenceMapData;

	/*4.12 修改为需要判断StreamLevel的名字了，那么，就需要判断当前StreamLevelName是否为空！ */
	if ( !LoadGameData.SerializeObjByMap.Contains(StreamLevelName))//LoadGameData.SerializeObjByMap[StreamLevelName].Num() < 1)
	{
		UE_LOG(LogTemp, Warning, TEXT(" Cur StreamLevelName Serialize OBJ is Null!! "));
		return false;
	}
	//for (TMap< FString, FObjSerializeData>::TConstIterator Iterator(LoadGameData.SerializeObj); Iterator; ++Iterator)
	for (TMap< FString, FObjSerializeData>::TConstIterator Iterator(LoadGameData.SerializeObjByMap[StreamLevelName]); Iterator; ++Iterator)
	{
		FObjSerializeData CurSerializeData = (Iterator->Value);

		/* 20.3.26 修改为递归 */
		if (SerializeObjList.Contains(CurSerializeData.ID))
			continue;
		else
			CreateActorDeperOuter(WorldContextObject ,CurSerializeData , LoadGameData.SerializeObjByMap[StreamLevelName],//LoadGameData.SerializeObj,
				SerializeObjList, RefurrenceData , RefurrenceArrayData, RefurrenceMapData);
	}

	//所有需要序列化的OBJ生成完毕之后，进行引用指针的重定向,所有能够保存的OBJ，都继承了保存接口
	for (TMap<FString, UObject* >::TConstIterator Iterator(SerializeObjList); Iterator; ++Iterator)
	{
		ISaveableActorInterface* SavableActor = Cast<ISaveableActorInterface>(Iterator->Value);
		if (SavableActor)
		{
			SavableActor->RePointRefurrence(Iterator->Value,RefurrenceData[Iterator->Key], RefurrenceArrayData[Iterator->Key],
				RefurrenceMapData[Iterator->Key], SerializeObjList);
			SavableActor->RefreshAfterRePoint();
		}
	}

	UE_LOG(LogTemp ,Warning , TEXT("Load Object OVer~!"));

	return true;
}
bool USerializeSystemNew::LoadActorData(const UObject* WorldContextObject, FString GameID)
{
	FString DataLoadPath = SavePath + GameID;

	TArray<uint8> BinaryData;
	//先写死作为测试
	if (FFileHelper::LoadFileToArray(BinaryData, *DataLoadPath))
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
	TMap<FString, TArray<FRefurrenceMapData>> RefurrenceMapData;

	TArray<FString> ShowStreamLevelNames = LoadGameData.ShowStreamLevelNames;

	/*4.12 修改为需要判断StreamLevel的名字了，那么，就需要判断当前StreamLevelName是否为空！ */
	for (int32 StreamLevelNamesCount = 0; StreamLevelNamesCount < ShowStreamLevelNames.Num(); ++StreamLevelNamesCount)
	{
		if (LoadGameData.SerializeObjByMap[ShowStreamLevelNames[StreamLevelNamesCount]].Num() < 1)
		{
			UE_LOG(LogTemp, Warning, TEXT(" Cur ShowStreamLevelNames Serialize OBJ is Null!! "));
			continue;
		}
		//for (TMap< FString, FObjSerializeData>::TConstIterator Iterator(LoadGameData.SerializeObj); Iterator; ++Iterator)
		for (TMap< FString, FObjSerializeData>::TConstIterator Iterator(LoadGameData.SerializeObjByMap[ShowStreamLevelNames[StreamLevelNamesCount]]); Iterator; ++Iterator)
		{
			FObjSerializeData CurSerializeData = (Iterator->Value);

			/* 20.3.26 修改为递归 */
			if (SerializeObjList.Contains(CurSerializeData.ID))
				continue;
			else
				CreateActorDeperOuter(WorldContextObject, CurSerializeData, LoadGameData.SerializeObjByMap[ShowStreamLevelNames[StreamLevelNamesCount]],//LoadGameData.SerializeObj,
					SerializeObjList, RefurrenceData, RefurrenceArrayData, RefurrenceMapData);
		}

		//所有需要序列化的OBJ生成完毕之后，进行引用指针的重定向,所有能够保存的OBJ，都继承了保存接口
		for (TMap<FString, UObject* >::TConstIterator Iterator(SerializeObjList); Iterator; ++Iterator)
		{
			ISaveableActorInterface* SavableActor = Cast<ISaveableActorInterface>(Iterator->Value);
			if (SavableActor)
			{
				SavableActor->RePointRefurrence(Iterator->Value, RefurrenceData[Iterator->Key], RefurrenceArrayData[Iterator->Key],
					RefurrenceMapData[Iterator->Key], SerializeObjList);
				SavableActor->RefreshAfterRePoint();
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Load Object OVer~!"));

	return true;
}
#pragma optimize("",off)
UObject* USerializeSystemNew::CreateActorDeperOuter(const UObject* WorldContextObject, FObjSerializeData InSerializeData,
	TMap< FString, FObjSerializeData> AllSerializeObj,
	TMap<FString, UObject *> &SerializeObjList,
	TMap<FString, TArray<FRefurrenceData>> &RefurrenceData, TMap<FString, TArray<FRefurrenceArrayData>> &RefurrenceArrayData,
	TMap<FString, TArray<FRefurrenceMapData>> &RefurrenceMapData)
{
	FVector SpawnPos = InSerializeData.ActorTransForm.GetLocation();
	FRotator SpawnRot = InSerializeData.ActorTransForm.Rotator();
	FActorSpawnParameters SpawnParam;
	SpawnParam.Name = InSerializeData.Name;

	UClass* SpawnClass = FindObject<UClass>(ANY_PACKAGE, *InSerializeData.Class);
	FString OuterID = InSerializeData.OuterID;
	FString OuterType = InSerializeData.OuterType;
	FString StreamLevelName = InSerializeData.StreamLevelName;

	/* 4.10 根据当前的LevelName等 获取当前的Level */
	ULevel* CurObjLevel = nullptr;
	UWorld* CurWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	TArray<ULevelStreaming*> LevelStreamings = CurWorld->GetStreamingLevels();
	if (OuterType == OUTER_TYPE_LEVEL && LevelStreamings.Num() > 0)
	{
		for (int32 LevelCount = 0; LevelCount < LevelStreamings.Num(); LevelCount++)
		{
			//获取当前流关卡的加载状态，如果名字相等,看下对应的StreamLevel是否已经加载了
			FName PackageNameToLoad = LevelStreamings[LevelCount]->PackageNameToLoad;
			FString StreamLevelNameErr = (FPackageName::GetShortFName(PackageNameToLoad)).ToString();
			
			if (StreamLevelNameErr.Equals(StreamLevelName))
			{
				ULevelStreaming::ECurrentState CurStreamLevelState = LevelStreamings[LevelCount]->GetCurrentState();
				if (CurStreamLevelState == ULevelStreaming::ECurrentState::LoadedVisible)
				{
					CurObjLevel = LevelStreamings[LevelCount]->GetLoadedLevel();

					break;
				}
			}
			
		}
	}


	//是有可能找不到的！如果未加载的话
	if (SpawnClass == nullptr)
		SpawnClass = LoadObject<UClass>(NULL, *InSerializeData.Class);
	//如果是Actor，则生成到场景中，如果不是，则使用NewObject生成
	if (SpawnClass)
	{
		UObject *Ret = SpawnClass->GetDefaultObject();
		if (!Ret->IsA(AActor::StaticClass()))
		{
			if (SpawnClass && CurWorld)
			{
				UObject* NewObj;
				//4.09 test
				ULevel* TTTTLevel = CurWorld->GetCurrentLevel();

				UObject* CurOuter = nullptr;
				if (OuterType.Equals(OUTER_TYPE_WORLD))
					CurOuter = CurWorld;
				else if(OuterType.Equals(OUTER_TYPE_LEVEL))
				{
					/*TArray<ULevel*> Levels = CurWorld->GetLevels();
					for (ULevel* level : Levels)
					{
						if (OuterID.Equals(level->GetName()))
						{
							CurOuter = level;
							break;
						}
					}*/
					if (CurObjLevel)
						CurOuter = CurObjLevel;
				}
				else
				{
					/* 如果该Outer已经生成出来，直接取用即可 */
					if (SerializeObjList.Contains(OuterID))
						CurOuter = SerializeObjList[OuterID];
					/* 如果该Outer并未生成 */
					else
					{
						if (AllSerializeObj.Contains(OuterID))
							CurOuter = CreateActorDeperOuter(WorldContextObject, AllSerializeObj[OuterID], AllSerializeObj,
							SerializeObjList, RefurrenceData, RefurrenceArrayData, RefurrenceMapData);
					}
				}

				//4.09 如果Find到了，直接把找到的指针赋予！ 第三个参数表示精确的Class
				UObject* FindedObj = FindObject<UObject>(CurOuter, *(SpawnParam.Name.ToString()));
				if (FindedObj != nullptr)
					NewObj = FindedObj;
				else
					NewObj = NewObject<UObject>(CurOuter, SpawnClass);
				
				FMemoryReader MemoryReader(InSerializeData.SerializeData, true);
				FSaveOjbArchive Ar(MemoryReader, true);
				NewObj->Serialize(Ar);
				SerializeObjList.Add(InSerializeData.ID, NewObj);
				RefurrenceData.Add(InSerializeData.ID, InSerializeData.RefurrenceList);
				RefurrenceArrayData.Add(InSerializeData.ID, InSerializeData.ArrayRefurrenceList);
				RefurrenceMapData.Add(InSerializeData.ID, InSerializeData.MapRefurrenceList);

				return NewObj;
			}
		}
		//如果是Actor
		else
		{
			//UWorld* CurWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
			if (SpawnClass && CurWorld)
			{
				//如果是通过Spawn方法创建出来的，则需要设置位置等
				bool BCreateWithSpawn = false;
				AActor* NewActor = nullptr;
				//如果该Obj已经作为Outer被生成出来了，则直接取用其指针即可
				if (SerializeObjList.Contains(InSerializeData.ID))
					NewActor = Cast<AActor>(SerializeObjList[InSerializeData.ID]);
				/* @!! 重要说明，
					Actor的SpawnActor生成，是生成在当前world对应的Level上的！所以通过Outer生成Actor时，
					如果该Actor的Outer不是Level的话，那么就不应该使用SpawnActor而是应该使用普通的NewOjbect方法去生成他的Outer
				*/
				else
				{
					if (OuterType.Equals(OUTER_TYPE_LEVEL))
					{
						//ULevel* level = CurWorld->GetCurrentLevel();
						//if (OuterID.Equals(level->GetName()))
						if(CurObjLevel)
						{
							//4.09 如果Find到了，直接把找到的指针赋予！ 第三个参数表示精确的Class
							AActor* FindedObj = FindObject<AActor>(CurObjLevel, *(SpawnParam.Name.ToString()));
							if (FindedObj != nullptr)
								NewActor = FindedObj;
							else
								NewActor = CurObjLevel->OwningWorld->SpawnActor(SpawnClass, &SpawnPos, &SpawnRot, SpawnParam);

							
							BCreateWithSpawn = true;
						}	
					}
					/* 如果Outer不是Level的话，要对应生成在对应的Object上 */
					if(NewActor == nullptr)
					{
						if (AllSerializeObj.Contains(OuterID))
						{
							UObject* CurOuter = CreateActorDeperOuter(WorldContextObject, AllSerializeObj[OuterID], AllSerializeObj,
								SerializeObjList, RefurrenceData, RefurrenceArrayData, RefurrenceMapData);

							//4.09 如果Find到了，直接把找到的指针赋予！ 第三个参数表示精确的Class
							AActor* FindedObj = FindObject<AActor>(CurOuter, *(SpawnParam.Name.ToString()));
							if (FindedObj != nullptr)
								NewActor = FindedObj;
							else
								NewActor = NewObject<AActor>(CurOuter, SpawnClass);
							//NewActor = NewObject<AActor>(CurOuter, SpawnClass);
						}
					}
				}
					
				FMemoryReader MemoryReader(InSerializeData.SerializeData, true);
				FSaveOjbArchive Ar(MemoryReader, true);
				NewActor->Serialize(Ar);
				if(BCreateWithSpawn)
					NewActor->SetActorTransform(InSerializeData.ActorTransForm);

				SerializeObjList.Add(InSerializeData.ID, NewActor);
				RefurrenceData.Add(InSerializeData.ID, InSerializeData.RefurrenceList);
				RefurrenceArrayData.Add(InSerializeData.ID, InSerializeData.ArrayRefurrenceList);
				RefurrenceMapData.Add(InSerializeData.ID, InSerializeData.MapRefurrenceList);

				return Cast<UObject>(NewActor);
			}
		}
	}

	return nullptr;
}
#pragma optimize("",on)