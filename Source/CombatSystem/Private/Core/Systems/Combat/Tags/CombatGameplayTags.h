// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NativeGameplayTags.h"

namespace Combat
{
	//Base attacks - main focus
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Attack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Attack_Light);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Attack_Heavy);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Attack_Special1);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Attack_Special2);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Attack_Special3);
	
	//secondary abilities - work on second
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Block);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Parry);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Dodge);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_HitReact);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Stagger);
	
	//Definind the states needed for interupting - During action tags
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Attacking);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Blocking);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Dodging);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Parrying);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Staggering);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Dead);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_ActionLocked);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Invulnerable); //TBD - If i want to have a few seconds of inv at spawn
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_HyperArmour); //if they have this active wont flinch or have a stance break
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_LockedOn);
	
	//Events
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Hit);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Stagger);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_HitReact);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Parried);
	
	//Caller data
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Damage);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_StanceDamage);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_StaminaCost);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Weapon_Damage);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Weapon_AttackSpeed);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Weapon_MoveSpeed);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Weapon_Weight);











	
}