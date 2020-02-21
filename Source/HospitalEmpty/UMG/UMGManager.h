// Fill out your copyright notice in the Description page of Project Settings.
//@ 张航 备注
/*
	其实也可以不引用FullScreenWidgetBase直接使用UUserwidget这样耦合性低一些，但这里我想让他们配套使用,
	因为个人觉得，是一定要继承的
*/

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UMG/UserWidgetBase.h"
#include "UMG/FullScreenRoot.h"
#include "Classes/Kismet/GameplayStatics.h"
#include "Engine/Classes/Engine/Engine.h"
#include "UMG/Public/Components/CanvasPanelSlot.h"
#include "UMGManager.generated.h"

/*
20.02.05 层级管理类型 按照层级显示顺序排序。层级越大显示越前。
	存储UI时，以层级作为Key，方便后续查找
	玩家传入的Zorder，将与层级进行相加，来设置Zorder,层级的值会乘上100
*/
UENUM(BlueprintType)
enum class EUMGLayer : uint8
{
	None = 0				UMETA(DisplayName = "如果不传入，默认为在这个层级上"),
	Item = 1				UMETA(DisplayName = "物品UMG"),	//物品上的UMG，虽然一般会直接加在物品上，但是万一呢
	Info = 2				UMETA(DisplayName = "玩家信息层，例如血量等"),	
	Tips = 3				UMETA(DisplayName = "提示、旁白等层"),	//提示、独白OS、旁白等需要显示的层级
	System = 4				UMETA(DisplayName = "系统UI层"),		//系统的UI层
};

//用来在Map中存放多个Widget
USTRUCT()
struct FUMGLayerWidget
{
	GENERATED_BODY()
		FUMGLayerWidget()
	{
		LayerWidgets.Reset();
	}

public:
	UPROPERTY()
		TArray<UUserWidgetBase*> LayerWidgets;

};


/**
 * UMG管理类 单例 存放于GameInstance中
 */
UCLASS(BlueprintType , Blueprintable)
class UUMGManager : public UObject
{
	GENERATED_BODY()

private:
	//static UUMGManager* Instace;

//实现父类方法相关等
public:
	UUMGManager(const FObjectInitializer& ObjectInitializer);
	
	static UUMGManager* Get(const UObject* WorldContextObject);
/************************************************************************/
/*						直接创建至屏幕相关                             */
/************************************************************************/
public:
	//向屏幕添加一个UMG
	UFUNCTION(BlueprintCallable , BlueprintCosmetic, meta = (WorldContext = "WorldContextObject"))
	UUserWidgetBase* CreateScreenWidget(const UObject* WorldContextObject,
		FString _widgetBlueprintPath, /* TSubclassOf<UUserWidgetBase> _widgetType,FString _widgetName,*/ 
		EUMGLayer Layer = EUMGLayer::None, int32 _zorder = 0);
	//向屏幕添加一个UMG
	
	template<typename T>
	T* CreateScreenWidget(FString _widgetBlueprintPath,UWorld* _world, /* TSubclassOf<UUserWidgetBase> _widgetType, FString _widgetName,*/
		EUMGLayer Layer = EUMGLayer::None, int32 _zorder = 0)
	{
		/*if (m_ScreenWidget.Num() > 0 && m_ScreenWidget.Find(_widgetName) != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("this name is aleady in the map!!!!"));
			return nullptr;
		}*/

		UClass* Temp_Widget = LoadClass<T>(NULL, _widgetBlueprintPath.GetCharArray().GetData());
		if (Temp_Widget != nullptr)
		{
			T* NewWidget = CreateWidget<T>(_world, Temp_Widget);
			if (NewWidget != nullptr)
			{
				(Cast<UUserWidgetBase>(NewWidget))->AddToViewport((int8)Layer * 100 +_zorder);

				if (m_ScreenWidget.Contains(Layer))
					m_ScreenWidget[Layer].LayerWidgets.Add(NewWidget);
				else
				{
					FUMGLayerWidget LayerWidgets;
					LayerWidgets.LayerWidgets.Add(NewWidget);
					m_ScreenWidget.Add(Layer, LayerWidgets);
				}
				return NewWidget;
			}


			//m_ScreenWidget.Add(_widgetName, NewWidget);

			return nullptr;
		}
		return nullptr;
	}

