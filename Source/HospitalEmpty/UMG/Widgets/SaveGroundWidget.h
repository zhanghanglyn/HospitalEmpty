// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UMG/UserWidgetBase.h"
#include "SaveGroundWidget.generated.h"

/**
	������Ʒ�Ĳ˵�����������ȡ��������ӵ������е���Ʒ��Ȼ����ʾ
	�����Ʒ�󣬻�ֱ�Ӵ���һ��
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
