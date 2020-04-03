#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class FLayoutDirection : uint8
{
	NONE = 0			UMETA(DisplayName = "没有方向"),
	Horizontal = 1		UMETA(DisplayName = "横向"),
	Vertical = 2		UMETA(DisplayName = "纵向")
};