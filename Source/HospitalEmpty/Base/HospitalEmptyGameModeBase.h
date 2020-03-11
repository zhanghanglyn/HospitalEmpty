// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HospitalEmptyGameModeBase.generated.h"

/**
 * GameMode的BeginPlay中，
 */
UCLASS()
class HOSPITALEMPTY_API AHospitalEmptyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AHospitalEmptyGameModeBase();

	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void BeginPlay() override;
	virtual void StartPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
