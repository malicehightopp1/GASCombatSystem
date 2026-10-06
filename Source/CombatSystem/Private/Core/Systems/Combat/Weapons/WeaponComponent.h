// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "Components/ActorComponent.h"
#include "WeaponComponent.generated.h"

class UUWeaponData;
class UWeaponData;
class UGameplayEffect;
class UAbilitySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponChanged, UUWeaponData*, NewWeapon);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UWeaponComponent();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void EquipWeapon(UUWeaponData* NewWeapon);

	UFUNCTION(BlueprintPure, Category = "Weapon")
	UUWeaponData* GetCurrentWeapon() const { return CurrentWeapon; }

	UFUNCTION(BlueprintPure, Category = "Weapon")
	float GetMoveSpeedForWeight(float InWeight) const;

	//============================================//
	//                Hit detection               //
	//============================================//
	
	void BeginAttackTrace(float Radius);
	void UpdateAttackTrace();
	void EndAttackTrace();

	UPROPERTY(BlueprintAssignable, Category = "Weapon")
	FOnWeaponChanged OnWeaponChanged;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	TSubclassOf<UGameplayEffect> WeaponStatsEffect;

	//============================================//
	//                movement                    //
	//============================================//
	
	UPROPERTY(EditAnywhere, Category = "Weapon|Tuning") float BaseMoveSpeed = 600.f;
	UPROPERTY(EditAnywhere, Category = "Weapon|Tuning") float SpeedLostPerWeight = 8.f;
	UPROPERTY(EditAnywhere, Category = "Weapon|Tuning") float MinMoveSpeed = 250.f;

	UPROPERTY(EditAnywhere, Category = "Weapon|Debug") bool bDrawDebugTraces = false;

private:
	UAbilitySystemComponent* GetOwnerASC() const;
	bool GetBladeLocations(FVector& OutStart, FVector& OutEnd) const;

	UPROPERTY() TObjectPtr<UUWeaponData> CurrentWeapon;
	FActiveGameplayEffectHandle WeaponStatsHandle;

	bool bTracing = false;
	float TraceRadius = 20.f;
	FVector PrevBladeStart = FVector::ZeroVector;
	FVector PrevBladeEnd = FVector::ZeroVector;
	TArray<TWeakObjectPtr<AActor>> HitActorsThisSwing;
};
