#include "SaveGameSystem.h"
#include "Base/HptGameInstance.h"
#include "Engine.h"
#include "Misc/Paths.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"
#include "Misc/FileHelper.h"
#include "SerializeSystemNew.h"
#include "Serialization/BufferArchive.h"
#include "GameFrame/LoadMapSystem/LoadMapSystem.h"

FString USaveGameSystem::SaveGamePath = "HospitalProject/SaveData/GameSaveCatalogue";
FString USaveGameSystem::GameIDName = "GameID_";

USaveGameSystem* USaveGameSystem::Get(const UObject* WorldContextObject)
{
	UWorld* MyWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (MyWorld)
	{
		UHptGameInstance* GameInstance = Cast<UHptGameInstance>(MyWorld->GetGameInstance());
		if (GameInstance)
		{
			USaveGameSystem* Out = GameInstance->GetSaveGameSystem();
			return Out;
		}
	}
	return nullptr;
}

USaveGameSystem::USaveGameSystem(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SaveGamePath = "HospitalProject/SaveData/GameSaveCatalogue";
	SaveGamePath = *FString::Printf(TEXT("%s%s"), *FPaths::ProjectContentDir(), *SaveGamePath);

	/* 在此取一次数据，初始化例如存档数量等参数 */
	RefreshGameSaveData();

}

bool USaveGameSystem::RefreshGameSaveData()
{
	TArray<uint8> BinaryData;
	//先写死作为测试
	if (!FFileHelper::LoadFileToArray(BinaryData, *SaveGamePath))
	{
		UE_LOG(LogTemp, Warning, TEXT(" USaveGameSystem Error!!! "));
		return false;
	}

	FMemoryReader FromBinary = FMemoryReader(BinaryData, true);
	FromBinary.Seek(0);

	FromBinary << GameSaveData;

	FromBinary.FlushCache();
	BinaryData.Empty();
	FromBinary.Close();


	/* 初始化一下当前存档的数量 */
	CurSaveDataNum = GameSaveData.StructData.Num();

	return true;
}

bool USaveGameSystem::SaveDataToFile()
{
	FBufferArchive BinaryData;
	BinaryData << GameSaveData;

	if (FFileHelper::SaveArrayToFile(BinaryData, *SaveGamePath))
	{
		UE_LOG(LogTemp, Warning, TEXT(" USaveGameSystem SaveToFile Over~~ "));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT(" USaveGameSystem SaveToFile Error!!! "));
		return false;
	}

	BinaryData.FlushCache();
	BinaryData.Empty();

	return true;
}


/************************************************************************/
/*                           保存游戏相关                               */
/************************************************************************/
bool USaveGameSystem::SaveGame(const UObject* WorldContextObject , FString GameID)
{
	if (SerializeSystem == nullptr)
		SerializeSystem = USerializeSystemNew::Get(WorldContextObject);

	FString SaveGameID = GameID;
	/* 如果存档ID为空，是新存档，则先根据当前存档的数量，设置要保存的存档ID */
	if (SaveGameID.IsEmpty() || SaveGameID.Equals(""))
	{
		CurSaveDataNum++;
		SaveGameID = GameIDName + "_" +  FString::FromInt(CurSaveDataNum);
	}

	//如果数据保存成功，则将该ID对应的数据写入
	FGameSerializeData CurSaveDataTemp;
	if (SerializeSystem->SaveAllActorData(WorldContextObject, SaveGameID , CurSaveDataTemp))
	{
		FSaveDataListStruct ListStruct;
		ListStruct.GameID = SaveGameID;
		ListStruct.MapData = CurSaveDataTemp.LevelName;
		ListStruct.NameData = L"随便乱写个名字";
		ListStruct.PlayerConfig = "PlayerData";
		ListStruct.DataPic = "Picture";
		ListStruct.SaveTime = CurSaveDataTemp.Timestamp.ToString();
		ListStruct.MapPackageName = CurSaveDataTemp.LevelPackageName;
		ListStruct.ShowStreamLevelNames = CurSaveDataTemp.ShowStreamLevelNames;

		GameSaveData.StructData.Add(SaveGameID , ListStruct);

		SaveDataToFile();
	}
	return false;
}

/************************************************************************/
/*                           读取游戏相关                               */
/************************************************************************/
//#pragma optimize("",off)
bool USaveGameSystem::LoadGame(const UObject* WorldContextObject ,FString GameID, FString InStreamLevelName)
{
	//先从本地中取出所有数据对应表
	RefreshGameSaveData();

	if (SerializeSystem == nullptr)
		SerializeSystem = USerializeSystemNew::Get(WorldContextObject);

	if (GameSaveData.StructData.Contains(GameID))
	{
		FSaveDataListStruct DataListStruct = GameSaveData.StructData[GameID];
		/* 先加载地图  先测试一下 */
		if (ULoadMapSystem* LoadMapSystem = ULoadMapSystem::Get(WorldContextObject))
		{
			//准备参数
			LoadParam.GameID = GameID;
			LoadMapSystem->LoadLevel(WorldContextObject, DataListStruct.MapPackageName);// , false, this, "LoadDataAfterLoaded");
		}
			
		//4.13 修改为使用Level打开加载
		/* 加载完毕地图后再加载Actor */
		//SerializeSystem->LoadActorData(WorldContextObject, GameID , InStreamLevelName);
	}

	return false;
}

