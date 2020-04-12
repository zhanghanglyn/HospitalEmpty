#pragma once

#include "CoreMinimal.h"
#include "LoadMapSystem.generated.h"

UCLASS(BlueprintType)
class ULoadMapSystem : public UObject
{
	GENERATED_BODY()

	//声明一个加载完成时的回调委托(内部使用)
	DECLARE_DELEGATE_ThreeParams(FOnPackageLoaded , const FName& , UPackage* , int32)
	FOnPackageLoaded OnPackageLoaded;

	//地图预加载完成后的回调委托（外部使用）,尝试使用一个自定义参数，Farchive！
	DECLARE_DELEGATE_TwoParams(FOnPackageLoadedOuter , FName ,const UObject*)
	FOnPackageLoadedOuter OnPackageLoadedOuter;

public:
	ULoadMapSystem(const FObjectInitializer& ObjectInitializer);

	static ULoadMapSystem* Get(const UObject* WorldContextObject);

	/* 外部调用加载Level，可以选择是异步加载还是同步加载 */
	void LoadLevel(const UObject* WorldContextObject , FName LevelName , bool BAsyn = true ,
	UObject* DelegateOuterObj = nullptr, FName OuterFunctionName = "");

	/* 4.12 原本的LoadLevel函数大概率要弃用，先添加一个LoadStreamLevel接口,用于地图块一块一块的加载 */
	void LoadStreamLevel(const UObject* WorldContextObject, FName LevelName, FName StreamLevelName);
protected:
	/* 异步加载完成后的回调 */
	UFUNCTION()
	void AsynLoadPackageCall(const FName& PackageName, UPackage* LoadedPackage, int32 InResult);

protected:
	/* 当加载地图时，在反射中使用的Object指针会丢失，应该是被回收了，所以保存记录下来使用 */
	UPROPERTY()
	const UObject* LoadWorldContextObject;
};