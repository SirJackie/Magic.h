# API 接口文档

## 1. 基本配置

### 我需要引入那些文件？

- 您需要确保四个文件存在：**Magic.h、MagicHost.exe、SDL2.dll、SDL2_mixer.dll**。

- 导入：您需要将**Magic.h**导入到工程中，同时确保**MagicHost.exe、SDL2.dll、SDL2_mixer.dll**放置在工程同目录下。
- 分发：当你要把编译好的程序（例如：MyGame.exe）分发给其他用户使用，您需要确保**MagicHost.exe、SDL2.dll、SDL2_mixer.dll**放置在编译好的程序（例如：MyGame.exe）的**同目录下**。

### 窗口是如何定义的？

- 为了方便初学者使用，创建窗口是无需定义窗口大小，而是**恒为800*600像素**，并且不支持改变。
- 对于高分辨率屏幕（例如2560*1440），Magic框架**会**进行双线性插值，**自动放大窗口**到**合适的尺寸**。

## 2. 窗口相关

### Magic()

启动Magic框架，并创建一个窗口：

```c
void Magic(int fps = 60);
```

**参数**

- fps：指定窗口的最大帧率（锁帧），可以不填写，默认为60

**返回值**

无

**示例**

```c
Magic();  // 一行代码创建窗口
```

### Quit()

退出Magic框架，并关闭之前创建的窗口：

```c
void Quit();
```

**参数**

无

**返回值**

无

**示例**

```c
Quit();  // 关闭窗口，退出框架
```

### Show()

刷新窗口，把刚才所有的像素改动显示出来：

```c
void Show();
```

> 注意：Magic采用双缓冲架构，确保了稳定性。因为有双缓冲，所以所有的像素改动并不会马上显示到屏幕上，而是要调用 `Show()` 后才会显示。所以您必须在每一帧的绘画结束后，调用 `Show()` 函数。

**参数**

无

**返回值**

无

**示例**

```c
while (true) {
	// Draw Something
	;;;
	
	// Update Screen
	Show();
}
```

### fpsLockRate

一个全局变量，用于设置窗口的最大帧率（锁帧效果）：

> 该全局变量会被定时读取，一旦用户更改后，马上自动生效，无需额外操作

```c
int fpsLockRate;
```

**参数**

无

**返回值**

无

**示例**

```c
fpsLockRate = 120;  // 修改后自动生效
```

### isWinFocus

一个全局变量，用于读取窗口是否处于焦点状态（也就是用户是否在使用这个窗口）：

> 该全局变量会被定时写入，用户可以直接读取，得到最新值，无需额外操作

```c
bool isWinFocus;
```

**参数**

无

**返回值**

无

**示例**

```c
if (isWinFocus) {
	printf("This window is focused.");
}
else {
	printf("This window is not focused.");
}
```

## 3. 绘制像素

### Fill()

用特定的颜色，填满某个矩形区域：

```c
void Fill(int x0, int y0, int x1, int y1, unsigned char r, unsigned char g, unsigned char b);
```

**参数**

- x0：矩形左上角的X坐标
- y0：矩形左上角的Y坐标
- x1：矩形右下角的X坐标
- y1：矩形右下角的Y坐标
- r：颜色的红色分量（取值范围：0-255）
- g：颜色的绿色分量（取值范围：0-255）
- b：颜色的蓝色分量（取值范围：0-255）

**返回值**

无

**示例**

```c
Fill(100, 100, 200, 200, 0, 0, 0);  // 用黑色填满一个 (100, 100) -> (200, 200) 的正方形
```

### Clean()

用特定的颜色，填满整个窗口：

```c
void Clean(unsigned char r, unsigned char g, unsigned char b);
```

**参数**

- r：颜色的红色分量（取值范围：0-255）
- g：颜色的绿色分量（取值范围：0-255）
- b：颜色的蓝色分量（取值范围：0-255）

**返回值**

无

**示例**

```c
Clean(255, 255, 255);  // 用白色填满整个窗口
```

### MagicSetPixel()

单独设置一个像素的颜色：

```c
void MagicSetPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b);
```

**参数**

- x：像素的X坐标
- y：像素的Y坐标
- r：颜色的红色分量（取值范围：0-255）
- g：颜色的绿色分量（取值范围：0-255）
- b：颜色的蓝色分量（取值范围：0-255）

**返回值**

无

**示例**

```c
MagicSetPixel(100, 100, 255, 0, 0);  // 设置位于 (100, 100) 的像素的颜色为：红色
```

### MagicGetPixel()

获取某一个像素的颜色：

