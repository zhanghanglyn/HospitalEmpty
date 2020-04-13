#pragma once

#include "CoreMinimal.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "SerializeSystemNew.generated.h"

USTRUCT()
struct FRefurrenceData
{
	GENERATED_USTRUCT_BODY()
public:
	//对应的序列化的Data ID
	FString SerializeDataID;
	//对应的Property的ID
	FName PropertyName;

	friend FArchive& operator<<(FArchive& Ar, FRefurrenceData& InData)
	{
		Ar << InData.SerializeDataID;
		Ar << InData.PropertyName;

		return Ar;
	}
};

/*
	用来存储Array引用对应关系的结构体
*/
USTRUCT()
struct FRefurrenceArrayData
{
	GENERATED_USTRUCT_BODY()
public:
	//对应的序列化的Data ID
	TArray<FString> SerializeListID;
	//对应的Property的ID
	FName PropertyName;

	friend FArchive& operator<<(FArchive& Ar, FRefurrenceArrayData& InData)
	{
		Ar << InData.SerializeListID;
		Ar << InData.PropertyName;

		return Ar;
	}
};

/*
	用来存储Map引用对应关系的结构体
*/
USTRUCT()
struct FRefurrenceMapData
{
	GENERATED_USTRUCT_BODY()
public:
	//对应的序列化的Data ID
	TArray<FString> SerializeListID;
	//对应的Property的ID
	FName PropertyName;

	friend FArchive& operator<<(FArchive& Ar, FRefurrenceMapData& InData)
	{
		Ar << InData.SerializeListID;
		Ar << InData.PropertyName;

		return Ar;
	}
};

/*
	20.3.20 尝试，重新定义。将每一个！指向外部的自定义的可以保存的Ojb，都作为一个结构体整体(FObjSerializeData)，而不是通过PropertyData的方式进行存储
	在序列化每一个Actor/Object的时候，会将指向外部的指针部分，保存其应该对应的FObjSerializeData的KeyID，同时将该Property的PropertyName保存，
	这样加载的时候，就会去外部取得反序列化生成的Obj并重新指定指针的引用,还更加清晰明了。
*/
USTRUCT()
struct FObjSerializeData
{
	GENERATED_USTRUCT_BODY()

public:
	/* 该序列化结构数据的ID */
	FString ID;

	FString Class;
	FName Name;
	FTransform ActorTransForm;
	TArray<uint8> SerializeData;
	/* Key 为一个FString的ID，该ID由Class_Name组成， Value 为该ID对应的自身Obj的PropertyName */
	TArray<FRefurrenceData> RefurrenceList;
	/* 该结构用来存储，是否有需要进行TArray的序列化 */
	TArray<FRefurrenceArrayData> ArrayRefurrenceList;
	/* 该结构用来存储，是否有需要进行TMap的序列化 */
	TArray< FRefurrenceMapData> MapRefurrenceList;
	/* 新添加字段，用来记录Outer的ID和TYPE */
	FString OuterID;
	FString OuterType;
	FString StreamLevelName;	//4.10 尝试用来记录StreamLevel的名字！从World中查找使用！

	friend FArchive& operator<<(FArchive& Ar, FObjSerializeData& InData)
	{
		Ar << InData.ID;
		Ar << InData.Class;
		Ar << InData.Name;
		Ar << InData.ActorTransForm;
		Ar << InData.SerializeData;
		Ar << InData.RefurrenceList;
		Ar << InData.ArrayRefurrenceList;
		Ar << InData.MapRefurrenceList;
		Ar << InData.OuterID;
		Ar << InData.OuterType;
		Ar << InData.StreamLevelName;

		return Ar;
	}
};

/*
	用来保存所有序列化数据的GameDataSaveStruct
	3.27扩展，将地图一起加入保存中，加载的时候，先加载地图再加载其余物品。
*/
USTRUCT()
struct FGameSerializeData
{
	GENERATED_USTRUCT_BODY()

public:
	FName GameID;
	FDateTime Timestamp;
	//以该Ojb存储生成的ID作为Key
	TMap< FString, FObjSerializeData> SerializeObj;  
	//4.10 存档的时候，根据StreamLevel的名字再分一下 , Key为StreamLevelName
	TMap< FString, TMap< FString, FObjSerializeData>> SerializeObjByMap;

	FString LevelName;	//保存当前存档对应的Level名称
	FName LevelPackageName;	//保存当前存档对应的Level的包

	//4.13 新添加当前显示保存的StreamLevelNames
	TArray<FString> ShowStreamLevelNames;

	friend FArchive& operator<<(FArchive& Ar, FGameSerializeData& InSerializeObj)
	{
		Ar << InSerializeObj.GameID;
		Ar << InSerializeObj.Timestamp;
		//Ar << InSerializeObj.SerializeObj;  //4.12 这都不用存了~
		Ar << InSerializeObj.SerializeObjByMap;
		Ar << InSerializeObj.LevelName;
		Ar << InSerializeObj.LevelPackageName;
		Ar << InSerializeObj.ShowStreamLevelNames;

		return Ar;
	}
};

