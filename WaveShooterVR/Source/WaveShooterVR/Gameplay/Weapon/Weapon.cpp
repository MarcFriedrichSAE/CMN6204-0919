#pragma region project include
#include "Weapon.h"
#pragma endregion

#pragma region UE4 include
#include "Components/StaticMeshComponent.h"
#pragma endregion

#pragma region constructor
// Sets default values
AWeapon::AWeapon()
{
	// disable update every frame
	PrimaryActorTick.bCanEverTick = false;

	// create default scene component and make root
	USceneComponent* pRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = pRoot;

	// create default static mesh component and attach to root
	RangeWeapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RangeWeapon"));
	RangeWeapon->SetupAttachment(pRoot);

	// set tage and collision profile
	RangeWeapon->ComponentTags.Add("Range");
	RangeWeapon->SetCollisionProfileName("Bullet");

	// create default static mesh component and attach to root
	MeleeWeapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeleeWeapon"));
	MeleeWeapon->SetupAttachment(pRoot);

	// set tage and collision profile
	MeleeWeapon->ComponentTags.Add("Melee");
	MeleeWeapon->SetCollisionProfileName("Bullet");
}
#pragma endregion