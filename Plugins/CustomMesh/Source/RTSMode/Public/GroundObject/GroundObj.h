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

	virtual void BeginPlay() override;
public:

	//设置0,0,0起点位置（用来计算相对位置）,会根据当前尺寸等自动计算 
	FVector GetRelativeStart();

	//显示/隐藏格子
	void SetGridState() {};

	class UGroundGridMgrComponent* GetGridMgr() {
		return GridMgr;
	};


	/* 获取地面格子右上角的坐标 */
	FVector GetTopRightLocation();
	/* 获取地面格子左下的坐标 */
	FVector GetButtomLeftLocation();

	/* 获取格子地面的宽高 */
	void GetGridWidthHeight(float &Width, float &Height);

	/* 放置当前家具，保存格子数据 */
	bool SaveCurDecoration( class ADecorationBase* SaveDecoration );

	/* 判断一个点是否在当前格子地面上,修改为会把移动点变成边缘 */
	bool CheckLocationInGround(FVector &InLocation);

	/* 判断一个左下角的点是否在格子地面上，如果不在，会重新设置左下角的坐标 */
	bool CheckLeftBUttomLocationInGround(FVector &LeftButtomLocation);

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

	UPROPERTY()
	class UGroundGridMgrComponent* GridMgr;

	/* 用来修改材质的参数 */
	UPROPERTY()
	FGridGroundMaterialParam MaterialParam;

	/* 动态创建的材质 */
	UPROPERTY()
	class UMaterialInstanceDynamic* GridDynamicMaterial;

};
