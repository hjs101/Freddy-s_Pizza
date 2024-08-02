// Fill out your copyright notice in the Description page of Project Settings.


#include "SB/Foxy.h"
#include "SB/LocationState.h"
#include "SB/AILevel.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "HJS/FreddyPlayer.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "SB/FoxDoll.h"

// Sets default values
AFoxy::AFoxy()
{
	PrimaryActorTick.bCanEverTick = true;

	AILevelComp = CreateDefaultSubobject<UAILevel>(TEXT("AILevelComp"));
}

// Called when the game starts or when spawned
void AFoxy::BeginPlay()
{
	Super::BeginPlay();
	
	UE_LOG(LogTemp, Warning, TEXT("Game Start"));

	TagArr.Add(FVector(0, 0, 0)); //[0]
	TagArr.Add(FindActorsWithTag(FName("Room1"))); //[1]
	TagArr.Add(FindActorsWithTag(FName("Room2"))); //[2]
	TagArr.Add(FindActorsWithTag(FName("Room3"))); //[3]
	TagArr.Add(FindActorsWithTag(FName("Room4"))); //[4]
	TagArr.Add(FindActorsWithTag(FName("Room5"))); //[5]
	TagArr.Add(FindActorsWithTag(FName("Room6"))); //[6]
	TagArr.Add(FindActorsWithTag(FName("Room7"))); //[7]
	TagArr.Add(FindActorsWithTag(FName("Room8"))); //[8]
	TagArr.Add(FindActorsWithTag(FName("Closet"))); //[9]
	TagArr.Add(FindActorsWithTag(FName("Bed"))); //[10]

	UE_LOG(LogTemp, Warning, TEXT("Room array complete"));

	CurrentState = ELocationState::IDLE;
}

void AFoxy::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void AFoxy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ��� ���� Ȯ�� (AlLevel �� �ִ� SetLevel())
	AILevelComp->SetLevel(this);
	// ��� State Ȯ��
	UpdateState(DeltaTime);

	// ��� ������On/Off, �� Open/Close Ȯ��
	FlashOn();
	DoorOpen();
}

void AFoxy::SetUpLocation(ELocationState State, float DeltaTime)
{
	if (CurrentState != State)
	{
		CurrentState = State;

		switch (State)
		{
		case ELocationState::IDLE:	Idle(DeltaTime);
			break;
		case ELocationState::MOVE:	Move();
			break;
		case ELocationState::ATTACK:	Attack();
			break;

		default:
			break;
		}
	}
}

void AFoxy::UpdateState(float DeltaTime)
{
	switch (CurrentState)
	{
	case ELocationState::IDLE:	Idle(DeltaTime);
		break;
	case ELocationState::MOVE:	Move();
		break;
	case ELocationState::ATTACK:	Attack();
		break;

	default:
		break;
	}
}

