// Fill out your copyright notice in the Description page of Project Settings.


#include "Tank_Pawn.h"

#include "Tank_PlayerController.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Project2/Project2.h"
#include "Cannon.h"
#include "TurretCannon.h"
#include "SprayCannon.h"
#include "HealthComponent.h"
#include "Turret.h"


// Sets default values
ATank_Pawn::ATank_Pawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/*BoxComponent=CreateDefaultSubobject<UBoxComponent>("BoxComponent");
	RootComponent=BoxComponent;

	BodyMesh=CreateDefaultSubobject<USkeletalMeshComponent>("BodyMesh");
	BodyMesh->SetupAttachment(RootComponent);

	TurretMesh=CreateDefaultSubobject<UStaticMeshComponent>("TurretMesh");
	TurretMesh->SetupAttachment(BodyMesh);

	BoxComponentTurret=CreateDefaultSubobject<UBoxComponent>("BoxCollision");
	BoxComponentTurret->SetupAttachment(TurretMesh);

	CapsuleComponentCannon=CreateDefaultSubobject<UCapsuleComponent>("CapsuleComponent");
	CapsuleComponentCannon->SetupAttachment(TurretMesh);

	MachineGunMesh=CreateDefaultSubobject<USkeletalMeshComponent>("MachineGunMesh");
	MachineGunMesh->SetupAttachment(TurretMesh);*/

	//CannonPosition = CreateDefaultSubobject<UArrowComponent>("CannonPosition");
	//CannonPosition->SetupAttachment(TurretMesh);

	//SprayCannonPosition=CreateDefaultSubobject<UArrowComponent>("SprayCannonPosition");
	//SprayCannonPosition->SetupAttachment(MachineGunMesh);

	ArmComponent=CreateDefaultSubobject<USpringArmComponent>("ArmComponent");
	ArmComponent->SetupAttachment(RootComponent);
	ArmComponent->bInheritYaw=false;
	ArmComponent->bInheritPitch=false;
	ArmComponent->bInheritRoll=false;

	CameraComponent=CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(ArmComponent);
	CameraComponent->bUsePawnControlRotation=false;

	//HealthComponent = CreateDefaultSubobject<UHealthComponent>("HealthComponent");
	HealthComponent->OnHealthChanged.AddUObject(this, &ATank_Pawn::OnHealthChanged);
	HealthComponent->OnDestroy.AddUObject(this, &ATank_Pawn::OnDestroy);
}

void ATank_Pawn::MoveForward(float Scale)
{
	MoveScaleTarget = Scale;
}

void ATank_Pawn::RotateRight(float Scale)
{
	RotationScaleTarget = Scale;
}


void ATank_Pawn::Shoot()
{
	if(SwapWep == 0)
	{
		if (Cannon &&AmmoTap>0)
		{
			Cannon->Shoot(AmmoTap);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(6, 2, FColor::Cyan, FString(TEXT("NOTHING AMMOTAP")));
		}
	}
	else
	{
		SprayShoot();
	}
}
void ATank_Pawn::SetupCannon(const TSubclassOf<ACannon>& CannonClass)
{
	CannonType = CannonClass;
	if (CannonType)
	{
		auto Transform = CannonPosition->GetComponentTransform();
		Cannon = GetWorld()->SpawnActor<ACannon>(CannonType, Transform);
		Cannon->AttachToComponent(CannonPosition, FAttachmentTransformRules::SnapToTargetIncludingScale);
		//auto Transform = CannonPosition->GetComponentTransform();
		//GetWorld()->SpawnActor(CannonType, &Transform);
	}
}

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


void ATank_Pawn::SprayShoot()
{
	if (SprayCannon &&AmmoSpray>0)
	{
		SprayCannon->SprayShoot(AmmoSpray);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(6, 2, FColor::Cyan, FString(TEXT("NOTHING AMMOSPRAY")));
	}
}
void ATank_Pawn::SetupCannonSpray(const TSubclassOf<ASprayCannon>& CannonClass)
{
	SprayCannonType = CannonClass;
	if (SprayCannonType)
	{
		auto Transform = CannonPosition->GetComponentTransform();
		SprayCannon = GetWorld()->SpawnActor<ASprayCannon>(SprayCannonType, Transform);
		SprayCannon->AttachToComponent(SprayCannonPosition, FAttachmentTransformRules::SnapToTargetIncludingScale);
		//auto Transform = CannonPosition->GetComponentTransform();
		//GetWorld()->SpawnActor(CannonType, &Transform);
	}
}

void ATank_Pawn::TurretShoot()
{
	
	if( TurretCannon != nullptr && AmmoTurret > 0)
	{
		TurretCannon->TurretShoot(AmmoTurret);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(3331, 1.5,FColor::Red,FString(TEXT("NO BULLET")));
	}
	
}


void ATank_Pawn::TurretSetupCannon(const TSubclassOf<ATurretCannon>& CannonClass)
{
	TurretCannonType = CannonClass;
	if(TurretCannonType)
	{
		auto Transform = CannonPosition->GetComponentTransform();
		TurretCannon = GetWorld()->SpawnActor<ATurretCannon>(TurretCannonType, Transform);
		TurretCannon->AttachToComponent(CannonPosition,FAttachmentTransformRules::SnapToTargetIncludingScale);
	}
}



