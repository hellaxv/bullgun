// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"

#include "DrawDebugHelpers.h"
#include "EnemyTank_Pawn.h"
#include "Tank_Pawn.h"
#include "WaypointActor.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

void AEnemyAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if(!EnemyTank_Pawn)
		return;
	if(PatrollingPoints.Num() > 0)
	{
		FVector CurrentPoint = PatrollingPoints[CurrentPatrolPointIndex];
		FVector PawnLocation = GetPawn()->GetActorLocation();
		
		if(FVector::Distance(CurrentPoint,PawnLocation)<MovementAccuracy)
		{
			CurrentPatrolPointIndex++;
			if(CurrentPatrolPointIndex >= PatrollingPoints.Num())
			{
				CurrentPatrolPointIndex=0;
			}
		}
		FVector Direction = CurrentPoint - PawnLocation;
		Direction.Normalize();
	
		auto ForwardVector = EnemyTank_Pawn->GetActorForwardVector();
		auto RightVector = EnemyTank_Pawn->GetActorRightVector();
		float ForwardAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(ForwardVector,Direction)));
		float RightAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct( RightVector, Direction)));
	
		int RotationValue = 0;
		if(ForwardAngle > 5)
		{
			if(ForwardAngle > 45)
			{
				EnemyTank_Pawn->MoveForward(0);
			}
			else
			{
				EnemyTank_Pawn->MoveForward(1);
			}
	
			if(RightAngle > 90)
				RotationValue = -1;
			
			else
				RotationValue = 1;
		}
		if(RotationValue != 0)
		{
			EnemyTank_Pawn->RotateRight(RotationValue);
		}
		else
		{
			EnemyTank_Pawn->RotateRight(0);
		}
	}
	
	if(Target.IsValid())
	{
		
		auto TargetRotation = UKismetMathLibrary::FindLookAtRotation(EnemyTank_Pawn->TurretMesh->GetComponentLocation(),Target->GetActorLocation());
		EnemyTank_Pawn->TurretMesh->SetWorldRotation(FMath::Lerp(EnemyTank_Pawn->TurretMesh->GetComponentRotation(), TargetRotation, TargetingSpeed));
		auto TargetingDirection = EnemyTank_Pawn->TurretMesh->GetForwardVector();
		auto PlayerDirection = Target->GetActorLocation() - EnemyTank_Pawn->GetActorLocation();
		PlayerDirection.Normalize();
		auto Angle=FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(TargetingDirection,PlayerDirection)));
		if(Angle < AimSlack)
		{
			Fire();
		}
	}
	else if (EnemyTank_Pawn)
	{
		auto LookAt = EnemyTank_Pawn->TurretMesh->GetComponentLocation() + EnemyTank_Pawn->GetActorForwardVector() * 100;
		auto TargetRotation = UKismetMathLibrary::FindLookAtRotation(EnemyTank_Pawn->TurretMesh->GetComponentLocation(),LookAt);
		EnemyTank_Pawn->TurretMesh->SetWorldRotation(FMath::Lerp(EnemyTank_Pawn->TurretMesh->GetComponentRotation(), TargetRotation, TargetingSpeed));
		
	}
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	EnemyTank_Pawn = Cast<AEnemyTank_Pawn>(InPawn);
	
	
	if(!EnemyTank_Pawn)
	{
		return;
	}
	
	WaypointTag = EnemyTank_Pawn->WaypointTag;
	
	TArray<AActor*> Actors;
	TArray<AWaypointActor*> Waypoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWaypointActor::StaticClass(),Actors);
	for(auto Actor : Actors)
	{
		auto Waypoint = CastChecked<AWaypointActor>(Actor);
		if(Waypoint->Tags.Contains(WaypointTag))
		Waypoints.Add(Waypoint);
	}
	Waypoints.Sort([] (AWaypointActor& a, const AWaypointActor& b)
	{
		return a.WaypointIndex > b.WaypointIndex;
	});
	for(auto Waypoint : Waypoints)
	{
		PatrollingPoints.Add(Waypoint->GetActorLocation());
	}
	for (int index = 1; index < PatrollingPoints.Num(); index++ )
	{
		DrawDebugLine(GetWorld(),PatrollingPoints[index-1],PatrollingPoints[index],FColor::Green,true);
	}
	if(PatrollingPoints.Num() > 0)
	DrawDebugLine(GetWorld(),PatrollingPoints[0],PatrollingPoints[PatrollingPoints.Num()-1],FColor::Green,true);
}

FVector AEnemyAIController::GetShootTarget() const
{
	
	if (Target.IsValid())
		return Target->GetActorLocation();
	
	if (EnemyTank_Pawn)
		return EnemyTank_Pawn->TurretMesh->GetComponentLocation() + EnemyTank_Pawn->GetActorForwardVector() * 100;
	return FVector::ZeroVector;
	
}

void AEnemyAIController::AddTarget(AActor* Actor)
{
	if (Actor == GetPawn())
		return;
	OtherActors.Add(Actor);
	
	if(!Target.IsValid())
	{
		FindNextTarget();
		
	}
}

void AEnemyAIController::RemoveTarget(AActor* Actor)
{
	OtherActors.Remove(Actor);
	if(Actor == Target)
	{
		FindNextTarget();
	}
}

void AEnemyAIController::FindNextTarget()
{
	if(!EnemyTank_Pawn)
	{
		return;
	}
	float MinRange = 100000;
	Target = nullptr;
	for(auto Actor : OtherActors)
	{
		float Range = FVector::Distance(Actor->GetActorLocation(),EnemyTank_Pawn->GetActorLocation());
		if(Range < MinRange)
		{
			MinRange=Range;
			auto TankPawn = Cast<ATank_Pawn>(Actor);
			if (TankPawn)
			{
				Target = Actor;
			}
		}
	}
}



void AEnemyAIController::Fire()
{
		if(!Target.IsValid())
		{
			return;
		}
		FHitResult Result;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		Params.AddIgnoredActor(EnemyTank_Pawn);
		Params.bTraceComplex = true;

	GEngine->AddOnScreenDebugMessage(1241115, 1, FColor::Purple,FString::Printf(TEXT("BAG Baag")));
				
		if(GetWorld()->LineTraceSingleByChannel(Result,EnemyTank_Pawn->TurretMesh->GetComponentLocation(),Target->GetActorLocation(), ECollisionChannel::ECC_Visibility, Params))
			
		{
			GEngine->AddOnScreenDebugMessage(1241115, 1, FColor::Purple,FString::Printf(TEXT("BAG Baag")));
			if(Result.Actor == Target.Get())
				//GEngine->AddOnScreenDebugMessage(1241115, 1, FColor::Purple,FString::Printf(TEXT("BAG Baag")));
			{
	
			if(Target.IsValid())
				{
					EnemyTank_Pawn->TurretShoot();
				}
			}
		}
	}