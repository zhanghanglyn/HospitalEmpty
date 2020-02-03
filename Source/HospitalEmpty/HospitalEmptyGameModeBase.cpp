// Fill out your copyright notice in the Description page of Project Settings.


#include "HospitalEmptyGameModeBase.h"
#include "RTSMode/Private/RTSControl/RTSPlayerCameraSpectatorPawn.h"
#include "RTSMode/Private/RTSControl/RTSPlayerController.h"

AHospitalEmptyGameModeBase::AHospitalEmptyGameModeBase() :Super()
{
	PlayerControllerClass = ARTSPlayerController::StaticClass();
	DefaultPawnClass = ARTSPlayerCameraSpectatorPawn::StaticClass();
}