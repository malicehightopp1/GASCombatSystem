// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Systems/Combat/GameplayAbilities/MeleeAttack.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Core/Systems/Combat/Character/CombatCharacter.h"
#include "Core/Systems/Combat/Sets/CombatAttributeSet.h"
#include "Core/Systems/Combat/Tags/CombatGameplayTags.h"
#include "Core/Systems/Combat/Weapons/WeaponComponent.h"

UMeleeAttack::UMeleeAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UMeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	const ACombatCharacter* Character = Cast<ACombatCharacter>(ActorInfo->AvatarActor.Get());
	UWeaponComponent* WeaponComp = Character ? Character->GetWeaponComponent() : nullptr;
	UUWeaponData* Weapon = WeaponComp ? WeaponComp->GetCurrentWeapon() : nullptr;

	if (!Weapon || !Weapon->GetAttack(AttackSlot, 0, CurrentAttack))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: equipped weapon has no attack for this slot"), *GetName());
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	const float AttackSpeed = GetAbilitySystemComponentFromActorInfo()->GetNumericAttribute(UCombatAttributeSet::GetAttackSpeedAttribute());
	const float PlayRate = AttackSpeed > 0.f ? AttackSpeed : 1.f;
	
	//Tells the weapon the hitbox
	if (WeaponComp)
	{
		WeaponComp->TraceRadius = CurrentAttack.HitBox.TraceRadius;
	}
	UAbilityTask_PlayMontageAndWait* MontageTask =UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, CurrentAttack.Montage, PlayRate);

	MontageTask->OnCompleted.AddDynamic(this, &UMeleeAttack::OnMontageFinished);
	MontageTask->OnBlendOut.AddDynamic(this, &UMeleeAttack::OnMontageFinished);
	MontageTask->OnInterrupted.AddDynamic(this, &UMeleeAttack::OnMontageCancelled);
	MontageTask->OnCancelled.AddDynamic(this, &UMeleeAttack::OnMontageCancelled);

	MontageTask->ReadyForActivation();
	
	UAbilityTask_WaitGameplayEvent* HitTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent( this, CombatTags::Event_Hit.GetTag(), nullptr, false, true);

	HitTask->EventReceived.AddDynamic(this, &UMeleeAttack::OnHitEvent);
	HitTask->ReadyForActivation();
}

float UMeleeAttack::GetStaminaCost(const FGameplayAbilityActorInfo* ActorInfo) const
{
	const ACombatCharacter* Character = ActorInfo ? Cast<ACombatCharacter>(ActorInfo->AvatarActor.Get()) : nullptr;
	const UWeaponComponent* WeaponComp = Character ? Character->GetWeaponComponent() : nullptr;
	const UUWeaponData* Weapon = WeaponComp ? WeaponComp->GetCurrentWeapon() : nullptr;
	
	//this is deciding the stamina cost per ability
	return Weapon ? Weapon->StaminaCost * CurrentAttack.StaminaCostMultiplier : 0.f;
	
}

void UMeleeAttack::OnMontageFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	UE_LOG(LogTemp, Warning, TEXT("Montage finished"))
}

void UMeleeAttack::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	UE_LOG(LogTemp, Warning, TEXT("Montage Cancelled"))
}

void UMeleeAttack::OnHitEvent(FGameplayEventData Payload)
{
	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Payload.Target.Get());

	UE_LOG(LogTemp, Warning, TEXT("HIT %s"), *GetNameSafe(Payload.Target.Get()));

	if (!SourceASC || !TargetASC || !DamageEffect) return;

	const float WeaponDamage = SourceASC->GetNumericAttribute(UCombatAttributeSet::GetDamageAttribute());
	const float FinalDamage  = WeaponDamage * CurrentAttack.DamageMultiplier;

	UE_LOG(LogTemp, Warning, TEXT("HIT %s | TargetASC=%s | DamageEffect=%s | WeaponDamage=%.1f | Multiplier=%.2f | Final=%.1f"),
	*GetNameSafe(Payload.Target.Get()),
	TargetASC ? TEXT("yes") : TEXT("NO"),
	*GetNameSafe(DamageEffect.Get()),
	WeaponDamage, CurrentAttack.DamageMultiplier, FinalDamage);
	
	FGameplayEffectSpecHandle Spec = MakeOutgoingGameplayEffectSpec(DamageEffect, GetAbilityLevel());
	if (!Spec.IsValid()) return;

	Spec.Data->SetSetByCallerMagnitude(CombatTags::Data_Damage.GetTag(), FinalDamage);
	Spec.Data->SetSetByCallerMagnitude(CombatTags::Data_StanceDamage.GetTag(), CurrentAttack.StanceDamage);

	SourceASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), TargetASC);
	
	// DEBUG: what is the target's health right after the hit?
	UE_LOG(LogTemp, Warning, TEXT("   -> %s Health now %.1f / %.1f"),
		*GetNameSafe(Payload.Target.Get()),
		TargetASC->GetNumericAttribute(UCombatAttributeSet::GetHealthAttribute()),
		TargetASC->GetNumericAttribute(UCombatAttributeSet::GetMaxHealthAttribute()));

}
