// Fill out your copyright notice in the Description page of Project Settings.


#include "Turret.h"

#include "Kismet/KismetMathLibrary.h"

// Sets default values
ATurret::ATurret()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;


	//Collision = CreateDefaultSubobject<UCapsuleComponent>("Collision");
	//RootComponent = Collision;

	//BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>("BodyMesh");
	//BodyMesh->SetupAttachment(RootComponent);

	//TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>("TurretMesh");
	//TurretMesh->SetupAttachment(BodyMesh);

	RangeSphere = CreateDefaultSubobject<USphereComponent>("RangeSphere");
	RangeSphere->SetupAttachment(RootComponent);
	RangeSphere->OnComponentBeginOverlap.AddDynamic(this, &ATurret::OnBeginOverlap); //юнит входит в область действия турели у врага. &ATurret::OnUnit.
	RangeSphere->OnComponentEndOverlap.AddDynamic(this, &ATurret::OnEndOverlap);

	//CannonPosition = CreateDefaultSubobject<UArrowComponent>("CannonPosition");
	//CannonPosition->SetupAttachment(TurretMesh);

	//HealthComponent = CreateDefaultSubobject<UHealthComponent>("HealthComponent");
	HealthComponent->OnHealthChanged.AddUObject(this, &ATurret::OnHealthChanged);
	HealthComponent->OnDestroy.AddUObject(this, &ATurret::OnDestroy);
}

// Called when the game starts or when spawned
void ATurret::BeginPlay()
{
	Super::BeginPlay();

	/*ТАЙМЕР ДЛЯ ПРИЦЕЛИВАНИЯ.
	 он имеет меньшую дескритизацию, поэтому будет меньше нагружать наши расчеты(меньше нагружать процессор), но его наличие тут не всегда уместно, так как у нас есть Tick.
	*/
	FTimerHandle Timer;
	GetWorld()->GetTimerManager().SetTimer(Timer, this, &ATurret::Targeting, TargetingRate, true);

	if (CannonClass)
	{
		auto Transform = CannonPosition->GetComponentTransform();
		TurretCannon = GetWorld()->SpawnActor<ATurretCannon>(CannonClass, Transform);
		TurretCannon->AttachToComponent(CannonPosition, FAttachmentTransformRules::SnapToTargetIncludingScale);
		//auto Transform = CannonPosition->GetComponentTransform();
		//GetWorld()->SpawnActor(CannonType, &Transform);
	}
}

void ATurret::TakeDamage(FDamageData Damage)
{
	if (HealthComponent)
		HealthComponent->TakeDamage(Damage);
}

void ATurret::Destroyed()
{
	Super::Destroyed();
	if (TurretCannon)
		TurretCannon->Destroy();
}

void ATurret::OnBeginOverlap(UPrimitiveComponent* PrimitiveComponent, AActor* Actor,
                             UPrimitiveComponent* OtherComponent, int I, bool bArg, const FHitResult& HitResult)
{
	OtherActors.Add(Actor);
	if (!Target.IsValid())
	{
		FindNextTarget();
	}
}

void ATurret::OnEndOverlap(UPrimitiveComponent* PrimitiveComponent, AActor* Actor,
	UPrimitiveComponent* OtherComponent, int I)
{
	OtherActors.Remove(Actor);
	if (Actor == Target)
	{
		FindNextTarget();
	}
}

// Called every frame
void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATurret::OnHealthChanged(float CurrentHealth)
{
	GEngine->AddOnScreenDebugMessage(987665, 1, FColor::Blue,FString::Printf(TEXT("Enemy Health: %f"), CurrentHealth));
}

void ATurret::FindNextTarget()
{
	float MinRange = 100000;
	Target = nullptr;
	for (auto Actor : OtherActors)
	{
		float Range = FVector::Distance(Actor->GetActorLocation(), GetActorLocation());
		if (Range < MinRange)
		{
			MinRange = Range;
			Target = Actor;
		}
	}
}

void ATurret::Fire()
{
	if(TurretCannon && Target.IsValid() && Target!= nullptr)
	{
		TurretCannon->TurretShoot(AmmoTurret);// что то пошло не так - создай снаряды для ИИ
	}
}

void ATurret::Targeting()
{
	//логика отвечающая за прицеливание.
	if (!Target.IsValid())
	{
		return;
	} 
	//проверка цели, есть ли она или нет. 
	auto TargetRotation = UKismetMathLibrary::FindLookAtRotation(TurretMesh->GetComponentLocation(), Target->GetActorLocation());
	TurretMesh->SetWorldRotation(FMath::Lerp(TurretMesh->GetComponentRotation(), TargetRotation, TargetingSpeed));


	auto TargetingDirection = TurretMesh->GetForwardVector();
	auto PlayerDirection = Target->GetActorLocation() - GetActorLocation();
	PlayerDirection.Normalize();
	auto Angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(TargetingDirection, PlayerDirection)));
	if (Angle < AimSlack)
	{
		Fire();
	}
}

void ATurret::OnDestroy()
{
	Destroy();
}