	//获取所有对应层的UMG
	TArray<UUserWidgetBase*> GetScreenWidget(EUMGLayer Layer);//FString _widgetName);
	/* 根据UID查找UMG */
	UUserWidgetBase* GetScreenWidget(FString UID);
	/*获取对应UI的层级*/
	EUMGLayer GetScreenWidgetLayer(FString UID);
	EUMGLayer GetScreenWidgetLayer(UUserWidgetBase* InWidget);
	//获取UMG
	/*template<typename T1>
	T1* GetScreenWidget(FString _widgetName)
	{
		if (m_ScreenWidget.Num() > 0 && m_ScreenWidget.Find(_widgetName) != nullptr)
		{
			//m_WidgetTypeRelate[1]::Class aa = Cast< m_WidgetTypeRelate[1]>(*m_ScreenWidget.Find(_widgetName));
			return Cast<T1>(*m_ScreenWidget.Find(_widgetName));
		}
		return nullptr;
	}*/

	//清除Widget
	void ClearWidget(FString UID);
	void ClearWidget(UUserWidgetBase* InWidget);
	/* 清除对应层的UI */
	void ClearWidget(EUMGLayer Layer);
	void ClearAll();

	//设置点击GameMode或是PlayerMode事件
	void SetInputMode(UWorld* _world, FInputModeDataBase& InData);

protected:
	//根据名字记录Widget
	UPROPERTY()
	TMap<EUMGLayer, FUMGLayerWidget> m_ScreenWidget;
	//记录每个名字对应的类型，用来转换  //暂时感觉没什么卵用 10.22
	//TMap<FString, TSubclassOf<UUserWidgetBase>> m_WidgetTypeRelate;

/************************************************************************/
/*						创建至GameInstance相关                          */
/************************************************************************/
public:
	//在此向GameInstance GameViewportClient中加入根节点UI 该UI不会被清除
	void CreateInstanceRootWidget(UGameInstance* GameInstance);

	//创建INstanceUMG
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, meta = (WorldContext = "WorldContextObject"))
	UUserWidgetBase* CreateInstanceWidget(const UObject* WorldContextObject ,FString _widgetBlueprintPath,
		EUMGLayer Layer = EUMGLayer::None, int32 _zorder = 0);
	UUserWidgetBase* CreateInstanceWidget(UWorld* _world, FString _widgetBlueprintPath, EUMGLayer Layer = EUMGLayer::None,
		int32 _zorder = 0);

	//根据UID查找某个UMD
	UUserWidgetBase* GetInsUMG(FString UID);
	/* 根据Layer查找所有的Widget */
	TArray<UUserWidgetBase*> GetInsUMGS(EUMGLayer Layer);

	/*获取对应UI的层级*/
	EUMGLayer GetInsWidgetLayer(FString UID);
	EUMGLayer GetInsWidgetLayer(UUserWidgetBase* InWidget);

	//删除全屏UMG
	//void DeleteInsUMGWidget(FString UID);
	//删除全屏UMG
	//void DeleteInsUMGWidget( UUserWidgetBase* widget);
	//删除全屏UMG
	void ClearInsWidget(FString UID);
	void ClearInsWidget(UUserWidgetBase* InWidget);
	/* 清除对应层的全屏UI */
	void ClearInsWidget(EUMGLayer Layer);
	//清空全屏UMG
	void ClearAllIns();

protected:
	//根节点的widget
	UPROPERTY()
	UFullScreenRoot* m_RootWidget;
	//存储在根节点上的UI的Array
	UPROPERTY()
	TMap<EUMGLayer, FUMGLayerWidget> m_InsWidgetList;
	//用来记录Widget名字对应的List中的Index  //19.11.4 由于引擎莫名其妙的清空行为，重构一次
	//TMap<FString, int32> m_InsWidgetIndexList;

	UPROPERTY()
	FVector2D m_ViewPortSize;

};
