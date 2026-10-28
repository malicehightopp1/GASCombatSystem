// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Systems/Combat/Character/PlayerCombatCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Core/Systems/Combat/Tags/CombatGameplayTags.h"
#include "Core/Systems/Combat/Targeting/TargetingComponent.h"
#include "Core/Systems/Combat/Weapons/WeaponComponent.h"
#include "Core/Systems/UI/PlayerUI/PlayerHUDWidget.h"
#include "Core/Systems/UI/PlayerUI/WeaponSelectionWidget.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"


APlayerCombatCharacter::APlayerCombatCharacter()
{
	//rotation
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	//character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	
	//camera boom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->TargetOffset = FVector(0.0f, 0.0f, 50.0f);
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bUseCameraLagSubstepping = true;
	CameraBoom->CameraLagSpeed = 10.0f;
	
	//camera setup
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	
	TargetingComponent = CreateDefaultSubobject<UTargetingComponent>(TEXT("TargetingComponent"));
}

void APlayerCombatCharacter::OnLockOn()
{
	TargetingComponent->ToggleLockOn();
}

void APlayerCombatCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();
	
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		//gibing mapping context and inputs
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				UE_LOG(LogTemp, Warning, TEXT("Mapping context was added"));
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
		
		//Creating the widget
		if (!HUDWidget && HUDWidgetClass && PC->IsLocalController())
		{
			HUDWidget = CreateWidget<UPlayerHUDWidget>(PC, HUDWidgetClass);
			UE_LOG(LogTemp, Warning, TEXT("HUDWidget was created"));
			if (HUDWidget)
			{
				UE_LOG(LogTemp, Warning, TEXT("HUDWidget added to viewport"));
				HUDWidget->InitFromAbilitySystemComponent(GetAbilitySystemComponent());
				HUDWidget->AddToViewport();
			}
		}
	}
}

void APlayerCombatCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	
	if (UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Locomotion
		Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCombatCharacter::Move);
		Input->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCombatCharacter::Look);
		
		//Attacks
		Input->BindAction(LightAttackAction, ETriggerEvent::Started, this, &APlayerCombatCharacter::OnLightAttack);
		Input->BindAction(HeavyAttackAction, ETriggerEvent::Started, this, &APlayerCombatCharacter::OnHeavyAttack);
		Input->BindAction(Special1Action, ETriggerEvent::Started, this, &APlayerCombatCharacter::OnSpecial1);
		Input->BindAction(Special2Action, ETriggerEvent::Started, this, &APlayerCombatCharacter::OnSpecial2);
		Input->BindAction(Special3Action, ETriggerEvent::Started, this, &APlayerCombatCharacter::OnSpecial3);
		
		Input->BindAction(DodgeAction, ETriggerEvent::Started, this, &APlayerCombatCharacter::OnDodge);
		
		Input->BindAction(ParryAction, ETriggerEvent::Started, this, &APlayerCombatCharacter::OnParry);
		
		Input->BindAction(LockOnAction, ETriggerEvent::Started, this, &APlayerCombatCharacter::OnLockOn);
		
		Input->BindAction(BlockAction, ETriggerEvent::Started,   this, &APlayerCombatCharacter::OnBlockStarted);
		Input->BindAction(BlockAction, ETriggerEvent::Completed, this, &APlayerCombatCharacter::OnBlockReleased);
		
		Input->BindAction(WeaponWheelAction, ETriggerEvent::Started,   this, &APlayerCombatCharacter::OpenWeaponWheel);
		Input->BindAction(WeaponWheelAction, ETriggerEvent::Completed, this, &APlayerCombatCharacter::CloseWeaponWheel);
	}
}

void APlayerCombatCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D Input = Value.Get<FVector2D>();
	if (!Controller) return;

	const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
	const FVector Forward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector Right   = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(Forward, Input.Y);
	AddMovementInput(Right, Input.X);
}

void APlayerCombatCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D Input = Value.Get<FVector2D>();

	if (bWheelOpen && WeaponWheelWidget)
	{
		const float Y = bInvertWheelY ? -Input.Y : Input.Y;
		WheelInput += FVector2D(Input.X, Y) * WheelSensitivity;
		if (WheelInput.Size() > 1.f)
		{
			WheelInput.Normalize();
		}
		WeaponWheelWidget->UpdateSelection(WheelInput);
		return;
	}

	if (TargetingComponent && TargetingComponent->IsLockedOn())
	{
		TargetingComponent->HandleSwitchInput(Input.X);
		return;
	}

	AddControllerYawInput(Input.X);
	AddControllerPitchInput(Input.Y);
}

void APlayerCombatCharacter::ActivateAbilityByTag(const FGameplayTag& AbilityTag)
{
	if (bWheelOpen) return;
	if (AbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTagContainer(AbilityTag)))
	{
		BufferedAbilityTag = FGameplayTag();   // it worke nothing to remember
		return;
	}

	BufferedAbilityTag = AbilityTag;
	BufferedTime = GetWorld()->GetTimeSeconds();
}

void APlayerCombatCharacter::OnLightAttack()
{
	ActivateAbilityByTag(CombatTags::Ability_Attack_Light.GetTag());
	UE_LOG(LogTemp, Warning, TEXT("Light attack called Ability should of been called"))
}

void APlayerCombatCharacter::OnHeavyAttack()
{
	ActivateAbilityByTag(CombatTags::Ability_Attack_Heavy.GetTag());
	UE_LOG(LogTemp, Warning, TEXT("Heavy attack called Ability should of been called"))
}