```c
void MagicGetPixel(int x, int y, unsigned char* r, unsigned char* g, unsigned char* b);
```

**参数**

- x：像素的X坐标
- y：像素的Y坐标
- *r：输出式参数，输出颜色的红色分量（取值范围：0-255）
- *g：输出式参数，输出颜色的绿色分量（取值范围：0-255）
- *b：输出式参数，输出颜色的蓝色分量（取值范围：0-255）

**返回值**

无

**示例**

```c
unsigned char r, g, b;
MagicGetPixel(100, 100, &r, &g, &b);
printf("%d %d %d", r, g, b);  // 输出结果：255 255 255
```

### MagicGetR()

获取某一个像素颜色的红色分量：

```c
unsigned char MagicGetR(int x, int y);
```

**参数**

- x：像素的X坐标
- y：像素的Y坐标

**返回值**

- unsigned char：输出颜色的红色分量

**示例**

```c
printf("%d", MagicGetR(100, 100));
```

### MagicGetG()

获取某一个像素颜色的绿色分量：

```c
unsigned char MagicGetG(int x, int y);
```

**参数**

- x：像素的X坐标
- y：像素的Y坐标

**返回值**

- unsigned char：输出颜色的绿色分量

**示例**

```c
printf("%d", MagicGetG(100, 100));
```

### MagicGetB()

获取某一个像素颜色的蓝色分量：

```c
unsigned char MagicGetB(int x, int y);
```

**参数**

- x：像素的X坐标
- y：像素的Y坐标

**返回值**

- unsigned char：输出颜色的蓝色分量

**示例**

```c
printf("%d", MagicGetB(100, 100));
```

## 4. 绘制图像

### Picture 类型

Magic框架提供的新数据类型，用于存储一张图片，可以用下面的方式创建：

```c
Picture picture;
```

**参数**

无

**返回值**

无

**示例**

```c
Picture myPicture1;  // 创建第一张图片
Picture myPicture2;  // 创建第一张图片
```

### picture.width / picture.height

Picture类型的两个成员变量，用于读取图片的宽度/高度：

```
picture.width;
picture.height;
```

**参数**

无

**返回值**

无

**示例**

```c
printf("%d %d", myPicture.width, myPicture.height);  // 打印出 myPicture1 的宽度和高度
```

### picture.Load()

Picture类型的自带函数，用于从文件中加载图片（仅支持加载BMP格式的图片文件）：

```c
void Picture::Load(const char* filename);
```

**参数**

- filename：一个字符串，用于指定BMP文件的路径

**返回值**

无

**示例**

```c
myPicture.Load(".\\BMP\\hamster.bmp");  // 将 hamster.bmp 文件加载到 myPicture 中，为后续使用
```

### picture.Draw()

Picture类型的自带函数，用于把图片绘制到屏幕上：

```c
void Picture::Draw(int x_, int y_);
```

**参数**

- x_：要绘制的位置，左上角的X坐标
- y_：要绘制的位置，左上角的X坐标

**返回值**

无

**特性**

- 支持边界裁剪，可以安全的在任意超出屏幕的XY坐标上绘制，而不会发生崩溃问题
- 支持透明绘制，会自动将RGB值为 `(255, 0, 255)` 的像素识别为透明，跳过绘制透明像素

**示例**

```c
myPicture.Draw(200, 200);  // 将 myPicture 图片绘制到屏幕上 (200, 200) 的位置
```

### picture.DrawZoom()

Picture类型的自带函数，用于按照比例，将图片添加缩放效果，然后绘制到屏幕上：

```c
void Picture::DrawZoom(int x_, int y_, float ratio);
```

**参数**

- x_：要绘制的位置，左上角的X坐标
- y_：要绘制的位置，左上角的X坐标
- ratio：缩放的比例；取值范围：`[0.0f, 1.0f]`

**返回值**

无

**特性**

- 支持边界裁剪，可以安全的在任意超出屏幕的XY坐标上绘制，而不会发生崩溃问题
- 支持透明绘制，会自动将RGB值为 `(255, 0, 255)` 的像素识别为透明，跳过绘制透明像素
- 以图片的**左上角**，作为缩放的中心点；如果图片缩小后，左上角位置保持不变

**示例**

```c
myPicture.DrawZoom(200, 200, 0.75f);  // 将 myPicture 图片缩放到0.75倍，绘制在屏幕上
```

### picture.DrawZoomCentered()

Picture类型的自带函数，用于按照比例，将图片添加缩放效果，然后绘制到屏幕上：

与 `picture.DrawZoom()` 不同的是，该函数以图片的**中心点**作为缩放的中心点。

