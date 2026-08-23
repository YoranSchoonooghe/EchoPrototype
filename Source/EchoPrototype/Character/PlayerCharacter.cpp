#include "PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EchoComponent.h"
#include "../Combat/CombatComponent.h"
#include "../Combat/HealthComponent.h"
#include "../Combat/StealthKillComponent.h"
#include "../Combat/DodgeComponent.h"
#include "../Combat/LockOnComponent.h"
#include "../Combat/WeaponData.h"
#include "../HUD/MenuFlowSubsystem.h"
#include "../HUD/States/MenuStateBase.h"
#include "../Enemies/EnemyCharacter.h"
#include "Components/CapsuleComponent.h"
#include "../Movement/ClimbingComponent.h"
#include "../SkillTree/SkillTreeComponent.h"
#include "../SkillTree/SkillTreeNodeData.h"
#include "../SaveGame/EchoSaveGame.h"
#include "States/PlayerStateBase.h"
#include "States/PlayerStates.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Kismet/GameplayStatics.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;


	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true; 
	bUseControllerRotationRoll = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 250.0f; 
	CameraBoom->bUsePawnControlRotation = true; 

	// over the shoulder
	CurrentPerspective = ECameraPerspective::ThirdPerson;
	CameraBoom->SocketOffset = FVector(0.0f, 60.0f, 50.0f);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); 
	FollowCamera->bUsePawnControlRotation = false; 

	Echo = CreateDefaultSubobject<UEchoComponent>(TEXT("EchoComponent"));
	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	Health = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	StealthKill = CreateDefaultSubobject<UStealthKillComponent>(TEXT("StealthKillComponent"));

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(GetMesh(), WeaponSocketName);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetVisibility(false);
	Dodge = CreateDefaultSubobject<UDodgeComponent>(TEXT("DodgeComponent"));
	LockOn = CreateDefaultSubobject<ULockOnComponent>(TEXT("LockOnComponent"));
	Climbing = CreateDefaultSubobject<UClimbingComponent>(TEXT("ClimbingComponent"));
	Interaction = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
	SkillTree = CreateDefaultSubobject<USkillTreeComponent>(TEXT("SkillTreeComponent"));

	StimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("AIPerceptionStimuliSourceComponent"));
	StimuliSource->RegisterForSense(UAISense_Sight::StaticClass());
	StimuliSource->RegisterWithPerceptionSystem();

	// crouching
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	// default speed
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->MaxWalkSpeedCrouched = SneakSpeed;
}

void APlayerCharacter::Move(const FVector2D& Value)
{
	LastMovementInput = Value;

	if (Climbing && Climbing->IsHanging())
	{
		Climbing->HandleShimmyInput(Value);
		return;
	}

	if (CurrentState && !CurrentState->CanMove()) return;

	if (Health && Health->IsStunned()) return;

	if (Echo && Echo->IsViewingThroughEcho())
	{
		Echo->AddEchoMoveInput(Value);
		return;
	}


	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();

		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, Value.Y);
		AddMovementInput(RightDirection, Value.X);
	}
}

void APlayerCharacter::Jump()
{
	if (Echo && Echo->GetEchoState() == EEchoState::Aiming)
	{
		return;
	}

	if (Climbing && Climbing->IsHanging())
	{
		Climbing->JumpFromLedge();
		return;
	}

	Super::Jump();
}

void APlayerCharacter::StartSprinting()
{
	if (CurrentState) ChangeState(CurrentState->OnSprintPressed(this));
}

void APlayerCharacter::StopSprinting()
{
	if (CurrentState) ChangeState(CurrentState->OnSprintReleased(this));
}

void APlayerCharacter::StartSneaking()
{
	if (CurrentState) ChangeState(CurrentState->OnSneakPressed(this));
}

void APlayerCharacter::StopSneaking()
{
	if (CurrentState) ChangeState(CurrentState->OnSneakReleased(this));
}

void APlayerCharacter::CameraMove(const FVector2D& Value)
{
	if (Echo && Echo->IsViewingThroughEcho())
	{
		Echo->AddEchoLookInput(Value.X, -Value.Y);
		return;
	}

	if (Controller != nullptr)
	{
		AddControllerYawInput(Value.X);
		AddControllerPitchInput(-Value.Y);
	}
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	ChangeState(NewObject<UPlayerState_IdleWalk>(this));

	if (Health)
	{
		Health->OnDeath.AddDynamic(this, &APlayerCharacter::HandleDeath);
		Health->OnDeathAnimationFinished.AddDynamic(this, &APlayerCharacter::HandleDeathAnimationFinished);

		Health->OnHealthChanged.AddDynamic(this, &APlayerCharacter::HandleHealthChanged);
	}

	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &APlayerCharacter::HandleCapsuleHit);

	if (LowHealthVignetteMaterial && FollowCamera)
	{
		LowHealthVignetteDynamicInst = UMaterialInstanceDynamic::Create(LowHealthVignetteMaterial, this);
		if (LowHealthVignetteDynamicInst)
		{
			FollowCamera->PostProcessSettings.WeightedBlendables.Array.Add(
				FWeightedBlendable(1.0f, LowHealthVignetteDynamicInst)
			);
		}
	}

	if (Health)
	{
		UpdateLowHealthVignette(Health->GetHealthPercent());
	}
}

