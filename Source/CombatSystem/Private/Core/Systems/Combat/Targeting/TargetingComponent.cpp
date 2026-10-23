// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Systems/Combat/Targeting/TargetingComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Targetable.h"
#include "Core/Systems/Combat/Character/CombatCharacter.h"
#include "Core/Systems/Combat/Tags/CombatGameplayTags.h"
#include "Engine/OverlapResult.h"

UTargetingComponent::UTargetingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;   // only tick while locked on
}

bool UTargetingComponent::GetCameraView(FVector& OutLocation, FRotator& OutRotation) const
{
	const APawn* OwnerPawn = Cast<APawn>(GetOwner());
	const APlayerController* PC = OwnerPawn ? Cast<APlayerController>(OwnerPawn->GetController()) : nullptr;
	if (!PC || !PC->PlayerCameraManager) return false;

	OutLocation = PC->PlayerCameraManager->GetCameraLocation();
	OutRotation = PC->PlayerCameraManager->GetCameraRotation();
	return true;
}

bool UTargetingComponent::IsValidTarget(AActor* Target) const
{
	if (!IsValid(Target) || Target == GetOwner()) return false;

	const ITargetable* Targetable = Cast<ITargetable>(Target);
	if (!Targetable || !Targetable->CanBeTargeted()) return false;

	const ACombatCharacter* Me   = Cast<ACombatCharacter>(GetOwner());
	const ACombatCharacter* Them = Cast<ACombatCharacter>(Target);
	if (Me && Them && Me->GetTeamId() == Them->GetTeamId()) return false;

	return true;
}

void UTargetingComponent::GatherCandidates(TArray<AActor*>& OutCandidates) const
{
	AActor* Owner = GetOwner();
	UWorld* World = GetWorld();
	FVector CamLoc; FRotator CamRot;
	if (!Owner || !World || !GetCameraView(CamLoc, CamRot)) return;

	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(LockOnSphere), false, Owner);
	World->OverlapMultiByObjectType(Overlaps, Owner->GetActorLocation(), FQuat::Identity,
		FCollisionObjectQueryParams(ECC_Pawn), FCollisionShape::MakeSphere(MaxLockDistance), Params);

	for (const FOverlapResult& Overlap : Overlaps)
	{
		AActor* Actor = Overlap.GetActor();
		if (!IsValidTarget(Actor)) continue;

		const FVector TargetLoc = Cast<ITargetable>(Actor)->GetLockOnLocation();
		FCollisionQueryParams SightParams(SCENE_QUERY_STAT(LockOnSight), false, Owner);
		SightParams.AddIgnoredActor(Actor);

		FHitResult Hit;
		if (!World->LineTraceSingleByChannel(Hit, CamLoc, TargetLoc, ECC_Visibility, SightParams))
		{
			OutCandidates.AddUnique(Actor);
		}
	}
}

AActor* UTargetingComponent::FindBestTarget() const
{
	TArray<AActor*> Candidates;
	GatherCandidates(Candidates);

	FVector CamLoc; FRotator CamRot;
	if (Candidates.Num() == 0 || !GetCameraView(CamLoc, CamRot)) return nullptr;

	const FVector CamForward = CamRot.Vector().GetSafeNormal2D();
	const float ConeCos = FMath::Cos(FMath::DegreesToRadians(PriorityConeAngle));

	AActor* Best = nullptr;
	float BestScore = -FLT_MAX;

	for (AActor* Candidate : Candidates)
	{
		const FVector ToTarget = Cast<ITargetable>(Candidate)->GetLockOnLocation() - GetOwner()->GetActorLocation();
		const float Distance = ToTarget.Size();

		const float Facing = FVector::DotProduct(CamForward, ToTarget.GetSafeNormal2D());

		float Score = (1.f - Distance / MaxLockDistance) + Facing;

		if (Facing >= ConeCos)
		{
			Score += 2.f;
		}

		if (Score > BestScore)
		{
			BestScore = Score;
			Best = Candidate;
		}
	}
	return Best;
}

void UTargetingComponent::ToggleLockOn()
{
	if (IsLockedOn())
	{
		ClearTarget();
	}
	else if (AActor* Target = FindBestTarget())
	{
		LockOnTo(Target);
	}
}

