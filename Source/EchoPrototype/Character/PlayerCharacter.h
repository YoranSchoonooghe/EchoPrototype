#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Interactables/InteractionComponent.h"
#include "PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UStaticMeshComponent;
class UEchoComponent;
class UCombatComponent;
class UHealthComponent;
class UStealthKillComponent;
class UClimbingComponent;
class UPlayerStateBase;
class UAIPerceptionStimuliSourceComponent;
class USkillTreeComponent;
class UEchoSaveGame;
class UDodgeComponent;
class ULockOnComponent;
class UWeaponData;

enum class EBufferedPlayerAction : uint8
{
	None,
	Attack,
	Dodge
};

//Camere perspective states
UENUM(BlueprintType)
enum class ECameraPerspective : uint8
{
	ThirdPerson,
	TopDown,
	FirstPerson
};

UCLASS()
class ECHOPROTOTYPE_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

	//Movement
	void Move(const FVector2D& Value);
	virtual void Jump() override;

	void StartSprinting();
	void StopSprinting();

	FORCEINLINE float GetWalkSpeed() const { return WalkSpeed; }
	FORCEINLINE float GetSprintSpeed() const { return SprintSpeed; }
	FORCEINLINE float GetSneakSpeed() const { return SneakSpeed; }

	void AddWalkSpeedBonus(float Delta) { WalkSpeed += Delta; }
	void AddSprintSpeedBonus(float Delta) { SprintSpeed += Delta; }
	void AddSneakSpeedBonus(float Delta) { SneakSpeed += Delta; }

	void StartSneaking();
	void StopSneaking();

	//Climbing
	void ClimbPressed();

	FORCEINLINE UClimbingComponent* GetClimbingComponent() const { return Climbing; }
	
	//Camera
	void CameraMove(const FVector2D& Value);
	void CycleCameraPerspective();

	//Echo
	void EchoPressed();
	void EchoReleased();

	void SelectTeleportEcho();
	void SelectVisionEcho();

	//Combat
	void AttackPressed();
	void AttackReleased();

	FORCEINLINE UCombatComponent* GetCombatComponent() const { return Combat; }

	UFUNCTION(BlueprintPure, Category = "Combat")
	bool IsAttacking() const;

	//Weapon
	void EquipWeapon(UWeaponData* WeaponData);

	UFUNCTION(BlueprintPure, Category = "Combat")
	FORCEINLINE bool IsWeaponEquipped() const { return EquippedWeaponData != nullptr; }

	UFUNCTION(BlueprintPure, Category = "Combat")
	FORCEINLINE UWeaponData* GetEquippedWeaponData() const { return EquippedWeaponData; }

	//Dodge
	void DodgePressed();

	FORCEINLINE UDodgeComponent* GetDodgeComponent() const { return Dodge; }
	FORCEINLINE FVector2D GetLastMovementInput() const { return LastMovementInput; }

	// Input buffer: remembers a press that couldn't act immediately (e.g. dodge pressed mid-
	// attack) so the relevant state can fire it the instant the blocking action ends, instead of
	// dropping it. ConsumeBuffered*IfFresh() clears the buffer regardless of the result.
	void BufferAttack();
	void BufferDodge();
	bool ConsumeBufferedAttackIfFresh();
	bool ConsumeBufferedDodgeIfFresh();

	//Lock On
	void ToggleLockOn();

	FORCEINLINE ULockOnComponent* GetLockOnComponent() const { return LockOn; }

	//Stealth Kill
	void StealthKillPressed();

	FORCEINLINE UStealthKillComponent* GetStealthKillComponent() const { return StealthKill; }

	FORCEINLINE UHealthComponent* GetHealthComponent() const { return Health; }

	//Skill Tree
	FORCEINLINE USkillTreeComponent* GetSkillTreeComponent() const { return SkillTree; }

	//Save Game
	UFUNCTION(BlueprintCallable, Category = "Save")
	UEchoSaveGame* CaptureSaveGame() const;

	UFUNCTION(BlueprintCallable, Category = "Save")
	void ApplySaveGame(UEchoSaveGame* SaveGameObject);

	//States
	void ChangeState(class UPlayerStateBase* NewState);

protected:
	virtual void BeginPlay() override;
	virtual void Landed(const FHitResult& Hit) override;

	UFUNCTION()
	void HandleDeath();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Echo, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UEchoComponent> Echo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCombatComponent> Combat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UHealthComponent> Health;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStealthKillComponent> StealthKill;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> WeaponMesh;

	UPROPERTY(EditAnywhere, Category = Combat)
	FName WeaponSocketName = "WeaponSocket_R";

	UPROPERTY(BlueprintReadOnly, Category = Combat)
	TObjectPtr<UWeaponData> EquippedWeaponData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDodgeComponent> Dodge;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ULockOnComponent> LockOn;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UClimbingComponent> Climbing;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Interaction, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInteractionComponent> Interaction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill Tree", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkillTreeComponent> SkillTree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float WalkSpeed = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float SprintSpeed = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float SneakSpeed = 250.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Camera")
	ECameraPerspective CurrentPerspective;

	UPROPERTY(BlueprintReadOnly, Category = "State Machine")
	TObjectPtr<UPlayerStateBase> CurrentState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EnemyAI")
	TObjectPtr<UAIPerceptionStimuliSourceComponent> StimuliSource;

	UPROPERTY(EditDefaultsOnly, Category = "UI|PostProcess")
	TObjectPtr<UMaterialInterface> LowHealthVignetteMaterial;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> LowHealthVignetteDynamicInst;

	UFUNCTION()
	void HandleHealthChanged(float HealthPercent);

	void UpdateLowHealthVignette(float HealthPercent);

	FVector2D LastMovementInput = FVector2D::ZeroVector;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	float InputBufferWindow = 0.3f;

	EBufferedPlayerAction PendingBufferedAction = EBufferedPlayerAction::None;
	float BufferedActionTimestamp = -1000.0f;

public:

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
};
