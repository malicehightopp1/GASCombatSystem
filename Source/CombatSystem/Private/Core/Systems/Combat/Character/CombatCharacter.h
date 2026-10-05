// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "CombatCharacter.generated.h"

class UGameplayAbility;
class UUWeaponData;
struct FOnAttributeChangeData;
class UGameplayEffect;
class UWeaponComponent;
class UCombatAttributeSet;

UCLASS()
class ACombatCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACombatCharacter();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }

	UFUNCTION(BlueprintPure, Category = "Combat") UCombatAttributeSet* GetCombatAttributes() const { return AttributeSet; }
	UFUNCTION(BlueprintPure, Category = "Combat") UWeaponComponent* GetWeaponComponent() const { return WeaponComponent; }
	UFUNCTION(BlueprintPure, Category = "Combat") UStaticMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	UFUNCTION(BlueprintPure, Category = "Combat") uint8 GetTeamId() const { return TeamId; }
	UFUNCTION(BlueprintPure, Category = "Combat") bool IsDead() const;

	/** Called by the attribute set when Health reaches 0. */
	virtual void HandleDeath();

protected:
	virtual void BeginPlay() override;
	virtual void InitAbilitySystem();
	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> EffectClass);
	void OnMovementSpeedChanged(const FOnAttributeChangeData& Data);

	/** Blueprint hook: death FX, loot drop, stopping AI, respawn UI... */
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat", meta = (DisplayName = "On Death")) void BP_OnDeath();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat") TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY() TObjectPtr<UCombatAttributeSet> AttributeSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat") TObjectPtr<UWeaponComponent> WeaponComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat") TObjectPtr<UStaticMeshComponent> WeaponMesh;

	/** Instant GE that sets Health/Stamina/Stance/Defense base values. */
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Setup") TSubclassOf<UGameplayEffect> DefaultAttributesEffect;

	/** Applied once at start (stamina regen, stance regen...). */
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Setup") TArray<TSubclassOf<UGameplayEffect>> StartupEffects;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Setup") TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	/** Every character needs one, use DA_Unarmed for fists. */
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Setup") TObjectPtr<UUWeaponData> StartingWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Setup") FName WeaponSocketName = TEXT("weapon_r");

	/** 0 = player, 1 = enemies. Same team can't hit each other. */
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Setup") uint8 TeamId = 0;

	/** Optional. If empty the character ragdolls. Turn OFF "Enable Auto Blend Out" on this montage. */
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Death") TObjectPtr<UAnimMontage> DeathMontage;
};
