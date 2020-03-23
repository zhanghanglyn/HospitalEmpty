#pragma once

#include "CoreMinimal.h"
#include "GridSystemMgrBase.generated.h"


/* 地面格子中包含的数据 */
USTRUCT()
struct FGridData
{
	GENERATED_USTRUCT_BODY()
public:
	/* 用来标识当前结构是否并未被初始化，如果调用了SetGridId则会初始化 */
	UPROPERTY()
	bool IsNull = true;

	UPROPERTY()
	bool BeOccupy = false; //是否被占用

	UPROPERTY()
	int32 GridRow = 0;	//格子的横向Index

	UPROPERTY()
	int32 GridColumn = 0;	//格子的纵向Index

	UPROPERTY()
	FString StartGridId;	//如果该字段不为空，则说明是多格子组成的部件的其余部分,如果为空，则自身就是StartGrid

	//20.2.12 格子的左上角坐标
	UPROPERTY()
	FVector StartLocation;

		/* 必须在创建时调用！ */
	bool SetGridId();
	FString GetGridId() {
		return GridId;
	};

	friend FArchive& operator<<(FArchive& Ar, FGridData& InGridData)
	{
		Ar << InGridData.IsNull;
		Ar << InGridData.BeOccupy;
		Ar << InGridData.GridRow;
		Ar << InGridData.GridColumn;
		Ar << InGridData.StartGridId;
		Ar << InGridData.StartLocation;
		
		Ar << InGridData.GridCount;
		Ar << InGridData.GridName;

		Ar << InGridData.GridId;

		return Ar;
	}

protected:
	UPROPERTY()
	FString GridId = "";

public:
	static int32 GridCount;// = 0;
	static FString GridName;// = "Grid_";

};

//格子数据的二元数组
USTRUCT()
struct FNestedArray
{
	GENERATED_USTRUCT_BODY()

public:
	FNestedArray()
	{
	}

	//typedef FGridData ElementType;
	UPROPERTY()
	TArray<FGridData> Array;

	void Add(FGridData& item)
	{
		Array.Add(item);
	}

	void RemoveAt(int32 Index)
	{
		Array.RemoveAt(Index);

	}

	FGridData& operator[](int32 Index)
	{
		check(Array.Num() > Index);

		return Array[Index];
	}

	friend FArchive& operator<<(FArchive& Ar, FNestedArray& InNestedArray)
	{
		Ar << InNestedArray.Array;

		return Ar;
	}
};