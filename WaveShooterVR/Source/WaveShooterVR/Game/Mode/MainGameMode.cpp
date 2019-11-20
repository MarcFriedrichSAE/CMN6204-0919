#pragma region project include
#include "MainGameMode.h"
#include "Character/Enemy/Enemy.h"
#include "Gameplay/Spawn/SpawnPoint.h"
#pragma endregion

#pragma region UE4 include
#include "Engine.h"
#include "Kismet/GameplayStatics.h"
#pragma endregion

#pragma region constructor
// constructor
AMainGameMode::AMainGameMode()
{
	// enable update every frame
	PrimaryActorTick.bCanEverTick = true;
}
#pragma endregion

#pragma region public override function
// update every frame
void AMainGameMode::Tick(float DeltaTime)
{
	// parent update every frame
	Super::Tick(DeltaTime);

	// if no wave left return
	if (!Waves.Num())
		return;

	// increase spawn timer
	m_spawnTime += DeltaTime;

	// if wave not started and spawn timer higher than wave spawn time
	if (!m_waveStarted && m_spawnTime > Waves[0].WaveStartTime)
		// set wave started true
		m_waveStarted = true;

	// if spawn timer lower than current wave spawn time return
	if (m_spawnTime < Waves[0].SpawnTime)
		return;

	// reset spawn timer
	m_spawnTime = 0.0f;

	// get random spawn location
	FVector spawnPos = m_spawnPoints[FMath::RandRange(0, m_spawnPoints.Num() - 1)];
	spawnPos.Z += 200.0f;

	// spawn enemy at spawn location
	AEnemy* pEnemy = (AEnemy*)(GetWorld()->SpawnActor<ABaseCharacter>(Waves[0].EnemyClass,
		spawnPos, FRotator()));

	// set offset to spline
	pEnemy->SetOffset(FMath::RandRange(-200.0f, 200.0f));

	// decrease enemy count of current wave
	Waves[0].Count--;

	// if enemies left return
	if (Waves[0].Count)
		return;

	// remove first wave
	Waves.RemoveAt(0);
}
#pragma endregion

#pragma region protected override function
// called at begin play
void AMainGameMode::BeginPlay()
{
	// parent begin play
	Super::BeginPlay();

	// get all spawn points
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnPoint::StaticClass(), FoundActors);

	// check every spawnpoint and add location to array
	for (int i = 0; i < FoundActors.Num(); i++)
		m_spawnPoints.Add(FoundActors[i]->GetActorLocation());
}
#pragma endregion