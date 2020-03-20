#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Serialize/SaveableActorInterface.h"
#include "BaseSerializeObj.generated.h"

UCLASS()
class ABaseSerializeObj : public AActor, public ISaveableActorInterface
{
	GENERATED_BODY()
public:
	ABaseSerializeObj(const FObjectInitializer& ObjectInitializer);

	virtual void Serialize(FArchive& Ar) override;

	virtual void OnConstruction(const FTransform& Transform) override;

	/* 测试函数,为Obj添加一个TestObj */
	UFUNCTION(BlueprintCallable)
	void AddTestSerializeObj(  );

protected:
	UPROPERTY()
	class UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere , BlueprintReadWrite , Category = "测试用")
	TArray<int32> TestArrayOne;

	UPROPERTY(VisibleAnywhere , BlueprintReadOnly , Category = "测试用")
	class USerializeComponent* TestComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "测试用")
	class ATestSerializeObj* TestOOOOO;

	//先暂时这么写，之后把插件的东西移出来
	virtual void RePointRefurrence(TArray< FRefurrenceData> InRefurrenceData, TMap<FString, UObject *> InSerializeObjList) override;

};