void USaveGameSystem::LoadDataAfterLoaded(FName LevelName, const UObject* WorldContextObject)
{
	SerializeSystem->LoadActorData(WorldContextObject, LoadParam.GameID, "11");
}

bool USaveGameSystem::LoadAfterLoaded(const UObject* WorldContextObject)
{
	//此时不需要加载数据或是没有数据可加载
	if (LoadParam.GameID.IsEmpty() || LoadParam.GameID.Equals(""))
	{
		return false;
	}

	if (SerializeSystem == nullptr)
		SerializeSystem = USerializeSystemNew::Get(WorldContextObject);

	//在此，根据存储的数据判断下当前需要加载的StreamLevelName是哪几个。
	FSaveDataListStruct DataList = GameSaveData.StructData[LoadParam.GameID];
	UWorld* CurWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!CurWorld)
		return false;
	ULoadMapSystem* LoadMapSystem = ULoadMapSystem::Get(WorldContextObject);
	if (!LoadMapSystem)
		return false;


	//找到当前world中所有的StreamLevel，判断是否已经进行了加载
	TArray<ULevelStreaming*> LevelStreamings = CurWorld->GetStreamingLevels();
	for (int32 NameCount = 0; NameCount < LevelStreamings.Num(); ++NameCount)
	{
		FName PackageNameToLoad = LevelStreamings[NameCount]->PackageNameToLoad;
		FString StreamLevelNameErr = (FPackageName::GetShortFName(PackageNameToLoad)).ToString();
		//如果待显示的LevelName在该World中存在，则加载
		if (DataList.ShowStreamLevelNames.Contains(StreamLevelNameErr))
		{
			ULoadStreamParam* TempParam = NewObject<ULoadStreamParam>();
			TempParam->GameID = LoadParam.GameID;
			TempParam->StreamLevelName = StreamLevelNameErr;
			TempParam->WorldContextObject = WorldContextObject;
			LoadMapSystem->LoadStreamLevel(WorldContextObject, *StreamLevelNameErr, this, "LoadStreamLevelOverCall" , TempParam);
		}
	}

	//LoadParam.GameID = "";
	//return SerializeSystem->LoadActorData(WorldContextObject, GameID);

	return true;
}
void USaveGameSystem::LoadStreamLevelOverCall(UObject* InParam)
{
	if (ULoadStreamParam* TempParam = Cast<ULoadStreamParam>(InParam))
	{
		SerializeSystem->LoadActorData(TempParam->WorldContextObject, TempParam->GameID, TempParam->StreamLevelName);
	}
}
//#pragma optimize("",on)

/*********************************             给外部使用的全部加载完毕后给出回调的加载                          ***************************/
bool USaveGameSystem::LoadStreamLevel(const UObject* WorldContextObject, TArray<FName> InStreamLevelNameS,
	UObject* CallOuter /* = nullptr */, FName CallBackName /* = "" */, UObject* InParam /* = nullptr */)
{
	if (LoadParam.GameID.IsEmpty() || LoadParam.GameID.Equals(""))
	{
		return false;
	}

	//在此，根据存储的数据判断下当前需要加载的StreamLevelName是哪几个。
	FSaveDataListStruct DataList = GameSaveData.StructData[LoadParam.GameID];
	UWorld* CurWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!CurWorld)
		return false;
	ULoadMapSystem* LoadMapSystem = ULoadMapSystem::Get(WorldContextObject);
	if (!LoadMapSystem)
		return false;

	/* 清空本次需要加载的StreamLevel */
	CurStraemLevelLoadedNames.Empty();

	StreamLevelLoadDelegate.Unbind();
	StreamLevelLoadDelegate.BindUFunction(CallOuter, CallBackName, InParam);

	//找到当前world中所有的StreamLevel，判断是否已经进行了加载
	TArray<ULevelStreaming*> LevelStreamings = CurWorld->GetStreamingLevels();
	for (int32 NameCount = 0; NameCount < LevelStreamings.Num(); ++NameCount)
	{
		//如果当前StreamLevel未被加载，才被加载
		if (LevelStreamings[NameCount]->GetCurrentState() == ULevelStreaming::ECurrentState::Unloaded)
		{
			FName PackageNameToLoad = LevelStreamings[NameCount]->PackageNameToLoad;
			FString StreamLevelNameErr = (FPackageName::GetShortFName(PackageNameToLoad)).ToString();
			//如果待显示的LevelName在该World中存在，则加载
			if (InStreamLevelNameS.Contains(*StreamLevelNameErr))
			{
				ULoadStreamParam* TempParam = NewObject<ULoadStreamParam>();
				TempParam->GameID = LoadParam.GameID;
				TempParam->StreamLevelName = StreamLevelNameErr;
				TempParam->WorldContextObject = WorldContextObject;
				LoadMapSystem->LoadStreamLevel(WorldContextObject, *StreamLevelNameErr, this, "AllLoadStreamLevelOverCall", TempParam);

				CurStraemLevelLoadedNames.Add(*StreamLevelNameErr);
			}
		}
	}

	return true;
}

