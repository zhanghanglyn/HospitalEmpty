#pragma once

#include "CoreMinimal.h"
#include "LoadMapSystem.generated.h"

/* 4.14  test 创建一个专门用来回调的UObject试试！*/
UCLASS(BlueprintType)
class ULoadStreamCallOjb : public UObject
{
	GENERATED_BODY()

public:
	/* 4.13 LoadStreamLevel使用，加载完毕后的回调 */
	DECLARE_DELEGATE_OneParam(FOnStreamLevelLoaded, UObject*)
	FOnStreamLevelLoaded OnStreamLevelLoaded;

public:
	ULoadStreamCallOjb(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer) {};

	UFUNCTION(BlueprintCallable)
	void StreamLevelLoaded();

	UPROPERTY()
	int32 UUID = -1;

public:
	UPROPERTY()
	UObject* CallParam;
};

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

	/* 4.13 LoadStreamLevel使用，加载完毕后的回调 */
	DECLARE_DELEGATE_OneParam( FOnStreamLevelLoaded ,UObject*)
	FOnStreamLevelLoaded OnStreamLevelLoaded;

public:
	ULoadMapSystem(const FObjectInitializer& ObjectInitializer);

	static ULoadMapSystem* Get(const UObject* WorldContextObject);

	/*	外部调用加载Level，可以选择是异步加载还是同步加载
		4.13 修改为只直接加载地图，不需要后续的回调了，因为，加载了地图之后World变化，会造成很多问题
	*/
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	void LoadLevel(const UObject* WorldContextObject , FName LevelName , bool BAsyn = false ,
	UObject* DelegateOuterObj = nullptr, FName OuterFunctionName = "");

	/*	4.12 原本的LoadLevel函数大概率要弃用，先添加一个LoadStreamLevel接口,用于地图块一块一块的加载
		该接口，先加载StreamLevelName对应的StreamLevel，之后再加载对应的StreamLevel上的序列化数据
		Param : CallOuter外部需要回调的OBJ
				CallBackName 外部需要回调的functionName,需要一个参数UObject*
	*/
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	void LoadStreamLevel(const UObject* WorldContextObject, FName InStreamLevelName , UObject* CallOuter = nullptr, FName CallBackName = "", UObject* InParam = nullptr);

protected:
	/* 异步加载完成后的回调 */
	UFUNCTION()
	void AsynLoadPackageCall(const FName& PackageName, UPackage* LoadedPackage, int32 InResult);

	/* 4.13 加载StreamLevel完毕后的回调 */
	UFUNCTION(BlueprintCallable)
	void StreamLevelLoaded(UObject* InParam);
protected:
	/* 当加载地图时，在反射中使用的Object指针会丢失，应该是被回收了，所以保存记录下来使用 */
	UPROPERTY()
	const UObject* LoadWorldContextObject;

	/* StreamLevel加载时使用的UID */
	UPROPERTY()
	int32 CallBackUID = 1;

public:
	/* StreamLevel加载时使用的保存参数集 , key 为UID，val 为对应的参数 */
	UPROPERTY()
	TMap< int32, ULoadStreamCallOjb*> StreamLevelCallBackParamObj;

};