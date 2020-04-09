// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UMG/UserWidgetBase.h"
#include "SaveGroundWidget.generated.h"

/**
	放置物品的菜单，从配置中取出可以添加到场景中的物品，然后显示
	点击物品后，会直接创建一个
 */
UCLASS()
class USaveGroundWidget : public UUserWidgetBase
{
	GENERATED_BODY()
	
public:
	USaveGroundWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime)override
	{
		Super::NativeTick(MyGeometry , InDeltaTime);
	};

	UFUNCTION()
	void SaveGround();

	UFUNCTION()
	void LoadGround();


public:
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	UButton *SaveButton;   

	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	UButton *LoadButton;   

	UPROPERTY()
	class UDecorationSystemMgr* DecorationSystemMgr;

};