void ATank_Pawn::Swap()
{
	if (SwapWep == 0)
	{
		SwapWep = 1;
	}
	else
	{
		SwapWep = 0;
	}
}

void ATank_Pawn::UploadAmmo(const int ShellCount)
{
	AmmoTap += ShellCount;
	GEngine->AddOnScreenDebugMessage(1234, 9, FColor::Blue, FString::Printf(TEXT("AmmoBB - %i"), AmmoTap));
	
	AmmoSpray += ShellCount;
	GEngine->AddOnScreenDebugMessage(1235, 9, FColor::Blue, FString::Printf(TEXT("AmmoAK - %i"), AmmoSpray));
}

void ATank_Pawn::TakeDamage(FDamageData Damage)
{
	if (HealthComponent)
		HealthComponent->TakeDamage(Damage);
}

// Called when the game starts or when spawned
void ATank_Pawn::BeginPlay()
{
	Super::BeginPlay();

	TankController = Cast<ATank_PlayerController>(GetController());

	//SetupCannon(CannonType); мы его создали выше и можем вызвать сюда, для сокращения кода.
	if (CannonType)
	{
		auto Transform = CannonPosition->GetComponentTransform();
		Cannon = GetWorld()->SpawnActor<ACannon>(CannonType, Transform);
		Cannon->AttachToComponent(CannonPosition, FAttachmentTransformRules::SnapToTargetIncludingScale);
		//auto Transform = CannonPosition->GetComponentTransform();
		//GetWorld()->SpawnActor(CannonType, &Transform);
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	TankController = Cast<ATank_PlayerController>(GetController());

	if (SprayCannonType)
	{
		auto Transform = SprayCannonPosition->GetComponentTransform();
		SprayCannon = GetWorld()->SpawnActor<ASprayCannon>(SprayCannonType, Transform);
		SprayCannon->AttachToComponent(SprayCannonPosition, FAttachmentTransformRules::SnapToTargetIncludingScale);
	}

}

void ATank_Pawn::Destroyed()
{
	Super::Destroyed();

	if (Cannon)
	{
		Cannon->Destroy();
	}
	if (SprayCannon)
	{
		SprayCannon->Destroy();
	}
	if(TurretCannon)
	{
		TurretCannon->Destroy();
	}
}

// Called every frame
void ATank_Pawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	MoveTank(DeltaTime);
	RotateTank(DeltaTime);
	RotateCannon(DeltaTime);

	/*доп.код для аналога движения влево\направо.
	CurrentRightAxisValue = FMath::Lerp(CurrentRightAxisValue, TargetRightAxisValue, InterpolationKey);
	float yawRotation = RotationSpeed * CurrentRightAxisValue * DeltaTime;
	FRotator currentRotation = GetActorRotation();
	yawRotation = currentRotation.Yaw + yawRotation;
	FRotator newRotation = FRotator(0, yawRotation, 0);
	SetActorRotation(newRotation);
	*/

}

// Called to bind functionality to input
void ATank_Pawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ATank_Pawn::MoveTank(float DeltaTime)
{
	MoveScaleCurrent = FMath::Lerp(MoveScaleCurrent, MoveScaleTarget, MovementAcceleration);
	const auto Location = GetActorLocation();
	SetActorLocation(Location + GetActorForwardVector() * MoveScaleCurrent * MovementSpeed * DeltaTime, false);

	/*почему-то у преподавателя работает с тру.
	SetActorLocation(Location + GetActorForwardVector() * MoveScaleCurrent * MovementSpeed * DeltaTime, true);
	*/
	
	//отображает на экране скорость танка в виде строчки когда
	GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Cyan, FString::Printf(TEXT("%f"), MoveScaleCurrent));
	
	
}

void ATank_Pawn::RotateTank(float DeltaTime)
{
	RotationScaleCurrent = FMath::Lerp(RotationScaleCurrent, RotationScaleTarget, RotationAcceleration);
	auto Rotation = GetActorRotation();
	Rotation.Yaw += RotationScaleCurrent * RotationSpeed * DeltaTime;
	SetActorRotation(Rotation);
	UE_LOG(LogTanks, Warning, TEXT("%s"), *Rotation.ToString())
}

void ATank_Pawn::RotateCannon(float DeltaTime)
{
	if (!TankController)
		return;
	auto BodyMeshRotation = BodyMesh->GetComponentRotation();
	auto OldRotation = TurretMesh->GetComponentRotation();
	FRotator TurretRotation =  UKismetMathLibrary::FindLookAtRotation(TurretMesh->GetComponentLocation(), TankController->GetShootTarget());
	OldRotation.Yaw = TurretRotation.Yaw;
	OldRotation.Pitch = BodyMeshRotation.Pitch;
	OldRotation.Roll = BodyMeshRotation.Roll;
	TurretMesh->SetWorldRotation(FMath::Lerp(TurretMesh->GetComponentRotation(), OldRotation, TurretRotationAcceleration));
	//56:13
}

void ATank_Pawn::OnHealthChanged(float CurrentHealth)
{
		GEngine->AddOnScreenDebugMessage(-3233, 3, FColor::Green, FString::Printf(TEXT("ZAEBAL: %f"), CurrentHealth));
}

void ATank_Pawn::OnDestroy()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, true);
}
