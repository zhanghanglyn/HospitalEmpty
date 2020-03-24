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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "测试用")
	class USerializeComponent* TestComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "测试用")
	class ATestSerializeObj* TestOOOOO;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "测试用")
	TMap<int32, AChildSerializeObj> MapTest;
};