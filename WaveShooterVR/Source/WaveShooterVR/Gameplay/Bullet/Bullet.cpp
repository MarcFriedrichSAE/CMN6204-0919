#pragma region project include
#include "Bullet.h"
#pragma endregion

#pragma region UE4 include
#include "Components/StaticMeshComponent.h"
#include "Engine.h"
#pragma endregion

#pragma region constructor
// Sets default values
ABullet::ABullet()
{
	// enable update every frame
	PrimaryActorTick.bCanEverTick = true;

	// create default static mesh component and make root
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
}
#pragma endregion

#pragma region public override function
// update every frame
void ABullet::Tick(float DeltaTime)
{
	// parent update every frame
	Super::Tick(DeltaTime);

	// move bullet forward by movement speed
	AddActorWorldOffset(GetActorForwardVector() * MovementSpeed * DeltaTime);
}
#pragma endregion

#pragma region UFUNCTION
// collide with other actor
void ABullet::Collide(AActor* OtherActor)
{
	// log collide actor name on screen
	GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor::Red, OtherActor->GetName());

	// destroy bullet
	Destroy();
}
#pragma endregion