// Called to bind functionality to input
void AFoxy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AFoxy::Idle(float DeltaTime)
{
	// ���� ��ġ == room1 || room3 || room4 || room6 || room8 ����

	// 4.98�ʸ��� AILevel�� �ִ� RandomMove() ȣ�� && Move�� ��������
	CurrentTime += DeltaTime;

	if (CurrentTime > MovableTime) // �̵� ������ �ð��� �Ǹ�
	{
		// RandomMove�� true�� ���� move
		if (AILevelComp->RandomMove(this, DeltaTime) == true)
		{

			if (RoomNum != 5 && RoomNum != 6 && RoomNum != 9) // room5, room6�� �ƴ� ���� Move()
			{
				CurrentState = ELocationState::MOVE;
			}
			// room5 -> room1 || room9 ����
			else if (RoomNum == 5)
			{
				AFreddyPlayer* FreddyPlayer = Cast<AFreddyPlayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
				AFreddyPlayer::LookAt LookState;
				
				// �߼Ҹ�
				UGameplayStatics::PlaySound2D(this, FootStepsSFX);

				//	����, ������ ON -> room1�� �̵� (�����̵�X)
				if (bIsFlashlightOn == true)
				{
					SetActorLocation(TagArr[1]);
					RoomNum = 1;
				}

				if (FreddyPlayer) // �÷��̾�� ������ �κ�
				{
					LookState = FreddyPlayer->GetLookAtState();
					
					// (�÷��̾� ��ġ == ������Door || �÷��̾� ��ġ == ħ��) && �̵� �ð��� �Ǹ�, 9�� �̵�
					if (LookState == AFreddyPlayer::LookAt::Right || LookState == AFreddyPlayer::LookAt::Bed)
					{
						CurrentTime += DeltaTime;
						if (CurrentTime > MovableTime)
						{
							SetActorLocation(TagArr[9]);
							RoomNum = 9;
						}
					}
				}

				CurrentState = ELocationState::IDLE;
			}
			// room6 -> room1 || room9 ����
			else if (RoomNum == 6)
			{
				AFreddyPlayer* FreddyPlayer = Cast<AFreddyPlayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
				AFreddyPlayer::LookAt LookState;

				// �߼Ҹ�
				UGameplayStatics::PlaySound2D(this, FootStepsSFX);

				//	����, ������ ON -> room1�� �̵� (�����̵�X)
				if (bIsFlashlightOn == true)
				{
					SetActorLocation(TagArr[1]);
					RoomNum = 1;
				}

				if (FreddyPlayer) // �÷��̾�� ������ �κ�
				{
					LookState = FreddyPlayer->GetLookAtState();

					// (�÷��̾� ��ġ == ����Door || �÷��̾� ��ġ == ħ��) && �̵� �ð��� �Ǹ�, 9�� �̵�
					if (LookState == AFreddyPlayer::LookAt::Left || LookState == AFreddyPlayer::LookAt::Bed)
					{
						CurrentTime += DeltaTime;
						if (CurrentTime > MovableTime)
						{
							SetActorLocation(TagArr[9]);
							RoomNum = 9;
						}
					}
				}

				CurrentState = ELocationState::IDLE;
			}
			// room9 �� ��
			else if (RoomNum == 9)
			{
				AFreddyPlayer* FreddyPlayer = Cast<AFreddyPlayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
				AFreddyPlayer::LookAt LookState;

				AActor * FoxDollInstance = UGameplayStatics::GetActorOfClass(GetWorld(), AFoxDoll::StaticClass());

				ShowFoxyDoll(FoxDollInstance, false); // ���� ������ �� �� ���̰�

				if (FreddyPlayer)
				{
					LookState = FreddyPlayer->GetLookAtState();

					// �÷��̾� ��ġ == ���, ������ ��¦ ������
					if (LookState == AFreddyPlayer::LookAt::Main && bClosetAnim == false)
					{  
						bClosetAnim = true;
						// ���� �����̴� anim(Loop�� ��)
						UE_LOG(LogTemp, Log, TEXT("Closet door Move !!"));
						
						CurrentState = ELocationState::IDLE; // �Ʒ� if�� �����ϱ� ����
					}

					// �÷��̾� ��ġ == CLOSET, �Ű�� anim ����
					else if (LookState == AFreddyPlayer::LookAt::Center)
					{
						if (bCTtoZero == true) // �ݺ������� currentTime�� 0�� ���� �ʵ���
						{
							CurrentTime = 0.f;
							bCTtoZero = false;
						}

						// �Ű�⸸ �ϴ� anim ����
						UE_LOG(LogTemp, Log, TEXT("Foxy anim"));

						// Door: bISCLOSED == false, 2�� �� ���������� (����) �� GAME OVER
						if (bIsDoorClose == false)
						{
							CurrentTime += DeltaTime;
							if (CurrentTime > 2.f)
							{
								CurrentState = ELocationState::ATTACK;
							}
							CurrentTime = 0.f;
						}
						// Door: bISCLOSED == true (���� ����� �ð�����), �� �ڸ��� ���� ���� (������ó�� ���� ����), ���ô� 1�� ���ư�
						if (bIsDoorClose == true)
						{
							CurrentTime += DeltaTime;
							if (CurrentTime > MovableTime)
							{
								// ���� ����
								// �׽�Ʈ�� ������Ʈ ��ġ
								ShowFoxyDoll(FoxDollInstance, true);

								SetActorLocation(TagArr[1]);
								RoomNum = 1;
								CurrentState = ELocationState::IDLE;

								UE_LOG(LogTemp, Log, TEXT("Spawn Foxy Doll"));
							}
							CurrentTime = 0.f;
						}
					}

					// �÷��̾� ��ġ �� CLOSET (���� ���� �ٲ� �ð�����), �������ɾ� (����) �� GAME OVER
					else if (LookState != AFreddyPlayer::LookAt::Center)
					{
						if (bCTtoZero == false) // �ݺ������� currentTime�� 0�� ���� �ʵ���
						{
							CurrentTime = 0.f;
							bCTtoZero = true;
						}
						CurrentTime += DeltaTime;
						if (CurrentTime > MovableTime)
							CurrentState = ELocationState::ATTACK;
					}
				}
			}
		}
		else
		{
			AILevelComp->RandomMove(this, DeltaTime);
		}
		CurrentTime = 0.f;
	}
}

