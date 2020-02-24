// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UMG/UserWidgetBase.h"
#include "UMGOverride/TestListView/HpTestListView.h"
#include "CreateWidget.generated.h"

/**
 * 
 */
UCLASS()
class UCreateWidget : public UUserWidgetBase
{
	GENERATED_BODY()
	
public:
	UCreateWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override {
		Super::NativeConstruct();
	};

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime)override
	{
		Super::NativeTick(MyGeometry , InDeltaTime);
	};

public:

	UFUNCTION(BlueprintCallable)
	void AddToListView(UWidget* Content);

public:
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	UHpTestListView *ListView;   
};
