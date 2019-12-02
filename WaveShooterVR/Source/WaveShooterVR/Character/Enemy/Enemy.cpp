#pragma region project include
#include "Enemy.h"
#include "Character/Friend/Friend.h"
#include "Gameplay/Spawn/SpawnPoint.h"
#pragma endregion

#pragma region UE4 include
#include "Components/SplineComponent.h"
#include "Components/CapsuleComponent.h"
#include "Gameplay/Bullet/Bullet.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#pragma endregion

#pragma region constructor
// constructor
AEnemy::AEnemy()
{
	// enable update every frame
	PrimaryActorTick.bCanEverTick = true;

	// set replicated
	bReplicates = true;

	// add enemy tag
	Tags.Add("Enemy");
}
#pragma endregion

#pragma region public override function
// update every frame
void AEnemy::Tick(float DeltaTime)
{
	// parent update every frame
	Super::Tick(DeltaTime);

	// if server
	if (HasAuthority())
	{
		// if squared distance to target is lower than squared shot range
		if ((m_target - Capsule->GetComponentLocation()).SizeSquared() < ShotRange * ShotRange)
		{
			// increase shot timer
			m_shotTimer += DeltaTime;

			// if shot timer lower than shot rate return
			if (m_shotTimer < m_shotRate)
				return;

			// reset shot timer
			m_shotTimer = 0.0f;

			// spawn bullet
			ABullet* pBullet = GetWorld()->SpawnActor<ABullet>(BulletClass,
				SpawnPoint->GetComponentLocation(), Capsule->GetComponentRotation());

			// if bullet valid set owner of bullet to enemy
			if (pBullet)
				pBullet->SetOriginCharacter("Enemy");
			return;
		}

		// get temp position on spline from current position on spline
		float tempPositionOnSpline = m_positionOnSpline;

		// increase temp position by movement speed
		tempPositionOnSpline += DeltaTime * MovementSpeed;

		// calculate angle to target
		FRotator rotation = m_pSpline->GetRotationAtDistanceAlongSpline(tempPositionOnSpline, ESplineCoordinateSpace::World);

		// rotate capsule
		Capsule->SetWorldRotation(FRotator(0.0f, rotation.Yaw, 0.0f));

		// hit result from movement
		FHitResult hit;

		// get location at distance of spline
		FVector location = m_pSpline->GetLocationAtDistanceAlongSpline(tempPositionOnSpline, ESplineCoordinateSpace::World)
			+ FVector(0.0f, 0.0f, Capsule->GetUnscaledCapsuleHalfHeight());

		// add offset to location
		location += Capsule->GetRightVector() * m_offset;

		// try to move capsule to next location on spline and save hit result
		Capsule->SetWorldLocation(location, true, &hit);

		// if no blocking hit
		if (!hit.bBlockingHit)
			// set current position on spline to temp position
			m_positionOnSpline = tempPositionOnSpline;

		// increase net update timer
		m_netUpdateTimer += DeltaTime;

		// if net update timer lower than update frequency return
		if (m_netUpdateTimer < m_netUpdateFrequency)
			return;

		// reset net update timer
		m_netUpdateTimer = 0.0f;

		// set location to move to on client
		SetMoveTo_Client(Capsule->GetComponentLocation());
	}

	// if client
	else
	{
		// get direction to move to on x and y axis
		FVector direction = m_moveTo - Capsule->GetComponentLocation();
		direction.Z = 0;

		// if direction is lower than 5cm retrun
		if (direction.SizeSquared() < 25.0f)
			return;
		
		// normalize direction
		direction.Normalize();

		// add world offset of capusle in direction and rotate to direction
		Capsule->AddWorldOffset(direction * MovementSpeed * DeltaTime);
		Capsule->SetWorldRotation(FRotator(0.0f, UKismetMathLibrary::FindLookAtRotation(Capsule->GetComponentLocation(), m_moveTo).Yaw, 0.0f));
	}
}
#pragma endregion

#pragma region UFUNCTION
// set location to move to on client
void AEnemy::SetMoveTo_Client_Implementation(FVector _location)
{
	// if client set location to move to
	if (!HasAuthority())
		m_moveTo = _location;
}
#pragma endregion

#pragma region protected override function
// called at begin play
void AEnemy::BeginPlay()
{
	// parent begin play
	Super::BeginPlay();

	// if client return
	if (!HasAuthority())
		return;
	
	// array for found actors
	TArray<AActor*> FoundActors;

	// get all friend actors
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFriend::StaticClass(), FoundActors);

	// if friend array valid
	if (FoundActors.Num())
		// set target location to actor location of first friend
		m_target = FoundActors[0]->GetActorLocation();

	// get all spawn point actors
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnPoint::StaticClass(), FoundActors);

	// set distance to max value
	float distance = INFINITY;

	// check every spawn point
	for (int i = 0; i < FoundActors.Num(); i++)
	{
		// if distance to spawn point is lower than distance
		if ((FoundActors[i]->GetActorLocation() - GetActorLocation()).SizeSquared() < distance)
		{
			// set new closest distance value
			distance = (FoundActors[i]->GetActorLocation() - GetActorLocation()).SizeSquared();

			// save current spline reference
			m_pSpline = ((ASpawnPoint*)(FoundActors[i]))->Spline;
		}
	}

	// set actor location to spline
	SetActorLocation(FVector(m_pSpline->GetComponentLocation().X, m_pSpline->GetComponentLocation().Y,
		m_pSpline->GetComponentLocation().Z + 0.1f));
}
#pragma endregion