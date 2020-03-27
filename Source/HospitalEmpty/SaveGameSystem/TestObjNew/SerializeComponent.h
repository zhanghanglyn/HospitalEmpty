#pragma once

#include "CoreMinimal.h"
#include "SaveGameSystem/SaveableActorInterface.h"
#include "Components/ActorComponent.h"
#include "SerializeComponent.generated.h"

/* 测试用Struct */
USTRUCT(BlueprintType)
struct FTstruct
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	TArray<FString> tttt1;

	UPROPERTY()
	FString aaa = "初始化！";
};


UCLASS(BlueprintType)
class USerializeComponent : public UActorComponent, public ISaveableActorInterface
{
	GENERATED_BODY()
public:
	USerializeComponent(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere , BlueprintReadWrite, Category = "测试用")
	TArray<FTstruct> TestStructOne;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "测试用")
	FString aaaaa = "32123";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "测试用")
	FTstruct StructTwo;
};