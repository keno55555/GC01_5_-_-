#include <Novice.h>
#include <Math.h>
#include <time.h>
#define M_PI 3.14159265358979323846

const char kWindowTitle[] = "GC1D_05_カン_ケンリャン_title";

int kWindowWidth = 1280, kWindowHeight = 720;

typedef struct Vector2 {
	float X;
	float Y;
}Vector2;

typedef struct Camera {
	Vector2 pos;
	Vector2 speed;
}Camera;

typedef struct Timer {
	int time;
	int clock;
	int handle;
}Timer;

typedef struct Scoreboard{
	int score;
	int time;
	int annko;
	int annkoScore;
	int timeAdder;
	int timeAdderTime;
	int stoper;
	int stoperScore;
}Scoreboard;

int randomOutput;
int randomMaker;
int randomSaver[10];

enum Direction {
	leftDown = -4,
	down = -3,
	rightDown = -2,
	left = -1,
	stop = 0,
	right = 1,
	leftUp = 2,
	up = 3,
	rightUp = 4
};

enum Stage {
	title = 1,
	play,
	end
};

typedef struct Border {
	Vector2 pos;
	Vector2 worldPos;
	int height;
	int width;
	int wide;
}Border;

typedef struct ClearBorder {
	Vector2 pos;
	int height;
	int width;
}ClearBorder;

typedef struct Player {
	Vector2 pos;
	Vector2 speed;
	Vector2 savePos;
	Vector2 handle;
	float speedPlus;
	float speedDiagonal;
	float speedDash;
	float speedHandle;
	int energy;
	int energyHandle;
	int dashFlat;
	float height;
	float width;
	float rotateSpeed;
	float rotateSpeedBuffer;
	float rotateSpeedHandle;
	float radians;
	int direction;
}Player;

typedef struct PlayerCheckPoint {
	Vector2 pos;
	Vector2 speed;
}PlayerCheckPoint;

typedef struct Object {
	Vector2 pos;
	Vector2 worldPos;
	Vector2 speed;
	float speedPlus;
	float speedDiagonal;
	float speedHandle;
	int radius;
	int isAlive;
}Object;

int	timeAdderSeachRadius[2];
int stoperSeachRadius[5];


//typedef struct Debuffer {
//	Vector pos;
//	int radius;
//}Enemy;


// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, kWindowWidth, kWindowHeight);

	///===========================================================================================================================================================
	///初期化
	///===========================================================================================================================================================

	///Camera
	Camera camera;
	camera.pos.X = 0;
	camera.pos.Y = 0;
	camera.speed.X = 0;
	camera.speed.Y = 0;

	///Main Timer
	Timer mainTimer;
	mainTimer.time = 60;
	mainTimer.clock = 60;
	mainTimer.handle = 0;

	///Annko Respawn Timer
	Timer AKRSTimer;
	AKRSTimer.time = 60;
	AKRSTimer.clock = 0;
	AKRSTimer.handle = 0;

	///Dash Timer
	Timer dashTimer;
	dashTimer.time = 60;
	dashTimer.clock = 0;
	dashTimer.handle = 0;

	///Stop Timer
	Timer stopTimer;
	stopTimer.time = 120;
	stopTimer.clock = 0;
	stopTimer.handle = 0;

	///stage
	int stage = 1;
	
	///Scoreboard
	Scoreboard score;
	score.score = 0;
	score.time = 60; 
	score.annko = 0;
	score.annkoScore = 10;
	score.timeAdder = 0;
	score.timeAdderTime = 2;
	score.stoper = 0;
	score.stoperScore = 20;

	///Border
	Border border;
	border.height = 5120;
	border.width = 5120;
	border.wide = 50;
	border.worldPos.X = (float)(kWindowWidth / 2 - border.height / 2);
	border.worldPos.Y = (float)(kWindowHeight / 2 - border.width / 2);
	border.pos.X = 0;
	border.pos.Y = 0;
	
	
	///player
	Player player;
	player.pos.X = (float)kWindowWidth / 2;
	player.pos.Y = (float)kWindowHeight / 2;
	
	player.speed.X = 0;
	player.speed.Y = 0;
	player.speedPlus = 5;
	player.speedDiagonal = (float)(player.speedPlus * sin(0.8));
	player.speedDash = 20;
	player.speedHandle = 5;
	player.energy = 5;
	player.energyHandle = 0;
	player.dashFlat = 0;


	player.height = 100;
	player.width = 100;

	player.savePos.X = -player.width / 2;
	player.savePos.Y = -player.height / 2;
	player.handle.X = -player.width / 2;
	player.handle.Y = -player.height / 2;
	player.rotateSpeed = 1;///degree
	player.rotateSpeedBuffer = 20;
	player.rotateSpeedHandle = 0;
	player.radians = 0.0f;
	player.direction = stop;

	PlayerCheckPoint playerCP[24];
	for (int i = 0; i < 24; i++) {
		playerCP[i].speed.X = 0;
		playerCP[i].speed.Y = 0;
	}


	///    0  4  5 6  7  8  1
	///    9               14
	///    10              15
	///    11              16
	///    12              17
	///    13              18
	///    2 19 20 21 22 23 3
	
	///playerの四角を定義する
	for (int i = 0; i < 4; i++) {
		playerCP[i].pos.X = player.width / 2;
		if (i % 2 == 0) {
			playerCP[i].pos.X = -playerCP[i].pos.X;
		}
	}
	for (int i = 0; i < 4; i++) {
		playerCP[i].pos.Y = player.height / 2;
		if (i < 2) {
			playerCP[i].pos.Y = -playerCP[i].pos.Y;
		}
	}
	///当たり判定用ポイット
	for (int i = 4; i < 9; i++) {
		playerCP[i].pos.X = -player.width / 6 * 3  + player.width / 6 * (i - 3);
		playerCP[i].pos.Y = -player.height / 2;
	}
	for (int i = 9; i < 14; i++) {
		playerCP[i].pos.X = -player.width / 2;
		playerCP[i].pos.Y = -player.height / 6 * 3 + player.height / 6 * (i - 8);
	}
	for (int i = 14; i < 19; i++) {
		playerCP[i].pos.X = player.width / 2;
		playerCP[i].pos.Y = -player.height / 6 * 3 + player.height / 6 * (i - 13);
	}
	for (int i = 19; i < 24; i++) {
		playerCP[i].pos.X = -player.width / 6 * 3 + player.width / 6 * (i - 18);
		playerCP[i].pos.Y = player.height / 2;
	}

	///handle
	float playerCPHandleX = 0;
	float playerCPHandleY = 0;

	///アンコ
	Object annko[20];
	for (int i = 0; i < 20; i++) {
		annko[i].pos.X = 0;
		annko[i].pos.Y = 0;
		annko[i].worldPos.X = 0;
		annko[i].worldPos.Y = 0;
		annko[i].speed.X = 0;
		annko[i].speed.Y = 0;
		annko[i].speedPlus = 0;
		annko[i].speedDiagonal = 0;
		annko[i].speedHandle = 0;
		annko[i].radius = 10;
		annko[i].isAlive = 0;
	}

	///時間延長子
	Object timeAdder[2];
	for (int i = 0; i < 2; i++) {
		timeAdder[i].pos.X = 0;
		timeAdder[i].pos.Y = 0;
		timeAdder[i].worldPos.X = 0;
		timeAdder[i].worldPos.Y = 0;
		timeAdder[i].speed.X = 0;
		timeAdder[i].speed.Y = 0;
		timeAdder[i].speedPlus = 5;
		timeAdder[i].speedDiagonal = (float)(timeAdder[i].speedPlus * sin(0.8));
		timeAdder[i].speedHandle = 0;
		timeAdder[i].radius = 10;
		timeAdder[i].isAlive = 0;
		timeAdderSeachRadius[i] = 200;
	}

	///障害子
	Object stoper[5];
	for (int i = 0; i < 5; i++) {
		stoper[i].pos.X = 0;
		stoper[i].pos.Y = 0;
		stoper[i].worldPos.X = 0;
		stoper[i].worldPos.Y = 0;
		stoper[i].speed.X = 0;
		stoper[i].speed.Y = 0;
		stoper[i].speedPlus = 6;
		stoper[i].speedDiagonal = (float)(stoper[i].speedPlus * sin(0.8));
		stoper[i].speedHandle = 0;
		stoper[i].radius = 10;
		stoper[i].isAlive = 0;
		stoperSeachRadius[i] = 400;
	}


	///乱数用
	randomOutput = 0;
	randomMaker = 1;
	for (int i = 1; i < 10; i++) {
		randomSaver[i] = 0;
	}


	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);
		switch (stage) {
		case title:
			///===========================================================================================================================================================
			///更新処理
			///===========================================================================================================================================================
			if (keys[DIK_SPACE] && !preKeys[DIK_SPACE]) {
				stage++;
			}

			///===========================================================================================================================================================
			///描画処理
			///===========================================================================================================================================================
			///BackGround
			Novice::DrawBox(0, 0, kWindowWidth, kWindowHeight, 0.0f, 0x984B4BFF, kFillModeSolid);
			Novice::ScreenPrintf(590, 300, "Annko Eater!!");
			Novice::ScreenPrintf(560, 400, "[Press Space Start]");

			break;

		case play:
			///キー規則設定
			if (keys[DIK_UP]) {
				keys[DIK_W] = keys[DIK_UP];
			}

			if (keys[DIK_DOWN]) {
				keys[DIK_S] = keys[DIK_DOWN];
			}

			if (keys[DIK_LEFT]) {
				keys[DIK_A] = keys[DIK_LEFT];
			}

			if (keys[DIK_RIGHT]) {
				keys[DIK_D] = keys[DIK_RIGHT];
			}

			///上
			if (keys[DIK_W] && !preKeys[DIK_W]) {
				player.direction = player.direction + 3;
			}
			if (!keys[DIK_W] && preKeys[DIK_W]) {
				player.direction = player.direction - 3;
			}

			///下
			if (keys[DIK_S] && !preKeys[DIK_S]) {
				player.direction = player.direction - 3;
			}
			if (!keys[DIK_S] && preKeys[DIK_S]) {
				player.direction = player.direction + 3;
			}

			///左
			if (keys[DIK_A] && !preKeys[DIK_A]) {
				player.direction = player.direction - 1;
			}
			if (!keys[DIK_A] && preKeys[DIK_A]) {
				player.direction = player.direction + 1;
			}

			///右
			if (keys[DIK_D] && !preKeys[DIK_D]) {
				player.direction = player.direction + 1;
			}
			if (!keys[DIK_D] && preKeys[DIK_D]) {
				player.direction = player.direction - 1;
			}

			switch (player.direction) {
				///動いてない
			case stop:
				player.speed.X = 0;
				player.speed.Y = 0;
				break;
				///上
			case up:
				player.speed.X = 0;
				player.speed.Y = -player.speedPlus;
				break;
				///下
			case down:
				player.speed.X = 0;
				player.speed.Y = player.speedPlus;
				break;
				///左
			case left:
				player.speed.X = -player.speedPlus;
				player.speed.Y = 0;
				break;
				///右
			case right:
				player.speed.X = player.speedPlus;
				player.speed.Y = 0;
				break;
				///左上
			case leftUp:
				player.speed.X = -player.speedDiagonal;
				player.speed.Y = -player.speedDiagonal;
				break;
				///右上
			case rightUp:
				player.speed.X = +player.speedDiagonal;
				player.speed.Y = -player.speedDiagonal;
				break;
				///左下
			case leftDown:
				player.speed.X = -player.speedDiagonal;
				player.speed.Y = player.speedDiagonal;
				break;
				///右下
			case rightDown:
				player.speed.X = player.speedDiagonal;
				player.speed.Y = player.speedDiagonal;
				break;
			}


			///
			/// ↓更新処理ここから
			///

			///===========================================================================================================================================================
			///乱数生成
			///===========================================================================================================================================================

			randomOutput = (int)time(nullptr) * randomMaker;
			srand(randomOutput);

			randomOutput = rand() + randomMaker;
			randomMaker++;

			randomSaver[0] = randomOutput;

			for (int i = 9; i > 0; i--) {
				randomSaver[i] = randomSaver[i - 1];
			}

			///===========================================================================================================================================================
			///ワールド設定
			///===========================================================================================================================================================
			///カメラ設定

			camera.pos.X += player.speed.X;
			camera.pos.Y += player.speed.Y;

			///外際
			border.pos.X = border.worldPos.X - camera.pos.X;
			border.pos.Y = border.worldPos.Y - camera.pos.Y;

			///Timer設定
			mainTimer.clock--;
			if (mainTimer.clock < 0) {
				mainTimer.clock = mainTimer.time;
			}

			///TimeとStage関係
			if( mainTimer.clock == 0){
				score.time--;
			}
			if (score.time == 0) {
				stage++;
			}



			///===========================================================================================================================================================
			///player設定
			///===========================================================================================================================================================
			///player回転
			player.radians += (float)(player.rotateSpeed * M_PI / 180);
			player.handle.X = player.savePos.X;
			player.handle.Y = player.savePos.Y;
			player.savePos.X = player.handle.X * cosf((float)(player.rotateSpeed * M_PI / 180)) - player.handle.Y * sinf((float)(player.rotateSpeed * M_PI / 180));
			player.savePos.Y = player.handle.Y * cosf((float)(player.rotateSpeed * M_PI / 180)) + player.handle.X * sinf((float)(player.rotateSpeed * M_PI / 180));


			///視界が外際に当たったら
			if ((camera.pos.X >= border.worldPos.X + border.width - kWindowWidth) || (player.pos.X > kWindowWidth / 2)) {
				player.pos.X += player.speed.X;
				camera.pos.X = border.worldPos.X + border.width - kWindowWidth;
				if (player.pos.X < kWindowWidth / 2) {
					player.pos.X = (float)kWindowWidth / 2;
					camera.pos.X = border.worldPos.X + border.width - kWindowWidth - 1;
				}
				if (player.pos.X + (player.width / 2 + 20) > kWindowWidth) {
					player.pos.X = (float)kWindowWidth - player.width / 2 - 20;
				}
			}
			if (camera.pos.X <= border.worldPos.X || (player.pos.X < kWindowWidth / 2)) {
				player.pos.X += player.speed.X;
				camera.pos.X = border.worldPos.X;
				if (player.pos.X > kWindowWidth / 2) {
					player.pos.X = (float)kWindowWidth / 2;
					camera.pos.X = border.worldPos.X + 1;
				}
				if (player.pos.X - (player.width / 2 + 20) < 0) {
					player.pos.X = player.width / 2 + 20;
				}
			}
			if (camera.pos.Y <= border.worldPos.Y || (player.pos.Y < kWindowHeight / 2)) {
				player.pos.Y += player.speed.Y;
				camera.pos.Y = border.worldPos.Y;
				if (player.pos.Y > kWindowHeight / 2) {
					player.pos.Y = (float)kWindowHeight / 2;
					camera.pos.Y = border.worldPos.Y + 1;
				}
				if (player.pos.Y - (player.height / 2 + 20) < 0) {
					player.pos.Y = player.height / 2 + 20;
				}
			}
			if ((camera.pos.Y >= border.worldPos.Y + border.height - kWindowHeight) || (player.pos.Y > kWindowHeight / 2)) {
				player.pos.Y += player.speed.Y;
				camera.pos.Y = border.worldPos.Y + border.height - kWindowHeight;
				if (player.pos.Y < kWindowHeight / 2) {
					player.pos.Y = (float)kWindowHeight / 2;
					camera.pos.Y = border.worldPos.Y + border.height - kWindowHeight - 1;
				}

				if (player.pos.Y + (player.height / 2 + 20) > kWindowHeight) {
					player.pos.Y = (float)kWindowHeight - player.height / 2 - 20;
				}
			}


			///player判定回転
			for (int i = 0; i < 24; i++) {
				playerCPHandleX = playerCP[i].pos.X;
				playerCPHandleY = playerCP[i].pos.Y;
				playerCP[i].pos.X = playerCPHandleX * cosf((float)(player.rotateSpeed * M_PI / 180)) - playerCPHandleY * sinf((float)(player.rotateSpeed * M_PI / 180)) + player.pos.X;
				playerCP[i].pos.Y = playerCPHandleY * cosf((float)(player.rotateSpeed * M_PI / 180)) + playerCPHandleX * sinf((float)(player.rotateSpeed * M_PI / 180)) + player.pos.Y;
			}

			///player描画修正
			if ((int)(playerCP[0].pos.X - player.pos.X) == -player.width / 2) {
				player.savePos.X = -player.width / 2;
				player.savePos.Y = -player.height / 2;
				player.radians = (float)(player.rotateSpeed * M_PI / 180);
			}


			///ダッシュ
			if ((keys[DIK_SPACE] && !preKeys[DIK_SPACE]) && (player.energyHandle > 0) && (dashTimer.handle == 0)) {
				dashTimer.handle = 1;
				dashTimer.clock = dashTimer.time;
				player.energyHandle--;
				player.speedPlus = player.speedDash;
			}

			if ((player.speedPlus > player.speedHandle)) {
				player.speedPlus -= (float)0.2;
				player.speedDiagonal = (float)(player.speedPlus * sin(0.8));
			}

			if (player.speedPlus <= player.speedHandle) {
				player.dashFlat = 0;
				player.speedPlus = player.speedHandle;
				player.speedDiagonal = (float)(player.speedPlus * sin(0.8));
			}

			///ダッシュタイマー
			if (dashTimer.clock > 0) {
				dashTimer.clock--;
			}
			if (dashTimer.clock == 0) {
				dashTimer.handle = 0;
			}

			///++++++無限ダッシュ++++++++
			/*dashTimer.handle = 0;
			player.energyHandle = 5;*/

			///===========================================================================================================================================================
			///アンコ設定
			///===========================================================================================================================================================

			///アンコタイマー
			AKRSTimer.clock--;
			if (AKRSTimer.clock < 0) {
				AKRSTimer.clock = AKRSTimer.time;
			}

			///アンコ生成
			if (AKRSTimer.clock == 0) {
				for (int i = 0; i < 20; i++) {
					if (annko[i].isAlive == 0) {
						if (randomSaver[i] % 1400 < 700) {
							annko[i].pos.X = randomSaver[i] % 1400 - camera.pos.X;
						}
						else {
							annko[i].pos.X = randomSaver[i] % 1400 + 300 - camera.pos.X;
						}
						if (randomSaver[i - 1] % 600 < 300) {
							annko[i].pos.Y = randomSaver[i - 1] % 600 - camera.pos.Y;
						}
						else {
							annko[i].pos.Y = randomSaver[i - 1] % 600 + 300 - camera.pos.Y;
						}
						annko[i].isAlive = 1;
						annko[i].worldPos.X = annko[i].pos.X;
						annko[i].worldPos.Y = annko[i].pos.Y;
						break;
					}

				}
			}

			///アンコ動く
			for (int i = 0; i < 20; i++) {
				annko[i].pos.X = annko[i].worldPos.X - camera.pos.X;
				annko[i].pos.Y = annko[i].worldPos.Y - camera.pos.Y;
			}


			///===========================================================================================================================================================
			///延長子設定
			///===========================================================================================================================================================
			///延長子生成
			if (AKRSTimer.clock == 0) {
				for (int i = 0; i < 2; i++) {
					if (timeAdder[i].isAlive == 0) {
						if (randomSaver[i] % 1400 < 700) {
							timeAdder[i].pos.X = randomSaver[i + 2] % 1400 - camera.pos.X;
						}
						else {
							timeAdder[i].pos.X = randomSaver[i + 2] % 1400 + 300 - camera.pos.X;
						}
						if (randomSaver[i - 1] % 600 < 300) {
							timeAdder[i].pos.Y = randomSaver[i + 2 - 1] % 600 - camera.pos.Y;
						}
						else {
							timeAdder[i].pos.Y = randomSaver[i + 2 - 1] % 600 + 300 - camera.pos.Y;
						}
						timeAdder[i].isAlive = 1;
						timeAdder[i].worldPos.X = timeAdder[i].pos.X;
						timeAdder[i].worldPos.Y = timeAdder[i].pos.Y;
						break;
					}

				}
			}

			///延長子がplayerから逃げるの処理
			for (int i = 0; i < 2; i++) {
				if (timeAdder[i].isAlive) {
					for (int j = 0; j < 24; j++) {
						if (timeAdder[i].radius + timeAdderSeachRadius[i] >=
							sqrtf(static_cast<float>(pow(timeAdder[i].pos.X - player.pos.X, 2) + pow(timeAdder[i].pos.Y - player.pos.Y, 2)))) {

							timeAdder[i].speedHandle = timeAdder[i].speedPlus *
								(1 - (sqrtf(static_cast<float>(pow(timeAdder[i].pos.X - player.pos.X, 2) + pow(timeAdder[i].pos.Y - player.pos.Y, 2)))
									/ (timeAdder[i].radius + timeAdderSeachRadius[i])));
						}
						else {
							timeAdder[i].speedHandle = 0;
						}
					}
				}
			}

			///逃げる向き
			for (int i = 0; i < 2; i++) {
				if (timeAdder[i].isAlive) {
					if (timeAdder[i].pos.X > player.pos.X) {
						timeAdder[i].speed.X = timeAdder[i].speedHandle;
					}
					if (timeAdder[i].pos.X < player.pos.X) {
						timeAdder[i].speed.X = -timeAdder[i].speedHandle;
					}
					if (timeAdder[i].pos.Y > player.pos.Y) {
						timeAdder[i].speed.Y = timeAdder[i].speedHandle;
					}
					if (timeAdder[i].pos.Y < player.pos.Y) {
						timeAdder[i].speed.Y = -timeAdder[i].speedHandle;
					}
				}
			}



			///延長子動く
			for (int i = 0; i < 2; i++) {
				timeAdder[i].pos.X = timeAdder[i].worldPos.X - camera.pos.X;
				timeAdder[i].pos.Y = timeAdder[i].worldPos.Y - camera.pos.Y;
				timeAdder[i].worldPos.X += timeAdder[i].speed.X;
				timeAdder[i].worldPos.Y += timeAdder[i].speed.Y;
			}

			///===========================================================================================================================================================
			///障害子設定
			///===========================================================================================================================================================
			///障害子生成
			if (AKRSTimer.clock == 0) {
				for (int i = 0; i < 5; i++) {
					if (stoper[i].isAlive == 0) {
						if (randomSaver[i] % 2000 < 500) {
							stoper[i].pos.X = randomSaver[i + 2] % 2000 - camera.pos.X;
						}
						else {
							stoper[i].pos.X = randomSaver[i + 2] % 2000 + 500 - camera.pos.X;
						}
						if (randomSaver[i - 1] % 1440 < 500) {
							stoper[i].pos.Y = randomSaver[i + 2 - 1] % 1440 - camera.pos.Y;
						}
						else {
							stoper[i].pos.Y = randomSaver[i + 2 - 1] % 1440 + 500 - camera.pos.Y;
						}
						stoper[i].isAlive = 1;
						stoper[i].worldPos.X = stoper[i].pos.X;
						stoper[i].worldPos.Y = stoper[i].pos.Y;
						break;
					}

				}
			}

			///障害子がplayerを追う処理
			for (int i = 0; i < 5; i++) {
				if (stoper[i].isAlive) {
					for (int j = 0; j < 24; j++) {
						if (stoperSeachRadius[i] >=
							sqrtf(static_cast<float>(pow(stoper[i].pos.X - player.pos.X, 2) + pow(stoper[i].pos.Y - player.pos.Y, 2)))) {

							stoper[i].speedHandle = stoper[i].speedPlus *
								(stoperSeachRadius[i] - sqrtf(static_cast<float>(pow(stoper[i].pos.X - player.pos.X, 2) + pow(stoper[i].pos.Y - player.pos.Y, 2))))
								/ stoperSeachRadius[i];
						}
						else {
							stoper[i].speedHandle = 0;
						}
					}
				}
			}

			///追うの向き
			for (int i = 0; i < 5; i++) {
				if (stoper[i].isAlive) {
					if (stoper[i].pos.X > player.pos.X) {
						stoper[i].speed.X = -stoper[i].speedHandle;
					}
					if (stoper[i].pos.X < player.pos.X) {
						stoper[i].speed.X = stoper[i].speedHandle;
					}
					if (stoper[i].pos.Y > player.pos.Y) {
						stoper[i].speed.Y = -stoper[i].speedHandle;
					}
					if (stoper[i].pos.Y < player.pos.Y) {
						stoper[i].speed.Y = stoper[i].speedHandle;
					}
				}
			}



			///障害子動く
			for (int i = 0; i < 5; i++) {
				stoper[i].pos.X = stoper[i].worldPos.X - camera.pos.X;
				stoper[i].pos.Y = stoper[i].worldPos.Y - camera.pos.Y;
				stoper[i].worldPos.X += stoper[i].speed.X;
				stoper[i].worldPos.Y += stoper[i].speed.Y;
			}


			///当たりパニッシュ
			if (stopTimer.handle == 1) {
				player.speedPlus = 0;
				player.speedDiagonal = 0;
				dashTimer.handle = 1;
			}



			///停止タイマー
			if (stopTimer.clock > -1 && stopTimer.handle == 1) {
				stopTimer.clock--;
			}
			if (stopTimer.clock == -1) {
				stopTimer.handle = 0;
				dashTimer.handle = 0;
				stopTimer.clock = 0;
				player.speedPlus = player.speedHandle;
				player.speedDiagonal = (float)(player.speedPlus * sin(0.8));
			}


			///===========================================================================================================================================================
			///消える判定
			///===========================================================================================================================================================
			/*///playerから遠すぎるアンコ
			for (int i = 0; i < 20; i++) {
				if (annko[i].isAlive) {
					for (int j = 0; j < 24; j++) {
						if (2560 <= sqrtf(static_cast<float>(pow(annko[i].pos.X - camera.pos.X, 2) + pow(annko[i].pos.Y - camera.pos.Y, 2)))) {
							annko[i].isAlive = false;
						}
					}
				}
			}*/

			///延長子が際の外に行ったら



			///===========================================================================================================================================================
			///画面から出られないように
			///===========================================================================================================================================================
			///cameraが画面外に出られないように
			if (camera.pos.X > border.worldPos.X + border.width - kWindowWidth) {
				camera.pos.X = border.worldPos.X + border.width - kWindowWidth;
			}
			if (camera.pos.X < border.worldPos.X) {
				camera.pos.X = border.worldPos.X;
			}
			if (camera.pos.Y < border.worldPos.Y) {
				camera.pos.Y = border.worldPos.Y;
			}
			if (camera.pos.Y > border.worldPos.Y + border.height - kWindowHeight) {
				camera.pos.Y = border.worldPos.Y + border.height - kWindowHeight;
			}

			///プレイヤーが画面外に出られないように
			if (player.pos.X + (player.width / 2 + 20) > kWindowWidth) {
				player.pos.X = (float)kWindowWidth - player.width / 2 - 20;
			}
			if (player.pos.X - (player.width / 2 + 20) < 0) {
				player.pos.X = player.width / 2 + 20;
			}
			if (player.pos.Y + (player.height / 2 + 20) > kWindowHeight) {
				player.pos.Y = (float)kWindowHeight - player.height / 2 - 20;
			}
			if (player.pos.Y - (player.height / 2 + 20) < 0) {
				player.pos.Y = player.height / 2 + 20;
			}


			///時間延長子画面外に出られないように
			for (int i = 0; i < 2; i++) {
				if (timeAdder[i].pos.X > border.worldPos.X + border.width - camera.pos.X - 50) {
					timeAdder[i].pos.X = border.worldPos.X + border.width - camera.pos.X - 50;
				}
				if (timeAdder[i].pos.X < border.worldPos.X - camera.pos.X + 50) {
					timeAdder[i].pos.X = border.worldPos.X - camera.pos.X + 50;
				}
				if (timeAdder[i].pos.Y < border.worldPos.Y - camera.pos.Y + 50) {
					timeAdder[i].pos.Y = border.worldPos.Y - camera.pos.Y + 50;
				}
				if (timeAdder[i].pos.Y > border.worldPos.Y + border.height - camera.pos.Y - 50) {
					timeAdder[i].pos.Y = border.worldPos.Y + border.height - camera.pos.Y - 50;
				}
			}

			///===========================================================================================================================================================
			///当たり判定
			///===========================================================================================================================================================
			///playerとアンコ
			for (int i = 0; i < 20; i++) {
				if (annko[i].isAlive) {
					for (int j = 0; j < 24; j++) {
						if (annko[i].radius >=
							sqrtf(static_cast<float>(pow(annko[i].pos.X - playerCP[j].pos.X, 2) + pow(annko[i].pos.Y - playerCP[j].pos.Y, 2)))) {
							annko[i].isAlive = false;
							score.score += score.annkoScore;
							score.annko++;
							if (player.energyHandle < 5) {
								player.energyHandle++;
							}
						}
					}
				}
			}

			///playerと延長子
			for (int i = 0; i < 2; i++) {
				if (timeAdder[i].isAlive) {
					for (int j = 0; j < 24; j++) {
						if (timeAdder[i].radius >=
							sqrtf(static_cast<float>(pow(timeAdder[i].pos.X - playerCP[j].pos.X, 2) + pow(timeAdder[i].pos.Y - playerCP[j].pos.Y, 2)))) {
							score.timeAdder++;
							score.time += score.timeAdderTime;
							timeAdder[i].isAlive = false;
						}
					}
				}
			}

			///playerと障害子
			for (int i = 0; i < 5; i++) {
				if (stoper[i].isAlive) {
					for (int j = 0; j < 24; j++) {
						if (stoper[i].radius >=
							sqrtf(static_cast<float>(pow(stoper[i].pos.X - playerCP[j].pos.X, 2) + pow(stoper[i].pos.Y - playerCP[j].pos.Y, 2)))) {
							stopTimer.clock = stopTimer.time;
							stopTimer.handle = 1;
							score.score -= score.stoperScore;
							score.stoper++;
							stoper[i].isAlive = false;
						}
					}
				}
			}

			///
			/// ↑更新処理ここまで
			///

			///
			/// ↓描画処理ここから
			///

			///===========================================================================================================================================================
			///外際描画
			///===========================================================================================================================================================
			Novice::DrawBox((int)border.pos.X - border.wide, (int)border.pos.Y - border.wide, border.height + border.wide * 2, border.width + border.wide * 2, 0.0f, 0xAAAAAAFF, kFillModeSolid);
			Novice::DrawBox((int)border.pos.X, (int)border.pos.Y, border.height, border.width, 0.0f, 0x613030FF, kFillModeSolid);




			///===========================================================================================================================================================
			///アンコ描画
			///===========================================================================================================================================================
			for (int i = 0; i < 20; i++) {
				if (annko[i].isAlive == 1) {
					Novice::DrawEllipse((int)annko[i].pos.X, (int)annko[i].pos.Y, (int)annko[i].radius, (int)annko[i].radius, 0.0f, 0xFE9882FF, kFillModeSolid);
				}
			}

			///===========================================================================================================================================================
			///延長子描画
			///===========================================================================================================================================================
			///逃げる範囲
			/*for (int i = 0; i < 2; i++) {
				if (timeAdder[i].isAlive == 1) {
					Novice::DrawEllipse((int)timeAdder[i].pos.X, (int)timeAdder[i].pos.Y,
						(int)timeAdder[i].radius + timeAdderSeachRadius[i], (int)timeAdder[i].radius + timeAdderSeachRadius[i], 0.0f, 0x97CBFFFF, kFillModeSolid);
				}
			}*/
			///本体
			for (int i = 0; i < 2; i++) {
				if (timeAdder[i].isAlive == 1) {
					Novice::DrawEllipse((int)timeAdder[i].pos.X, (int)timeAdder[i].pos.Y, (int)timeAdder[i].radius, (int)timeAdder[i].radius, 0.0f, 0x00E3E3FF, kFillModeSolid);
				}
			}

			///===========================================================================================================================================================
			///障害子描画
			///===========================================================================================================================================================
			///追う範囲
			/*for (int i = 0; i < 5; i++) {
				if (stoper[i].isAlive == 1) {
					Novice::DrawEllipse((int)stoper[i].pos.X, (int)stoper[i].pos.Y,
						(int)stoper[i].radius + stoperSeachRadius[i], (int)stoper[i].radius + stoperSeachRadius[i], 0.0f, 0xD200D2FF, kFillModeSolid);
				}
			}*/
			///本体
			for (int i = 0; i < 5; i++) {
				if (stoper[i].isAlive == 1) {
					Novice::DrawEllipse((int)stoper[i].pos.X, (int)stoper[i].pos.Y, (int)stoper[i].radius, (int)stoper[i].radius, 0.0f, 0xEA0000FF, kFillModeSolid);
				}
			}

			///===========================================================================================================================================================
			///player描画
			///===========================================================================================================================================================

			Novice::DrawBox((int)(player.savePos.X + player.pos.X), (int)(player.savePos.Y + player.pos.Y),
				(int)player.width, (int)player.height, player.radians, 0xFFFFFFFF, kFillModeSolid);


			///===========================================================================================================================================================
			///UI描画
			///===========================================================================================================================================================
			///エナジーバー
			for (int i = 0; i < 5; i++) {
				Novice::DrawEllipse(30 + i * 50, 30, 20, 20, 0.0f, 0xFFFFFFFF, kFillModeWireFrame);
			}
			for (int i = 0; i < player.energyHandle; i++) {
				Novice::DrawEllipse(30 + i * 50, 30, 20, 20, 0.0f, 0xFFFFFFFF, kFillModeSolid);
			}

			///ダッシュCD
			Novice::DrawBox(10, 60, 210, 20, 0.0f, 0xFFFFFFFF, kFillModeWireFrame);
			Novice::DrawBox(10, 60, (210 * dashTimer.clock / dashTimer.time), 20, 0.0f, 0xFFFFFFFF, kFillModeSolid);
			///パニッシュCD
			Novice::DrawBox(10, 60, (210 * stopTimer.clock / stopTimer.time), 19, 0.0f, 0xAE0000FF, kFillModeSolid);
			
			///scroe
			Novice::ScreenPrintf(10, 90, "Score :%05d", score.score);
			///time
			Novice::ScreenPrintf(600, 10, "Stage :%d", score.time);

			



			/////ミニマップ描画
			///外際の四角形
			Novice::DrawBox(kWindowWidth - 10 - 256 - ((int)camera.pos.X / 20), 10 - ((int)camera.pos.Y / 20), 256, 256, 0.0, 0xAAAAAAFF, kFillModeWireFrame);
			///視界の四角形
			Novice::DrawBox(kWindowWidth - 10 - 128 - 32, 10 + 128 - 18, 64, 36, 0.0, 0xffd700FF, kFillModeWireFrame);
			///視界の四角形
			Novice::DrawBox(kWindowWidth - 10 - 128 - 64, 10 + 64, 128, 128, 0.0, 0xFF2D2DFF, kFillModeWireFrame);
			///playerの位置
			Novice::DrawBox(kWindowWidth - 10 - 128 - 2 - (int)((kWindowWidth / 2 - player.pos.X) / 20),
				10 + 128 - 2 - (int)((kWindowHeight / 2 - player.pos.Y) / 20), 5, 5, 0.0, 0xFFFFFFFF, kFillModeSolid);
			///アンコの位置
			for (int i = 0; i < 20; i++) {
				if (annko[i].isAlive == 1) {
					Novice::DrawBox(kWindowWidth - 10 - 128 - 32 + ((int)annko[i].pos.X / 20), 10 + 128 - 18 + ((int)annko[i].pos.Y / 20),
						5, 5, 0.0f, 0xFE9882FF, kFillModeSolid);
				}
			}
			///延長子の位置
			for (int i = 0; i < 2; i++) {
				if (timeAdder[i].isAlive == 1) {
					Novice::DrawBox(kWindowWidth - 10 - 128 - 32 + ((int)timeAdder[i].pos.X / 20),
						10 + 128 - 18 + ((int)timeAdder[i].pos.Y / 20),
						5, 5, 0.0f, 0x00E3E3FF, kFillModeSolid);
				}
			}
			///障害子の位置
			for (int i = 0; i < 5; i++) {
				if (stoper[i].isAlive == 1) {
					Novice::DrawBox(kWindowWidth - 10 - 128 - 32 + ((int)stoper[i].pos.X / 20),
						10 + 128 - 18 + ((int)stoper[i].pos.Y / 20),
						5, 5, 0.0f, 0xD200D2FF, kFillModeSolid);
				}
			}


			///===========================================================================================================================================================

			///検査用

			/*///当たり判定位置表示用
			for (int i = 0; i < 24; i++) {
				Novice::DrawEllipse((int)playerCP[i].pos.X, (int)playerCP[i].pos.Y, 10, 10, 0.0f, 0xFE9882FF, kFillModeSolid);
			}*/

			/*for (int i = 9; i > 0; i--) {
				Novice::ScreenPrintf(10, 685 - i*15, "%d",randomSaver[i]);
			}*/

			///後処理
			for (int i = 0; i < 24; i++) {
				playerCP[i].pos.X -= player.pos.X;
				playerCP[i].pos.Y -= player.pos.Y;
			}

			///
			/// ↑描画処理ここまで
			///
			break;
			
		case end:
			
			///===========================================================================================================================================================
			///更新処理
			///===========================================================================================================================================================
			
			///Titletに戻る及び初期化
			if (keys[DIK_SPACE] && !preKeys[DIK_SPACE]) {
				camera.pos.X = 0;
				camera.pos.Y = 0;
				mainTimer.time = 60;
				mainTimer.clock = 60;
				mainTimer.handle = 0;
				AKRSTimer.time = 60;
				AKRSTimer.clock = 0;
				AKRSTimer.handle = 0;
				score.score = 0;
				score.time = 60;
				score.annko = 0;
				score.timeAdder = 0;
				score.stoper = 0;
				player.pos.X = (float)kWindowWidth / 2;
				player.pos.Y = (float)kWindowHeight / 2;
				player.speed.X = 0;
				player.speed.Y = 0;
				player.speedPlus = 5;
				player.speedDiagonal = (float)(player.speedPlus * sin(0.8));
				player.speedDash = 20;
				player.speedHandle = 5;
				player.energy = 5;
				player.energyHandle = 0;
				player.dashFlat = 0;
				player.rotateSpeed = 1;///degree
				player.rotateSpeedBuffer = 20;
				player.rotateSpeedHandle = 0;
				player.radians = 0.0f;
				player.direction = stop;
				for (int i = 0; i < 24; i++) {
					playerCP[i].speed.X = 0;
					playerCP[i].speed.Y = 0;
				}
				///playerの四角を定義する
				for (int i = 0; i < 4; i++) {
					playerCP[i].pos.X = player.width / 2;
					if (i % 2 == 0) {
						playerCP[i].pos.X = -playerCP[i].pos.X;
					}
				}
				for (int i = 0; i < 4; i++) {
					playerCP[i].pos.Y = player.height / 2;
					if (i < 2) {
						playerCP[i].pos.Y = -playerCP[i].pos.Y;
					}
				}
				///当たり判定用ポイット
				for (int i = 4; i < 9; i++) {
					playerCP[i].pos.X = -player.width / 6 * 3 + player.width / 6 * (i - 3);
					playerCP[i].pos.Y = -player.height / 2;
				}
				for (int i = 9; i < 14; i++) {
					playerCP[i].pos.X = -player.width / 2;
					playerCP[i].pos.Y = -player.height / 6 * 3 + player.height / 6 * (i - 8);
				}
				for (int i = 14; i < 19; i++) {
					playerCP[i].pos.X = player.width / 2;
					playerCP[i].pos.Y = -player.height / 6 * 3 + player.height / 6 * (i - 13);
				}
				for (int i = 19; i < 24; i++) {
					playerCP[i].pos.X = -player.width / 6 * 3 + player.width / 6 * (i - 18);
					playerCP[i].pos.Y = player.height / 2;
				}
				playerCPHandleX = 0;
				playerCPHandleY = 0;
				///アンコ
				for (int i = 0; i < 20; i++) {
					annko[i].pos.X = 0;
					annko[i].pos.Y = 0;
					annko[i].worldPos.X = 0;
					annko[i].worldPos.Y = 0;
					annko[i].speed.X = 0;
					annko[i].speed.Y = 0;
					annko[i].speedPlus = 0;
					annko[i].speedDiagonal = 0;
					annko[i].speedHandle = 0;
					annko[i].radius = 10;
					annko[i].isAlive = 0;
				}
				///時間延長子
				for (int i = 0; i < 2; i++) {
					timeAdder[i].pos.X = 0;
					timeAdder[i].pos.Y = 0;
					timeAdder[i].worldPos.X = 0;
					timeAdder[i].worldPos.Y = 0;
					timeAdder[i].speed.X = 0;
					timeAdder[i].speed.Y = 0;
					timeAdder[i].speedPlus = 5;
					timeAdder[i].speedDiagonal = (float)(timeAdder[i].speedPlus * sin(0.8));
					timeAdder[i].speedHandle = 0;
					timeAdder[i].radius = 10;
					timeAdder[i].isAlive = 0;
					timeAdderSeachRadius[i] = 200;
				}
				///障害子
				for (int i = 0; i < 5; i++) {
					stoper[i].pos.X = 0;
					stoper[i].pos.Y = 0;
					stoper[i].worldPos.X = 0;
					stoper[i].worldPos.Y = 0;
					stoper[i].speed.X = 0;
					stoper[i].speed.Y = 0;
					stoper[i].speedPlus = 6;
					stoper[i].speedDiagonal = (float)(stoper[i].speedPlus * sin(0.8));
					stoper[i].speedHandle = 0;
					stoper[i].radius = 10;
					stoper[i].isAlive = 0;
					stoperSeachRadius[i] = 400;
				}
				///乱数用
				randomOutput = 0;
				randomMaker = 1;
				for (int i = 1; i < 10; i++) {
					randomSaver[i] = 0;
				}



				stage = 1;
			}

			///===========================================================================================================================================================
			///描画処理
			///===========================================================================================================================================================
			///BackGround
			Novice::DrawBox(0, 0, kWindowWidth, kWindowHeight, 0.0f, 0x613030FF, kFillModeSolid);
			//Novice::DrawBox(0, 0, kWindowWidth/2, kWindowHeight, 0.0f, 0xFFFFFFFF, kFillModeWireFrame);
			Novice::ScreenPrintf(590, 250, "--GameOver--");
			Novice::ScreenPrintf(595, 280, "ScoreBoard");
			Novice::ScreenPrintf(570, 295, "Score:     %05d",score.score);
			Novice::ScreenPrintf(570, 310, "Annko:     %05d", score.annko);
			Novice::ScreenPrintf(570, 325, "TimeAdder: %05d", score.timeAdder);
			Novice::ScreenPrintf(570, 340, "Stopper:   %05d", score.stoper);
			Novice::ScreenPrintf(520, 400, "[Press Space Back To Title]");

			break;


			
		}

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
