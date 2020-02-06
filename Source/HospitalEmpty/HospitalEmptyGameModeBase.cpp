// Fill out your copyright notice in the Description page of Project Settings.


#include "HospitalEmptyGameModeBase.h"
//#include "RTSMode/Private/RTSControl/RTSPlayerCameraSpectatorPawn.h"
//#include "RTSMode/Private/RTSControl/RTSPlayerController.h"
#include "PlayerController/HptPlayerController.h"
#include "PlayerController/HptPlayerCameraPawn.h"

AHospitalEmptyGameModeBase::AHospitalEmptyGameModeBase() :Super()
{
	PlayerControllerClass = AHptPlayerController::StaticClass();
	DefaultPawnClass = AHptPlayerCameraPawn::StaticClass();
}