/*
2020.3.13 序列化保存结构体等相关
*/

#pragma once

#include "CoreMinimal.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "SerializeSystem.generated.h"

/* 
	用来保存序列化数据的结构体
*/
USTRUCT()
struct FActorSerializeData
{
	GENERATED_USTRUCT_BODY()

public:
	FString ActorClass;
	FName ActorName;
	FTransform ActorTransForm;
	TArray<uint8> ActorData;

	friend FArchive& operator<<(FArchive& Ar, FActorSerializeData& InActorSerializeData)
	{
		Ar << InActorSerializeData.ActorClass;
		Ar << InActorSerializeData.ActorName;
		Ar << InActorSerializeData.ActorTransForm;
		Ar << InActorSerializeData.ActorData;

		return Ar;
	}
};

/* 
	用来保存所有Actor数据的GameDataSaveStruct
*/
USTRUCT()
struct FGameActorSerializeData
{
	GENERATED_USTRUCT_BODY()

public:
	FName GameID;
	FDateTime Timestamp;
	TArray< FActorSerializeData> SerializeActors;

	friend FArchive& operator<<(FArchive& Ar, FGameActorSerializeData& InSerializeData)
	{
		Ar << InSerializeData.GameID;
		Ar << InSerializeData.Timestamp;
		Ar << InSerializeData.SerializeActors;

		return Ar;
	}
};

/*
	用来进行Actor序列化的结构，基类是 : 实现将UObjects和FNames序列化为字符串数据的代理存档。
*/
struct FSaveActorArchive : public FObjectAndNameAsStringProxyArchive
{
	FSaveActorArchive(FArchive& InInnerArchive, bool bInLoadIfFindFails)
		: FObjectAndNameAsStringProxyArchive(InInnerArchive, bInLoadIfFindFails) {};

};

/*
	全局游戏的序列化存储系统直接就放在这
*/
UCLASS()
class USerializeSystem : public UObject
{
	GENERATED_BODY()

public:
	USerializeSystem(const FObjectInitializer& ObjectInitializer);

	static USerializeSystem* Get(const UObject* WorldContextObject);

	/* 保存某个单独Actor */
	UFUNCTION()
	bool SaveActorData( class AActor* InActor );

	/* 保存所有场景中可存储的Actor */
	UFUNCTION()
	bool SaveAllActorData() { return true; };

	/* 从配置中加载 */
	bool LoadActorData(const UObject* WorldContextObject, FString LoadPath);
protected:
	/* 存储FGameActorSerializeData至本地文件中 */
	bool SaveGameSerializeDataToFile(FGameActorSerializeData &InData);

protected:
	/* 存档文件的保存路径 */
	UPROPERTY()
	FString SavePath;

	UPROPERTY( BlueprintReadWrite , EditAnywhere , meta = (DisplayName = "保存文件路径") )
	FString SaveFileName = "HospitalProject/SaveData/Test1";

	/* 保存当前游戏中所有Actor的结构体，暂时只存放一个场景，后续对应多个场景进行添加 */
	//UPROPERTY()
	//FGameActorSerializeData GameSaveSerializeData;

};