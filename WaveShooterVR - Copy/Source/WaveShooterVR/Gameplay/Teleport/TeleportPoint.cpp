#pragma region project include
#include "TeleportPoint.h"
#pragma endregion

#pragma region UE4 include
#include "Components/StaticMeshComponent.h"
#include "UnrealNetwork.h"
#pragma endregion

#pragma region constructor
// constructor
ATeleportPoint::ATeleportPoint()
{
	// disable update every frame
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

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

#pragma region network
/// <summary>
/// get lifetime replicated props of player
/// all properties that has to be replicated
/// </summary>
/// <param name="OutLifetimeProps">array of all properties that has to be replicated</param>
void ATeleportPoint::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	// parent get lifetime replicated props
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// add properties to array
	DOREPLIFETIME(ATeleportPoint, InUse);
}
#pragma endregion