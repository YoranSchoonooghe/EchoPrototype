#include "EchoCombatComponent.h"
#include "EchoCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UEchoCombatComponent::UEchoCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}


void UEchoCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	auto* pOwner = Cast<AEchoCharacter>(GetOwner());
	if (!pOwner) return;

	pOwner->GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
}


void UEchoCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

