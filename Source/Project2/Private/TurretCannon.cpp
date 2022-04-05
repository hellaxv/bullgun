// Fill out your copyright notice in the Description page of Project Settings.


#include "TurretCannon.h"

#include "DamageTarget.h"
#include "DrawDebugHelpers.h"


// Sets default values
ATurretCannon::ATurretCannon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>("RootComponent");

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(RootComponent);

	ProjectileSpawnPoint = CreateDefaultSubobject<UArrowComponent>("ProjectileSpawnPoint");
	ProjectileSpawnPoint->SetupAttachment(RootComponent);

}

void ATurretCannon::TurretShoot(int& AmmoTurret)
    {
		if(!bReadyToShootTurret)
			return;
	
		switch (Type1)
		{
		case ETurretCannonType::FireProjectile1:
			{
				GEngine->AddOnScreenDebugMessage(-1, 1,FColor::Red,FString(TEXT("")));
				GetWorld()->SpawnActor<AProjectile>(ProjectileClass,ProjectileSpawnPoint->GetComponentLocation(),ProjectileSpawnPoint->GetComponentRotation());
				AmmoTurret--;
				//GEngine->AddOnScreenDebugMessage(-1234, 1, FColor::Blue, FString::Printf(TEXT("AmmoII - %i"), AmmoII));
				break;
			}
	
		case ETurretCannonType::FireTrace1:
			{
				GEngine->AddOnScreenDebugMessage(-1, 1,FColor::Green,FString(TEXT("")));
				FHitResult Result;
				FCollisionObjectQueryParams Params;
				Params.AddObjectTypesToQuery(ECollisionChannel::ECC_Vehicle);
				Params.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldDynamic);
				Params.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic);
		
				auto Start = ProjectileSpawnPoint->GetComponentLocation();
				auto End = Start + ProjectileSpawnPoint->GetForwardVector() * FireRange1;
				bool HasHit = GetWorld()->LineTraceSingleByObjectType(
					Result, Start, End, Params);
		
				if(HasHit)
				{
					End = Result.Location;
					if(Result.Actor.IsValid())
					{
						auto Target = Cast<IDamageTarget>(Result.Actor);
						if(Target)
						{
							FDamageData DamageData;
							DamageData.Instigator = this;
							DamageData.DamageValue = LaserDamage1;
							Target->TakeDamage(DamageData);
						}
					}
				}
				DrawDebugLine(GetWorld(),Start, End, FColor::Turquoise,false, 0.1, 0, 10);
				break;
			}
		}
			bReadyToShootTurret = false;
	        GetWorld()->GetTimerManager().SetTimer(TurretTimerHandle, FTimerDelegate::CreateUObject(this, &ATurretCannon::ResetShootStateTurret), 1 / FireRate1, false);
    }


void ATurretCannon::ResetShootStateTurret()
{
	
	bReadyToShootTurret = true;
}

// Called when the game starts or when spawned
void ATurretCannon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATurretCannon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}



