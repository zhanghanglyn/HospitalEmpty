﻿#pragma once

#include "CoreMinimal.h"
#include "TestSerializeObj.generated.h"
/*
	序列化测试OBJ
*/
UCLASS(BlueprintType, Blueprintable)
class ATestSerializeObj : public AActor
{
	GENERATED_BODY()
public:
	/** Constructor for AActor that takes an ObjectInitializer for backward compatibility */
	ATestSerializeObj(const FObjectInitializer& ObjectInitializer) {};

	//测试序列化
	virtual void Serialize(FArchive& Ar) override;

	//保存该Obj到文件
	UFUNCTION(BlueprintCallable)
	bool SaveObjectToFile(FString FilePath);
	//从文件中读取该Object
	UFUNCTION(BlueprintCallable)
	bool LoadObjectFromFile(FString FilePath);

protected:
	//尝试存取Obj相关,Ar操作函数
	void SaveLoadData(FArchive& Ar);

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite , DisplayName = "序列化测试" ,meta = (Category = "序列化测试"))
	FString testText = "empty";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "子Object", meta = (Category = "序列化测试"))
	class AChildSerializeObj* ChildObj;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//FString FilePath;
};
