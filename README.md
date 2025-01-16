# Magic.h

## 项目简介

简洁优雅的图形学编程框架，**仅需一行代码即可创建窗口**！

```c
#include "Magic.h"

int main(){
	Magic();
}
```

## 使用说明

我们准备了完善的入门教程，帮助你快速上手，迈入图形学编程的魔法大门：

### 新手教程

- [Chapter.0 下载配置](./Documentation/GetStarted/C00Setup.md)
- [Chapter.1 快速开始](./Documentation/GetStarted/C01Intro.md)
- [Chapter.2 绘制像素](./Documentation/GetStarted/C02Pixels.md)
- [Chapter.3 绘制图片](./Documentation/GetStarted/C03Picture.md)

### API 接口文档

- [API 接口文档](./Documentation/APIs/APIs.md)

🍊 如果遇到任何技术问题，欢迎加入QQ交流群进行讨论：981285817

## 开源声明

本软件依据 Mulan PSL v2 进行开源，授予您永久性的、全球性的、免费的、非独占的、不可撤销的版权许可。您可以复制、使用、修改、分发该软件，不论修改与否。

协议内容详见本目录下“LICENSE”文件。

本工程使用了 SDL 和 SDL_mixer 库作为音频接口的依赖项，非常感谢 SDL 项目组提供优秀的开源软件。关于 SDL 和 SDL_mixer 库的开源协议，详见：`./MagicHost/AudioDependency/SDL2-2.30.10-LICENSE.txt` 和 `SDL2_mixer-2.8.0-LICENSE.txt`

## Demo 展示

![Demo_Trampoline.gif](./Documentation/GetStarted/C00Setup.assets/Demo_Trampoline.gif)

![SinWaveEffect.gif](./Documentation/GetStarted/C03Picture.assets/SinWaveEffect.gif)

Magic.h 图形框架，也非常适合构建复杂的完整游戏程序。例如由作者开发的示例程序：MagicXiyou 魔法西游

魔法西游是一款回合制战斗游戏的Demo示例程序，用于演示Magic.h图形框架的性能和可能性。

目前实现了如下特性：

- 开始界面：按钮Hover效果，Logo图标的非线性缩放动画
- 地图界面：2.5D正交投影的地图场景，实现人物自由移动，实现人物帧动画
- 战斗场景：战斗特效，魔法特效，非线性动画，血量扣除，文字提示信息渲染

您可以点击如下链接，下载并游玩该游戏：

- https://gitee.com/SirJackie/MagicXiyou
- https://github.com/SirJackie/MagicXiyou

![MagicXiyou_Preview_v1.0.0](README.assets/MagicXiyou_Preview_v1.0.0.png)

## 关于 MagicHost 编译

> 注：用户无需阅读本节内容即可使用框架，该节属于为框架开发者准备的备注

- MagicHost必须使用Release x64设置进行编译
- Release设置能确保框架的运行性能：同等硬件情况下，Release能稳定165FPS，Debug仅能达到45FPS
- x64设置能确保与调用的SDL DLL相兼容。如果您希望使用x86编译，请将根目录所有SDL DLL更换为x86版

## 开发日记

开发之旅饶有趣味，既能汲取诸多知识，又能催生不少新想法。

于是，萌生出了写开发日记的想法，记录一些开发时的思路，供未来的自己参考：

### 关于框架

- [Magic框架背后的故事：为什么开发这个框架？](./Documentation/Machanism/TheStoryBehindThisFrameworkAndWhyDevelopThis.md)
- [为什么编译基于Magic程序，如此简单？](./Documentation/Machanism/WhyCompilationSoEasy.md)
- [Client-Host分离机制，和背后的设计考量](./Documentation/Machanism/ClientHostSeperation.md)

### 关于进程管道

- [内部参考文档：进程管道的变量分布](./Documentation/Machanism/SharedPipeVariableDistribution.md)
- [基于进程管道的长字符串分段传输方法](./Documentation/Machanism/StringTransferMachanism.md)

### 关于图形绘制

- [Invoke 机制解释](./Documentation/Machanism/InvokeMachanism.md)
- [双缓冲：前后帧缓冲的交换机制和时序](./Documentation/Machanism/BufferSwapingMachanism.md)
- [讨厌的MSVC编译器优化](./Documentation/Machanism/AnnoyingMSVCOptimization.md)
- [位图加载：如何加载各种离谱的非规范BMP文件](./Documentation/Machanism/BitmapLoaderMachanism.md)
- [缩放绘制位图：两种设计方案的优劣](./Documentation/Machanism/DrawZoomMachanism.md)
- [关于加载动画和非线性动画的几件事](./Documentation/Machanism/AboutLoadingAnimationAndNonLinearity.md)

### 关于FPS

- [FPS计数器的实现原理](./Documentation/Machanism/FPSCounterMachanism.md)
- [FPS锁帧器的实现原理](./Documentation/Machanism/FPSLockerMachanism.md)
- [高精度：如何实现纳秒级精确度的Clock&Sleep](./Documentation/Machanism/HighPrecisionTimeMachanism.md)

### 音乐、文字和杂项

- [Magic音频接口：如何实现的？](./Documentation/Machanism/MagicMusicInterfaceMachanism.md)
- [三重指针：参数解析器背后的故事](./Documentation/Machanism/ArgParserMachanism.md)
- [DebuggerLog小工具，和宽字符串转换](./Documentation/Machanism/DebuggerLogMachanism.md)

### FAQ 常见问题

- [为什么不提供DeltaTime？](./Documentation/Machanism/WhyDontWeUseDeltatime.md)
- [为什么要使用固定的分辨率？](./Documentation/Machanism/WhyTheWindowSizeIsFixed.md)
- [为什么不适用Win32 API提供的Winmm多媒体库，来实现音频接口，而是外部引入SDL库？](./Documentation/Machanism/WhyWeUseSDLNotWinmmAsAudioInterface.md)

### 未来展望

- [Magic框架，未来会支持3D渲染 / OpenGL API吗？](./Documentation/Machanism/Will3DOrOpenGLBeSupportedInTheFuture.md)
- [Magic框架，未来会支持跨平台 / 手机应用开发吗？](./Documentation/Machanism/WillCrossPlatformBeSupportedInTheFuture.md)


