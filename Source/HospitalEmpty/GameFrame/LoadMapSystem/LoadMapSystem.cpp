#include "LoadMapSystem.h"
#include "Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Base/HptGameInstance.h"

ULoadMapSystem* ULoadMapSystem::Get(const UObject* WorldContextObject)
{
	UWorld* MyWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (MyWorld)
	{
		UHptGameInstance* GameInstance = Cast<UHptGameInstance>(MyWorld->GetGameInstance());
		if (GameInstance)
		{
			ULoadMapSystem* Out = GameInstance->GetULoadMapSystem();
			return Out;
		}
	}
	return nullptr;
}

ULoadMapSystem::ULoadMapSystem(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	//直接为其先绑定一个回调，固定回调到该System的函数中.
	OnPackageLoaded.BindUFunction(this, "AsynLoadPackageCall");
}

void ULoadMapSystem::LoadLevel(const UObject* WorldContextObject , FName LevelName , bool BAsyn /* = true  */,
	UObject* DelegateOuterObj /* = nullptr */, FName OuterFunctionName /* = "" */)
{
	UWorld* CurWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (CurWorld == nullptr)
	{
		UE_LOG( LogTemp , Warning , TEXT("ULoadMapSystem LoadLevel Is Error!!!") );
		return;
	}

	if (OnPackageLoadedOuter.IsBound())
		OnPackageLoadedOuter.Unbind();
	if (DelegateOuterObj != nullptr && !OuterFunctionName.IsEqual(""))
		OnPackageLoadedOuter.BindUFunction(DelegateOuterObj, OuterFunctionName);

	if (BAsyn == false)
	{
		UGameplayStatics::OpenLevel(WorldContextObject, LevelName, false);
		OnPackageLoadedOuter.Execute("", WorldContextObject);
	}
	/* 异步加载地图对应的包，加载完成后调用OpenLevel */
	else
	{
		FTimerHandle* TimerHandle = nullptr;

		LoadWorldContextObject = WorldContextObject;

		LoadPackageAsync(LevelName.ToString(),
			FLoadPackageAsyncDelegate::CreateLambda([=](const FName& PackageName, UPackage* LoadedPackage,
			EAsyncLoadingResult::Type Result) {
				OnPackageLoaded.ExecuteIfBound(PackageName , LoadedPackage, int32(Result));
			}),
			0, PKG_ContainsMap);
		//EAsyncLoadingResult::Type
	}
}

void ULoadMapSystem::AsynLoadPackageCall(const FName& PackageName, UPackage* LoadedPackage, int32 InResult)
{
	if (InResult == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT(" 异步加载地图完毕~"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT(" 异步加载地图失败！！！ "));
		if (OnPackageLoadedOuter.IsBound())
			OnPackageLoadedOuter.Unbind();
		LoadWorldContextObject = nullptr;
		return;
	}
		

	if (OnPackageLoadedOuter.IsBound())
	{
		UGameplayStatics::OpenLevel(LoadWorldContextObject, PackageName, false);
		OnPackageLoadedOuter.Execute(PackageName, LoadWorldContextObject);

		LoadWorldContextObject = nullptr;
	}
		

}