// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombatGameplayAbilities.h"
#include "Abilities/GameplayAbility.h"
#include "Core/Systems/Combat/Weapons/UWeaponData.h"
#include "MeleeAttack.generated.h"


UCLASS()
class UMeleeAttack : public UCombatGameplayAbilities
{
	GENERATED_BODY()
	
public:
	UMeleeAttack();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo,const FGameplayTagContainer* SourceTags = nullptr,const FGameplayTagContainer* TargetTags = nullptr,FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo,const FGameplayAbilityActivationInfo ActivationInfo,bool bReplicateEndAbility, bool bWasCancelled) override;
protected:
	//============================================//
	//					Basic setup               //
	//============================================//
	virtual float GetStaminaCost(const FGameplayAbilityActorInfo* ActorInfo) const override;
	UPROPERTY(EditDefaultsOnly, Category = "Attack") EAttackSlot AttackSlot = EAttackSlot::Light;
	
	UPROPERTY(BlueprintReadOnly, Category = "Attack") FAttackData CurrentAttack;
	
	UFUNCTION() void OnMontageFinished();
	UFUNCTION() void OnMontageCancelled();
	
	//============================================//
	//					Damage                    //
	//============================================//
	
	UPROPERTY(EditDefaultsOnly, Category = "Damage") TSubclassOf<UGameplayEffect> DamageEffect;
	UFUNCTION() void OnHitEvent(FGameplayEventData Payload);
	
	//============================================//
	//					Combo                     //
	//============================================//
	
	int32 ComboIndex = 0;
	bool bComboReady = false;
	float ComboReadyTime = -1.0f;
};