void APlayerCharacter::Landed(const FHitResult& Hit)
{
	UAISense_Hearing::ReportNoiseEvent(
		GetWorld(),
		GetActorLocation(),
		1.0f,
		this,
		0.0f,
		TEXT("Landed")
	);
}

void APlayerCharacter::HandleDeath()
{
	// Dying while hanging would otherwise leave the corpse floating in flying mode.
	if (Climbing && Climbing->IsHanging())
	{
		Climbing->CancelHanging();
	}

	ChangeState(NewObject<UPlayerState_Dead>(this));
}

void APlayerCharacter::HandleCapsuleHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(OtherActor))
	{
		Enemy->Ragdoll();
	}
}

void APlayerCharacter::HandleDeathAnimationFinished()
{
	if (!DeathMenuStateClass)
	{
		return;
	}

	if (UMenuFlowSubsystem* Flow = GetGameInstance() ? GetGameInstance()->GetSubsystem<UMenuFlowSubsystem>() : nullptr)
	{
		Flow->PushState(DeathMenuStateClass);
	}
}

void APlayerCharacter::HandleHealthChanged(float HealthPercent)
{
	UpdateLowHealthVignette(HealthPercent);
}

void APlayerCharacter::UpdateLowHealthVignette(float HealthPercent)
{
	if (!LowHealthVignetteDynamicInst)
	{
		return;
	}

	const float DamageIntensity = 1.0f - FMath::Clamp(HealthPercent, 0.0f, 1.0f);

	LowHealthVignetteDynamicInst->SetScalarParameterValue(FName("DamageIntensity"), DamageIntensity);
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentState)
	{
		ChangeState(CurrentState->UpdateState(this, DeltaTime));
	}
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

UEchoSaveGame* APlayerCharacter::CaptureSaveGame() const
{
	UEchoSaveGame* SaveGameObject = NewObject<UEchoSaveGame>();

	if (SkillTree)
	{
		SaveGameObject->SkillPoints = SkillTree->GetAvailableSkillPoints();
		for (USkillTreeNodeData* Node : SkillTree->GetUnlockedNodes())
		{
			SaveGameObject->UnlockedNodes.Add(Node);
		}
	}

	SaveGameObject->LevelName = FName(*UGameplayStatics::GetCurrentLevelName(this, true));
	SaveGameObject->PlayerLocation = GetActorLocation();
	SaveGameObject->PlayerRotation = GetActorRotation();
	SaveGameObject->EquippedWeapon = EquippedWeaponData;

	return SaveGameObject;
}

void APlayerCharacter::ApplySaveGame(UEchoSaveGame* SaveGameObject)
{
	if (!SaveGameObject)
	{
		return;
	}

	SetActorLocationAndRotation(SaveGameObject->PlayerLocation, SaveGameObject->PlayerRotation);

	if (SkillTree)
	{
		TArray<USkillTreeNodeData*> ResolvedNodes;
		ResolvedNodes.Reserve(SaveGameObject->UnlockedNodes.Num());

		for (const TSoftObjectPtr<USkillTreeNodeData>& SoftNode : SaveGameObject->UnlockedNodes)
		{
			if (USkillTreeNodeData* Node = SoftNode.LoadSynchronous())
			{
				ResolvedNodes.Add(Node);
			}
		}

		SkillTree->LoadUnlockedNodes(SaveGameObject->SkillPoints, ResolvedNodes);
	}

	if (UWeaponData* Weapon = SaveGameObject->EquippedWeapon.LoadSynchronous())
	{
		EquipWeapon(Weapon);
	}
}

void APlayerCharacter::ChangeState(UPlayerStateBase* NewState)
{
	if (NewState == nullptr || NewState == CurrentState) return;

	if (CurrentState)
	{
		CurrentState->ExitState(this);
	}

	CurrentState = NewState;

	if (CurrentState)
	{
		CurrentState->EnterState(this);
	}
}

void APlayerCharacter::EchoPressed()
{
	if (!Echo) return;

	if (Climbing && Climbing->IsHanging())
	{
		Climbing->CancelHanging();
	}

	Echo->OnEchoPressed();
}

