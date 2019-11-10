#pragma region project include
#include "BaseCharacter.h"
#pragma endregion

#pragma region UE4 include
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#pragma endregion

#pragma region constructor
// constructor
ABaseCharacter::ABaseCharacter()
{
	// enable update every frame
	PrimaryActorTick.bCanEverTick = true;

	// create default scene component and make root
	USceneComponent* pRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = pRoot;

	// create default capsule component and attach to root
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetupAttachment(pRoot);

	// create default skeletal mesh component and attach to capsule
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Capsule);

	// create default scene component and attach to capsule
	SpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnPoint"));
	SpawnPoint->SetupAttachment(Capsule);
}
#pragma endregion

#pragma region public function
// take damage
void ABaseCharacter::TakeCharacterDamage(float _damage)
{
	// decrease health by damage
	m_health -= _damage;

	// update health visual
	UpdateHealth(m_health);

	// if health over zero return
	if (m_health > 0.0f)
		return;

	// destroy character
	Destroy();
}
#pragma endregion