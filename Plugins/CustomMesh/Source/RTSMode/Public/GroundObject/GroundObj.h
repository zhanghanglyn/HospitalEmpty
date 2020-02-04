#pragma once

#include "CoreMinimal.h"
#include "ActorBase.h"
#include "GroundObj.generated.h"

//定义一个材质参数集所需的结构
USTRUCT()
struct FGridGroundMaterialParam
{
	GENERATED_BODY()
public:
	UPROPERTY()
	int32 RowNum = 0;
	UPROPERTY()
	int32 Column = 0;
	UPROPERTY()
	float BorderWidth = 0.01f;
};

/*
	包含格子信息的地面
	暂时先默认一个底面对应一个GridMgr
*/
UCLASS(BlueprintType, Blueprintable)
class RTSMODE_API AGroundObj : public AActorBase
{
	GENERATED_BODY()
public:
	AGroundObj( FString InActorName );

	/** Constructor for AActor that takes an ObjectInitializer for backward compatibility */
	AGroundObj(const FObjectInitializer& ObjectInitializer);

	/* virtual */
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void OnConstruction(const FTransform& Transform) override;

	/* 开始被点击 */
	virtual void StartTouch(FVector TouchLocation) override;
	virtual void TouchHold(FVector TouchLocation) override;
	virtual void TouchEnd(FVector TouchLocation) override;
public:

	//设置右下角的起点位置（用来计算相对位置）,会根据当前尺寸等自动计算 
	FVector GetRelativeStart();

	//显示/隐藏格子
	void SetGridState() {};

protected:
	/* 为地面格子创建一个材质实例 */
	void CreateMaterialInstance();

	/* 更新格子参数 */
	void UpdateMaterialParam();

protected:
	UPROPERTY(VisibleAnywhere , meta = (Category = "地面模型"))
	class UStaticMeshComponent* GroundMeshComponent;
	//模型路径 暂时这么标
	UPROPERTY(EditAnywhere, meta = (Category = "地面模型"))
	FString GroundMeshPath = "/Game/StarterContent/Shapes/Shape_Plane";

	UPROPERTY(EditAnywhere, meta = (Category = "格子地面配置"))
	FVector2D GroundWidthHeight = FVector2D(100,100);

	UPROPERTY(EditAnywhere, meta = (Category = "格子地面配置"))
	float BorderWidth = 0.01f;

	UPROPERTY()
	class UGroundGridMgrComponent* GridMgr;

	/* 用来修改材质的参数 */
	UPROPERTY()
	FGridGroundMaterialParam MaterialParam;

	/* 动态创建的材质 */
	UPROPERTY()
	class UMaterialInstanceDynamic* GridDynamicMaterial;
};