void UTargetingComponent::LockOnTo(AActor* NewTarget)
{
	if (ITargetable* Old = Cast<ITargetable>(CurrentTarget.Get()))
	{
		Old->SetLockOnMarkerVisible(false);
	}

	CurrentTarget = NewTarget;
	Cast<ITargetable>(NewTarget)->SetLockOnMarkerVisible(true);

	if (ACombatCharacter* Me = Cast<ACombatCharacter>(GetOwner()))
	{
		Me->SetStrafeMode(true);
	}

	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()))
	{
		ASC->SetLooseGameplayTagCount(CombatTags::State_LockedOn.GetTag(), 1);
	}

	SetComponentTickEnabled(true);
}

void UTargetingComponent::ClearTarget()
{
	if (IsValid(CurrentTarget))
	{
		if (ITargetable* Old = Cast<ITargetable>(CurrentTarget.Get()))
		{
			Old->SetLockOnMarkerVisible(false);
		}
	}
	CurrentTarget = nullptr;

	// Back to normal free-roam movement and camera
	if (ACombatCharacter* Me = Cast<ACombatCharacter>(GetOwner()))
	{
		Me->SetStrafeMode(false);
	}
	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()))
	{
		ASC->SetLooseGameplayTagCount(CombatTags::State_LockedOn.GetTag(), 0);
	}

	SetComponentTickEnabled(false);
}


void UTargetingComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AActor* Target = CurrentTarget.Get();
	const bool bTooFar = IsValid(Target) &&
		FVector::Dist(Target->GetActorLocation(), GetOwner()->GetActorLocation()) > BreakLockDistance;

	if (!IsValidTarget(Target) || bTooFar)
	{
		AActor* Next = bTooFar ? nullptr : FindBestTarget();
		if (Next) { LockOnTo(Next); } else { ClearTarget(); }
		return;
	}

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	AController* Controller = OwnerPawn ? OwnerPawn->GetController() : nullptr;
	FVector CamLoc; FRotator CamRot;
	if (!Controller || !GetCameraView(CamLoc, CamRot)) return;

	FRotator Desired = (Cast<ITargetable>(Target)->GetLockOnLocation() - CamLoc).Rotation();
	Desired.Pitch = FMath::Clamp(Desired.Pitch + CameraPitchOffset, -40.f, 20.f);
	Desired.Roll = 0.f;

	Controller->SetControlRotation(
		FMath::RInterpTo(Controller->GetControlRotation(), Desired, DeltaTime, CameraInterpSpeed));
}

void UTargetingComponent::HandleSwitchInput(float AxisX)
{
	if (!IsLockedOn() || FMath::Abs(AxisX) < SwitchInputThreshold) return;

	const float Now = GetWorld()->GetTimeSeconds();
	if (Now - LastSwitchTime < SwitchCooldown) return;

	LastSwitchTime = Now;
	SwitchTarget(FMath::Sign(AxisX)); 
}

void UTargetingComponent::SwitchTarget(float Direction)
{
	FVector CamLoc; FRotator CamRot;
	if (!IsValid(CurrentTarget) || !GetCameraView(CamLoc, CamRot)) return;

	auto ScreenYaw = [&](AActor* Actor)
	{
		const FVector Loc = Cast<ITargetable>(Actor)->GetLockOnLocation();
		return FMath::FindDeltaAngleDegrees(CamRot.Yaw, (Loc - CamLoc).Rotation().Yaw);
	};

	const float CurrentYaw = ScreenYaw(CurrentTarget.Get());

	TArray<AActor*> Candidates;
	GatherCandidates(Candidates);

	AActor* Best = nullptr;
	float BestGap = FLT_MAX;

	for (AActor* Candidate : Candidates)
	{
		if (Candidate == CurrentTarget) continue;

		const float Gap = ScreenYaw(Candidate) - CurrentYaw; 
		if (FMath::Sign(Gap) != Direction) continue;   

		if (FMath::Abs(Gap) < BestGap)   
		{
			BestGap = FMath::Abs(Gap);
			Best = Candidate;
		}
	}

	if (Best)
	{
		LockOnTo(Best);
	}
}