```c
void Picture::DrawZoomCentered(int x_, int y_, float ratio);
```

**参数**

- x_：要绘制的位置，左上角的X坐标
- y_：要绘制的位置，左上角的X坐标
- ratio：缩放的比例；取值范围：`[0.0f, 1.0f]`

**返回值**

无

**特性**

- 支持边界裁剪，可以安全的在任意超出屏幕的XY坐标上绘制，而不会发生崩溃问题
- 支持透明绘制，会自动将RGB值为 `(255, 0, 255)` 的像素识别为透明，跳过绘制透明像素
- 以图片的**中心点**，作为缩放的中心点；如果图片缩小后，左上角位置保持不变

**示例**

```c
myPicture.DrawZoomCentered(200, 200, 0.75f);  // 将 myPicture 图片缩放到0.75倍，绘制在屏幕上
```

### picture.DrawBrightness()

Picture类型的自带函数，用于按照亮度值，将图片添加亮度变化，然后绘制到屏幕上：

```c
void Picture::DrawBrightness(int x_, int y_, float brightness);
```

**参数**

- x_：要绘制的位置，左上角的X坐标
- y_：要绘制的位置，左上角的X坐标
- brightness：亮度值；取值范围：`[0.0f, 正无穷]`

**返回值**

无

**特性**

- 支持边界裁剪，可以安全的在任意超出屏幕的XY坐标上绘制，而不会发生崩溃问题
- 支持透明绘制，会自动将RGB值为 `(255, 0, 255)` 的像素识别为透明，跳过绘制透明像素

**示例**

```c
myPicture.DrawBrightness(200, 200, 0.5f);  // 将 myPicture 图片亮度调为0.5，绘制在屏幕上
```

### picture.SetPixel()

单独设置一个像素的颜色：

```c
void Picture::SetPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b);
```

**参数**

- x：像素的X坐标
- y：像素的Y坐标
- r：颜色的红色分量（取值范围：0-255）
- g：颜色的绿色分量（取值范围：0-255）
- b：颜色的蓝色分量（取值范围：0-255）

**返回值**

无

**示例**

```c
myPicture.SetPixel(100, 100, 255, 0, 0);  // 设置位于 (100, 100) 的像素的颜色为：红色
```

### picture.GetPixel()

获取某一个像素的颜色：

```c
void Picture::GetPixel(int x, int y, unsigned char* r, unsigned char* g, unsigned char* b);
```

**参数**

- x：像素的X坐标
- y：像素的Y坐标
- *r：输出式参数，输出颜色的红色分量（取值范围：0-255）
- *g：输出式参数，输出颜色的绿色分量（取值范围：0-255）
- *b：输出式参数，输出颜色的蓝色分量（取值范围：0-255）

**返回值**

无

**示例**

```c
unsigned char r, g, b;
myPicture.GetPixel(100, 100, &r, &g, &b);
printf("%d %d %d", r, g, b);  // 输出结果：255 255 255
```

### picture.GetR()

获取某一个像素颜色的红色分量：

```c
unsigned char Picture::GetR(int x, int y);
```

**参数**

- x：像素的X坐标
- y：像素的Y坐标

**返回值**

- unsigned char：输出颜色的红色分量

**示例**

```c
printf("%d", myPicture.GetR(100, 100));
```

### picture.GetG()

获取某一个像素颜色的绿色分量：

```c
unsigned char Picture::GetG(int x, int y);
```

**参数**

- x：像素的X坐标
- y：像素的Y坐标

**返回值**

- unsigned char：输出颜色的绿色分量

**示例**

```c
printf("%d", myPicture.GetG(100, 100));
```

### picture.GetB()

获取某一个像素颜色的蓝色分量：

```c
unsigned char Picture::GetB(int x, int y);
```

**参数**

- x：像素的X坐标
- y：像素的Y坐标

**返回值**

- unsigned char：输出颜色的蓝色分量

**示例**

```c
printf("%d", myPicture.GetB(100, 100));
```

## 5. 音频播放

### MagicMusic()

音乐播放接口，用于播放音频，仅支持WAV格式音频：

```c
void MagicMusic(const char* command);
```

**参数**

- command：一个字符串，传入音频命令（该字符串为窄字符串，**不支持中文**）

**音频接口介绍**

- Magic 图形框架，定义了一套易于使用的、使用自定义音频终端命令，控制的音频接口API
- 无需创建和使用任何的变量、对象、函数，即可控制音乐的播放、暂停

**总体使用原则**

- **命令分为 `n` 个部分**，每个部分用空格隔开

