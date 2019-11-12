#pragma region project include
#include "PlayerPawn.h"
#include "Gameplay/Bullet/Bullet.h"
#include "Gameplay/Teleport/TeleportPoint.h"
#include "Gameplay/Spawn/SpawnPlayer.h"
#pragma endregion

#pragma region UE4 include
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
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

	// create default capsule component and attach to root
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetupAttachment(pRoot);

	// create default scene component and attach to capsule
	SpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnPoint"));
	SpawnPoint->SetupAttachment(Capsule);

	// create default skeletal mesh component and attach to capsule
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Capsule);

	// create default scene component and attach to capsule
	CameraRoot = CreateDefaultSubobject<USceneComponent>(TEXT("CameraRoot"));
	CameraRoot->SetupAttachment(Capsule);

	// create default camera component and attach to camera root
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(CameraRoot);

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
	FVector target = Camera->GetComponentLocation() + Camera->GetForwardVector() * 10000.0f;

	// if trace from camera to target hit nothing
	if (!GetWorld()->LineTraceSingleByChannel(hit, Camera->GetComponentLocation(), target, ECollisionChannel::ECC_Camera))
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

	// calculate direction to move to
	FVector dir = Capsule->GetForwardVector() * ForwardBack;
	dir += Capsule->GetRightVector() * LeftRight;
	dir *= GetWorld()->GetDeltaSeconds() * MovementSpeed;

	// try to add world offset with collision detection
	Capsule->AddWorldOffset(dir, true);

	// if server move player on client
	if (HasAuthority())
		Move_Client(Capsule->GetComponentLocation());
	
	// if client move player on server
	else
		Move_Server(Capsule->GetComponentLocation());
}

// rotate player and camera
void APlayerPawn::Rotate(float LeftRight, float UpDown)
{
	// if not local player return
	if (!IsLocallyControlled())
		return;

	// calculate yaw rotation and add to capsule
	FRotator rotation = FRotator(0.0f, LeftRight * GetWorld()->GetDeltaSeconds() * RotationSpeed, 0.0f);
	Capsule->AddLocalRotation(rotation);

	// calculate pitch rotation
	rotation = FRotator(UpDown * GetWorld()->GetDeltaSeconds() * RotationSpeed, 0.0f, 0.0f);

	// if pitch rotation is out of bounds return
	if (CameraRoot->RelativeRotation.Pitch + rotation.Pitch > 45.0f || CameraRoot->RelativeRotation.Pitch + rotation.Pitch < -45.0f)
		return;

	// add pitch rotation
	CameraRoot->AddLocalRotation(rotation);

	// if server move player on client
	if (HasAuthority())
		Rotate_Client(Capsule->GetComponentRotation().Yaw);

	// if client move player on server
	else
		Rotate_Server(Capsule->GetComponentRotation().Yaw);
}

// shoot a bullet
void APlayerPawn::Shoot()
{
	// has not to be because input is allready only on local client
	if (!IsLocallyControlled())
		return;

	// if server
	if (HasAuthority())
	{
		// spawn bullet
		ABullet* pBullet = GetWorld()->SpawnActor<ABullet>(BulletClass,
			SpawnPoint->GetComponentLocation(), CameraRoot->GetComponentRotation());

		// if bullet valid set owner of bullet to player
		if(pBullet)
			pBullet->SetOriginCharacter("Player");
	}

	// if client
	else
	{
		// shoot bullet on server
		Shoot_Server(SpawnPoint->GetComponentLocation(), CameraRoot->GetComponentRotation());
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

// spawn player on client implementation
void APlayerPawn::SpawnPlayer_Client_Implementation(FVector _location)
{
	// set actor location to spawn point
	SetActorLocation(_location);

	// add actor offset by half height of capsule
	AddActorWorldOffset(FVector(0.0f, 0.0f, Capsule->GetScaledCapsuleHalfHeight() * 1.01f));
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
	if(!IsLocallyControlled())
		// set world location of capsule
		Capsule->SetWorldLocation(_location);
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
	AddActorWorldOffset(FVector(0.0f, 0.0f, Capsule->GetScaledCapsuleHalfHeight() * 1.01f));

	// reset capsule relative location
	Capsule->SetRelativeLocation(FVector::ZeroVector);

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