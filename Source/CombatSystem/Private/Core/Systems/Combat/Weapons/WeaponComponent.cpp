// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Systems/Combat/Weapons/WeaponComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "UWeaponData.h"
#include "Core/Systems/Combat/Character/CombatCharacter.h"
#include "Core/Systems/Combat/Tags/CombatGameplayTags.h"

// Sets default values for this component's properties
UWeaponComponent::UWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}
UAbilitySystemComponent* UWeaponComponent::GetOwnerASC() const
{
	return UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
}

float UWeaponComponent::GetMoveSpeedForWeight(float InWeight) const
{
	return FMath::Max(MinMoveSpeed, BaseMoveSpeed - InWeight * SpeedLostPerWeight);
}

void UWeaponComponent::EquipWeapon(UUWeaponData* NewWeapon)
{
	UAbilitySystemComponent* ASC = GetOwnerASC();
	if (!NewWeapon || !ASC) return;

	// 1) Remove the old weapon's stats
	if (WeaponStatsHandle.IsValid())
	{
		ASC->RemoveActiveGameplayEffect(WeaponStatsHandle);
		WeaponStatsHandle.Invalidate();
	}

	CurrentWeapon = NewWeapon;

	// 2) Swap the visible mesh
	if (ACombatCharacter* Character = Cast<ACombatCharacter>(GetOwner()))
	{
		Character->GetWeaponMesh()->SetStaticMesh(NewWeapon->WeaponMesh);
	}

	// 3) Push the new weapon's stats into the attribute set
	if (WeaponStatsEffect)
	{
		FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
		Context.AddSourceObject(NewWeapon);

		FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(WeaponStatsEffect, 1.f, Context);
		if (Spec.IsValid())
		{
			Spec.Data->SetSetByCallerMagnitude(CombatTags::Data_Weapon_Damage,      NewWeapon->Damage);
			Spec.Data->SetSetByCallerMagnitude(CombatTags::Data_Weapon_AttackSpeed, NewWeapon->AttackSpeed);
			Spec.Data->SetSetByCallerMagnitude(CombatTags::Data_Weapon_Weight,      NewWeapon->Weight);
			Spec.Data->SetSetByCallerMagnitude(CombatTags::Data_Weapon_MoveSpeed,   GetMoveSpeedForWeight(NewWeapon->Weight));
			WeaponStatsHandle = ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
		}
	}

	OnWeaponChanged.Broadcast(NewWeapon);
}

bool UWeaponComponent::GetBladeLocations(FVector& OutStart, FVector& OutEnd) const
{
	const ACombatCharacter* Character = Cast<ACombatCharacter>(GetOwner());
	if (!Character || !CurrentWeapon) return false;

	const FName StartName = CurrentWeapon->TraceStartSocket;
	const FName EndName = CurrentWeapon->TraceEndSocket;

	// Weapon mesh sockets (swords, hammers)
	const UStaticMeshComponent* Weapon = Character->GetWeaponMesh();
	if (Weapon && Weapon->GetStaticMesh() && Weapon->DoesSocketExist(StartName) && Weapon->DoesSocketExist(EndName))
	{
		OutStart = Weapon->GetSocketLocation(StartName);
		OutEnd = Weapon->GetSocketLocation(EndName);
		return true;
	}

	// Character bones/sockets (fists, kicks)
	const USkeletalMeshComponent* Body = Character->GetMesh();
	if (Body && Body->DoesSocketExist(StartName) && Body->DoesSocketExist(EndName))
	{
		OutStart = Body->GetSocketLocation(StartName);
		OutEnd = Body->GetSocketLocation(EndName);
		return true;
	}
	return false;
}

void UWeaponComponent::BeginAttackTrace(float Radius)
{
	TraceRadius = Radius;
	HitActorsThisSwing.Reset();
	bTracing = GetBladeLocations(PrevBladeStart, PrevBladeEnd);
}

void UWeaponComponent::EndAttackTrace()
{
	bTracing = false;
	HitActorsThisSwing.Reset();
}

void UWeaponComponent::UpdateAttackTrace()
{
	if (!bTracing) return;

	FVector BladeStart, BladeEnd;
	if (!GetBladeLocations(BladeStart, BladeEnd)) return;

	UWorld* World = GetWorld();
	AActor* Owner = GetOwner();
	const ACombatCharacter* OwnerChar = Cast<ACombatCharacter>(Owner);

	const FCollisionObjectQueryParams ObjectParams(ECC_Pawn);
	FCollisionQueryParams Params(SCENE_QUERY_STAT(WeaponTrace), false, Owner);
	const FCollisionShape Sphere = FCollisionShape::MakeSphere(TraceRadius);

	TArray<FHitResult> AllHits;
	auto Sweep = [&](const FVector& From, const FVector& To)
	{
		TArray<FHitResult> Hits;
		World->SweepMultiByObjectType(Hits, From, To, FQuat::Identity, ObjectParams, Sphere, Params);
		AllHits.Append(Hits);
	};

	// Along the blade now, plus tip & middle from last frame to this frame (fast swings don't skip)
	Sweep(BladeStart, BladeEnd);
	Sweep(PrevBladeEnd, BladeEnd);
	Sweep((PrevBladeStart + PrevBladeEnd) * 0.5f, (BladeStart + BladeEnd) * 0.5f);

	for (const FHitResult& Hit : AllHits)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor || HitActor == Owner || HitActorsThisSwing.Contains(HitActor)) continue;
		if (!UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(HitActor)) continue;

		if (const ACombatCharacter* HitChar = Cast<ACombatCharacter>(HitActor))
		{
			if (HitChar->IsDead()) continue;
			if (OwnerChar && HitChar->GetTeamId() == OwnerChar->GetTeamId()) continue; // no friendly fire
		}

		HitActorsThisSwing.Add(HitActor);

		FGameplayEventData Payload;
		Payload.EventTag = CombatTags::Event_Hit;
		Payload.Instigator = Owner;
		Payload.Target = HitActor;
		Payload.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromHitResult(Hit);
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, CombatTags::Event_Hit, Payload);
	}

	if (bDrawDebugTraces)
	{
		DrawDebugLine(World, BladeStart, BladeEnd, FColor::Red, false, 1.f, 0, 2.f);
		DrawDebugSphere(World, BladeEnd, TraceRadius, 8, FColor::Orange, false, 1.f);
	}

	PrevBladeStart = BladeStart;
	PrevBladeEnd = BladeEnd;
}
