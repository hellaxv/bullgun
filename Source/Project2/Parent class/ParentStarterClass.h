
#pragma once

#include "CoreMinimal.h"
#include "AmmoBox.h"
#include "DamageTarget.h"
#include "HealthComponent.h"
#include "GameFramework/SpectatorPawn.h"
#include "ParentStarterClass.generated.h"

UCLASS(Blueprintable)
class PROJECT2_API AParentStarterClass : public APawn, public IDamageTarget
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AParentStarterClass();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UBoxComponent* BoxComponent;

	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	//UBoxComponent* Collision;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USkeletalMeshComponent* BodyMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* TurretMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
	USkeletalMeshComponent* MachineGunMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UArrowComponent* CannonPosition;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UArrowComponent* SprayCannonPosition;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UHealthComponent* HealthComponent;
	
	virtual void TakeDamage(FDamageData Damage) override;

protected:
	// Called when the game starts or when spawned
	
	virtual void BeginPlay() override;
	
	virtual void Destroyed() override;
	
	virtual void OnDestroy();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	
};


