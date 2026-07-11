#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UEchoComponent;

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

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Echo, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UEchoComponent> Echo;

	UPROPERTY(BlueprintReadOnly, Category = "Camera")
	ECameraPerspective CurrentPerspective;
public:

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

};
