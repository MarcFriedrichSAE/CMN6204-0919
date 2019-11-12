#pragma region project include
#include "Bullet.h"
#include "Character/Base/BaseCharacter.h"
#pragma endregion

#pragma region UE4 include
#include "Components/StaticMeshComponent.h"
#include "Engine.h"
#pragma endregion

#pragma region constructor
// constructor
ABullet::ABullet()
{
	// enable update every frame
	PrimaryActorTick.bCanEverTick = true;

	// set replicated
	bReplicates = true;

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

	// move player forward by movement speed
	AddActorWorldOffset(GetActorForwardVector() * MovementSpeed * DeltaTime);

	// if client return
	if (!HasAuthority())
		return;

	// increase life time
	m_lifeTime += DeltaTime;

	// if life time of bullet is higher than ten seconds destroy bullet
	if (m_lifeTime > 10.0f)
		Destroy();
}
#pragma endregion

#pragma region UFUNCTION
// collide with other actor
void ABullet::Collide(AActor* OtherActor)
{
	// if client return
	if (!HasAuthority())
		return;

	// if other actor has enemy tag and origin is not enemy
	if (OtherActor->ActorHasTag("Enemy") && m_origin != "Enemy")
		// take damage on other actor
		((ABaseCharacter*)(OtherActor))->TakeCharacterDamage(34.0f);

	// if other actor has friend tag and origin is enemy
	else if ((OtherActor->ActorHasTag("Friend") && m_origin == "Enemy"))
		// take damage on other actor
		((ABaseCharacter*)(OtherActor))->TakeCharacterDamage(2.0f);

	// destroy bullet
	Destroy();
}
#pragma endregion