// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UWeaponData.generated.h"

class UNiagaraSystem;
class UAnimMontage;
class USoundBase;

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Unarmed, Dagger, StraightSword, GreatSword, Hammer, Spear
};
UENUM(BlueprintType)
enum class EAttackSlot : uint8
{
	Light, Heavy, Special1, Special2, Special3
};

USTRUCT(BlueprintType)
struct FHitBoxData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HitBox", meta = (ClampMin = "1")) float TraceRadius = 20.0f;
};
USTRUCT(BlueprintType)
struct FAttackData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack") TObjectPtr<UAnimMontage> Montage = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack", meta = (ClampMin = "0")) float DamageMultiplier = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack", meta = (ClampMin = "0")) float StanceDamage = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack",  meta = (ClampMin = "0")) float StaminaCostMultiplier = 1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack") FHitBoxData HitBox;	
};
UCLASS()
class UUWeaponData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
	
public:
	UUWeaponData();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Feedback")TObjectPtr<UNiagaraSystem> TrailEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Feedback")TObjectPtr<UNiagaraSystem> HitEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Feedback")TObjectPtr<USoundBase> HitSound;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Info") FText WeaponName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Info") TObjectPtr<UTexture2D> Icon;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Info") EWeaponType WeaponType = EWeaponType::StraightSword;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Info") TObjectPtr<UStaticMesh> WeaponMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats", meta = (ClampMin = "0")) float Damage = 50.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats", meta = (ClampMin = "0")) float Weight = 5.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats", meta = (ClampMin = "0.1")) float AttackSpeed = 1.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats", meta = (ClampMin = "0")) float StaminaCost = 15.f;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hitbox") FName TraceStartSocket = TEXT("TraceStart");
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hitbox") FName TraceEndSocket = TEXT("TraceEnd");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Moveset") TArray<FAttackData> LightAttacks;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Moveset") TArray<FAttackData> HeavyAttacks;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Moveset", meta = (EditFixedSize)) TArray<FAttackData> SpecialAttacks;

	UFUNCTION(BlueprintCallable, Category = "Weapon") bool GetAttack(EAttackSlot Slot, int32 ComboIndex, FAttackData& OutAttack) const;
	UFUNCTION(BlueprintPure, Category = "Weapon")int32 GetComboLength(EAttackSlot Slot) const;
};