void USaveGameSystem::AllLoadStreamLevelOverCall(UObject* InParam)
{
	if (ULoadStreamParam* TempParam = Cast<ULoadStreamParam>(InParam))
	{
		SerializeSystem->LoadActorData(TempParam->WorldContextObject, TempParam->GameID, TempParam->StreamLevelName);

		//加载完一个，清除一个
		if (CurStraemLevelLoadedNames.Contains(*TempParam->StreamLevelName))
			CurStraemLevelLoadedNames.Remove(*TempParam->StreamLevelName);

		if (CurStraemLevelLoadedNames.Num() < 1)
		{
			StreamLevelLoadDelegate.ExecuteIfBound();
			StreamLevelLoadDelegate.Unbind();
		}
	}
}

/********************  卸载  ********************/
bool USaveGameSystem::UnLoadStreamLevel(const UObject* WorldContextObject, TArray<FName> InStreamLevelNameS, UObject* CallOuter,
	FName CallBackName /* = "" */, UObject* InParam /* = nullptr */)
{
	if (LoadParam.GameID.IsEmpty() || LoadParam.GameID.Equals(""))
	{
		return false;
	}

	//在此，根据存储的数据判断下当前需要加载的StreamLevelName是哪几个。
	FSaveDataListStruct DataList = GameSaveData.StructData[LoadParam.GameID];
	UWorld* CurWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!CurWorld)
		return false;
	ULoadMapSystem* LoadMapSystem = ULoadMapSystem::Get(WorldContextObject);
	if (!LoadMapSystem)
		return false;

	/* 清空本次需要加载的StreamLevel */
	CurStraemLevelUnLoadedNames.Empty();

	StreamLevelUnLoadDelegate.Unbind();
	StreamLevelUnLoadDelegate.BindUFunction(CallOuter, CallBackName, InParam);

	//找到当前world中所有的StreamLevel，判断是否已经进行了加载
	TArray<ULevelStreaming*> LevelStreamings = CurWorld->GetStreamingLevels();
	for (int32 NameCount = 0; NameCount < LevelStreamings.Num(); ++NameCount)
	{
		//如果当前StreamLevel已被加载，才卸载
		if (LevelStreamings[NameCount]->GetCurrentState() == ULevelStreaming::ECurrentState::LoadedVisible)
		{
			FName PackageNameToLoad = LevelStreamings[NameCount]->PackageNameToLoad;
			FString StreamLevelNameErr = (FPackageName::GetShortFName(PackageNameToLoad)).ToString();
			//如果待显示的LevelName在该World中存在，则加载
			if (InStreamLevelNameS.Contains(*StreamLevelNameErr))
			{
				ULoadStreamParam* TempParam = NewObject<ULoadStreamParam>();
				TempParam->GameID = LoadParam.GameID;
				TempParam->StreamLevelName = StreamLevelNameErr;
				TempParam->WorldContextObject = WorldContextObject;

				/* 4.17 应该要在此判断，当前Level上有多少个Actor，将其移除！因为创建的时候，是创建在整个世界中的 */
				ULevelStreaming* CurStraemLevel = LevelStreamings[NameCount];
				LoadMapSystem->RemoveAllSaveableActor(CurStraemLevel->GetLoadedLevel(), StreamLevelNameErr);

				LoadMapSystem->UnLoadStreamLevel(WorldContextObject, *StreamLevelNameErr, this, "AllUnLoadStreamLevelOverCall", TempParam);

				CurStraemLevelUnLoadedNames.Add(*StreamLevelNameErr);
			}
		}
	}

	return true;
}

void USaveGameSystem::AllUnLoadStreamLevelOverCall(UObject* InParam)
{
	if (ULoadStreamParam* TempParam = Cast<ULoadStreamParam>(InParam))
	{
		//加载完一个，清除一个
		if (CurStraemLevelUnLoadedNames.Contains(*TempParam->StreamLevelName))
			CurStraemLevelUnLoadedNames.Remove(*TempParam->StreamLevelName);

		if (CurStraemLevelUnLoadedNames.Num() < 1)
		{
			StreamLevelUnLoadDelegate.ExecuteIfBound();
			StreamLevelUnLoadDelegate.Unbind();
		}
	}
}