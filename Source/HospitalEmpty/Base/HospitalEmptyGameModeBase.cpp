// Fill out your copyright notice in the Description page of Project Settings.


#include "HospitalEmptyGameModeBase.h"
#include "PlayerController/HptPlayerController.h"
#include "PlayerController/HptPlayerCameraPawn.h"

AHospitalEmptyGameModeBase::AHospitalEmptyGameModeBase() :Super()
{
	PlayerControllerClass = AHptPlayerController::StaticClass();
	DefaultPawnClass = AHptPlayerCameraPawn::StaticClass();
}

void AHospitalEmptyGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);


}

void AHospitalEmptyGameModeBase::StartPlay()
{
	Super::StartPlay();
}

void AHospitalEmptyGameModeBase::BeginPlay()
{
	Super::BeginPlay();
}

void AHospitalEmptyGameModeBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}