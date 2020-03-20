/*
2020.3.13 序列化保存结构体等相关
*/

#pragma once

#include "CoreMinimal.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "SerializeSystem.generated.h"

/*
	用来存储序列化的继承了接口SaveableActorInterface的Component
*/
USTRUCT()
struct FPropertySerializeData
{
	GENERATED_USTRUCT_BODY()

public:
	FString Class;
	FName Name;
	TArray<uint8> PropertyData;

	/* 该Component附加的Actor的FName */
	FName OuterName;
	/* 该Component附加的Actor的Class,该两个用来进行指针的重置 */
	FString OuterClass;

	friend FArchive& operator<< (FArchive& Ar, FPropertySerializeData& InFPropertySerializeData)
	{
		Ar << InFPropertySerializeData.Class;
		Ar << InFPropertySerializeData.Name;
		Ar << InFPropertySerializeData.PropertyData;
		Ar << InFPropertySerializeData.OuterName;
		Ar << InFPropertySerializeData.OuterClass;

		return Ar;
	}
};


/* 
	用来保存序列化数据的结构体,用来存储Actor
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
	/* 该Actor身上，继承了接口可以保存的部分 */
	TArray<FPropertySerializeData> PropertyData;

	friend FArchive& operator<<(FArchive& Ar, FActorSerializeData& InActorSerializeData)
	{
		Ar << InActorSerializeData.ActorClass;
		Ar << InActorSerializeData.ActorName;
		Ar << InActorSerializeData.ActorTransForm;
		Ar << InActorSerializeData.ActorData;
		Ar << InActorSerializeData.PropertyData;

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
	20.3.19 之前的想法是，全部通过序列化来实现整个场景的保存和加载，实际体验起来，不是特别的现实，如果想要达到UE的序列化效果，需要单独进行很多
	很多很多额外的处理，感觉性价比就低了。因为改为使用序列化+数据生成+重新定位Pointer的方式来进行，即加载完毕Actor后，重新遍历并且对应一下指针。
*/
UCLASS()
class USerializeSystem : public UObject
{
	GENERATED_BODY()

public:
	USerializeSystem(const FObjectInitializer& ObjectInitializer);

	static USerializeSystem* Get(const UObject* WorldContextObject);

	/* 保存所有场景中可存储的Actor */
	UFUNCTION()
	bool SaveAllActorData(const UObject* WorldContextObject);

	/*	从配置中加载 
		生成Actor后，生成该Actor对应的Property部分，并且将其生成时使用NewActor作为Outer，因为保存时做过筛选，不会将场景中存在的Actor放入Property部分,
		所以不用使用world来Spawn
	*/
	bool LoadActorData(const UObject* WorldContextObject, FString LoadPath);

protected:
	/* 存储FGameActorSerializeData至本地文件中 */
	bool SaveGameSerializeDataToFile(FGameActorSerializeData &InData);

	/* 获取一个Actor上的所有UPROPERTY指针，如果是继承自ISaveableActorInterface的可保存项，保存它 */
	TArray<FPropertySerializeData> GetSavableObjAndSave(AActor* InActor ,const TArray<AActor*> InSaveActor);

	/* 辅助函数，判断Actor是否在TArray中 */
	bool CheckActorInArray(UObject* InActor, const TArray<AActor*> InSaveActor);

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