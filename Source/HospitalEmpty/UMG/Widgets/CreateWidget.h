// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UMG/UserWidgetBase.h"
#include "UMGOverride/TestListView/HpTestListView.h"
#include "CreateWidget.generated.h"

/**
	������Ʒ�Ĳ˵�����������ȡ��������ӵ������е���Ʒ��Ȼ����ʾ
	�����Ʒ�󣬻�ֱ�Ӵ���һ��
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
	//���Զ�Ӧ���3����ͬ�ļҾ�
	UFUNCTION(BlueprintCallable)
	void OnClickedListItem0();
	UFUNCTION(BlueprintCallable)
	void OnClickedListItem1();
	UFUNCTION(BlueprintCallable)
	void OnClickedListItem2();


public:
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	UHpTestListView *ListView;   

	UPROPERTY()
	class UDecorationSystemMgr* DecorationSystemMgr;
};