/*
	用来进行Obj序列化的结构，基类是 : 实现将UObjects和FNames序列化为字符串数据的代理存档。
*/
struct FSaveOjbArchive : public FObjectAndNameAsStringProxyArchive
{
	FSaveOjbArchive(FArchive& InInnerArchive, bool bInLoadIfFindFails)
		: FObjectAndNameAsStringProxyArchive(InInnerArchive, bInLoadIfFindFails) {};

};

/*
	全局游戏的序列化存储系统直接就放在这
	20.3.19 之前的想法是，全部通过序列化来实现整个场景的保存和加载，实际体验起来，不是特别的现实，如果想要达到UE的序列化效果，需要单独进行很多
	很多很多额外的处理，感觉性价比就低了。因为改为使用序列化+数据生成+重新定位Pointer的方式来进行，即加载完毕Actor后，重新遍历并且对应一下指针。
*/
UCLASS()
class USerializeSystemNew : public UObject
{
	GENERATED_BODY()

public:
	USerializeSystemNew(const FObjectInitializer& ObjectInitializer);

	static USerializeSystemNew* Get(const UObject* WorldContextObject);

	/* 保存所有场景中可存储的Actor */
	UFUNCTION()
	bool SaveAllActorData(const UObject* WorldContextObject, FString GameID , FGameSerializeData &OutData);

	/*	从配置中加载
		Note：该函数还有一个很大的效率问题，因为是当所有的Object生成完了之后，遍历所有的Object去重定向指针，多出了一倍的循环消耗，
		之后可以使用更好的算法优化。
		3.27 注，之后可以再其中添加回调，已达到每一阶段不同处理的效果 
		4.12 新添加一个StreamLevelName ，只加载对应的StreamLevelName的Actor
		4.13 支持一个什么都不传的版本，会自动从数据中获取StreamLevelName一次性加载
	*/
	bool LoadActorData(const UObject* WorldContextObject, FString GameID , FString StreamLevelName);
	bool LoadActorData(const UObject* WorldContextObject, FString GameID);
protected:
	/* 
		存储FGameSerializeData至本地文件中 
		Param : GameID  会以GameID当做文件名进行存储
	*/
	bool SaveGameSerializeDataToFile(FGameSerializeData &InData , FString GameID);

	/*  
		将一个Actor/Object生成为一份序列化数据  
		
	*/
	void SaveObjToData(UObject* InObj, TMap< FString, FObjSerializeData> &OutData, const TArray<AActor*> InSaveActor);

	/*  获取一个Actor上的所有UPROPERTY指针，如果是继承自ISaveableActorInterface的可保存项，保存它
		并且把Object上的该PropertyName记录为需要外部加载
		TArray<FRefurrenceData> &RefData 对应的引用list , TArray<FRefurrenceArrayData> &RefArrayData   对应的引用ArrayList
	*/
	void CheckSavableProject(UObject* InObj, TMap< FString, FObjSerializeData> &OutData, TArray<FRefurrenceData> &RefData ,
		TArray<FRefurrenceArrayData> &RefArrayData , TArray<FRefurrenceMapData> &RefMapData,
		const TArray<AActor*> InSaveActor);

	/* 辅助函数，判断Object是否在TArray中 */
	bool CheckObjInArray(UObject* InActor, const TArray<AActor*> InSaveActor);

	/* 辅助函数，判断当前需要序列化的Object是否已经序列化过了 */
	bool CheckObjectBeSerialized(UObject* InObject);

	/*
		20.3.26 LoadActorData使用的函数，遍历所有的要生成的Actor并找到对应的Outer进行生成，如果该Outer还没有生成，
		则递归调用自身生成Outer

		Param: 
	*/
	UObject* CreateActorDeperOuter(const UObject* WorldContextObject , FObjSerializeData InSerializeData ,
		TMap< FString, FObjSerializeData> AllSerializeObj,
		TMap<FString, UObject* > &SerializeObjList,
		TMap<FString, TArray<FRefurrenceData>> &RefurrenceData , TMap<FString, TArray<FRefurrenceArrayData>> &RefurrenceArrayData,
		TMap<FString, TArray<FRefurrenceMapData>> &RefurrenceMapData);

public:
	static FString OUTER_TYPE_WORLD;// = "WORLD"
	static FString OUTER_TYPE_LEVEL;// = "LEVEL"

protected:
	/* 存档文件的保存路径 */
	UPROPERTY()
	FString SavePath;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "保存文件路径"))
	FString SaveFileName = "HospitalProject/SaveData/DataList/";

	/* 当前保存的所有的ObjID,每次SaveALL时会重置 */
	TArray<FString> CurrentFObjSerializeDataID;

	/* 当前已经创建的Outer Object列表，每次SaveAll的时候会重置 */
	//TMap<

};