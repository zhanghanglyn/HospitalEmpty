#pragma once

#include "CoreMinimal.h"
#include "ActorBase.h"
#include "GridSystemMgrBase.h"
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
	直接通过蓝图编辑的家具Config，就不需要去读取数据，
	子类继承该结构并且填充自己的参数
*/
USTRUCT(BlueprintType)
struct FDecorationConfig
{
	GENERATED_BODY()

public:
	/* 该家具显示的模型 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMesh* DecorationMesh;
	
	/* 名字 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DecorationName;

	/*格子数据*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite,meta = (Category = "格子数据相关"))
	FGridParam GridParamData;

	/* 格子材质 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "格子数据相关"))
	class UMaterial* GridMaterial;
};

/*
 状态机部分，家具应该由状态机来运行当前状态
 暂时分为状态：
	放置完毕（正常状态）
	拖动中
	不可使用（暂定）
*/

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
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay();

	virtual void StartTouch(FVector TouchLocation) override {};
	virtual void TouchHold(FVector TouchLocation) override {};
	virtual void TouchEnd(FVector TouchLocation) override {};
	
	virtual void Hover(FVector TouchLocation) override {};
	/* virtual end */

	/* 拖动物体时的移动,根据传入的坐标，会自动对齐到最近的格子 */
	void MoveTo( FVector InLocation );

	/* 设置对应的地面以及对应的地面格子Mgr */
	void SetGround(AActorBase* InGround);

	/* 根据右上角的坐标，更新整个家具的坐标 */
	void SetLocationByTopRight(FVector TopRightLocation);

	/* 获取地面格子右上角的坐标 */
	FVector GetTopRightLocation();

	/* 根据传入点作为当前家具中心取右上角坐标 */
	FVector GetTopRightLocation(FVector InLocation);

	/* 根据地面更新自身格子以及位置信息初始化 */
	//void UpdateGridByGround();

	/* 获取格子地面的宽高 */
	void GetGridWidthHeight(float &Width, float &Height);

	/* 设置阻挡状态，是否变红，会传入具体的格子，之后用来进行单独格子的变色 */
	void SetBlockState(FGridData BlockGridParam ,bool BeOccupy);

	/* 获取地面Grid数据 */
	TArray< FNestedArray > GetGridList();

protected:
	/* 根据配置等更新格子数以及更新模型信息 */
	void UpdateGridInfo( class UStaticMesh* InMesh );


public:
	//模型路径 暂时这么标
	UPROPERTY(EditAnywhere, meta = (Category = "格子模型"))
	FString GroundMeshPath = "/Game/StarterContent/Shapes/Shape_Plane";

	/* 暂时把占地多少，模型大小等直接通过配置写出来 */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite,meta = (Category = "格子数据相关"))
	//FGridParam GridParamData;

	/* 该家具的配置 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "格子数据相关"))
	FDecorationConfig ParamData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "格子数据相关"))
	float BorderWidth = 0.05f;

protected:
	/* 占地格子模型 */
	UPROPERTY(VisibleAnywhere, meta = (Category = "占地格子相关"))
	class UStaticMeshComponent* GridMeshComponent;

	/* 占地格子材质 */
	UPROPERTY(VisibleAnywhere, meta = (Category = "占地格子相关"))
	class UMaterialInstanceDynamic* GridMeshMaterial;

	/* 家具模型 */
	UPROPERTY(VisibleAnywhere, meta = (Category = "占地格子相关"))
	class UStaticMeshComponent* DecorationMeshComponent;

	/* 根Component */
	UPROPERTY()
	class UDtSceneComponentBase* RootSceneComponent;

/* 数据等处理方法相关 */
protected:
	/* 格子Mgr */
	UPROPERTY()
	class UDecorationGridMgrComponent* GridMgr;

	/* 当前所被添加到的地面 */
	UPROPERTY(VisibleAnywhere)
	class AGroundObj* Ground;

};

