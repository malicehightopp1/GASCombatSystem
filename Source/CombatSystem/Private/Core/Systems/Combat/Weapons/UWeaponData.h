// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UWeaponData.generated.h"

class UAnimMontage;

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
	
	// ---- Info ----
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Info") FText WeaponName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Info") EWeaponType WeaponType = EWeaponType::StraightSword;

	/** Leave empty for Unarmed. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Info") TObjectPtr<UStaticMesh> WeaponMesh = nullptr;

	// ---- Stats (pushed into the attribute set on equip) ----
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats", meta = (ClampMin = "0")) float Damage = 50.f;

	/** Drives movement speed (see UWeaponComponent tuning). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats", meta = (ClampMin = "0")) float Weight = 5.f;

	/** Montage play rate. 1 = normal, 1.3 = fast dagger, 0.75 = slow hammer. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats", meta = (ClampMin = "0.1")) float AttackSpeed = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats", meta = (ClampMin = "0")) float StaminaCost = 15.f;

	// ---- Hit detection ----
	/** Socket names looked up on the weapon mesh first, then on the character mesh (fists). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hitbox") FName TraceStartSocket = TEXT("TraceStart");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hitbox") FName TraceEndSocket = TEXT("TraceEnd");

	// ---- Moveset ----
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Moveset") TArray<FAttackData> LightAttacks;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Moveset") TArray<FAttackData> HeavyAttacks;

	/** Exactly 3 entries: Special 1, 2, 3. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Moveset", meta = (EditFixedSize)) TArray<FAttackData> SpecialAttacks;

	/** Gets the attack for a slot. ComboIndex wraps around the chain for Light/Heavy. */
	UFUNCTION(BlueprintCallable, Category = "Weapon") bool GetAttack(EAttackSlot Slot, int32 ComboIndex, FAttackData& OutAttack) const;

	UFUNCTION(BlueprintPure, Category = "Weapon")int32 GetComboLength(EAttackSlot Slot) const;
};