- 命令的 `n` 个部分，**不可省略，不可调换顺序，否则都会出错**（未来将支持省略、调换顺序）

- **可以同时播放多个音频**，用通道数来指定。通道共有128个，通道数为0到127

- **第0通道有特殊优化处理**，使用流加载，节约内存，更加稳定，因此适合播放长音频 (>=30秒的音频)

  如果您有长音频 / 背景音乐需要播放，请尽量**安排在第0通道**。而第1到127通道完全相同，没有额外处理

**命令使用方法：**

-----

#### Open 命令：打开音频文件

```c
MagicMusic("open .\\Music\\bg.wav on_channel 0");
```

各部分填写方法：

- 第1部分：字符串 `open`，用于唤醒 `open` 命令
- 第2部分：音频文件的路径，**不支持中文**；**支持路径空格，只需使用双引号包裹整个路径即可**；例如：`".\\Music\\background music 1.wav"`
- 第3部分：字符串 `on_channel`，用于指定音频播放通道，**注意：`on` 和 `channel` 之间不能有空格！**
- 第4部分：使用哪一个通道（多音频播放），取值范围0到127

#### Play 命令：播放音频

```c
MagicMusic("play channel 0 times -1");
```

各部分填写方法：

- 第1部分：字符串 `play`，用于唤醒 `play` 命令
- 第2部分：字符串 `channel`，用于指定音频播放通道
- 第3部分：使用哪一个通道（多音频播放），取值范围0到127
- 第4部分：字符串 `times`，用于指定播放几次音频
- 第5部分：播放几次音频，填写一个数字，`-1` 代表无限循环播放，`0` 代表播放一次，`1` 代表播放两次，`2` 代表播放三次，以此类推

**特别提醒**：当使用有限次数播放（例如 `MagicMusic("play channel 0 times 0");` ）时，如果播放完毕，要进行下一次播放，必须先运行 Stop 命令 `MagicMusic("stop channel 0");`，才能再次运行 Play 命令。

#### Stop 命令：停止播放

```c
MagicMusic("stop channel 0");
```

各部分填写方法：

- 第1部分：字符串 `stop`，用于唤醒 `stop` 命令
- 第2部分：字符串 `channel`，用于指定音频播放通道
- 第3部分：使用哪一个通道（多音频播放），取值范围0到127

#### Pause 命令：暂停播放

```c
MagicMusic("pause channel 0");
```

各部分填写方法：

- 第1部分：字符串 `pause`，用于唤醒 `pause` 命令
- 第2部分：字符串 `channel`，用于指定音频播放通道
- 第3部分：使用哪一个通道（多音频播放），取值范围0到127

#### Resume 命令：继续播放

```c
MagicMusic("resume channel 0");
```

各部分填写方法：

- 第1部分：字符串 `resume`，用于唤醒 `resume` 命令
- 第2部分：字符串 `channel`，用于指定音频播放通道
- 第3部分：使用哪一个通道（多音频播放），取值范围0到127

#### Close 命令：关闭音频文件

```c
MagicMusic("close channel 0");
```

各部分填写方法：

- 第1部分：字符串 `close`，用于唤醒 `close` 命令
- 第2部分：字符串 `channel`，用于指定音频播放通道
- 第3部分：使用哪一个通道（多音频播放），取值范围0到127

-----

**返回值**

无

<u>虽然没有返回值，但当出现错误（命令非法 / 命令参数不对）时，会自动在命令行显示错误信息，供开发者检查。</u>

**示例**

```c
#include <stdio.h>
#include "Magic.h"

int main() {
	Magic();

	MagicMusic("open .\\Music\\MapBGM.wav on_channel 0");
	MagicMusic("play channel 0 times -1");
	MagicMusic("open .\\Music\\FootSound.wav on_channel 1");
	MagicMusic("play channel 1 times -1");

	while (true) {
		for (int i = 0; i < 100; i++) {
			MagicSetPixel(100, 100, 255, 255, 255);
			printf("Pushed.\n");
			Show();
		}

		MagicMusic("pause channel 0");
		MagicMusic("pause channel 1");

		for (int i = 0; i < 100; i++) {
			MagicSetPixel(100, 100, 255, 255, 255);
			printf("Pushed.\n");
			Show();
		}

		MagicMusic("resume channel 0");
		MagicMusic("resume channel 1");
	}

	MagicMusic("stop channel 0");
	MagicMusic("close channel 0");
	MagicMusic("stop channel 1");
	MagicMusic("close channel 1");
}
```

## 6. 文字显示

暂未实现，请稍等

## 7. 键盘鼠标检测

