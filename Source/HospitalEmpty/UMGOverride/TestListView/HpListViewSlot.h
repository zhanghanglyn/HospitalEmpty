#pragma once

#include "CoreMinimal.h"
#include "Components/PanelSlot.h"
#include "Components/Widget.h"
#include "SHpListView.h"
#include "HpListViewSlot.generated.h"

/*
	测试自定义ListView UMG部分
*/
UCLASS(BlueprintType)
class UHpListViewSlot : public UPanelSlot
{
	GENERATED_UCLASS_BODY()

public:

	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	/** Applies all properties to the live slot if possible. */
	virtual void SynchronizeProperties() override;

	void BuildSlot(TSharedRef<SHpListView> InHpListView);

protected:
	SHpListView::FSlot* Slot;
};