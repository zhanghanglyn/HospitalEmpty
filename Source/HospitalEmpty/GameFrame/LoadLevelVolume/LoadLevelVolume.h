#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Volume.h"
#include "LoadLevelVolume.generated.h"

USTRUCT(BlueprintType)
struct FLLVolumeBoundLocation
{
	GENERATED_USTRUCT_BODY()

public:
	float LeftX = 0;
	float RightX = 0;
	float TopY = 0;
	float BottomY = 0;

	FVector2D Center = FVector2D::ZeroVector;
};

UENUM(BlueprintType)
enum class EOverlepLocationType : uint8
{

	LEFT,
	RIGHT ,
	TOP,
	BOTTOM,

};


/* 
	一个用来进行StreamLevel加载的Volume
	四个边可以分别设置当玩家碰撞到之后会去进行加载的StreamLevel
*/
UCLASS(hidecategories = (Advanced, Attachment, Collision, Volume))
class ALoadLevelVolume : public AVolume
{
	GENERATED_UCLASS_BODY()

public:

	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	/*
		开始触碰的回调，需要判断下，是碰到了整个Volumn的哪一面，进而判断其需要加载的地图数据
	*/
	UFUNCTION()
	void OnTriggerOverlapBegin(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnTriggerOverlapEnd(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/*  初始化周边的四个面的坐标，用来当Overlap发生时，判断是碰到了哪一个面
		先尝试使用简单粗暴一点的方式来进行，直接判断与X Y 哪一个最接近，进而判断是与哪一个面发生了碰撞
	*/ 
	void InitAroundLocation();

	/* 判断一个传入点是位于该Volume的哪一边 */
	void SetCurOverLapLocationType( FVector InLocation );

	/* 根据OverLapLocationType取对应的加载地图数据 */
	TArray<FName> GetStreamLevelNames(EOverlepLocationType InOverlepLocationType);

	/* 所有StreamLevel加载完的回调 */
	UFUNCTION()
	void StreamLevelLoadedCallBack(UObject* InParam );

protected:
	/* 进入触发器以及离开触发器的回调 */
	FScriptDelegate OverlapBeginDele;
	FScriptDelegate OverlapEndDele;

public:
	UPROPERTY(EditAnywhere , BlueprintReadWrite , Category = "地图加载相关" , meta = (DisplayName = "左边加载的地图"))
	TArray<FName> LeftStringLevelNames;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "地图加载相关", meta = (DisplayName = "右边加载的地图"))
	TArray<FName> RightStringLevelNames;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "地图加载相关", meta = (DisplayName = "上加载的地图"))
	TArray<FName> TopStringLevelNames;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "地图加载相关", meta = (DisplayName = "下边加载的地图"))
	TArray<FName> BottomStringLevelNames;

protected:
	UPROPERTY()
	FLLVolumeBoundLocation LLBoundLocation;

	UPROPERTY()
	EOverlepLocationType CurOverlapLocationType;

};