#pragma region project include
#include "TeleportPoint.h"
#pragma endregion

#pragma region UE4 include
#include "Components/StaticMeshComponent.h"
#pragma endregion

#pragma region constructor
// constructor
ATeleportPoint::ATeleportPoint()
{
	// disable update every frame
	PrimaryActorTick.bCanEverTick = false;

	// create default scene component and make root
	USceneComponent* pRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = pRoot;

	// create default static mesh component and attach to root
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(pRoot);

	// add teleport target
	Tags.Add("Teleport");
}
#pragma endregion