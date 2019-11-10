#pragma region project include
#include "PlayerPawn.h"
#include "Gameplay/Bullet/Bullet.h"
#include "Gameplay/Teleport/TeleportPoint.h"
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
	// enable update
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
	// calculate direction to move to
	FVector dir = Capsule->GetForwardVector() * ForwardBack;
	dir += Capsule->GetRightVector() * LeftRight;
	dir *= GetWorld()->GetDeltaSeconds() * MovementSpeed;

	// try to add world offset with collision detection
	Capsule->AddWorldOffset(dir, true);
}

// rotate player and camera
void APlayerPawn::Rotate(float LeftRight, float UpDown)
{
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
}

// shoot a bullet
void APlayerPawn::Shoot()
{
	// spawn bullet
	ABullet* pBullet = GetWorld()->SpawnActor<ABullet>(BulletClass, 
		SpawnPoint->GetComponentLocation(), CameraRoot->GetComponentRotation());

	// set owner of bullet to player
	pBullet->SetOriginCharacter("Player");
}

// show teleport locations
void APlayerPawn::ShowTeleport()
{
	// set teleport active true
	m_teleportActive = true;

	// check every teleport
	for (int i = 0; i < m_teleports.Num(); i++)
	{
		// if current teleport is active teleport
		if (m_teleports[i] == m_pActiveTeleport)
		{
			// disable collision of current teleport
			m_teleports[i]->Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}

		// if current teleport not equal active teleport
		else
		{
			// show current teleport and enable collision
			m_teleports[i]->SetActorHiddenInGame(false);
			m_teleports[i]->Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
	}
}

// teleport to location
void APlayerPawn::Teleport()
{
	// if teleport target valid
	if (m_pTeleportTarget)
	{
		// set location of player to teleport and increase z value by capsule half height
		SetActorLocation(m_pTeleportTarget->GetActorLocation());
		AddActorWorldOffset(FVector(0.0f, 0.0f, Capsule->GetScaledCapsuleHalfHeight() * 1.01f));

		// reset capsule relative location
		Capsule->SetRelativeLocation(FVector::ZeroVector);

		// set active teleport to teleport target
		m_pActiveTeleport = m_pTeleportTarget;
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
#pragma endregion

#pragma region protected override function
// called at begin play
void APlayerPawn::BeginPlay()
{
	// parent begin play
	Super::BeginPlay();

	// get all teleports
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATeleportPoint::StaticClass(), FoundActors);

	// add teleport to array
	for (int i = 0; i < FoundActors.Num(); i++)
		m_teleports.Add((ATeleportPoint*)FoundActors[i]);
}
#pragma endregion