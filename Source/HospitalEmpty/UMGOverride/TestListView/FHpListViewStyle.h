#pragma once

#include "CoreMinimal.h"
#include "SlateWidgetStyle.h"
#include "SlateBasics.h"
#include "FHpListViewStyle.generated.h"

/*
	测试自定义ListView UMG部分
*/
USTRUCT(BlueprintType)
struct FHpListViewStyle : public FSlateWidgetStyle
{
	GENERATED_USTRUCT_BODY()

public:
	FHpListViewStyle();

	virtual ~FHpListViewStyle() {};

	virtual void GetResources(TArray<const FSlateBrush *> & OutBrushes) const override;

	static const FName TypeName;
	virtual const FName GetTypeName() const override { return TypeName; };

	// Allows us to set default values for our various styles. 
	static const FHpListViewStyle& GetDefault();

	/** ListView的背景图片 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Appearance)
	FSlateBrush BackgroundImage;

	FHpListViewStyle& SetBackgroundImage(const FSlateBrush& InBackgroundImage);

};