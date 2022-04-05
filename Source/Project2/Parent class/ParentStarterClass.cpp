// Fill out your copyright notice in the Description page of Project Settings.

#include "ParentStarterClass.h"
#include "Tank_Pawn.h"
#include "Kismet/KismetSystemLibrary.h"
#include "HealthComponent.h"


// Sets default values
AParentStarterClass::AParentStarterClass()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>("BoxComponent");
	RootComponent = BoxComponent;

//	Collision = CreateDefaultSubobject<UBoxComponent>("Collision");
	//RootComponent = Collision;

	BodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>("BodyMesh");
	BodyMesh -> SetupAttachment(RootComponent);
	
	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>("TurretMesh");
	TurretMesh -> SetupAttachment(BodyMesh);

	MachineGunMesh=CreateDefaultSubobject<USkeletalMeshComponent>("MachineGunMesh");
	MachineGunMesh->SetupAttachment(TurretMesh);

	CannonPosition = CreateDefaultSubobject<UArrowComponent>("CannonPosition");
	CannonPosition->SetupAttachment(TurretMesh);

	SprayCannonPosition=CreateDefaultSubobject<UArrowComponent>("SprayCannonPosition");
	SprayCannonPosition->SetupAttachment(MachineGunMesh);
	
	HealthComponent = CreateDefaultSubobject<UHealthComponent>("HealthComponent");
	
}

void AParentStarterClass::TakeDamage(FDamageData Damage)
{
	if(HealthComponent)
		HealthComponent->TakeDamage(Damage);
}

void AParentStarterClass::BeginPlay()
{
	Super::BeginPlay();
}

void AParentStarterClass::Destroyed()
{
	Super::Destroyed();
}

void AParentStarterClass::OnDestroy()
{
}

void AParentStarterClass::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AParentStarterClass::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


