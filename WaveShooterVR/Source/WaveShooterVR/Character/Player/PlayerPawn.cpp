#pragma region project include
#include "PlayerPawn.h"
#include "Character/Base/BaseCharacter.h"
#include "Gameplay/Bullet/Bullet.h"
#include "Gameplay/Teleport/TeleportPoint.h"
#include "Gameplay/Spawn/SpawnPlayer.h"
#pragma endregion

#pragma region UE4 include
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "MotionControllerComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMeshActor.h"
#include "DrawDebugHelpers.h" // only for debug line
#pragma endregion

#pragma region constructor
// constructor
APlayerPawn::APlayerPawn()
{
	// enable update every frame
	PrimaryActorTick.bCanEverTick = true;
	
	// create default scene component and make root
	USceneComponent* pRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = pRoot;

	// create default camera component and attach to root
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(pRoot);

	// create default capsule component and attach to root
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetupAttachment(pRoot);

	// create default motion controller component and attach to root
	m_pLeftController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("LeftController"));
	m_pLeftController->SetupAttachment(pRoot);

	// create default static mesh component, attach to left controller and set collision profile to player
	LeftHand = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftHand"));
	LeftHand->SetupAttachment(m_pLeftController);
	LeftHand->SetCollisionProfileName("Player");

	// create default motion controller component, attach to root and set as right hand
	m_pRightController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightController"));
	m_pRightController->SetupAttachment(pRoot);
	m_pRightController->SetTrackingSource(EControllerHand::Right);

	// create default static mesh component, attach to right controller and set collision profile to player
	RightHand = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightHand"));
	RightHand->SetupAttachment(m_pRightController);
	RightHand->SetCollisionProfileName("Player");

	// create default scene component and attach to right controller
	SpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnPoint"));
	SpawnPoint->SetupAttachment(m_pRightController);

	// create default skeletal mesh component and attach to capsule
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Capsule);	

	// add player tag
	Tags.Add("Player");
}
#pragma endregion

