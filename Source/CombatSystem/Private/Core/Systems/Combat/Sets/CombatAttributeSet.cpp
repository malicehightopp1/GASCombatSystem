// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Systems/Combat/Sets/CombatAttributeSet.h"

#include "GameplayEffectExtension.h"

namespace CombatTuning
{
	constexpr float FrontDotThreshold = 0.5f;
	constexpr float BlockDamageAbsorb = 0.8f;
	constexpr float BlockStaminaPerSwing = 0.5;
	constexpr float CounterHitMultiplier = 1.5f;
	constexpr float DefenseScale = 100.0f;
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

}
