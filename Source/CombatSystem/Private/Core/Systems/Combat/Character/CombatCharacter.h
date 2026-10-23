// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Core/Systems/Combat/Targeting/Targetable.h"
#include "GameFramework/Character.h"
#include "CombatCharacter.generated.h"

class UWidgetComponent;
class UGameplayAbility;
class UUWeaponData;
struct FOnAttributeChangeData;
class UGameplayEffect;
class UWeaponComponent;
class UCombatAttributeSet;

UCLASS()
class ACombatCharacter : public ACharacter, public IAbilitySystemInterface, public ITargetable
{
	GENERATED_BODY()

public:
	ACombatCharacter();

	//============================================//
	//					Getters                   //
	//============================================//
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }

	UFUNCTION(BlueprintPure, Category = "Combat") UCombatAttributeSet* GetCombatAttributes() const { return AttributeSet; }
	UFUNCTION(BlueprintPure, Category = "Combat") UWeaponComponent* GetWeaponComponent() const { return WeaponComponent; }
	UFUNCTION(BlueprintPure, Category = "Combat") UStaticMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	UFUNCTION(BlueprintPure, Category = "Combat") uint8 GetTeamId() const { return TeamId; }
	
	
	//============================================//
	//					Player death              //
	//============================================//
	UFUNCTION(BlueprintPure, Category = "Combat") bool IsDead() const;
	virtual void HandleDeath();

	//For targeting locking
	UFUNCTION(BlueprintCallable, Category = "Combat|Movement") void SetStrafeMode(bool bStrafe);
	
	// ITargetable
	virtual bool CanBeTargeted() const override;
	virtual FVector GetLockOnLocation() const override;
	virtual void SetLockOnMarkerVisible(bool bVisible) override;
	
protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Targeting") TObjectPtr<UWidgetComponent> LockOnMarker;
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Targeting") FName LockOnSocket = TEXT("spine_03");
	
	
	virtual void BeginPlay() override;
	
	virtual void InitAbilitySystem();
	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> EffectClass);
	void OnMovementSpeedChanged(const FOnAttributeChangeData& Data);

	//============================================//
	//				player ability setup          //
	//============================================//
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat") TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY() TObjectPtr<UCombatAttributeSet> AttributeSet;

	//============================================//
	//					Weapon stats              //
	//============================================//
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat") TObjectPtr<UWeaponComponent> WeaponComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat") TObjectPtr<UStaticMeshComponent> WeaponMesh;
	UPROPERTY(EditAnywhere, Category = "Combat|Setup") TObjectPtr<UUWeaponData> StartingWeapon;
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Setup") FName WeaponSocketName = TEXT("weapon_r");

	//============================================//
	//				Gameplay effects              //
	//============================================//
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Setup") TSubclassOf<UGameplayEffect> DefaultAttributesEffect;
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Setup") TArray<TSubclassOf<UGameplayEffect>> StartupEffects;
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Setup") TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;
	

	//============================================//
	//					Player death              //
	//============================================//
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat", meta = (DisplayName = "On Death")) void BP_OnDeath();
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Death") TObjectPtr<UAnimMontage> DeathMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Setup") uint8 TeamId = 0;
};
