#include "SB/Chica.h"
#include "SB/LocationState.h"
#include "SB/AILevel.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "HJS/FreddyPlayer.h"
#include "GameFramework/PlayerController.h"

// Sets default values
AChica::AChica()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
	AILevelComp = CreateDefaultSubobject<UAILevel>(TEXT("AILevelComp"));
}

// Called when the game starts or when spawned
void AChica::BeginPlay()
{
	Super::BeginPlay();
	
	UE_LOG(LogTemp, Warning, TEXT("Game Start"));

	TagArr.Add(FVector(0,0,0)); //[0]
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

// Called every frame
void AChica::Tick(float DeltaTime)
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

void AChica::SetUpLocation(ELocationState State, float DeltaTime)
{
	if(CurrentState != State)
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
		case ELocationState::CUPCAKE:	Cupcake();
			break;
		default:
			break;
		}
	}
}

void AChica::UpdateState(float DeltaTime)
{
	switch (CurrentState)
	{
	case ELocationState::IDLE:	Idle(DeltaTime);
		break;
	case ELocationState::MOVE:	Move();
		break;
	case ELocationState::ATTACK:	Attack();
		break;
	case ELocationState::CUPCAKE:	Cupcake();
		break;
	default:
		break;
	}
}

// Called to bind functionality to input
void AChica::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


void AChica::Idle(float DeltaTime)
{
	// ���� ��ġ == room1 || room3 || room4 || room6 || room8 ����
	
	// 4.98�ʸ��� AILevel�� �ִ� RandomMove() ȣ�� && Move�� ��������
	CurrentTime += DeltaTime;
	if (CurrentTime > MovableTime)
	{
		AILevelComp->RandomMove(this, DeltaTime); // RandomMove�ȿ� �������� ����

		CurrentTime = 0.f;
	}
}

void AChica::Move() // ������ �Ѱ� ������ 1,3,4�θ� �̵�
{
	UE_LOG(LogTemp, Warning, TEXT("Chica Move()"));
	FVector CurrentLocation = this->GetActorLocation();
	// ġī ��ġ�� room number �� ����
	for(int32 i=1; i<TagArr.Num(); ++i)
	{
		if(CurrentLocation.Equals(TagArr[i], 1.0f))
		{	
			RoomNum = i;
			break;
		}
	}

	// room1 || room4 -> room3 ����
	if (RoomNum == 1 || RoomNum == 4)
	{
		SetActorLocation(TagArr[3]);
	}
	// room3 -> room1 || room4 || room6 ����
	else if (RoomNum == 3)
	{
		TArray<int32> RoomTags = { 1, 4, 6 };
		int32 RandomIndex = FMath::RandRange(0, RoomTags.Num() - 1);

		SetActorLocation(TagArr[RoomTags[RandomIndex]] );
	}
	// room6 -> room3 || room8 ����
	else if (RoomNum == 6)
	{
		TArray<int32> RoomTags = { 3, 8 };
		int32 RandomIndex = FMath::RandRange(0, RoomTags.Num() - 1);

		SetActorLocation(TagArr[RoomTags[RandomIndex]]);

		// �߼Ҹ�

		//	����, ������ ON -> room1�� �̵� (�����̵�X)
		if(bIsFlashlightOn == true) 
		{
			// ������ ON && Door: Close -> room8�� �̵�
			if (bIsDoorClose == true)
			{
				SetActorLocation(TagArr[8]);
			}
			MoveToTaggedLocation(1);
		}
	}
	// room8 -> room6 ����
	else if (RoomNum == 8)
	{
		AFreddyPlayer* FreddyPlayer = Cast<AFreddyPlayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

		AFreddyPlayer::LookAt LookState;
		LookState = FreddyPlayer->GetLookAtState();

		if(FreddyPlayer)
		{
			//�� �÷��̾� ��ġ == Door && ������ ON : ����������(����) 
			if ((LookState == AFreddyPlayer::LookAt::Right && bIsDoorClose == false) && bIsFlashlightOn == true)
			{
				CurrentState = ELocationState::ATTACK;
			}
			//�� �÷��̾� ��ġ == Door && bCLOSE == true (���� �ð����� CLOSE �� Ȯ�������� 1,3,4 �� �̵�)
			if (LookState == AFreddyPlayer::LookAt::Right && bIsDoorClose == true)
			{
				for (float cnt = 0.f; cnt < 6.f; cnt++)
				{
					if (cnt > MovableTime)
					{
						TArray<int32> RoomTags = { 1, 3, 4 };
						int32 RandomIndex = FMath::RandRange(0, RoomTags.Num() - 1);

						SetActorLocation(TagArr[RoomTags[RandomIndex]]);
					}
				}
			}
		}
		//�� �÷��̾� ��ġ��Door �� ��, ���� �ð� �Ŀ� ������ũ ����������(����) �� GAME OVER
		if (LookState != AFreddyPlayer::LookAt::Right)
		{
			CurrentState = ELocationState::CUPCAKE;
		}
		
		SetActorLocation(TagArr[6]);
	}
}

void AChica::Attack()
{
	UE_LOG(LogTemp, Warning, TEXT("Attack !"));
	// ���������� anim ���

	// ���� ����
	
}

void AChica::Cupcake()
{
	UE_LOG(LogTemp, Warning, TEXT("CupCake Attack !"));
	// ������ũ ���������� anim ���

	// ���� ����
}

FVector AChica::FindActorsWithTag(FName Tag)
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

void AChica::MoveToTaggedLocation(int32 room)
{
	GetController()->StopMovement();

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
}

// ������ ���� �������� �Լ�
void AChica::FlashOn()
{
	AFreddyPlayer* FreddyPlayer = Cast<AFreddyPlayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if(FreddyPlayer)
	{
		bIsFlashlightOn = FreddyPlayer->GetFlash();
	}
}

// �� ���� �������� �Լ�
void AChica::DoorOpen()
{
	AFreddyPlayer* FreddyPlayer = Cast<AFreddyPlayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	if(FreddyPlayer)
	{
		bIsDoorClose = FreddyPlayer->GetrCloseDoor();
	}
}

