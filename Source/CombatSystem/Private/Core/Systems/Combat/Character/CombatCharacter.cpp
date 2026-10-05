// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Systems/Combat/Character/CombatCharacter.h"

#include "AbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Core/Systems/Combat/Sets/CombatAttributeSet.h"
#include "Core/Systems/Combat/Tags/CombatGameplayTags.h"
#include "Core/Systems/Combat/Weapons/WeaponComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ACombatCharacter::ACombatCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// Created as a subobject of the owner, so the ASC finds it automatically
	AttributeSet = CreateDefaultSubobject<UCombatAttributeSet>(TEXT("AttributeSet"));

	WeaponComponent = CreateDefaultSubobject<UWeaponComponent>(TEXT("WeaponComponent"));

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(GetMesh(), WeaponSocketName);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

bool ACombatCharacter::IsDead() const
{
	return AbilitySystemComponent && AbilitySystemComponent->HasMatchingGameplayTag(CombatTags::State_Dead);
}

void ACombatCharacter::BeginPlay()
{

	//playing these first before start
	WeaponMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, WeaponSocketName);
	InitAbilitySystem();
	
	Super::BeginPlay();
}

void ACombatCharacter::InitAbilitySystem()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	ApplyEffectToSelf(DefaultAttributesEffect);
	for (const TSubclassOf<UGameplayEffect>& Effect : StartupEffects)
	{
		ApplyEffectToSelf(Effect);
	}

	for (const TSubclassOf<UGameplayAbility>& Ability : StartupAbilities)
	{
		if (Ability)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Ability, 1, INDEX_NONE, this));
		}
	}

	// Weapon weight -> MovementSpeed attribute -> CharacterMovement
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetMovementSpeedAttribute())
		.AddUObject(this, &ACombatCharacter::OnMovementSpeedChanged);

	if (StartingWeapon)
	{
		WeaponComponent->EquipWeapon(StartingWeapon);
	}
}

void ACombatCharacter::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> EffectClass)
{
	if (!EffectClass) return;

	FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
	Context.AddSourceObject(this);

	const FGameplayEffectSpecHandle Spec = AbilitySystemComponent->MakeOutgoingSpec(EffectClass, 1.f, Context);
	if (Spec.IsValid())
	{
		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
	}
}

void ACombatCharacter::OnMovementSpeedChanged(const FOnAttributeChangeData& Data)
{
	if (Data.NewValue > 0.f)
	{
		GetCharacterMovement()->MaxWalkSpeed = Data.NewValue;
	}
}

void ACombatCharacter::HandleDeath()
{
	if (IsDead()) return;

	AbilitySystemComponent->AddLooseGameplayTag(CombatTags::State_Dead);
	AbilitySystemComponent->CancelAllAbilities();
	WeaponComponent->EndAttackTrace();

	GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}
	else
	{
		GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
		GetMesh()->SetSimulatePhysics(true);
	}

	BP_OnDeath();
}