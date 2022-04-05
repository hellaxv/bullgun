// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tank_Pawn.h"
#include "TargetController.h"
#include "GameFramework/PlayerController.h"
#include "Tank_PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT2_API ATank_PlayerController : public APlayerController, public ITargetController
{
	GENERATED_BODY()
	
public:
	
	virtual void SetupInputComponent() override;
	
	void OnMoveForward (float Value);
	void OnRotateRight (float Value);
	void OnShoot();
	void OnSprayShoot();
	void OnTurretShoot();

	void OnSwapWeapon();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual FVector GetShootTarget() const override { return MouseWorldPosition; }
	
private:
	UPROPERTY()
	ATank_Pawn* PlayerTank;
	
	FVector MouseWorldPosition;
	
};

