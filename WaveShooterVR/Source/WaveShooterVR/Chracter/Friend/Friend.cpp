#pragma region project include
#include "Friend.h" 
#pragma endregion

#pragma region UE4 include
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#pragma endregion

#pragma region constructor
// constructor
AFriend::AFriend()
{
	// disable update every frame
	PrimaryActorTick.bCanEverTick = false;

	// add friend tag
	Tags.Add("Friend");
}
#pragma endregion

#pragma region public override function
// called after actor was destroyed
void AFriend::Destroyed()
{
	// parent actor was destroyed
	Super::Destroyed();

	// load main level again
	UGameplayStatics::OpenLevel(GetWorld(), "Main");
}
#pragma endregion