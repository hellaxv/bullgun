// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamageTarget.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT2_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

	DECLARE_EVENT(UHealthComponent, FDestroyedEvent);
	DECLARE_EVENT_OneParam(UHealthComponent, FHealthChanged, float);

	/*
	DECLARE_DELEGATE()
	DECLARE_DYNAMIC_DELEGATE()
	DECLARE_MULTICAST_DELEGATE()
	DECLARE_DYNAMIC_MULTICAST_DELEGATE()
	*/
public:
	
	// Sets default values for this component's properties
	UHealthComponent();

	void TakeDamage(FDamageData DamageData);

	float GetHealth() const
	{
		return CurrentHealth;
	}

	FDestroyedEvent OnDestroy;
	FHealthChanged OnHealthChanged;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float MaxHealth = 10;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float CurrentHealth;

	

/*
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
*/
};
