#pragma region project include
#include "SpawnPlayer.h"
#pragma endregion

#pragma region UE4 include
#include "Components/SphereComponent.h"
#pragma endregion

#pragma region constructor
// constructor
ASpawnPlayer::ASpawnPlayer()
{
	// disable update every frame
	PrimaryActorTick.bCanEverTick = false;

	// create default sphere component and make root
	USphereComponent* pSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	RootComponent = pSphere;

	// disable collision of sphere
	pSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
#pragma endregion