void APlayerCombatCharacter::OnSpecial1()
{
	ActivateAbilityByTag(CombatTags::Ability_Attack_Special1.GetTag());
	UE_LOG(LogTemp, Warning, TEXT("Special 1 attack called Ability should of been called"))
}

void APlayerCombatCharacter::OnSpecial2()
{
	ActivateAbilityByTag(CombatTags::Ability_Attack_Special2.GetTag());
	UE_LOG(LogTemp, Warning, TEXT("Special 2 attack called Ability should of been called"))
}

void APlayerCombatCharacter::OnSpecial3()
{
	ActivateAbilityByTag(CombatTags::Ability_Attack_Special3.GetTag());
	UE_LOG(LogTemp, Warning, TEXT("Special 3 attack called Ability should of been called"))
}

void APlayerCombatCharacter::OnDodge()
{
	ActivateAbilityByTag(CombatTags::Ability_Dodge.GetTag());
	UE_LOG(LogTemp, Warning, TEXT("Dodge got called Ability should of been called"))
}

void APlayerCombatCharacter::OnParry()
{
	ActivateAbilityByTag(CombatTags::Ability_Parry.GetTag());
	UE_LOG(LogTemp, Warning, TEXT("parry got called Ability should of been called"))
}

void APlayerCombatCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	AbilitySystemComponent->RegisterGameplayTagEvent(CombatTags::State_HitStunned.GetTag(), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &APlayerCombatCharacter::OnBufferTagChanged);
	AbilitySystemComponent->RegisterGameplayTagEvent(CombatTags::State_ComboWindow.GetTag(), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &APlayerCombatCharacter::OnBufferTagChanged);
	AbilitySystemComponent->RegisterGameplayTagEvent(CombatTags::State_Attacking.GetTag(), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &APlayerCombatCharacter::OnBufferTagChanged);
	AbilitySystemComponent->RegisterGameplayTagEvent(CombatTags::State_Dodging.GetTag(), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &APlayerCombatCharacter::OnBufferTagChanged);
}

void APlayerCombatCharacter::OnBufferTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	const bool bComboWindowOpened = Tag == CombatTags::State_ComboWindow.GetTag() && NewCount > 0;
	const bool bAttackFinished = Tag == CombatTags::State_Attacking.GetTag() && NewCount == 0;
	const bool bDodgeFinished = Tag == CombatTags::State_Dodging.GetTag() && NewCount == 0;
	const bool bStunFinished = Tag == CombatTags::State_HitStunned.GetTag() && NewCount == 0;

	if (bComboWindowOpened || bAttackFinished || bDodgeFinished || bStunFinished)
	{
		GetWorldTimerManager().SetTimerForNextTick(this, &APlayerCombatCharacter::TryBufferedAbility);
	}
}

void APlayerCombatCharacter::TryBufferedAbility()
{
	if (!BufferedAbilityTag.IsValid()) return;

	if (GetWorld()->GetTimeSeconds() - BufferedTime > InputBufferTime)
	{
		BufferedAbilityTag = FGameplayTag();
		return;
	}

	const FGameplayTag Tag = BufferedAbilityTag;
	BufferedAbilityTag = FGameplayTag();
	AbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTagContainer(Tag));
}
void APlayerCombatCharacter::OnBlockStarted()
{
	const bool bActivated = AbilitySystemComponent->TryActivateAbilitiesByTag(
	FGameplayTagContainer(CombatTags::Ability_Block.GetTag()));

}

void APlayerCombatCharacter::OnBlockReleased()
{
	FGameplayTagContainer BlockTags(CombatTags::Ability_Block.GetTag());
	AbilitySystemComponent->CancelAbilities(&BlockTags);

	if (BufferedAbilityTag == CombatTags::Ability_Block.GetTag())
	{
		BufferedAbilityTag = FGameplayTag();
	}
}
void APlayerCombatCharacter::OpenWeaponWheel()
{
	if (bWheelOpen || WheelWeapons.Num() == 0 || !WeaponWheelWidgetClass) return;

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;

	if (!WeaponWheelWidget)
	{
		WeaponWheelWidget = CreateWidget<UWeaponSelectionWidget>(PC, WeaponWheelWidgetClass);
		if (!WeaponWheelWidget) return;
		WeaponWheelWidget->AddToViewport(10); 
	}

	const int32 CurrentIndex = WheelWeapons.Find(WeaponComponent->GetCurrentWeapon());
	WeaponWheelWidget->Open(WheelWeapons, CurrentIndex);

	WheelInput = FVector2D::ZeroVector;
	bWheelOpen = true;

	UGameplayStatics::SetGlobalTimeDilation(this, WheelTimeDilation);   // slowmo while choosing
}

void APlayerCombatCharacter::CloseWeaponWheel()
{
	if (!bWheelOpen) return;
	bWheelOpen = false;

	UGameplayStatics::SetGlobalTimeDilation(this, 1.f);   // normal speed

	if (!WeaponWheelWidget) return;

	const int32 Index = WeaponWheelWidget->GetHighlightedIndex();
	
	WeaponWheelWidget->Close();

	const bool bMidSwing = AbilitySystemComponent->HasMatchingGameplayTag(CombatTags::State_Attacking.GetTag());
	if (!bMidSwing && WheelWeapons.IsValidIndex(Index) && WheelWeapons[Index]
		&& WheelWeapons[Index] != WeaponComponent->GetCurrentWeapon())
	{
		WeaponComponent->EquipWeapon(WheelWeapons[Index]);
	}
}
