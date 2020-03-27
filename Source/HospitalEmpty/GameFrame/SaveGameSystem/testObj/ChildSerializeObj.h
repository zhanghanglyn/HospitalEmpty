#pragma once

#include "CoreMinimal.h"
#include "GameFrame/SaveGameSystem/SaveableActorInterface.h"
#include "ChildSerializeObj.generated.h"
/*
	序列化测试OBJ
*/
UCLASS(BlueprintType, Blueprintable)
class AChildSerializeObj : public AActor , public ISaveableActorInterface
{
	GENERATED_BODY()
public:
	/** Constructor for AActor that takes an ObjectInitializer for backward compatibility */
	AChildSerializeObj(const FObjectInitializer& ObjectInitializer) {};

	friend FArchive& operator<<(FArchive& Ar, AChildSerializeObj& SaveActorRef);

	//测试序列化
	virtual void Serialize(FArchive& Ar) override;
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite , DisplayName = "子序列化测试" , meta = (Category = "序列化测试"))
	FString testTextChild = "哈哈哈";

};
