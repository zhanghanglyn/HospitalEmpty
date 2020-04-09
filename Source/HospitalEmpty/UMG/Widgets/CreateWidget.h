// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UMG/UserWidgetBase.h"
#include "Util/StateEnum.h"
#include "UMGOverride/TestListView/HpTestListView.h"
#include "CreateWidget.generated.h"

/* 测试用Item Info ,包含了身上所持有的数量以及该道具的类型 */
USTRUCT(BlueprintType)
struct FDecorationInfo
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadWrite , EditAnywhere , meta = (DisplayName = "家具类型"))
	EDecorationType DecorationType = EDecorationType::None;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "家具数量"))
	int32 DecorationCount = 0;
};

/**
	放置物品的菜单，从配置中取出可以添加到场景中的物品，然后显示
	点击物品后，会直接创建一个
 */
UCLASS()
class UCreateWidget : public UUserWidgetBase
{
	GENERATED_BODY()
	
public:
	UCreateWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime)override
	{
		Super::NativeTick(MyGeometry , InDeltaTime);
	};

public:

	UFUNCTION(BlueprintCallable)
	void AddToListView(UWidget* Content);

protected:
	UFUNCTION(BlueprintCallable)
	void OnClickListView(FString Param, FString TestParam);
	UFUNCTION(BlueprintCallable)
	void OnClickListViewNoExParam(FString Param);
	
	//测试对应点击3个不同的家具
	UFUNCTION(BlueprintCallable)
	void OnClickedListItem0();
	UFUNCTION(BlueprintCallable)
	void OnClickedListItem1();
	UFUNCTION(BlueprintCallable)
	void OnClickedListItem2();
	UFUNCTION(BlueprintCallable)
	void OnClickedListItem3();

	//3.10 序列化用测试
	void CreateSerializeObj();
	void SaveSerialize();
	void LoadSerialize();
	void CreateObjAndSerialize();

	/************************************************************************/
	/*                      新的以蓝图的方式来处理                          */
	/************************************************************************/
	/* 更新并返回玩家身上当前的可放置道具数量 */
	UFUNCTION(BlueprintCallable)
	TArray<FDecorationInfo> UpdateDecInfo();
	/* 清除当前List */
	UFUNCTION(BlueprintCallable)
	void ClearCurList();
public:
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	UHpTestListView *ListView;   

	UPROPERTY()
	class UDecorationSystemMgr* DecorationSystemMgr;

	//序列化测试用
	UPROPERTY()
	class ATestSerializeObj* TestSerializeObj;
};
