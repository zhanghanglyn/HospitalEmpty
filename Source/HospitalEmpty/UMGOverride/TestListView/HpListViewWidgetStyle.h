#pragma once

#include "CoreMinimal.h"
#include "SlateWidgetStyleContainerBase.h"
#include "FHpListViewStyle.h"
#include "HpListViewWidgetStyle.generated.h"

/*
	测试自定义ListView UMG部分
*/
UCLASS(BlueprintType, hidecategories = Object)
class UHpListViewWidgetStyle : public USlateWidgetStyleContainerBase
{
	GENERATED_BODY()

public:
	/** The actual data describing the progress's appearance. */
	UPROPERTY(Category = "Style", EditAnywhere, BlueprintReadWrite, meta = (ShowOnlyInnerProperties))
	FHpListViewStyle HpListViewStyle;

	virtual const struct FSlateWidgetStyle* const GetStyle() const override
	{
		return static_cast<const struct FSlateWidgetStyle*>(&HpListViewStyle);
	}
};