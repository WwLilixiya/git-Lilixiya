# C++17 图书管理系统复习项目

本项目以一条“建模图书 → 管理馆藏 → 持久化 → 命令行交互 → 验证”的主线复习 C++ 基础语法。详细的语法说明、注意事项和分阶段练习见 `C++图书管理系统语法复习报告.docx`。

## 构建

环境：支持 C++17 的 g++（本机已用 MinGW-w64 g++ 8.1.0 验证）。在 PowerShell 中：

```powershell
cd D:\chatGPT\list
.\build.ps1
.\library_tests.exe .\test_catalog.db
.\syntax_lab.exe
```

直接编译主程序也可以：

```powershell
g++ -std=c++17 -Wall -Wextra -Wpedantic -I include src/book.cpp src/storage.cpp src/library.cpp src/main.cpp -o library_manager.exe
```

## 运行

```powershell
cd D:\chatGPT\list
.\library_manager.exe .\books.db
```

首次成功修改时会创建数据文件。传入不同的路径可使用独立书库；不传参数时，在当前工作目录使用 `books.db`。菜单支持新增、列表排序、关键词搜索、借阅、归还、删除和统计，成功修改后自动保存。删除有未归还副本的图书会被拒绝。

## 文件结构

| 文件 | 作用 |
| --- | --- |
| `include/book.hpp`、`src/book.cpp` | 图书类、封装、构造校验、运算符重载 |
| `include/library.hpp`、`src/library.cpp` | 馆藏操作、容器、算法、lambda、指针与引用 |
| `include/storage.hpp`、`src/storage.cpp` | 抽象接口、虚函数、文本文件读写 |
| `src/main.cpp` | 菜单、控制流、函数模板、输入与异常处理 |
| `tests/test_library.cpp` | 业务流程、边界条件与文件往返测试 |
| `examples/syntax_lab.cpp` | 与书架相关的补充语法实验 |
| `build.ps1` | 一键编译主程序与测试程序 |

数据格式为 `LIBRARY_V1` 文本，每行一条图书记录，字符串使用 `std::quoted`。请把 `books.db` 视作应用数据，不要在程序运行时手工编辑。

## 范围说明

这是单机命令行教学项目，借阅按副本计数，不记录读者、借阅日期或并发操作。搜索是大小写敏感的子串匹配。文件保存采用覆盖写入，进程在写入时意外中断可能造成数据损坏；可把临时文件加原子替换作为进阶练习。
