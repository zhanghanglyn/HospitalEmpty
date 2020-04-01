// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UMG/UserWidgetBase.h"
#include "UMGOverride/TestListView/HpTestListView.h"
#include "CreateWidget.generated.h"

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

public:
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	UHpTestListView *ListView;   

	UPROPERTY()
	class UDecorationSystemMgr* DecorationSystemMgr;

	//序列化测试用
	UPROPERTY()
	class ATestSerializeObj* TestSerializeObj;
};
