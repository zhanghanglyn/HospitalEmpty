#pragma once

#include "CoreMinimal.h"
#include "SerializeSystemNew.h"
#include "SaveGameSystem.generated.h"

/*
	对应真正游戏存档的Struct，该Stuct可用来展示存档，展示对应图片之类的,
	并且根据GameID取到对应的存档文件
*/
USTRUCT()
struct FGameSaveData
{
	GENERATED_USTRUCT_BODY()

public:
	/* 
		用来在加载列表中显示的游戏存档数据,
		Key为FGameSerializeData 的GameID ，
		Value为对应存档的文件
	*/
	TMap<FString, FString> SaveDataList;
	/*
		用来保存在列表中显示游戏的存档的图片
		Value为对应的图片路径
	*/
	TMap<FString, FString> DataPicList;
	/*
		玩家身上的数据可能会存成不同的文件，因此，在此先添加一个对应关系
		Value为对应的玩家数据存档路径
	*/
	TMap<FString, FString> PlayerDataList;

	friend FArchive& operator<<(FArchive &Ar, FGameSaveData &InData)
	{
		Ar << InData.SaveDataList;
		Ar << InData.DataPicList;
		Ar << InData.PlayerDataList;

		return Ar;
	}
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
	USaveGameSystem(const FObjectInitializer& ObjectInitializer);

};
