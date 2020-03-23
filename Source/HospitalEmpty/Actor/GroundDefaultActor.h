#pragma once

#include "CoreMinimal.h"
#include "GameBase/ObjectBase/ActorBase.h"
#include "GroundDefaultActor.generated.h"
/*
	包含格子信息的地面
	暂时先默认一个底面对应一个GridMgr
*/
UCLASS(BlueprintType, Blueprintable)
class AGroundDefaultActor : public AActorBase
{
	GENERATED_BODY()
public:
	AGroundDefaultActor( FString InActorName );

	/** Constructor for AActor that takes an ObjectInitializer for backward compatibility */
	AGroundDefaultActor(const FObjectInitializer& ObjectInitializer);

	/* virtual */
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void OnConstruction(const FTransform& Transform) override;

	/* 开始被点击 */
	virtual void StartTouch(FVector TouchLocation) override;
	virtual void TouchHold(FVector TouchLocation) override;
	virtual void TouchEnd(FVector TouchLocation) override;

	virtual void BeginPlay() override;
public:

protected:
	/* 为地面格子创建一个材质实例 */
	void CreateMaterialInstance();

	/* 更新格子参数 */
	void UpdateMaterialParam();

protected:
	//地面格子模型
	UPROPERTY(VisibleAnywhere , meta = (Category = "地面模型"))
	class UStaticMeshComponent* GroundMeshComponent;
	//模型路径 暂时这么标
	UPROPERTY(EditAnywhere, meta = (Category = "地面模型"))
	FString GroundMeshPath = "/Game/StarterContent/Shapes/Shape_Plane";

	UPROPERTY(EditAnywhere, meta = (Category = "格子地面配置"))
	FVector2D GroundWidthHeight = FVector2D(100,100);

	UPROPERTY(EditAnywhere, meta = (Category = "格子地面配置"))
	float BorderWidth = 0.01f;

	/* 占地格子材质 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "格子数据相关"))
	class UMaterial* GridMaterial;

	/* 动态创建的材质 */
	UPROPERTY()
	class UMaterialInstanceDynamic* GridDynamicMaterial;

};
