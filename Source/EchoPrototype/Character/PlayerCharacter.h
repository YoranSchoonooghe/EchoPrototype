#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Interactables/InteractionComponent.h"
#include "PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UEchoComponent;
class UCombatComponent;
class UHealthComponent;
class UStealthKillComponent;
class UPlayerStateBase;
class UAIPerceptionStimuliSourceComponent;

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

	void StartSprinting();
	void StopSprinting();

	void StartSneaking();
	void StopSneaking();
	
	//Camera
	void CameraMove(const FVector2D& Value);
	void CycleCameraPerspective();

	//Echo
	void EchoPressed();
	void EchoReleased();
	void LookThroughEcho();
	void TeleportToEcho();

	//Combat
	void AttackPressed();
	void AttackReleased();

	FORCEINLINE UCombatComponent* GetCombatComponent() const { return Combat; }

	UFUNCTION(BlueprintPure, Category = "Combat")
	bool IsAttacking() const;

	//Stealth Kill
	void StealthKillPressed();

	FORCEINLINE UStealthKillComponent* GetStealthKillComponent() const { return StealthKill; }

	//States
	void ChangeState(class UPlayerStateBase* NewState);

protected:
	virtual void BeginPlay() override;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Interaction, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInteractionComponent> Interaction;

	UPROPERTY(BlueprintReadOnly, Category = "Camera")
	ECameraPerspective CurrentPerspective;

	UPROPERTY(BlueprintReadOnly, Category = "State Machine")
	TObjectPtr<UPlayerStateBase> CurrentState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EnemyAI")
	TObjectPtr<UAIPerceptionStimuliSourceComponent> StimuliSource;

public:

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
};