void APlayerCharacter::EchoReleased()
{
	if (!Echo) return;

	if (Climbing && Climbing->IsHanging())
	{
		Climbing->CancelHanging();
	}

	Echo->OnEchoReleased();
}

void APlayerCharacter::SelectTeleportEcho()
{
	if (Echo && Echo->GetEchoState() == EEchoState::Aiming)
	{
		Echo->SetSelectedEchoType(EEchoType::Teleport);
	}
}

void APlayerCharacter::SelectVisionEcho()
{
	if (Echo && Echo->GetEchoState() == EEchoState::Aiming)
	{
		Echo->SetSelectedEchoType(EEchoType::Vision);
	}
}

void APlayerCharacter::AttackPressed()
{
	if (CurrentState) ChangeState(CurrentState->OnAttackPressed(this));
}

void APlayerCharacter::AttackReleased()
{
	if (CurrentState) ChangeState(CurrentState->OnAttackReleased(this));
}

void APlayerCharacter::DodgePressed()
{
	if (CurrentState) ChangeState(CurrentState->OnDodgePressed(this));
}

void APlayerCharacter::BufferAttack()
{
	PendingBufferedAction = EBufferedPlayerAction::Attack;
	BufferedActionTimestamp = GetWorld()->GetTimeSeconds();
}

void APlayerCharacter::BufferDodge()
{
	PendingBufferedAction = EBufferedPlayerAction::Dodge;
	BufferedActionTimestamp = GetWorld()->GetTimeSeconds();
}

bool APlayerCharacter::ConsumeBufferedAttackIfFresh()
{
	if (PendingBufferedAction != EBufferedPlayerAction::Attack)
	{
		return false;
	}

	const bool bFresh = (GetWorld()->GetTimeSeconds() - BufferedActionTimestamp) <= InputBufferWindow;
	PendingBufferedAction = EBufferedPlayerAction::None;
	return bFresh;
}

bool APlayerCharacter::ConsumeBufferedDodgeIfFresh()
{
	if (PendingBufferedAction != EBufferedPlayerAction::Dodge)
	{
		return false;
	}

	const bool bFresh = (GetWorld()->GetTimeSeconds() - BufferedActionTimestamp) <= InputBufferWindow;
	PendingBufferedAction = EBufferedPlayerAction::None;
	return bFresh;
}

void APlayerCharacter::ToggleLockOn()
{
	if (LockOn)
	{
		LockOn->ToggleLockOn();
	}
}

void APlayerCharacter::StealthKillPressed()
{
	if (CurrentState) ChangeState(CurrentState->OnStealthKillPressed(this));
}

void APlayerCharacter::ClimbPressed()
{
	if (CurrentState) ChangeState(CurrentState->OnClimbPressed(this));
}

bool APlayerCharacter::IsAttacking() const
{
	return Combat && Combat->IsAttacking();
}

void APlayerCharacter::EquipWeapon(UWeaponData* WeaponData)
{
	if (!WeaponData)
	{
		return;
	}

	EquippedWeaponData = WeaponData;

	if (WeaponMesh)
	{
		WeaponMesh->SetStaticMesh(WeaponData->Mesh);
		WeaponMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketName);
		WeaponMesh->SetVisibility(true);
	}

	if (Combat)
	{
		Combat->SetWeaponEquipped(true);
	}
}

void APlayerCharacter::CycleCameraPerspective()
{
	switch (CurrentPerspective)
	{
	case ECameraPerspective::ThirdPerson:
		// SWITCHING TO TOP-DOWN
		CameraBoom->TargetArmLength = 800.0f; 
		CameraBoom->SocketOffset = FVector::ZeroVector; 
		CameraBoom->bUsePawnControlRotation = false;
		CameraBoom->SetRelativeRotation(FRotator(-50.0f, 0.0f, 0.0f));

		CurrentPerspective = ECameraPerspective::TopDown;
		break;

	case ECameraPerspective::TopDown:
		// SWITCHING TO FIRST-PERSON
		CameraBoom->TargetArmLength = -10.0f;
		CameraBoom->SocketOffset = FVector(0.0f, 0.0f, BaseEyeHeight);
		CameraBoom->bUsePawnControlRotation = true;
		CameraBoom->SetRelativeRotation(FRotator::ZeroRotator);

		GetMesh()->SetOwnerNoSee(true);

		CurrentPerspective = ECameraPerspective::FirstPerson;
		break;

	case ECameraPerspective::FirstPerson:
		// SWITCHING BACK TO THIRD-PERSON
		CameraBoom->TargetArmLength = 250.0f; 
		CameraBoom->SocketOffset = FVector(0.0f, 60.0f, 50.0f);
		CameraBoom->bUsePawnControlRotation = true;

		GetMesh()->SetOwnerNoSee(false);

		CurrentPerspective = ECameraPerspective::ThirdPerson;
		break;
	}
}