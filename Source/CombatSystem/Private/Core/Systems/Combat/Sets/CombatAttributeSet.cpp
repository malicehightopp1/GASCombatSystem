// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Systems/Combat/Sets/CombatAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectExtension.h"
#include "Core/Systems/Combat/Character/CombatCharacter.h"
#include "Core/Systems/Combat/Tags/CombatGameplayTags.h"

namespace CombatTuning
{
	constexpr float FrontDotThreshold = 0.5f;
	constexpr float BlockDamageAbsorb = 0.8f;
	constexpr float BlockStaminaPerSwing = 0.5;
	constexpr float CounterHitMultiplier = 1.5f;
	constexpr float DefenseScale = 100.0f;
	constexpr float StaggeredDamageBonus  = 1.5f; 
	constexpr float BlockStaminaPerDamage = 1.0f; 
	constexpr float BlockStanceMultiplier = 0.5f;  
}


static void SendCombatEvent(AActor* Target, const FGameplayTag& Tag, AActor* Instigator)
{
	if (!Target) return;
	FGameplayEventData Payload;
	Payload.EventTag = Tag;
	Payload.Instigator = Instigator;
	Payload.Target = Target;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Target, Tag, Payload);
}
void UCombatAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	ClampAttributes(Attribute, NewValue);
}

void UCombatAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	ClampAttributes(Attribute, NewValue);
}

void UCombatAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& EffectData)
{
	Super::PostGameplayEffectExecute(EffectData);
	
	if (EffectData.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		const float RawDamage = GetIncomingDamage();
		SetIncomingDamage(0.0f);
		if (RawDamage > 0.0f)
		{
			HandleIncomingDamage(EffectData, RawDamage);
		}
	}
}

void UCombatAttributeSet::ClampAttributes(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	else if (Attribute == GetStaminaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxStamina());
	}
	else if (Attribute == GetStanceAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxStance());
	}
	else if (Attribute == GetDefenseAttribute() || Attribute == GetDamageAttribute() || Attribute == GetAttackSpeedAttribute() || Attribute == GetMovementSpeedAttribute() || Attribute == GetWeightAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.0f);
	}
}

void UCombatAttributeSet::HandleIncomingDamage(const FGameplayEffectModCallbackData& Data, float RawDamage)
{

	float IncomingDamageForCharacter = RawDamage;

    UAbilitySystemComponent* TargetASC = &Data.Target;
    AActor* TargetActor = TargetASC->GetAvatarActor();
    UAbilitySystemComponent* SourceASC = Data.EffectSpec.GetContext().GetOriginalInstigatorAbilitySystemComponent();
    AActor* SourceActor = SourceASC ? SourceASC->GetAvatarActor() : nullptr;

    if (TargetASC->HasMatchingGameplayTag(CombatTags::State_Dead)) return;

    float StanceDamage = Data.EffectSpec.GetSetByCallerMagnitude(CombatTags::Data_StanceDamage, false, 0.f);

    const bool bFromFront = IsAttackerInFront(TargetActor, SourceActor);
    const bool bWasStaggered = TargetASC->HasMatchingGameplayTag(CombatTags::State_Staggered);

    if (bFromFront && TargetASC->HasMatchingGameplayTag(CombatTags::State_Parrying))
    {
        SendCombatEvent(SourceActor, CombatTags::Event_Stagger, TargetActor);
        SendCombatEvent(TargetActor, CombatTags::Event_Parried, SourceActor); // for feedback later
        return;
    }

    if (bWasStaggered) IncomingDamageForCharacter *= CombatTuning::StaggeredDamageBonus;

    bool bBlocked = false, bGuardBroken = false;
    if (bFromFront && TargetASC->HasMatchingGameplayTag(CombatTags::State_Blocking))
    {
        const float StaminaCost = IncomingDamageForCharacter * CombatTuning::BlockStaminaPerDamage;
        if (GetStamina() >= StaminaCost)
        {
            SetStamina(GetStamina() - StaminaCost);
            IncomingDamageForCharacter *= (1.f - CombatTuning::BlockDamageAbsorb);
            StanceDamage *= CombatTuning::BlockStanceMultiplier;
            bBlocked = true;
        }
        else
        {
            SetStamina(0.f);
            bGuardBroken = true;
        }
    }

    const float FinalDamage = FMath::Max(IncomingDamageForCharacter - GetDefense(), 1.f);
    SetHealth(FMath::Clamp(GetHealth() - FinalDamage, 0.f, GetMaxHealth()));

    if (GetHealth() <= 0.f)
    {
        if (ACombatCharacter* Character = Cast<ACombatCharacter>(TargetActor))
            Character->HandleDeath();
        return;
    }

    if (!bWasStaggered)
        SetStance(FMath::Clamp(GetStance() - StanceDamage, 0.f, GetMaxStance()));

    if (!bWasStaggered && (bGuardBroken || GetStance() <= 0.f))
    {
        SetStance(GetMaxStance());
        SendCombatEvent(TargetActor, CombatTags::Event_Stagger, SourceActor);
    }
    else if (!bBlocked && !bWasStaggered && !TargetASC->HasMatchingGameplayTag(CombatTags::State_HyperArmour))
    {
        SendCombatEvent(TargetActor, CombatTags::Event_HitReact, SourceActor);
    }
}
