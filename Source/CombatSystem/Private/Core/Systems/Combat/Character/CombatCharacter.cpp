// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Systems/Combat/Character/CombatCharacter.h"

#include "AbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Core/Systems/Combat/Sets/CombatAttributeSet.h"
#include "Core/Systems/Combat/Tags/CombatGameplayTags.h"
#include "Core/Systems/Combat/Weapons/WeaponComponent.h"
#include "Core/Systems/UI/EnemyUI/EnemyHealthBarWidget.h"
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
	
	LockOnMarker = CreateDefaultSubobject<UWidgetComponent>(TEXT("LockOnMarker"));
	LockOnMarker->SetupAttachment(GetMesh(), TEXT("spine_03"));
	
	LockOnMarker->SetWidgetSpace(EWidgetSpace::Screen); 
	LockOnMarker->SetDrawSize(FVector2D(24.f, 24.f));
	LockOnMarker->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LockOnMarker->SetVisibility(false);  
	
	HealthBarComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	HealthBarComponent->SetupAttachment(RootComponent);
	
	HealthBarComponent->SetRelativeLocation(FVector(0.f, 0.f, 115.f));
	HealthBarComponent->SetWidgetSpace(EWidgetSpace::Screen); 
	HealthBarComponent->SetDrawSize(FVector2D(120.f, 50.f));
	
	HealthBarComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

bool ACombatCharacter::IsDead() const
{
	return AbilitySystemComponent && AbilitySystemComponent->HasMatchingGameplayTag(CombatTags::State_Dead);
}

bool ACombatCharacter::CanBeTargeted() const
{
	return !IsDead();
}

FVector ACombatCharacter::GetLockOnLocation() const
{
	return GetMesh()->DoesSocketExist(LockOnSocket) ? GetMesh()->GetSocketLocation(LockOnSocket) : GetActorLocation();
}

void ACombatCharacter::SetLockOnMarkerVisible(bool bVisible)
{
	if (LockOnMarker)
	{
		LockOnMarker->SetVisibility(bVisible);
	}
	if (UEnemyHealthBarWidget* Bar = Cast<UEnemyHealthBarWidget>(HealthBarComponent->GetUserWidgetObject()))
	{
		Bar->SetLockedOn(bVisible);
	}
}

void ACombatCharacter::PlayHitFeedback(float Intensity, bool bWasHit)
{
	const float duration = FMath::Clamp(HitStopBase * Intensity, 0.f, HitStopMax);
	if (duration > 0.f)
	{
		CustomTimeDilation = HitStopTimeScale;
		GetWorldTimerManager().SetTimer(HitStopTimer, this, &ACombatCharacter::EndHitStop, duration, false);
	}
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		const TSubclassOf<UCameraShakeBase> Shake = bWasHit ? HitRecievedShake : HitLandedShake;
		if (Shake)
		{
			PC->ClientStartCameraShake(Shake, Intensity);
		}
	}
}

void ACombatCharacter::EndHitStop()
{
	CustomTimeDilation = 1.f;
}

void ACombatCharacter::BeginPlay()
{
	//playing these first before start
	WeaponMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, WeaponSocketName);
	InitAbilitySystem();
	
	Super::BeginPlay();
	
	HealthBarComponent->InitWidget();
	if (UEnemyHealthBarWidget* Bar = Cast<UEnemyHealthBarWidget>(HealthBarComponent->GetUserWidgetObject()))
	{
		Bar->InitFromASC(AbilitySystemComponent);
	}
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

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetMovementSpeedAttribute()) .AddUObject(this, &ACombatCharacter::OnMovementSpeedChanged);

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

	HealthBarComponent->SetVisibility(false);
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

void ACombatCharacter::SetStrafeMode(bool bStrafe)
{
	UCharacterMovementComponent* Move = GetCharacterMovement();
	Move->bOrientRotationToMovement = !bStrafe; 
	Move->bUseControllerDesiredRotation =  bStrafe; 
}
