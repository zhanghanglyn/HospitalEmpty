#pragma once

#include "CoreMinimal.h"
#include "ActorBase.h"
#include "DecorationBase.generated.h"

USTRUCT(BlueprintType)
struct FGridParam
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RowNum;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ColumnNum;


};

/*
	可放置物体的基类，具有属性：
				格子绿色占地模型（直接Add一个staticMesh上去即可）
				格子Mgr  ：存储当前家具的格子信息
				家具模型 ：关于模型具有什么属性如何与病人互动，提供接口给上层使用？还是； 互动点；也负责将特效、UI等加载在上面显示；（SceneComponentBase）
	该类也充当一个Mgr的作用
*/
UCLASS(BlueprintType, Blueprintable)
class RTSMODE_API ADecorationBase : public AActorBase
{
	GENERATED_BODY()

public:
	ADecorationBase(FString InActorName) : Super(InActorName) {};

	/** Constructor for AActor that takes an ObjectInitializer for backward compatibility */
	ADecorationBase(const FObjectInitializer& ObjectInitializer);

	/* virtual */
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override {};
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay();

	virtual void StartTouch(FVector TouchLocation) override {};
	virtual void TouchHold(FVector TouchLocation) override {};
	virtual void TouchEnd(FVector TouchLocation) override {};
	
	/* virtual end */

	/* 根据配置等更新格子数以及更新模型信息 */
	void UpdateGridInfo();

protected:


public:
	//模型路径 暂时这么标
	UPROPERTY(EditAnywhere, meta = (Category = "格子模型"))
	FString GroundMeshPath = "/Game/StarterContent/Shapes/Shape_Plane";

	/* 暂时把占地多少，模型大小等直接通过配置写出来 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite,meta = (Category = "格子数据相关"))
	FGridParam GridParamData;

protected:
	/* 占地格子模型 */
	UPROPERTY(VisibleAnywhere, meta = (Category = "占地格子相关"))
	class UStaticMeshComponent* GridMeshComponent;

	/* 格子Mgr */
	UPROPERTY()
	class UDecorationGridMgrComponent* GridMgr;

	/* 当前所被添加到的地面 */
	UPROPERTY(VisibleAnywhere)
	AActorBase* Ground;
};