#pragma region public override function
// update every frame
void APlayerPawn::Tick(float DeltaTime)
{
	// parent update
	Super::Tick(DeltaTime);

	// if local player
	if (IsLocallyControlled())
	{
		// add current right hand location
		m_rightHandLocations.Add(RightHand->GetComponentLocation());

		// if number of right hand location more than ten remove first location
		if (m_rightHandLocations.Num() > 10)
			m_rightHandLocations.RemoveAt(0);

		// increase net update timer
		m_netUpdateTimer += DeltaTime;

		// if net update timer lower than net update frequency return
		if (m_netUpdateTimer < m_netUpdateFrequency)
			return;

		// reset net update timer
		m_netUpdateTimer = 0.0f;

		// if server
		if (HasAuthority())
		{
			// set tracking devices transform on client
			SetTransforms_Client(Camera->RelativeLocation, Camera->RelativeRotation.Yaw,
				m_pLeftController->RelativeLocation, m_pLeftController->RelativeRotation,
				m_pRightController->RelativeLocation, m_pRightController->RelativeRotation);

			// move player on client
			Move_Client(GetActorLocation());
		}

		// if client
		else
		{
			// set tracking devices transform on server
			SetTransforms_Server(Camera->RelativeLocation, Camera->RelativeRotation.Yaw,
				m_pLeftController->RelativeLocation, m_pLeftController->RelativeRotation,
				m_pRightController->RelativeLocation, m_pRightController->RelativeRotation);

			// move player on server
			Move_Server(GetActorLocation());
		}
	}

	// if not local player
	else
	{
		// move capsule by direction to move to
		FVector direction = m_camMoveTo - Capsule->RelativeLocation;
		Capsule->AddRelativeLocation(direction * DeltaTime);

		// set capsule scale and location
		SetCapsule();

		// rotate capsule to angle
		float angle = m_camAngleRotTo - Capsule->RelativeRotation.Yaw;
		Capsule->AddRelativeRotation(FRotator(0.0f, angle * DeltaTime, 0.0f));

		// move left controller to location to move to
		direction = m_leftMoveTo - m_pLeftController->RelativeLocation;
		m_pLeftController->AddRelativeLocation(direction * DeltaTime);

		// move right controller to location to move to
		direction = m_rightMoveTo - m_pRightController->RelativeLocation;
		m_pRightController->AddRelativeLocation(direction * DeltaTime);

		// rotate left controller to rotation to rotate to
		FRotator rotateDirection = m_leftRotTo - m_pLeftController->RelativeRotation;
		m_pLeftController->AddRelativeRotation(rotateDirection * DeltaTime);

		// rotate right controller to rotation to rotate to
		rotateDirection = m_rightRotTo - m_pRightController->RelativeRotation;
		m_pRightController->AddRelativeRotation(rotateDirection * DeltaTime);

		// move player to location to move to
		direction = m_actMoveTo - GetActorLocation();
		AddActorWorldOffset(direction * DeltaTime);
	}

	// if show teleport not active return
	if (!m_teleportActive)
		return;

	// check every teleport
	for (int i = 0; i < m_teleports.Num(); i++)
	{
		// if current teleport is active teleport or teleport allready in use
		if (m_teleports[i] == m_pActiveTeleport || m_teleports[i]->InUse)
		{
			// hide teleport and disable collision
			m_teleports[i]->SetActorHiddenInGame(true);
			m_teleports[i]->Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}

		// if current teleport not equal active teleport or not in use
		else
		{
			// show current teleport and enable collision
			m_teleports[i]->SetActorHiddenInGame(false);
			m_teleports[i]->Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
	}

	// hit result
	FHitResult hit;

	// get target to trace to
	FVector target = m_pLeftController->GetComponentLocation() + m_pLeftController->GetForwardVector() * 10000.0f;

	// debug line to visualize teleport target
	DrawDebugLine(GetWorld(), m_pLeftController->GetComponentLocation(), target, FColor::Blue, false, 2.0f * DeltaTime, 0, 1.0f);

	// if trace from camera to target hit nothing
	if (!GetWorld()->LineTraceSingleByChannel(hit, m_pLeftController->GetComponentLocation(), target, ECollisionChannel::ECC_Camera))
	{
		// if teleport target valid reset scale
		if (m_pTeleportTarget)
			m_pTeleportTarget->SetActorScale3D(FVector::OneVector);

		// reset teleport target reference
		m_pTeleportTarget = nullptr;
		return;
	}

	// if not actor hit
	if (!hit.GetActor())
	{
		// if teleport target valid reset scale
		if (m_pTeleportTarget)
			m_pTeleportTarget->SetActorScale3D(FVector::OneVector);

		// reset teleport target reference
		m_pTeleportTarget = nullptr;
		return;
	}

	// if hit actor is no teleport
	if (!hit.GetActor()->ActorHasTag("Teleport"))
	{
		// if teleport target valid reset scale
		if (m_pTeleportTarget)
			m_pTeleportTarget->SetActorScale3D(FVector::OneVector);

		// reset teleport target reference
		m_pTeleportTarget = nullptr;
		return;
	}

	// if teleport target is not equal hit actor
	if (m_pTeleportTarget != hit.GetActor())
	{
		// scale hit actor by two
		hit.GetActor()->SetActorScale3D(FVector(2.0f, 2.0f, 2.0f));

		// if teleport target valid reset scale
		if (m_pTeleportTarget)
			m_pTeleportTarget->SetActorScale3D(FVector::OneVector);

		// set teleport target to hit actor
		m_pTeleportTarget = (ATeleportPoint*)hit.GetActor();
	}
}
#pragma endregion

#pragma region UFUNCTION
// move player
void APlayerPawn::Move(float ForwardBack, float LeftRight)
{
	// if not local player return
	if (!IsLocallyControlled())
		return;

	// set capsule to camera
	Capsule->SetRelativeLocation(Camera->RelativeLocation);

	// reset scale of capsule
	Capsule->SetRelativeScale3D(FVector::OneVector);

	// set capsule half height depending on camera location z
	Capsule->SetCapsuleHalfHeight(0.5f * Camera->RelativeLocation.Z);

	// add capsule location to negative half height
	Capsule->AddRelativeLocation(FVector(0.0f, 0.0f, -Capsule->GetUnscaledCapsuleHalfHeight() + 1.0f));

	// calculate direction to move to
	FVector dir = Camera->GetForwardVector() * ForwardBack;
	dir += Camera->GetRightVector() * LeftRight;
	dir *= GetWorld()->GetDeltaSeconds() * MovementSpeed;
	dir.Z = 0.0f;

	// hit result of movement
	FHitResult hit;

	// try to add world offset by direction
	Capsule->AddWorldOffset(dir, true, &hit);

	// if movement hit anything return
	if (hit.bBlockingHit)
		return;

	// move actor to new location
	AddActorWorldOffset(dir);

	// move capsule back
	Capsule->AddWorldOffset(-dir);
}

// shoot a bullet
void APlayerPawn::Shoot()
{
	// has not to be because input is allready only on local client
	if (!IsLocallyControlled())
		return;

	// if valid weapon target, target is range weapon and range not active
	if (m_pWeaponTarget && m_pWeaponTarget->ComponentHasTag("Range") && !m_range)
	{
		// set range active true
		m_range = true;

		// set staic mesh of right hand to target
		RightHand->SetStaticMesh(m_pWeaponTarget->GetStaticMesh());

		// set transform of mesh only for now, mesh transform has to be correct
		RightHand->SetRelativeLocation(FVector::ZeroVector);
		RightHand->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
		RightHand->SetRelativeScale3D(FVector(0.2f, 0.2f, 0.2f));
		return;
	}

	// if valid weapon target, target is melee weapon and range active
	else if (m_pWeaponTarget && m_pWeaponTarget->ComponentHasTag("Melee") && m_range)
	{
		// set range active false
		m_range = false;

		// set staic mesh of right hand to target
		RightHand->SetStaticMesh(m_pWeaponTarget->GetStaticMesh());

		// set transform of mesh only for now, mesh transform has to be correct
		RightHand->SetRelativeLocation(FVector(50.0f, 0.0f, 0.0f));
		RightHand->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
		RightHand->SetRelativeScale3D(FVector(0.1f, 0.1f, 1.0f));
	}

	// if range not active return
	if (!m_range)
		return;

	// if server
	if (HasAuthority())
	{
		// spawn bullet
		ABullet* pBullet = GetWorld()->SpawnActor<ABullet>(BulletClass,
			SpawnPoint->GetComponentLocation(), SpawnPoint->GetComponentRotation());

		// if bullet valid set owner of bullet to player
		if(pBullet)
			pBullet->SetOriginCharacter("Player");
	}

	// if client
	else
	{
		// shoot bullet on server
		Shoot_Server(SpawnPoint->GetComponentLocation(), SpawnPoint->GetComponentRotation());
	}
}

// show teleport locations
void APlayerPawn::ShowTeleport()
{
	// set teleport active true
	m_teleportActive = true;
}

// teleport to location
void APlayerPawn::Teleport()
{
	// if server
	if (HasAuthority())
	{
		// if teleport target valid and not in use
		if (m_pTeleportTarget && !m_pTeleportTarget->InUse)
		{
			// if current teleport valid
			if(m_pActiveTeleport)
				// set current teleport not in use
				m_pActiveTeleport->InUse = false;

			// teleport client
			Teleport_Client(m_pTeleportTarget);

			// set teleport target in use
			m_pTeleportTarget->InUse = true;
		}
	}

	// if client and teleport target valid
	else if(m_pTeleportTarget)
	{
		// teleport player on server
		Teleport_Server(m_pTeleportTarget, m_pActiveTeleport);
	}

	// check ever teleport
	for (int i = 0; i < m_teleports.Num(); i++)
	{
		// reset scale and hide current teleport
		m_teleports[i]->SetActorScale3D(FVector::OneVector);
		m_teleports[i]->SetActorHiddenInGame(true);
	}

	// reset teleport target
	m_pTeleportTarget = nullptr;

	// set show teleport active false
	m_teleportActive = false;
}

// set weapon switch target
void APlayerPawn::SetWeaponTarget(UStaticMeshComponent* OtherComp)
{
	// set weapon target to other component
	m_pWeaponTarget = OtherComp;
}

// collide weapon with other actor
void APlayerPawn::CollideWeapon(AActor* OtherActor)
{
	// if other actor is enemy and range not active
	if (OtherActor->ActorHasTag("Enemy") && !m_range)
		// take damage on other actor
		((ABaseCharacter*)(OtherActor))->TakeCharacterDamage(10.0f);
}

// spawn cube
void APlayerPawn::SpawnCube(TSubclassOf<AStaticMeshActor> Cube, float Force)
{
	// if not local player return
	if (!IsLocallyControlled())
		return;

	// spawn subclass at spawn point
	AStaticMeshActor* pCube = GetWorld()->SpawnActor<AStaticMeshActor>(Cube, SpawnPoint->GetComponentLocation(), SpawnPoint->GetComponentRotation());

	// direction to throw
	FVector forceDirection;

	// add every saved right hand location
	for (FVector vec : m_rightHandLocations)
		forceDirection += vec;

	// calculate average direction
	forceDirection /= m_rightHandLocations.Num();

	// calculate correct direction from hand to target and inverse
	forceDirection -= RightHand->GetComponentLocation();
	forceDirection *= -1;

	// add impulse to direction by force
	pCube->GetStaticMeshComponent()->AddImpulse(forceDirection * Force);
}

// spawn player on client implementation
void APlayerPawn::SpawnPlayer_Client_Implementation(FVector _location)
{
	// set actor location to spawn point
	SetActorLocation(_location);
}

// move player on server implementation
void APlayerPawn::Move_Server_Implementation(FVector _location)
{
	// move player on client
	Move_Client(_location);
}

// move player on server validate
bool APlayerPawn::Move_Server_Validate(FVector _location)
{
	return true;
}

// move player on client implementation
void APlayerPawn::Move_Client_Implementation(FVector _location)
{
	// if not local player
	if (!IsLocallyControlled())
		// set world location of capsule
		m_actMoveTo = _location;
}

// set tracking devices transform on server validate
bool APlayerPawn::SetTransforms_Server_Validate(FVector _locationCam, float _angle,
	FVector _locationLeft, FRotator _rotationLeft, FVector _locationRight, FRotator _rotationRight)
{
	return true;
}

// set tracking devices transform on server implementation
void APlayerPawn::SetTransforms_Server_Implementation(FVector _locationCam, float _angle,
	FVector _locationLeft, FRotator _rotationLeft, FVector _locationRight, FRotator _rotationRight)
{
	// set tracking devices transform on client
	SetTransforms_Client(_locationCam, _angle, _locationLeft, _rotationLeft, _locationRight, _rotationRight);
}

// set tracking devices transform on client implementation
void APlayerPawn::SetTransforms_Client_Implementation(FVector _locationCam, float _angle,
	FVector _locationLeft, FRotator _rotationLeft, FVector _locationRight, FRotator _rotationRight)
{
	// if local player return
	if (IsLocallyControlled())
		return;

	// set locations to move to and rotation to rotate to
	m_camMoveTo = _locationCam - FVector(0.0f, 0.0f, Capsule->GetScaledCapsuleHalfHeight());
	m_camAngleRotTo = _angle;
	m_leftMoveTo = _locationLeft;
	m_rightMoveTo = _locationRight;
	m_leftRotTo = _rotationLeft;
	m_rightRotTo = _rotationRight;
}

// rotate player on server implementation
void APlayerPawn::Rotate_Server_Implementation(float _angle)
{
	// rotate player on client
	Rotate_Client(_angle);
}

// rotate player on server validate
bool APlayerPawn::Rotate_Server_Validate(float _angle)
{
	return true;
}

// rotate player on client implementation
void APlayerPawn::Rotate_Client_Implementation(float _angle)
{
	// if not local player
	if (!IsLocallyControlled())
		// set world rotation of capsule
		Capsule->SetWorldRotation(FRotator(0.0f, _angle, 0.0f));
}

// shoot bullet on server implementation
void APlayerPawn::Shoot_Server_Implementation(FVector _location, FRotator _rotation)
{
	// spawn bullet
	ABullet* pBullet = GetWorld()->SpawnActor<ABullet>(BulletClass,
		_location, _rotation);

	// if bullet valid set owner of bullet to player
	if(pBullet)
		pBullet->SetOriginCharacter("Player");
}

// shoot bullet on server validate
bool APlayerPawn::Shoot_Server_Validate(FVector _location, FRotator _rotation)
{
	return true;
}

// teleport player on server implementation
void APlayerPawn::Teleport_Server_Implementation(ATeleportPoint* _pTeleport, ATeleportPoint* _pCurrent)
{
	// if teleport target in use return
	if (_pTeleport->InUse)
		return;

	// if current teleport target valid
	if(_pCurrent)
		// set in use of current teleport target false
		_pCurrent->InUse = false;

	// teleport player on client
	Teleport_Client(_pTeleport);

	// set teleport target in use
	_pTeleport->InUse = true;

	// set location of player to teleport and increase z value by capsule half height
	SetActorLocation(_pTeleport->GetActorLocation());

	// reset capsule relative location
	FVector offset = _pTeleport->GetActorLocation() - Camera->GetComponentLocation();
	offset.Z = 0.0f;
	AddActorWorldOffset(offset);

	// set active teleport to teleport target
	m_pActiveTeleport = _pTeleport;
}

// teleport player on server validate
bool APlayerPawn::Teleport_Server_Validate(ATeleportPoint* _pTeleport, ATeleportPoint* _pCurrent)
{
	return true;
}

// teleport player on client implementation
void APlayerPawn::Teleport_Client_Implementation(ATeleportPoint* _pTeleport)
{
	// set location of player to teleport and increase z value by capsule half height
	SetActorLocation(_pTeleport->GetActorLocation());

	// reset capsule relative location
	FVector offset = _pTeleport->GetActorLocation() - Camera->GetComponentLocation();
	offset.Z = 0.0f;
	AddActorWorldOffset(offset);

	// set active teleport to teleport target
	m_pActiveTeleport = _pTeleport;
}
#pragma endregion

#pragma region protected override function
// called at begin play
void APlayerPawn::BeginPlay()
{
	// parent begin play
	Super::BeginPlay();

	// array of found actors
	TArray<AActor*> FoundActors;

	// get all teleports
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATeleportPoint::StaticClass(), FoundActors);

	// add teleport to array
	for (int i = 0; i < FoundActors.Num(); i++)
		m_teleports.Add((ATeleportPoint*)FoundActors[i]);

	// if client return
	if (!HasAuthority())
		return;

	// get all spawn points
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnPlayer::StaticClass(), FoundActors);

	// while valid spawn point found
	while (true)
	{
		// get random spawn point
		ASpawnPlayer* pSpawn = (ASpawnPlayer*)FoundActors[FMath::RandRange(0, FoundActors.Num() - 1)];

		// if spawn point in use continue
		if (pSpawn->GetInUse())
			continue;

		// set current spawn point in use
		pSpawn->SetInUse(true);

		// spawn client at current spawn point
		SpawnPlayer_Client(pSpawn->GetActorLocation());
		break;
	}
}
#pragma endregion

#pragma region private function
// set scale and location of capsule
void APlayerPawn::SetCapsule()
{
	// if local player return
	if (IsLocallyControlled())
		return;

	// set capsule half height to 50 and scale depending on half height and camera location z
	Capsule->SetCapsuleHalfHeight(50.0f);
	Capsule->SetRelativeScale3D(FVector(1.0f, 1.0f, (Capsule->RelativeLocation.Z + Capsule->GetScaledCapsuleHalfHeight()) / 100.0f));

	// set relative location of capsule on z by scaled half height
	Capsule->SetRelativeLocation(FVector(Capsule->RelativeLocation.X, Capsule->RelativeLocation.Y, Capsule->GetScaledCapsuleHalfHeight()));

	// set mesh scale and location depending on capsule
	Mesh->SetRelativeScale3D(FVector(1.0f, 1.0f, 0.55f));
	Mesh->SetRelativeLocation(FVector(Mesh->RelativeLocation.X, Mesh->RelativeLocation.Y, -Capsule->GetUnscaledCapsuleHalfHeight()));
}
#pragma endregion