### mouseX / mouseY

两个全局变量，用于读取鼠标的X和Y坐标：

> 该全局变量会被定时写入，用户可以直接读取，得到最新值，无需额外操作

```c
int mouseX;
int mouseY;
```

**参数**

无

**返回值**

无

**示例**

```c
printf("%d %d", mouseX, mouseY);  // 输出：155 263
```

### isLeftClick / isRightClick

两个全局变量，用于读取鼠标的左键/右键是否被点击。

如果鼠标键处于按下状态，则变量为 `true`；如果鼠标键处于松开状态，则变量为 `false` 。

> 该全局变量会被定时写入，用户可以直接读取，得到最新值，无需额外操作

```c
bool isLeftClick;
bool isRightClick;
```

**参数**

无

**返回值**

无

**示例**

```c
if (isLeftClick) {
	printf("The left button of the mouse have been clicked.");
}

if (isRightClick) {
	printf("The right button of the mouse have been clicked.");
}
```

### keyboard

一个全局数组变量，用于读取键盘的任何一个键，是否被点击。

如果某个键处于按下状态，数组的对应位置会变为非0值；如果某个键处于松开状态，对应位置的值会变为0。

> 该全局数组变量会被定时写入，用户可以直接读取，得到最新值，无需额外操作

```c
char keyboard[256];
```

**用法说明**

- 对于英文字母，例如A键：可以通过 `keyboard['A']` 来获得按键状态

  > 注意：`keyboard['A']` 必须大写，`keyboard['a']` 不行

- 对于阿拉伯数字，例如3键：可以通过 `keyboard['3']` 来获得按键状态

  > 注意：`keyboard['3']` 必须为字符 `'3'` 而不是数字 `3` ，`keyboard[3]` 不行

- 对于特殊按键，例如上箭头键：可以通过 `keyboard[VK_UP]` 来获得按键状态

  > 解释：`VK_UP` 是一个宏定义，代表上箭头键。每一个特殊按键都有一个宏定义，见后文表格

**参数**

无

**返回值**

无

**示例**

```c
if (keyboard['A']) {
	printf("Key A is Pressed!");
}

if (keyboard['3']) {
	printf("Key 3 is Pressed!");
}

if (keyboard[VK_UP]) {
	printf("Arrow Up Key is Pressed!");
}
```

### 附表：特殊按键的宏定义

