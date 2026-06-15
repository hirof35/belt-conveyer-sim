#include "DxLib.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

struct Conveyor {
    float x, y;
    float width, height;
    float speed;
    float scrollOffset;
};

struct Box {
    float x, y;
    float size;
    float vx;
    float vy;
    bool isOnConveyor;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    ChangeWindowMode(TRUE);
    SetGraphMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32);
    if (DxLib_Init() == -1) return -1;
    SetDrawScreen(DX_SCREEN_BACK);

    // 1. ベルトコンベアの配置
    Conveyor conveyor = { 100.0f, 250.0f, 300.0f, 30.0f, 2.5f, 0.0f };

    // 2. 物体（箱）の配置【最初からコンベアに乗せる】
    Box box;
    box.size = 24.0f;
    box.x = conveyor.x + 30.0f;               // コンベアの左端から少し内側
    box.y = conveyor.y - (box.size / 2.0f);   // ★コンベアの真上にぴったり合わせる
    box.vx = conveyor.speed;                  // ★最初からコンベアと同じ速度
    box.vy = 0.0f;
    box.isOnConveyor = true;                  // ★最初から乗っている状態にする

    float gravity = 0.4f;

    while (ProcessMessage() == 0 && ClearDrawScreen() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0) {

        // --- 1. 物理演算（位置の更新） ---
        box.x += box.vx;
        box.y += box.vy;

        if (!box.isOnConveyor) {
            box.vy += gravity; // コンベアから落ちたら重力を適用
        }
        else {
            box.vy = 0.0f;     // コンベア上では落下しない
        }

        // --- 2. 当たり判定と状態遷移 ---
        float boxBottom = box.y + box.size / 2.0f;
        float boxLeft = box.x - box.size / 2.0f;
        float boxRight = box.x + box.size / 2.0f;

        // コンベアの上にいるかどうかの判定
        if (boxRight >= conveyor.x && boxLeft <= conveyor.x + conveyor.width &&
            boxBottom >= conveyor.y && boxBottom <= conveyor.y + 8.0f) {

            if (!box.isOnConveyor) {
                box.isOnConveyor = true;
                box.y = conveyor.y - box.size / 2.0f;
            }
            box.vx = conveyor.speed;

        }
        else {
            // 右端からこぼれ落ちた瞬間
            if (box.isOnConveyor) {
                box.isOnConveyor = false; // 空中状態へ
                // box.vx は維持されるため、右への慣性を持ったまま落ちる
            }
        }

        // --- 3. コンベアのアニメーション ---
        conveyor.scrollOffset += conveyor.speed;
        if (conveyor.scrollOffset >= 20.0f) {
            conveyor.scrollOffset -= 20.0f;
        }

        // --- 4. 描画処理 ---
        // コンベア
        DrawBox((int)conveyor.x, (int)conveyor.y, (int)(conveyor.x + conveyor.width), (int)(conveyor.y + conveyor.height), GetColor(100, 100, 100), TRUE);
        for (float lineX = conveyor.x + conveyor.scrollOffset; lineX < conveyor.x + conveyor.width; lineX += 20.0f) {
            DrawLine((int)lineX, (int)conveyor.y, (int)lineX, (int)(conveyor.y + conveyor.height), GetColor(255, 255, 255));
        }

        // 箱
        DrawBox((int)(box.x - box.size / 2.0f), (int)(box.y - box.size / 2.0f),
            (int)(box.x + box.size / 2.0f), (int)(box.y + box.size / 2.0f),
            GetColor(240, 100, 50), TRUE);

        // ループ用：画面外に落ちたら、また「最初からコンベアの上に乗った状態」にリセット
        if (box.y > SCREEN_HEIGHT || box.x > SCREEN_WIDTH) {
            box.x = conveyor.x + 30.0f;
            box.y = conveyor.y - (box.size / 2.0f);
            box.vx = conveyor.speed;
            box.vy = 0.0f;
            box.isOnConveyor = true;
        }

        ScreenFlip();
    }

    DxLib_End();
    return 0;
}
