#pragma once

#include "CoreMinimal.h"
#include "SerializeSystemNew.h"
#include "Common/CommonUtil.h"
#include "SaveGameSystem.generated.h"

/*
	正式存档数据中的结构
*/
USTRUCT()
struct FSaveDataListStruct
{
	GENERATED_USTRUCT_BODY()
public:
	/*
		用来在加载列表中显示的游戏存档数据,
		Value为对应存档的文件名GameID
	*/
	FString GameID;
	/* 对应的存档的名字 */
	FString NameData;
	/* 用来保存在列表中显示游戏的存档的图片 */
	FString DataPic;
	/*一个存档对应的一个地图，放在外部把由外部加载并进行控制*/
	FString MapData;
	/* 地图包名 */
	FName MapPackageName;
	/*玩家身上的数据可能会存成不同的文件，因此，在此先添加一个对应关系*/
	FString PlayerConfig;
	/* 保存时间 */
	FString SaveTime;
	//4.13 新添加当前显示保存的StreamLevelNames
	TArray<FString> ShowStreamLevelNames;

	friend FArchive& operator<<(FArchive &Ar, FSaveDataListStruct &InData)
	{
		Ar << InData.GameID;
		Ar << InData.DataPic;
		Ar << InData.MapData;
		Ar << InData.NameData;
		Ar << InData.PlayerConfig;
		Ar << InData.SaveTime;
		Ar << InData.MapPackageName;
		Ar << InData.ShowStreamLevelNames;

		return Ar;
	}
};

/*
	对应真正游戏存档的Struct，该Stuct可用来展示存档，展示对应图片之类的,
	并且根据GameID取到对应的存档文件
*/
USTRUCT()
struct FGameSaveData
{
	GENERATED_USTRUCT_BODY()

public:
	/* Key为存档的GameID */
	TMap<FString , FSaveDataListStruct> StructData;

	friend FArchive& operator<<(FArchive &Ar, FGameSaveData &InData)
	{
		Ar << InData.StructData;

		return Ar;
	}
};

/*
	当前Load地图的参数
*/
USTRUCT()
struct FLoadGameParam
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	FString GameID = "";
};

/* 定义一个用来在加载StreamLevel时进行传递的参数 */
UCLASS(BlueprintType)
class ULoadStreamParam : public UObject
{
	GENERATED_BODY()

public:
	ULoadStreamParam(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer) {};

public:
	UPROPERTY()
	FString GameID;

	UPROPERTY()
	FString StreamLevelName;

	UPROPERTY()
	const UObject* WorldContextObject;
};


/*
	正式用于游戏存档的系统
	设计思路，多个存档分别存于多个文件，该系统自身会写一个文件，用来标记当前的所有文档列表
*/
UCLASS(BlueprintType)
class USaveGameSystem : public UObject
{

	GENERATED_BODY()

public:
	DECLARE_DELEGATE(FStreamLevelLoadDelegate)
	FStreamLevelLoadDelegate StreamLevelLoadDelegate;

	DECLARE_DELEGATE(FStreamLevelUnLoadDelegate)
	FStreamLevelUnLoadDelegate StreamLevelUnLoadDelegate;

public:
	static USaveGameSystem* Get(const UObject* WorldContextObject);

	USaveGameSystem(const FObjectInitializer& ObjectInitializer);

	/* 
		保存游戏当前进度 
		Param : GameID 是可以为空的，如果为空，说明是新存档，则重新设置一个GameID
	*/
	UFUNCTION(BlueprintCallable)
	bool SaveGame(const UObject* WorldContextObject, FString GameID);

	/* 根据存档ID读取游戏 */
	UFUNCTION(BlueprintCallable)
	bool LoadGame(const UObject* WorldContextObject , FString GameID , FString InStreamLevelName);

	/* 由Level的Start调用，在地图加载完毕之后，根据之前选择的存档数据，加载地图上的数据 */
	UFUNCTION(BlueprintCallable ,meta = (WorldContext = "WorldContextObject"))
	bool LoadAfterLoaded(const UObject* WorldContextObject);

	/* 4.16 新添加接口，给外部加载StreamLevel使用的接口，Load一个StreamLevel */
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	bool LoadStreamLevel(const UObject* WorldContextObject, TArray<FName> InStreamLevelNameS, UObject* CallOuter = nullptr, FName CallBackName = "", UObject* InParam = nullptr);
	/* 4.17 移除StreamLevel */
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	bool UnLoadStreamLevel(const UObject* WorldContextObject, TArray<FName> InStreamLevelNameS, UObject* CallOuter = nullptr, FName CallBackName = "", UObject* InParam = nullptr);


	/* 获取当前的数据配置表 */
	FGameSaveData GetGameSaveData()
	{
		return GameSaveData;
	};

protected:
	/* 刷新数据 */
	bool RefreshGameSaveData();

	/* 将当前Data保存到本地 */
	bool SaveDataToFile();

	UFUNCTION()
	void LoadDataAfterLoaded(FName LevelName, const UObject* WorldContextObject);

	/* 加载完成每一个StreamLevel之后的回调 */
	UFUNCTION()
	void LoadStreamLevelOverCall(UObject* InParam);

	/* 4.16 给外部StreamLevelVolume使用的加载回调 */
	UFUNCTION()
	void AllLoadStreamLevelOverCall(UObject* InParam);

	/* 4.16 给外部StreamLevelVolume使用的UnLoad回调 */
	UFUNCTION()
	void AllUnLoadStreamLevelOverCall(UObject* InParam);

protected:
	/* 存档数量 */
	UPROPERTY()
	int32 CurSaveDataNum = -1;

	/* 当前的存档数据 */
	UPROPERTY()
	FGameSaveData GameSaveData;

	/* 当前加载地图的数据 */
	UPROPERTY()
	FLoadGameParam LoadParam;

	/* 序列化系统 */
	UPROPERTY()
	USerializeSystemNew* SerializeSystem;

	/* 4.16 给外部StreamLevelVolume使用的本次所需要的加载的StreamLevelS */
	UPROPERTY()
	TArray<FName> CurStraemLevelLoadedNames;
	/* 4.16 给外部StreamLevelVolume使用的本次所需要的卸载的StreamLevelS */
	UPROPERTY()
	TArray<FName> CurStraemLevelUnLoadedNames;

	/* 游戏存档路径，该路径直接写死吧 */
	static FString GameIDName;

	static FString SaveGamePath;
};
