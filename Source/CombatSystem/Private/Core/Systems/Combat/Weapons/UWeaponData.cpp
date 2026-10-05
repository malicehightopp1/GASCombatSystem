// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Systems/Combat/Weapons/UWeaponData.h"

UUWeaponData::UUWeaponData()
{
	SpecialAttacks.SetNum(3);
}

bool UUWeaponData::GetAttack(EAttackSlot Slot, int32 ComboIndex, FAttackData& OutAttack) const
{
	const FAttackData* Found = nullptr;
	ComboIndex = FMath::Max(0, ComboIndex);

	switch (Slot)
	{
		case EAttackSlot::Light:
			if (LightAttacks.Num() > 0)
			{
				Found = &LightAttacks[ComboIndex % LightAttacks.Num()];
			}
		break;
		case EAttackSlot::Heavy:
			if (HeavyAttacks.Num() > 0)
			{
				Found = &HeavyAttacks[ComboIndex % HeavyAttacks.Num()];
			}
		break;
		case EAttackSlot::Special1:
		case EAttackSlot::Special2:
		case EAttackSlot::Special3:
			{
				const int32 Index = static_cast<int32>(Slot) - static_cast<int32>(EAttackSlot::Special1);
				if (SpecialAttacks.IsValidIndex(Index))
				{
					Found = &SpecialAttacks[Index];
				}
				break;
			}
	}

	if (Found && Found->Montage)
	{
		OutAttack = *Found;
		return true;
	}
	return false;
}

int32 UUWeaponData::GetComboLength(EAttackSlot Slot) const
{
	switch (Slot)
	{
		case EAttackSlot::Light: 
			return FMath::Max(1, LightAttacks.Num());
		case EAttackSlot::Heavy: 
			return FMath::Max(1, HeavyAttacks.Num());
		
		default:                 
			return 1;
	}
}