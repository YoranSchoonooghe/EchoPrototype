#include "EnemyAIController.h"
#include "EnemyCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "EchoPrototype/Character/PlayerCharacter.h"
#include "EchoPrototype/Combat/HealthComponent.h"
#include "../Echo/EchoComponentInterface.h"
#include "EchoPrototype/Echo/EchoCharacter.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "TimerManager.h"
#include "EchoPrototype/Combat/CombatComponent.h"

AEnemyAIController::AEnemyAIController()
{
	PrimaryActorTick.bCanEverTick = true;

	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	RunBehaviorTree(BTEnemy);
	InitBBKeys();

	AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::HandlePerception);

	auto* pHealth = GetPawn()->GetComponentByClass<UHealthComponent>();
	if (pHealth)
	{
		pHealth->OnDamage.AddDynamic(this, &AEnemyAIController::SetTargetActor);
	}
}

void AEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateDetection(DeltaTime);
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	_controlledEnemy = Cast<AEnemyCharacter>(InPawn);
}

void AEnemyAIController::HandlePerception(AActor* Actor, FAIStimulus Stimulus)
{
	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
		HandleSightPerception(Actor, Stimulus);
	else if (Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
		HandleSoundPerception(Actor, Stimulus);
}

void AEnemyAIController::SetTargetActor(AActor* Aggressor)
{
	auto const TargetPlayerKeyName = TEXT("TargetPlayer");

	auto* pBlackboardComponent = GetBlackboardComponent();
	if (!pBlackboardComponent) return;

	auto* pAggressorCharacter = Cast<APlayerCharacter>(Aggressor);
	if (!pAggressorCharacter) return;

	if (!IsLocationReachable(pAggressorCharacter->GetActorLocation())) return;

	if (!pBlackboardComponent->GetValueAsObject(TargetPlayerKeyName))
	{
		pBlackboardComponent->SetValueAsObject(TargetPlayerKeyName, pAggressorCharacter);
		StartReachabilityMonitor();
	}
}

void AEnemyAIController::InitBBKeys()
{
	auto* pBlackboardComponent = GetBlackboardComponent();
	if (!pBlackboardComponent) return;

	auto* pEnemy = Cast<AEnemyCharacter>(GetCharacter());
	if (!pEnemy) return;

	if (pEnemy->bIsGuarding)
	{
		pBlackboardComponent->SetValueAsVector(TEXT("GuardLocation"), GetPawn()->GetActorLocation());
		pBlackboardComponent->SetValueAsRotator(TEXT("GuardRotation"), GetPawn()->GetActorRotation());

		return;
	}

	auto& patrolPoints = pEnemy->PatrolPoints;
	if (patrolPoints.IsEmpty()) return;

	pBlackboardComponent->SetValueAsObject(TEXT("PatrolPointIndex"), 0);
	pBlackboardComponent->SetValueAsObject(TEXT("PatrolPoint"), patrolPoints[0]);
}

void AEnemyAIController::UpdateTargetEcho()
{
	if (!_targetEcho || !IsEchoDetectable(_targetEcho))
	{
		return;
	}

	HandleDetectableEchoSpotted(_targetEcho);
}

void AEnemyAIController::HandleDetectableEchoSpotted(AEchoCharacter* Echo)
{
	if (!Echo)
	{
		return;
	}

	if (auto* pBlackboardComponent = GetBlackboardComponent())
	{
		pBlackboardComponent->SetValueAsVector(TEXT("SusLocation"), Echo->GetActorLocation());
	}

	StartEchoAttackMonitor(Echo);
}

bool AEnemyAIController::IsEchoDetectable(AEchoCharacter* Echo) const
{
	if (!Echo)
	{
		return false;
	}

	TArray<UActorComponent*> EchoComponents = Echo->GetComponentsByInterface(UEchoComponentInterface::StaticClass());
	for (UActorComponent* Component : EchoComponents)
	{
		if (IEchoComponentInterface::Execute_GetDetectability(Component))
		{
			return true;
		}
	}

	return false;
}

bool AEnemyAIController::IsLocationReachable(const FVector& Location) const
{
	if (!bIgnoreUnreachableTargets)
	{
		return true;
	}

	APawn* pSelf = GetPawn();
	if (!pSelf)
	{
		return true;
	}

	if (Location.Z - pSelf->GetActorLocation().Z > MaxReachableHeightAboveSelf)
	{
		return false;
	}

	if (bRequireNavPath)
	{
		UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(GetWorld(), pSelf->GetActorLocation(), Location, pSelf);
		if (!NavPath || !NavPath->IsValid() || NavPath->IsPartial())
		{
			return false;
		}
	}

	return true;
}

void AEnemyAIController::StartReachabilityMonitor()
{
	if (!bIgnoreUnreachableTargets)
	{
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(ReachabilityCheckTimerHandle, this, &AEnemyAIController::CheckTargetReachability, ReachabilityCheckInterval, true);
}

void AEnemyAIController::StopReachabilityMonitor()
{
	GetWorld()->GetTimerManager().ClearTimer(ReachabilityCheckTimerHandle);
}

void AEnemyAIController::CheckTargetReachability()
{
	auto const TargetPlayerKeyName = TEXT("TargetPlayer");

	auto* pBlackboardComponent = GetBlackboardComponent();
	if (!pBlackboardComponent)
	{
		StopReachabilityMonitor();
		return;
	}

	auto* pPlayer = Cast<APlayerCharacter>(pBlackboardComponent->GetValueAsObject(TargetPlayerKeyName));
	if (!pPlayer)
	{
		StopReachabilityMonitor();
		return;
	}

	if (!IsLocationReachable(pPlayer->GetActorLocation()))
	{
		pBlackboardComponent->ClearValue(TargetPlayerKeyName);
		pBlackboardComponent->SetValueAsVector(TEXT("SusLocation"), pPlayer->GetActorLocation());
		StopReachabilityMonitor();
	}
}

void AEnemyAIController::StartEchoAttackMonitor(AEchoCharacter* Echo)
{
	_targetEcho = Echo;
	GetWorld()->GetTimerManager().SetTimer(EchoAttackCheckTimerHandle, this, &AEnemyAIController::CheckEchoAttackRange, EchoAttackCheckInterval, true);
}

void AEnemyAIController::StopEchoAttackMonitor()
{
	GetWorld()->GetTimerManager().ClearTimer(EchoAttackCheckTimerHandle);
}

void AEnemyAIController::CheckEchoAttackRange()
{
	if (!IsValid(_targetEcho))
	{
		StopEchoAttackMonitor();
		return;
	}

	APawn* pSelf = GetPawn();
	if (!pSelf)
	{
		StopEchoAttackMonitor();
		return;
	}

	const float DistSq = FVector::DistSquared(pSelf->GetActorLocation(), _targetEcho->GetActorLocation());
	if (DistSq <= FMath::Square(EchoAttackRange))
	{
		AttackTargetEcho();
	}
}

void AEnemyAIController::AttackTargetEcho()
{
	if (!IsValid(_targetEcho))
	{
		return;
	}

	const float Now = GetWorld()->GetTimeSeconds();
	if (Now - LastEchoAttackTime < EchoAttackCooldown)
	{
		return;
	}

	auto* pEnemy = Cast<AEnemyCharacter>(GetCharacter());
	UCombatComponent* pCombat = pEnemy ? pEnemy->GetCombatComponent() : nullptr;
	if (!pCombat)
	{
		return;
	}

	LastEchoAttackTime = Now;
	pCombat->OnAttackReleased();
}

void AEnemyAIController::HandleSightPerception(AActor* Actor, FAIStimulus Stimulus)
{
	auto const TargetPlayerKeyName = TEXT("TargetPlayer");

	auto* pEcho = Cast<AEchoCharacter>(Actor);
	if (pEcho)
	{
		if (_spottedEchoes.Contains(pEcho)) return;

		if (pEcho->GetVisualState() == EEchoVisualState::Placed)
		{
			if (IsEchoDetectable(pEcho))
			{
				HandleDetectableEchoSpotted(pEcho);
			}
		}
		else
		{
			if (_targetEcho)
				_targetEcho->OnPlaced.RemoveDynamic(this, &AEnemyAIController::UpdateTargetEcho);

			_targetEcho = pEcho;
			_targetEcho->OnPlaced.AddDynamic(this, &AEnemyAIController::UpdateTargetEcho);
		}

		_spottedEchoes.Add(pEcho);
		return;
	}

	auto* pPlayer = Cast<APlayerCharacter>(Actor);
	if (pPlayer)
	{
		auto* pBlackboardComponent = GetBlackboardComponent();
		if (!pBlackboardComponent) return;

		if (Stimulus.WasSuccessfullySensed())
		{
			if (!IsLocationReachable(pPlayer->GetActorLocation())) return;

			_targetPlayer = pPlayer;
			_bIsPlayerInSight = true;
		}
		else
		{
			_bIsPlayerInSight = false;

			if (!_controlledEnemy) return;
			if (_controlledEnemy->GetAlertState() == EAlertState::Alert)
			{
				pBlackboardComponent->ClearValue(TargetPlayerKeyName);
				pBlackboardComponent->SetValueAsVector(TEXT("SusLocation"), pPlayer->GetActorLocation());
				StopReachabilityMonitor();
			}

		}

		return;
	}
}

void AEnemyAIController::HandleSoundPerception(AActor* Actor, FAIStimulus Stimulus)
{
	auto* pBlackboardComponent = GetBlackboardComponent();
	if (!pBlackboardComponent) return;
	
	auto* navSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (!navSystem) return;

	FNavLocation projectedLocation;
	if (navSystem->ProjectPointToNavigation(Stimulus.StimulusLocation, projectedLocation))
	{
		pBlackboardComponent->SetValueAsVector(TEXT("SusLocation"), projectedLocation);
	}
}

void AEnemyAIController::UpdateDetection(float DeltaTime)
{
	if (!_controlledEnemy) return;
	if (_controlledEnemy->GetAlertState() == EAlertState::Alert) return;

	if (_bIsPlayerInSight)
	{
		if (_detectionValue >= DetectionThreshold) return;

		_detectionValue += DetectionRate * DeltaTime;

		if (_detectionValue >= DetectionThreshold)
		{
			SpotPlayer();
		}
	}
	else
	{
		if (_detectionValue <= 0.0f) return;

		_detectionValue -= DecayRate * DeltaTime;

		if (_detectionValue <= 0.0f && _targetPlayer)
		{
			_targetPlayer = nullptr;
		}
	}

	_detectionValue = FMath::Clamp(_detectionValue, 0.0f, DetectionThreshold);
	OnDetectionValueChanged.Broadcast(_detectionValue);
}

void AEnemyAIController::SpotPlayer()
{
	if (!_targetPlayer) return;

	auto* pBlackboardComponent = GetBlackboardComponent();
	if (!pBlackboardComponent) return;

	pBlackboardComponent->SetValueAsObject(TEXT("TargetPlayer"), _targetPlayer);
	StartReachabilityMonitor();

	UAISense_Hearing::ReportNoiseEvent(
		GetWorld(),
		GetPawn()->GetActorLocation(),
		1.0f,
		this,
		0.0f,
		TEXT("Spotted")
	);
}
