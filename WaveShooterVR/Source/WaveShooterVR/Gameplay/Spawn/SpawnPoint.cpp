#pragma region project include
#include "SpawnPoint.h" 
#pragma endregion

#pragma region UE4 include
#include "Components/SphereComponent.h"
#include "Components/SplineComponent.h"
#pragma endregion

#pragma region constructor
// constructor
ASpawnPoint::ASpawnPoint()
{
	// disable update every frame
	PrimaryActorTick.bCanEverTick = false;

	// create default sphere component and make root
	USphereComponent* pSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	RootComponent = pSphere;

	// disable collision of sphere
	pSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// create default spline component and attach to root
	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	Spline->SetupAttachment(pSphere);
}
#pragma endregion