void AFoxy::Move()
{
	UE_LOG(LogTemp, Warning, TEXT("Chica Move()"));
	FVector CurrentLocation = this->GetActorLocation();
	// ġī ��ġ�� room number �� ����
	for (int32 i = 1; i < TagArr.Num(); ++i)
	{
		if (CurrentLocation.Equals(TagArr[i], 1.0f))
		{
			RoomNum = i;
			break;
		}
	}

	// room1 -> room2, room3, room4, room5, room6 ����
	if (RoomNum == 1)
	{
		TArray<int32> RoomTags = { 2, 3, 4, 5, 6 };
		int32 RandomIndex = FMath::RandRange(0, RoomTags.Num() - 1);

		//SetActorLocation(TagArr[RoomTags[RandomIndex]] );
		SetActorLocation(TagArr[5]); // �׽�Ʈ��
	}
	// room2 -> room1, room3, room5 ����
	else if (RoomNum == 2)
	{
		TArray<int32> RoomTags = { 1, 3, 5 };
		int32 RandomIndex = FMath::RandRange(0, RoomTags.Num() - 1);

		//SetActorLocation(TagArr[RoomTags[RandomIndex]] );
		SetActorLocation(TagArr[5]); // �׽�Ʈ��
	}
	// room3 -> room1, room2, room4, room6 ����
	else if (RoomNum == 3)
	{
		TArray<int32> RoomTags = { 1, 2, 4, 6 };
		int32 RandomIndex = FMath::RandRange(0, RoomTags.Num() - 1);

		//SetActorLocation(TagArr[RoomTags[RandomIndex]] );
		SetActorLocation(TagArr[6]); // �׽�Ʈ��
	}
	// room4 -> room1, room3, room6 ����
	else if (RoomNum == 4)
	{
		TArray<int32> RoomTags = { 1, 3, 6 };
		int32 RandomIndex = FMath::RandRange(0, RoomTags.Num() - 1);

		//SetActorLocation(TagArr[RoomTags[RandomIndex]] );
		SetActorLocation(TagArr[6]); // �׽�Ʈ��
	}
	
	CurrentState = ELocationState::IDLE;
}

void AFoxy::Attack()
{
	UE_LOG(LogTemp, Warning, TEXT("Attack !"));
	// ���������� anim ���

	// �׽�Ʈ�� -> ī�޶� ������ SetActorLocation
	AFreddyPlayer* FreddyPlayer = Cast<AFreddyPlayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	FTransform JmpScare = FreddyPlayer->GetCameraTransform();
	SetActorTransform(JmpScare); // ī�޶� ��ġ�� �̵� (�������ɾ�)

	// �������ɾ� �Ҹ�
	UGameplayStatics::PlaySound2D(this, JumpScareSFX);

	// ���� ����

}

FVector AFoxy::FindActorsWithTag(FName Tag)
{
	// �� ���� ��ġ ������ �±׷� �޾ƿͼ� �迭�� ����
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), Tag, FoundActors);

	if (FoundActors.Num() > 0)
	{
		AActor* TargetActor = FoundActors[0];
		FVector TargetLocation = TargetActor->GetActorLocation();

		return TargetLocation;
		// �迭 �ε��� �� = �� ��ȣ
	}

	return FVector::ZeroVector; // FoundActors�� ������� ���, �⺻�� ��ȯ
}

void AFoxy::MoveToTaggedLocation(int32 room)
{
	GetController()->StopMovement();

	ACharacter* Character = Cast<ACharacter>(this);
	if (Character)
	{
		Character->bUseControllerRotationYaw = false; // ĳ���� ȸ���� ���
		Character->GetCharacterMovement()->bOrientRotationToMovement = false; // �̵� �������� ȸ������ ����
	}

	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController)
	{
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalLocation(TagArr[room]);
		MoveRequest.SetAcceptanceRadius(5.0f); // ��ǥ ��ġ�� �����ϴ� ���� ����

		FNavPathSharedPtr NavPath;
		EPathFollowingRequestResult::Type MoveResult = AIController->MoveTo(MoveRequest, &NavPath);

		// �̵� ��û ��� �α� ���
		switch (MoveResult)
		{
		case EPathFollowingRequestResult::Failed:
			UE_LOG(LogTemp, Warning, TEXT("MoveTo request failed."));
			break;
		case EPathFollowingRequestResult::AlreadyAtGoal:
			UE_LOG(LogTemp, Warning, TEXT("Already at goal location."));
			break;
		case EPathFollowingRequestResult::RequestSuccessful:
			UE_LOG(LogTemp, Warning, TEXT("MoveTo request successful."));
			break;
		}
	}

	if (room == 5)
	{
		RoomNum = 2;
		GetWorld()->GetTimerManager().SetTimer(Handle, this, &AFoxy::CanMove, 1.f, false);
	}
	else if (room == 6)
	{
		RoomNum = 3;
		GetWorld()->GetTimerManager().SetTimer(Handle, this, &AFoxy::CanMove, 1.f, false);
	}
}

void AFoxy::CanMove()
{
	RoomNum = 1;

	SetActorLocation(TagArr[1]);
	CurrentState = ELocationState::IDLE;
}

void AFoxy::FlashOn()
{
	AFreddyPlayer* FreddyPlayer = Cast<AFreddyPlayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (FreddyPlayer)
	{
		bIsFlashlightOn = FreddyPlayer->GetFlash();
	}
}

void AFoxy::DoorOpen()
{
	AFreddyPlayer* FreddyPlayer = Cast<AFreddyPlayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	if (FreddyPlayer)
	{
		bIsDoorClose = FreddyPlayer->GetrCloseDoor();
	}
}

void AFoxy::ShowFoxyDoll(AActor* actor, bool bShow)
{
	AFoxDoll* FoxyDoll = Cast<AFoxDoll>(actor);

	if (FoxyDoll && FoxyDoll->FoxyDollComp)
	{
		FoxyDoll->FoxyDollComp->SetVisibility(bShow);
	}
}



