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

## 开源声明

本软件依据 Mulan PSL v2 进行开源，授予您永久性的、全球性的、免费的、非独占的、不可撤销的版权许可。您可以复制、使用、修改、分发该软件，不论修改与否。

协议内容详见本目录下“LICENSE”文件。

## Demo 展示

![Demo_Trampoline.gif](./Documentation/GetStarted/C00Setup.assets/Demo_Trampoline.gif)

![SinWaveEffect.gif](./Documentation/GetStarted/C03Picture.assets/SinWaveEffect.gif)

## 开发日记

开发之旅饶有趣味，既能汲取诸多知识，又能催生不少新想法。

于是，萌生出了写开发日记的想法，记录一些开发时的思路，供未来的自己参考：

- [关于加载动画和非线性动画的几件事](./Documentation/Machanism/AboutLoadingAnimationAndNonLinearity.md)
- [讨厌的MSVC编译器优化](./Documentation/Machanism/AnnoyingMSVCOptimization.md)
- [三重指针：参数解析器背后的故事](./Documentation/Machanism/ArgParserMachanism.md)
- [位图加载：如何加载各种离谱的非规范BMP文件](./Documentation/Machanism/BitmapLoaderMachanism.md)
- [双缓冲：前后帧缓冲的交换机制和时序](./Documentation/Machanism/BufferSwapingMachanism.md)
- [Client-Host分离机制，和背后的设计考量](./Documentation/Machanism/ClientHostSeperation.md)
- [DebuggerLog小工具，和宽字符串转换](./Documentation/Machanism/DebuggerLogMachanism.md)
- [缩放绘制位图：两种设计方案的优劣](./Documentation/Machanism/DrawZoomMachanism.md)
- [FPS计数器的实现原理](./Documentation/Machanism/FPSCounterMachanism.md)
- [FPS锁帧器的实现原理](./Documentation/Machanism/FPSLockerMachanism.md)
- [高精度：如何实现纳秒级精确度的Clock&Sleep](./Documentation/Machanism/HighPrecisionTimeMachanism.md)
- [Invoke 机制解释](./Documentation/Machanism/InvokeMachanism.md)
- [Magic音频接口：如何实现的？](./Documentation/Machanism/MagicMusicInterfaceMachanism.md)
- [内部参考文档：进程管道的变量分布](./Documentation/Machanism/SharedPipeVariableDistribution.md)
- [基于进程管道的长字符串分段传输方法](./Documentation/Machanism/StringTransferMachanism.md)
- [Magic框架背后的故事：为什么开发这个框架？](./Documentation/Machanism/TheStoryBehindThisFrameworkAndWhyDevelopThis.md)
- [为什么编译基于Magic程序，如此简单？](./Documentation/Machanism/WhyCompilationSoEasy.md)
- [为什么不提供DeltaTime？](./Documentation/Machanism/WhyDontWeUseDeltatime.md)
- [为什么要使用固定的分辨率？](./Documentation/Machanism/WhyTheWindowSizeIsFixed.md)
- [为什么不适用Win32 API提供的Winmm多媒体库，来实现音频接口，而是外部引入SDL库？](./Documentation/Machanism/WhyWeUseSDLNotWinmmAsAudioInterface.md)
- [Magic框架，未来会支持3D渲染 / OpenGL API吗？](./Documentation/Machanism/Will3DOrOpenGLBeSupportedInTheFuture.md)
- [Magic框架，未来会支持跨平台 / 手机应用开发吗？](./Documentation/Machanism/WillCrossPlatformBeSupportedInTheFuture.md)


