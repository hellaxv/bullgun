// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyTank_Pawn.h"
#include "EnemyAIController.h"
#include "TurretCannon.h"
#include "Kismet/KismetSystemLibrary.h"
#include "HealthComponent.h"
#include "Tank_PlayerController.h"
#include "Turret.h"
#include "Components/AudioComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AEnemyTank_Pawn::AEnemyTank_Pawn():AParentStarterClass()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RangeSphere = CreateDefaultSubobject<USphereComponent>("RangeSphere");
	RangeSphere -> SetupAttachment(RootComponent);
	RangeSphere -> OnComponentBeginOverlap.AddDynamic(this, &AEnemyTank_Pawn::OnBeginOverlap);
	RangeSphere -> OnComponentEndOverlap.AddDynamic(this, &AEnemyTank_Pawn::OnEndOverlap);
	
	HealthComponent->OnHealthChanged.AddUObject(this, &AEnemyTank_Pawn::OnHealthChanged);
	HealthComponent->OnDestroy.AddUObject(this, &AEnemyTank_Pawn::OnDestroy);


}

void AEnemyTank_Pawn::MoveForward(float Scale)
{
	MoveScaleTarget = Scale;
}

void AEnemyTank_Pawn::RotateRight(float Scale)
{
	RotationScaleTarget = Scale;
}


void AEnemyTank_Pawn::TurretShoot()
{
	
	if( TurretCannon != nullptr )
	{
		TurretCannon->TurretShoot(AmmoII);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(3331, 1.5,FColor::Red,FString(TEXT("NO BULLET")));
	}
	
}

void AEnemyTank_Pawn::TurretSetupCannon(const TSubclassOf<ATurretCannon>& CannonClass)
{
	TurretCannonType = CannonClass;
	if(TurretCannonType)
	{
		auto Transform = CannonPosition->GetComponentTransform();
		TurretCannon = GetWorld()->SpawnActor<ATurretCannon>(TurretCannonType, Transform);
		TurretCannon->AttachToComponent(CannonPosition,FAttachmentTransformRules::SnapToTargetIncludingScale);
	}
}

void AEnemyTank_Pawn::TakeDamage(FDamageData Damage)
{
	if(HealthComponent)
		HealthComponent->TakeDamage(Damage);
}

void AEnemyTank_Pawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemyTank_Pawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	EnemyAITankController = Cast<AEnemyAIController>(NewController);
	TargetController = Cast<ITargetController>(NewController);
}

// Called when the game starts or when spawned
void AEnemyTank_Pawn::BeginPlay()
{
	Super::BeginPlay();
	
	TurretSetupCannon(TurretCannonType);
}

void AEnemyTank_Pawn::Destroyed()
{
	if(TurretCannon)
	{
		TurretCannon->Destroy();
	}
	
}

// Called every frame
void AEnemyTank_Pawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MoveTank(DeltaTime);
	RotateTank(DeltaTime);
	//RotateCannon(DeltaTime);
}

void AEnemyTank_Pawn::OnBeginOverlap(UPrimitiveComponent* PrimitiveComponent, AActor* Actor,
							 UPrimitiveComponent* OtherComponent, int I, bool bArg, const FHitResult& HitResult)
{
	auto AIController = Cast<AEnemyAIController>(GetController());
	if(AIController)
		AIController->AddTarget(Actor);
	
}

void AEnemyTank_Pawn::OnEndOverlap(UPrimitiveComponent* PrimitiveComponent, AActor* Actor,
	UPrimitiveComponent* OtherComponent, int I)
{
	auto AIController = Cast<AEnemyAIController>(GetController());
	if(AIController)
	AIController->RemoveTarget(Actor);
}



void AEnemyTank_Pawn::MoveTank(float DeltaTime)
{
	MoveScaleCurrent = FMath::Lerp(MoveScaleCurrent, MoveScaleTarget,MovementAcceleration);
	const auto Location = GetActorLocation();
	SetActorLocation(Location + GetActorForwardVector() * MoveScaleCurrent * MovementSpeed * DeltaTime, false);
//	GEngine->AddOnScreenDebugMessage(-666, -5, FColor::Turquoise, FString::Printf(TEXT("Speed - %f"), MoveScaleCurrent));
}

void AEnemyTank_Pawn::RotateTank(float DeltaTime)
{
	RotationScaleCurrent = FMath::Lerp(RotationScaleCurrent, RotationScaleTarget, RotationAcceleration);
	auto Rotation = GetActorRotation();
	Rotation.Yaw += RotationScaleCurrent * RotationSpeed * DeltaTime;
	SetActorRotation(Rotation);
	//UE_LOG(LogTankProject, Warning, TEXT("%s"), *Rotation.ToString())
}

void AEnemyTank_Pawn::RotateCannon(float DeltaTime)
{
	if(!TargetController)
		return;
	//auto TC = reinterpret_cast<ITargetController*>(TargetController);
	auto BodyMeshRotation = BodyMesh->GetComponentRotation();
	auto OldRotation = TurretMesh->GetComponentRotation();
	FRotator TurretRotation = UKismetMathLibrary::FindLookAtRotation(TurretMesh->GetComponentLocation(),TargetController->GetShootTarget());
	OldRotation.Yaw = TurretRotation.Yaw;
	OldRotation.Pitch = BodyMeshRotation.Pitch;
	OldRotation.Roll = BodyMeshRotation.Roll;
	TurretMesh->SetWorldRotation(FMath::Lerp(TurretMesh->GetComponentRotation(),OldRotation,RotationAcceleration));
}


void AEnemyTank_Pawn::OnDestroy()
{
	Destroy();
	GEngine->AddOnScreenDebugMessage(9871, 3, FColor::White,FString::Printf(TEXT("Enemy Tank Destroyed")));
	//UKismetSystemLibrary::QuitGame(GetWorld(),GetWorld()->GetNumPlayerControllers(),EQuitPreference::Quit,true);
}

void AEnemyTank_Pawn::OnHealthChanged(float CurrentHealth)
{
	GEngine->AddOnScreenDebugMessage(83235042, 3, FColor::White,FString::Printf(TEXT("Health Enemy Tank: %f"), CurrentHealth));
}



