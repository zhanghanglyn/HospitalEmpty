// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UMG/UserWidgetBase.h"
#include "SaveDataItemWidget.generated.h"

/**
	放置物品的菜单，从配置中取出可以添加到场景中的物品，然后显示
	点击物品后，会直接创建一个
 */
UCLASS()
class USaveDataItemWidget : public UUserWidgetBase
{
	GENERATED_BODY()

public:
	/* 声明一个被点击的委托 */
	DECLARE_DELEGATE_OneParam(FMouseClickDelegate , FString)
	FMouseClickDelegate MouseClickDelegate;

public:
	USaveDataItemWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime)override
	{
		Super::NativeTick(MyGeometry , InDeltaTime);
	};
	
	//virtual FEventReply OnMouseButtonDown( const FGeometry& InGeometry, const FPointerEvent& InMouseEvent )override;

public:
	/* 存档缩略图 */
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* DataPic;  
	/* 存档名字 */
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* DataName;
	/* 存档时间 */
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* SaveTime;

	UPROPERTY()
	FString GameID = "";

};
