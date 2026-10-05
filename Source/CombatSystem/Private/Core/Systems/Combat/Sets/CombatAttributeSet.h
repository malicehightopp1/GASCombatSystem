// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "CombatAttributeSet.generated.h"

#define COMBAT_ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName) \
	

UCLASS()
class UCombatAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
	
	//============================================//
	//                 Stats                      //
	//============================================//
public:
	
	//Health
	UPROPERTY(BlueprintReadOnly, Category = "Player | Stats | Health") FGameplayAttributeData Health;
	COMBAT_ATTRIBUTE_ACCESSORS(UCombatAttributeSet, Health)
	
	UPROPERTY(BlueprintReadOnly, Category = "Player | Stats | Health") FGameplayAttributeData MaxHealth;
	COMBAT_ATTRIBUTE_ACCESSORS(UCombatAttributeSet, MaxHealth)
	
	//Stamina
	UPROPERTY(BlueprintReadOnly, Category = "Player | Stats | Stamina") FGameplayAttributeData Stamina;
	COMBAT_ATTRIBUTE_ACCESSORS(UCombatAttributeSet, Stamina)

	UPROPERTY(BlueprintReadOnly, Category = "Player | Stats | Stamina") FGameplayAttributeData MaxStamina;
	COMBAT_ATTRIBUTE_ACCESSORS(UCombatAttributeSet, MaxStamina)
	
	UPROPERTY(BlueprintReadOnly, Category = "Player | Stats | Stamina") FGameplayAttributeData Stance;
	COMBAT_ATTRIBUTE_ACCESSORS(UCombatAttributeSet, Stance)

	UPROPERTY(BlueprintReadOnly, Category = "Player | Stats | Stamina") FGameplayAttributeData MaxStance;
	COMBAT_ATTRIBUTE_ACCESSORS(UCombatAttributeSet, MaxStance)
	
	//Defense
	UPROPERTY(BlueprintReadOnly, Category = "Player | Stats | Defense") FGameplayAttributeData Defense;
	COMBAT_ATTRIBUTE_ACCESSORS(UCombatAttributeSet, Defense)

	//============================================//
	//                 WeaponValues               //
	//============================================//
	
	UPROPERTY(BlueprintReadOnly, Category = "Player | Weapon") FGameplayAttributeData Damage;
	COMBAT_ATTRIBUTE_ACCESSORS(UCombatAttributeSet, Damage)
	
	UPROPERTY(BlueprintReadOnly, Category = "Player | Weapon") FGameplayAttributeData AttackSpeed;
	COMBAT_ATTRIBUTE_ACCESSORS(UCombatAttributeSet, AttackSpeed)
	
	UPROPERTY(BlueprintReadOnly, Category = "Player | Weapon") FGameplayAttributeData MovementSpeed;
	COMBAT_ATTRIBUTE_ACCESSORS(UCombatAttributeSet, MovementSpeed)
	
	UPROPERTY(BlueprintReadOnly, Category = "Player | Weapon") FGameplayAttributeData Weight;
	COMBAT_ATTRIBUTE_ACCESSORS(UCombatAttributeSet, Weight)
	
	UPROPERTY(BlueprintReadOnly, Category = "Player | Meta") FGameplayAttributeData IncomingDamage;
	COMBAT_ATTRIBUTE_ACCESSORS(UCombatAttributeSet, IncomingDamage)
	
	//============================================//
	//                 Functions				  //
	//============================================//
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& EffectData) override;
	
private:
	void ClampAttributes(const FGameplayAttribute& Attribute, float& NewValue) const;
	void HandleIncomingDamage(const FGameplayEffectModCallbackData& Data, float RawDamage);
};

