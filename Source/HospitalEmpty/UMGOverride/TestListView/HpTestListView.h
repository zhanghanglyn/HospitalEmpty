#pragma once

#include "CoreMinimal.h"
#include "Components/PanelWidget.h"
#include "HpTestListView.generated.h"

/*
	测试自定义ListView UMG部分
*/
UCLASS()
class UHpTestListView : public UPanelWidget
{
	GENERATED_UCLASS_BODY()

public:
#if WITH_EDITOR
	/* 返回一个在UMG对应列表中的分类 */
	virtual const FText GetPaletteCategory() override;
#endif
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	/** Function implemented by all subclasses of UWidget is called when the underlying SWidget needs to be constructed. */
	virtual TSharedRef<SWidget> RebuildWidget() override;


	/* UPanelWidget Virtual */
	virtual UClass* GetSlotClass() const override;
	virtual void OnSlotAdded(UPanelSlot* InSlot);
	virtual void OnSlotRemoved(UPanelSlot* InSlot);
	


public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "自定义ListView测试")
	FMargin ContentMargin;

protected:
	/* 对应的SlistView Widget */
	
};