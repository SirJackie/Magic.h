# Invoke 机制解释

> 注意：该文档并不是框架用户文档，而是框架开发者文档，用户无需阅读该文档。

## 目的

Invoke机制，是使用进程管道，唤醒MagicHost执行特定任务的机制。

## 禁用优化

由于MSVC编译器优化的特性，会导致Invoke操作无效化。

任何需要使用Invoke机制的函数，都必须在函数代码的上方和下方，加入如下MSVC优化禁用代码：

```c
// DISABLE MSVC OPTIMIZATION: START
#if defined(_MSC_VER)
#pragma optimize( "", off )
#endif

void Function() {
    ;
}

// DISABLE MSVC OPTIMIZATION: END
#if defined(_MSC_VER)
#pragma optimize( "", on )
#endif
```

## Invoke 管道变量

Invoke管道变量，由如下两个部分构成：

- `invokeSomething`：用于激发Invoke信号的变量
- `invokeReceived`：用于确认Invoke信号已经被接收的变量

以下是目前已有的Invoke变量列表（可能滞后，请以Magic.h文件中的最新状态为准）：

```c
// 激发Invoke信号：前后帧缓冲交换
#define invokeBufSwap  (((char*)G_pBuf)[ 1])   // [ 1]
// 确认Invoke信号已经被接收
#define invokeReceived (((char*)G_pBuf)[ 2])   // [ 2]

// 激发Invoke信号：开始长字符串传输
#define invokeTransfer (((char*)G_pBuf)[294])  // [294]
// 激发Invoke信号：开始长字符串的小Batch传输
#define invokeSendBtch (((char*)G_pBuf)[295])  // [295]

// 激发Invoke信号：发送音频终端命令
#define invokeMusic    (((char*)G_pBuf)[296])  // [296]
// 激发Invoke信号：发送文字终端命令
#define invokeText     (((char*)G_pBuf)[297])  // [297]
```

## Invoke 激发流程

### 流程步骤

1. 管道初始化时：`invokeSomething` 默认为0，`invokeReceived` 默认为0

2. Client端：将 `invokeReceived` 置为0，将 `invokeSomething` 置为1，表示要开始发送

   注意，设置顺序很重要，如果颠倒，会导致脏写入 `invokeReceived`，从而Host和Client写入冲突

   Host端：循环读取 `invokeSomething` ，发现 `invokeSomething` 变为1，于是被激活

   Host端：进行处理，执行响应程序

3. Host端：当且仅当处理完毕后，才将 `invokeSomething` 置为0，将 `invokeReceived` 置为1

   Client端：循环读取 `invokeReceived` ，发现 `invokeReceived` 变为1，于是得知信号收到

4. Client端：进行收尾工作，将 `invokeReceived` 设为0，恢复初始状态

### 时序表

（加粗字体：表示设置顺序的优先性）

| 状态 | `invokeSomething` | `invokeReceived` |
| ---- | ----------------- | ---------------- |
| #1   | 0                 | 0                |
| #2   | 1                 | **0**            |
| #3   | **0**             | 1                |
| #4   | 0                 | **0**            |

### 示例代码

Client：

```c
// Invoke Something
invokeReceived = 0;
invokeSomething = 1;
while(invokeReceived == 0);  // Wait for Response
invokeReceived = 0;
```

Host（非阻塞式）：

```c
while (true) {
    // Process Invoke Signal
    if (invokeSomething == 1) {
        // Do Something
        ;;;

        // Process Invoke Signal
        invokeSomething = 0;
        invokeReceived = 1;
    }
}
```

Host（阻塞式）：

```c
// Wait for Invoke Signal
while (invokeSomething == 0);

// Do Something
;;;

// Process Invoke Signal
invokeSomething = 0;
invokeReceived = 1;
```

