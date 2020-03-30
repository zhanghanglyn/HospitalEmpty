// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UMG/UserWidgetBase.h"
#include "UMGOverride/TestListView/HpTestListView.h"
#include "SaveDataPlWidget.generated.h"

/**
	放置物品的菜单，从配置中取出可以添加到场景中的物品，然后显示
	点击物品后，会直接创建一个
 */
UCLASS()
class USaveDataPlWidget : public UUserWidgetBase
{
	GENERATED_BODY()
	
public:
	USaveDataPlWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime)override
	{
		Super::NativeTick(MyGeometry , InDeltaTime);
	};

	UFUNCTION(BlueprintCallable)
	void AddSaveItemToList();

	/* 点击存档的回调 */
	UFUNCTION(BlueprintCallable)
	void ClickDataCall( FString GameID);


public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UHpTestListView *SaveList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "Item蓝图路径" ,  meta = ( Category = "BindWidget"))
	FString ItemPath;

};