| 常数                     | Value   | 说明                                                         |
| :----------------------- | :------ | :----------------------------------------------------------- |
| `VK_LBUTTON`             | 0x01    | 鼠标左键                                                     |
| `VK_RBUTTON`             | 0x02    | 鼠标右键                                                     |
| `VK_CANCEL`              | 0x03    | 控制中断处理                                                 |
| `VK_MBUTTON`             | 0x04    | 鼠标中键                                                     |
| `VK_XBUTTON1`            | 0x05    | X1 鼠标按钮                                                  |
| `VK_XBUTTON2`            | 0x06    | X2 鼠标按钮                                                  |
| `-`                      | 0x07    | 保留                                                         |
| `VK_BACK`                | 0x08    | BACKSPACE 键                                                 |
| `VK_TAB`                 | 0x09    | Tab 键                                                       |
| `-`                      | 0x0A-0B | 预留                                                         |
| `VK_CLEAR`               | 0x0C    | CLEAR 键                                                     |
| `VK_RETURN`              | 0x0D    | Enter 键                                                     |
| `-`                      | 0x0E-0F | 未分配                                                       |
| `VK_SHIFT`               | 0x10    | SHIFT 键                                                     |
| `VK_CONTROL`             | 0x11    | CTRL 键                                                      |
| `VK_MENU`                | 0x12    | Alt 键                                                       |
| `VK_PAUSE`               | 0x13    | PAUSE 键                                                     |
| `VK_CAPITAL`             | 0x14    | CAPS LOCK 键                                                 |
| `VK_KANA`                | 0x15    | IME Kana 模式                                                |
| `VK_HANGUL`              | 0x15    | IME Hanguel 模式                                             |
| `VK_IME_ON`              | 0x16    | IME 打开                                                     |
| `VK_JUNJA`               | 0x17    | IME Junja 模式                                               |
| `VK_FINAL`               | 0x18    | IME 最终模式                                                 |
| `VK_HANJA`               | 0x19    | IME Hanja 模式                                               |
| `VK_KANJI`               | 0x19    | IME Kanji 模式                                               |
| `VK_IME_OFF`             | 0x1A    | IME 关闭                                                     |
| `VK_ESCAPE`              | 0x1B    | ESC 键                                                       |
| `VK_CONVERT`             | 0x1C    | IME 转换                                                     |
| `VK_NONCONVERT`          | 0x1D    | IME 不转换                                                   |
| `VK_ACCEPT`              | 0x1E    | IME 接受                                                     |
| `VK_MODECHANGE`          | 0x1F    | IME 模式更改请求                                             |
| `VK_SPACE`               | 0x20    | 空格键                                                       |
| `VK_PRIOR`               | 0x21    | PAGE UP 键                                                   |
| `VK_NEXT`                | 0x22    | PAGE DOWN 键                                                 |
| `VK_END`                 | 0x23    | END 键                                                       |
| `VK_HOME`                | 0x24    | HOME 键                                                      |
| `VK_LEFT`                | 0x25    | LEFT ARROW 键                                                |
| `VK_UP`                  | 0x26    | UP ARROW 键                                                  |
| `VK_RIGHT`               | 0x27    | RIGHT ARROW 键                                               |
| `VK_DOWN`                | 0x28    | DOWN ARROW 键                                                |
| `VK_SELECT`              | 0x29    | SELECT 键                                                    |
| `VK_PRINT`               | 0x2A    | PRINT 键                                                     |
| `VK_EXECUTE`             | 0x2B    | EXECUTE 键                                                   |
| `VK_SNAPSHOT`            | 0x2C    | PRINT SCREEN 键                                              |
| `VK_INSERT`              | 0x2D    | INS 键                                                       |
| `VK_DELETE`              | 0x2E    | DEL 键                                                       |
| `VK_HELP`                | 0x2F    | HELP 键                                                      |
|                          | 0x30    | 0 键                                                         |
|                          | 0x31    | 1 个键                                                       |
|                          | 0x32    | 2 键                                                         |
|                          | 0x33    | 3 键                                                         |
|                          | 0x34    | 4 键                                                         |
|                          | 0x35    | 5 键                                                         |
|                          | 0x36    | 6 键                                                         |
|                          | 0x37    | 7 键                                                         |
|                          | 0x38    | 8 键                                                         |
|                          | 0x39    | 9 键                                                         |
| `-`                      | 0x3A-40 | Undefined                                                    |
|                          | 0x41    | A 键                                                         |
|                          | 0x42    | B 键                                                         |
|                          | 0x43    | C 键                                                         |
|                          | 0x44    | D 键                                                         |
|                          | 0x45    | E 键                                                         |
|                          | 0x46    | F 键                                                         |
|                          | 0x47    | G 键                                                         |
|                          | 0x48    | H 键                                                         |
|                          | 0x49    | I 键                                                         |
|                          | 0x4A    | J 键                                                         |
|                          | 0x4B    | K 键                                                         |
|                          | 0x4C    | L 键                                                         |
|                          | 0x4D    | M 键                                                         |
|                          | 0x4E    | N 键                                                         |
|                          | 0x4F    | O 键                                                         |
|                          | 0x50    | P 键                                                         |
|                          | 0x51    | Q 键                                                         |
|                          | 0x52    | R 键                                                         |
|                          | 0x53    | S 键                                                         |
|                          | 0x54    | T 键                                                         |
|                          | 0x55    | U 键                                                         |
|                          | 0x56    | V 键                                                         |
|                          | 0x57    | W 键                                                         |
|                          | 0x58    | X 键                                                         |
|                          | 0x59    | Y 键                                                         |
|                          | 0x5A    | Z 键                                                         |
| `VK_LWIN`                | 0x5B    | 左 Windows 键                                                |
| `VK_RWIN`                | 0x5C    | 右侧 Windows 键                                              |
| `VK_APPS`                | 0x5D    | 应用程序密钥                                                 |
| `-`                      | 0x5E    | 预留                                                         |
| `VK_SLEEP`               | 0x5F    | 计算机休眠键                                                 |
| `VK_NUMPAD0`             | 0x60    | 数字键盘 0 键                                                |
| `VK_NUMPAD1`             | 0x61    | 数字键盘 1 键                                                |
| `VK_NUMPAD2`             | 0x62    | 数字键盘 2 键                                                |
| `VK_NUMPAD3`             | 0x63    | 数字键盘 3 键                                                |
| `VK_NUMPAD4`             | 0x64    | 数字键盘 4 键                                                |
| `VK_NUMPAD5`             | 0x65    | 数字键盘 5 键                                                |
| `VK_NUMPAD6`             | 0x66    | 数字键盘 6 键                                                |
| `VK_NUMPAD7`             | 0x67    | 数字键盘 7 键                                                |
| `VK_NUMPAD8`             | 0x68    | 数字键盘 8 键                                                |
| `VK_NUMPAD9`             | 0x69    | 数字键盘 9 键                                                |
| `VK_MULTIPLY`            | 0x6A    | 乘号键                                                       |
| `VK_ADD`                 | 0x6B    | 加号键                                                       |
| `VK_SEPARATOR`           | 0x6C    | 分隔符键                                                     |
| `VK_SUBTRACT`            | 0x6D    | 减号键                                                       |
| `VK_DECIMAL`             | 0x6E    | 句点键                                                       |
| `VK_DIVIDE`              | 0x6F    | 除号键                                                       |
| `VK_F1`                  | 0x70    | F1 键                                                        |
| `VK_F2`                  | 0x71    | F2 键                                                        |
| `VK_F3`                  | 0x72    | F3 键                                                        |
| `VK_F4`                  | 0x73    | F4 键                                                        |
| `VK_F5`                  | 0x74    | F5 键                                                        |
| `VK_F6`                  | 0x75    | F6 键                                                        |
| `VK_F7`                  | 0x76    | F7 键                                                        |
| `VK_F8`                  | 0x77    | F8 键                                                        |
| `VK_F9`                  | 0x78    | F9 键                                                        |
| `VK_F10`                 | 0x79    | F10 键                                                       |
| `VK_F11`                 | 0x7A    | F11 键                                                       |
| `VK_F12`                 | 0x7B    | F12 键                                                       |
| `VK_F13`                 | 0x7C    | F13 键                                                       |
| `VK_F14`                 | 0x7D    | F14 键                                                       |
| `VK_F15`                 | 0x7E    | F15 键                                                       |
| `VK_F16`                 | 0x7F    | F16 键                                                       |
| `VK_F17`                 | 0x80    | F17 键                                                       |
| `VK_F18`                 | 0x81    | F18 键                                                       |
| `VK_F19`                 | 0x82    | F19 键                                                       |
| `VK_F20`                 | 0x83    | F20 键                                                       |
| `VK_F21`                 | 0x84    | F21 键                                                       |
| `VK_F22`                 | 0x85    | F22 键                                                       |
| `VK_F23`                 | 0x86    | F23 键                                                       |
| `VK_F24`                 | 0x87    | F24 键                                                       |
| `-`                      | 0x88-8F | 保留                                                         |
| `VK_NUMLOCK`             | 0x90    | NUM LOCK 键                                                  |
| `VK_SCROLL`              | 0x91    | SCROLL LOCK 键                                               |
| `-`                      | 0x92-96 | OEM 特有                                                     |
| `-`                      | 0x97-9F | 未分配                                                       |
| `VK_LSHIFT`              | 0xA0    | 左 SHIFT 键                                                  |
| `VK_RSHIFT`              | 0xA1    | 右 SHIFT 键                                                  |
| `VK_LCONTROL`            | 0xA2    | 左 Ctrl 键                                                   |
| `VK_RCONTROL`            | 0xA3    | 右 Ctrl 键                                                   |
| `VK_LMENU`               | 0xA4    | 左 ALT 键                                                    |
| `VK_RMENU`               | 0xA5    | 右 ALT 键                                                    |
| `VK_BROWSER_BACK`        | 0xA6    | 浏览器后退键                                                 |
| `VK_BROWSER_FORWARD`     | 0xA7    | 浏览器前进键                                                 |
| `VK_BROWSER_REFRESH`     | 0xA8    | 浏览器刷新键                                                 |
| `VK_BROWSER_STOP`        | 0xA9    | 浏览器停止键                                                 |
| `VK_BROWSER_SEARCH`      | 0xAA    | 浏览器搜索键                                                 |
| `VK_BROWSER_FAVORITES`   | 0xAB    | 浏览器收藏键                                                 |
| `VK_BROWSER_HOME`        | 0xAC    | 浏览器“开始”和“主页”键                                       |
| `VK_VOLUME_MUTE`         | 0xAD    | 静音键                                                       |
| `VK_VOLUME_DOWN`         | 0xAE    | 音量减小键                                                   |
| `VK_VOLUME_UP`           | 0xAF    | 音量增加键                                                   |
| `VK_MEDIA_NEXT_TRACK`    | 0xB0    | 下一曲目键                                                   |
| `VK_MEDIA_PREV_TRACK`    | 0xB1    | 上一曲目键                                                   |
| `VK_MEDIA_STOP`          | 0xB2    | 停止媒体键                                                   |
| `VK_MEDIA_PLAY_PAUSE`    | 0xB3    | 播放/暂停媒体键                                              |
| `VK_LAUNCH_MAIL`         | 0xB4    | 启动邮件键                                                   |
| `VK_LAUNCH_MEDIA_SELECT` | 0xB5    | 选择媒体键                                                   |
| `VK_LAUNCH_APP1`         | 0xB6    | 启动应用程序 1 键                                            |
| `VK_LAUNCH_APP2`         | 0xB7    | 启动应用程序 2 键                                            |
| `-`                      | 0xB8-B9 | 预留                                                         |
| `VK_OEM_1`               | 0xBA    | 用于杂项字符；它可能因键盘而异。 对于美国标准键盘，键`;:`    |
| `VK_OEM_PLUS`            | 0xBB    | 对于任何国家/地区，键`+`                                     |
| `VK_OEM_COMMA`           | 0xBC    | 对于任何国家/地区，键`,`                                     |
| `VK_OEM_MINUS`           | 0xBD    | 对于任何国家/地区，键`-`                                     |
| `VK_OEM_PERIOD`          | 0xBE    | 对于任何国家/地区，键`.`                                     |
| `VK_OEM_2`               | 0xBF    | 用于杂项字符；它可能因键盘而异。 对于美国标准键盘，键`/?`    |
| `VK_OEM_3`               | 0xC0    | 用于杂项字符；它可能因键盘而异。 对于美国标准键盘，键``~`    |
| `-`                      | 0xC1-DA | 保留                                                         |
| `VK_OEM_4`               | 0xDB    | 用于杂项字符；它可能因键盘而异。 对于美国标准键盘，键`[{`    |
| `VK_OEM_5`               | 0xDC    | 用于杂项字符；它可能因键盘而异。 对于美国标准键盘，键`\\|`   |
| `VK_OEM_6`               | 0xDD    | 用于杂项字符；它可能因键盘而异。 对于美国标准键盘，键`]}`    |
| `VK_OEM_7`               | 0xDE    | 用于杂项字符；它可能因键盘而异。 对于美国标准键盘，键`'"`    |
| `VK_OEM_8`               | 0xDF    | 用于杂项字符；它可能因键盘而异。                             |
| `-`                      | 0xE0    | 预留                                                         |
| `-`                      | 0xE1    | OEM 特有                                                     |
| `VK_OEM_102`             | 0xE2    | 美国标准键盘上的 `<>` 键，或非美国 102 键键盘上的 `\\|` 键   |
| `-`                      | 0xE3-E4 | OEM 特有                                                     |
| `VK_PROCESSKEY`          | 0xE5    | IME PROCESS 键                                               |
| `-`                      | 0xE6    | OEM 特有                                                     |
| `VK_PACKET`              | 0xE7    | 用于将 Unicode 字符当作键击传递。 `VK_PACKET` 键是用于非键盘输入法的 32 位虚拟键值的低位字。 有关更多信息，请参阅 [`KEYBDINPUT`](https://learn.microsoft.com/zh-cn/windows/win32/api/winuser/ns-winuser-keybdinput)、[`SendInput`](https://learn.microsoft.com/zh-cn/windows/win32/api/winuser/nf-winuser-sendinput)、[`WM_KEYDOWN`](https://learn.microsoft.com/zh-cn/windows/win32/inputdev/wm-keydown) 和 [`WM_KEYUP`](https://learn.microsoft.com/zh-cn/windows/win32/inputdev/wm-keyup) 中的注释 |
| `-`                      | 0xE8    | 未分配                                                       |
| `-`                      | 0xE9-F5 | OEM 特有                                                     |
| `VK_ATTN`                | 0xF6    | Attn 键                                                      |
| `VK_CRSEL`               | 0xF7    | CrSel 键                                                     |
| `VK_EXSEL`               | 0xF8    | ExSel 键                                                     |
| `VK_EREOF`               | 0xF9    | Erase EOF 键                                                 |
| `VK_PLAY`                | 0xFA    | Play 键                                                      |
| `VK_ZOOM`                | 0xFB    | Zoom 键                                                      |
| `VK_NONAME`              | 0xFC    | 预留                                                         |
| `VK_PA1`                 | 0xFD    | PA1 键                                                       |
| `VK_OEM_CLEAR`           | 0xFE    | Clear 键                                                     |

> 上文表格来自：https://learn.microsoft.com/zh-cn/windows/win32/inputdev/virtual-key-codes
>
> 您可能已经注意到，Magic框架中的特殊按键宏定义，与Win32 API是保持一致的。这是为了方便习惯使用Win32 API的用户，能更快上手我们的框架。

