// Fill out your copyright notice in the Description page of Project Settings.

#include "CombatGameplayTags.h"
#include "NativeGameplayTags.h"

namespace CombatTags
{
	UE_DEFINE_GAMEPLAY_TAG(Ability_Attack, "Ability.Attack");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Attack_Light, "Ability.Attack.Light");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Attack_Heavy, "Ability.Attack.Heavy");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Attack_Special1, "Ability.Attack.Special1");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Attack_Special2, "Ability.Attack.Special2");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Attack_Special3, "Ability.Attack.Special3");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Block, "Ability.Block");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Parry, "Ability.Parry");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Dodge, "Ability.Dodge");
	UE_DEFINE_GAMEPLAY_TAG(Ability_HitReact, "Ability.HitReact");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Stagger, "Ability.Stagger");
	
	UE_DEFINE_GAMEPLAY_TAG(State_Attacking, "State.Attacking");
	UE_DEFINE_GAMEPLAY_TAG(State_Blocking, "State.Blocking");
	UE_DEFINE_GAMEPLAY_TAG(State_Sprinting, "State.Sprinting");
	UE_DEFINE_GAMEPLAY_TAG(State_Dodging, "State.Dodging");
	UE_DEFINE_GAMEPLAY_TAG(State_Parrying, "State.Parrying");
	UE_DEFINE_GAMEPLAY_TAG(State_HitStunned, "State.HitStunned");
	UE_DEFINE_GAMEPLAY_TAG(State_Dead, "State.Dead");
	UE_DEFINE_GAMEPLAY_TAG(State_ActionLocked, "State.ActionLocked");
	UE_DEFINE_GAMEPLAY_TAG(State_Invulnerable, "State.Invulnerable");
	UE_DEFINE_GAMEPLAY_TAG(State_HyperArmour, "State.HyperArmour");
	UE_DEFINE_GAMEPLAY_TAG(State_LockedOn, "State.LockedOn");
	UE_DEFINE_GAMEPLAY_TAG(State_ComboWindow, "State.ComboWindow");

	UE_DEFINE_GAMEPLAY_TAG(Event_Hit, "Event.Hit");
	UE_DEFINE_GAMEPLAY_TAG(Event_HitReact, "Event.HitReact");
	UE_DEFINE_GAMEPLAY_TAG(Event_Stagger, "Event.Stagger");
	UE_DEFINE_GAMEPLAY_TAG(Event_Parried, "Event.Parried");
	
	UE_DEFINE_GAMEPLAY_TAG(Data_Damage, "Data.Damage");
	UE_DEFINE_GAMEPLAY_TAG(Data_StanceDamage, "Data.StanceDamage");
	UE_DEFINE_GAMEPLAY_TAG(Data_StaminaCost, "Data.StaminaCost");
	UE_DEFINE_GAMEPLAY_TAG(Data_Weapon_Damage, "Data.Weapon.Damage");
	UE_DEFINE_GAMEPLAY_TAG(Data_Weapon_AttackSpeed, "Data.Weapon.AttackSpeed");
	UE_DEFINE_GAMEPLAY_TAG(Data_Weapon_MoveSpeed, "Data.Weapon.MoveSpeed");
	UE_DEFINE_GAMEPLAY_TAG(Data_Weapon_Weight, "Data.Weapon.Weight");
}
