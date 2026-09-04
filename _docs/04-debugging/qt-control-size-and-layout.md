---
id: LINEAR-MOTOR-DBG-001
title: Qt 控件尺寸与布局比例问题定位和修复
document_type: debugging
module: ui-layout
status: confirmed

implementation_status: partially-implemented
verification_status: unverified

related_requirements: []
related_documents: []
code_refs:
  - ../../main/widget.ui
  - ../../main/widget.cpp
  - ../../main/widget.h
config_refs:
  - ../../CMakePresets.json

created: 2026-09-03
updated: 2026-09-03
summary: 说明 LinearMotor 项目中 Qt 控件不随窗口缩放或布局比例不生效时的定位顺序、修复方式和验证边界。
---

# Qt 控件尺寸与布局比例问题定位和修复

## 1. 调试目标

当界面出现控件无法随窗口缩放、局部区域比例异常、设置 stretch 后看不到变化或高分辨率下内容溢出时，按固定顺序定位真正限制尺寸的对象，避免只调整最外层窗口或反复修改无效的布局属性。

本文只讨论 Qt Widgets 布局和控件尺寸问题。显示缩放、字体渲染和操作系统 DPI 属于相关但不同的问题。

## 2. 典型现象

- 主窗口可以缩放，但页面内容仍保持设计时的固定宽高；
- `QStackedWidget`、页面或 `QFrame` 超出窗口可用区域；
- `QHBoxLayout` 或 `QVBoxLayout` 设置 stretch 后，两侧仍不像目标比例；
- Qt Designer 对象树的显示顺序与 stretch 实际对应对象不一致；
- 控件的 `.ui` 属性已修改，但运行时又被 C++ 或样式表恢复为固定尺寸；
- 修改 `.ui` 后编译仍引用已经删除的控件。

## 3. 已确认原因

### 3.1 外层固定尺寸会阻止整个页面收缩

外层容器的 `minimumSize` 会向内层传递空间要求。即使内部控件设置为 `Expanding`，外层仍可能强制窗口保持较大的最小尺寸。

本项目曾在 `stackedWidget` 上设置 `2380 x 1650` 的最小尺寸。当前实现已移除该限制。

### 3.2 子控件的最小或最大尺寸会覆盖布局分配结果

常见组合包括：

- `minimumWidth` 与 `maximumWidth` 相同；
- `minimumHeight` 与 `maximumHeight` 相同；
- 按钮、标签或页面容器固定为设计分辨率下的像素值；
- 子控件虽然位于布局中，但最大尺寸限制使其无法获得布局分配的空间。

修复时应同时检查最小值和最大值。只删除 `minimumSize`，而保留较小的 `maximumSize`，控件仍然不能放大。

### 3.3 stretch 按布局项索引生效，不按对象树视觉顺序生效

必须从 `.ui` 文件中的 `<item>` 顺序或 `row`、`column` 属性确认映射关系。Qt Designer 对象树主要反映对象归属，不能替代布局项索引。

例如 `verticalLayout_23` 当前为 `stretch="3,8"`，其含义由第 0、1 个布局项决定。若目标对象顺序与对象树观察结果不同，应交换权重，而不是继续增大数值。

### 3.4 stretch 不是无条件的绝对像素比例

布局会先满足以下约束，再按 stretch 分配可用空间：

1. `minimumSize`；
2. 子布局计算出的 `minimumSizeHint`；
3. `maximumSize`；
4. `QSizePolicy`；
5. 布局边距、间距和 spacer；
6. 剩余空间才按 stretch 分配。

因此，设置 `3:8` 只表示两个区域获得额外空间时的相对权重。若一侧包含多个固定为 50px 高的控件，它可能先占用内容所需高度，最终显示不会严格等于 3:8。

### 3.5 C++ 和样式表可以覆盖 `.ui` 设置

除 `.ui` 外，还必须搜索以下运行时调用：

```text
setFixedSize
setFixedWidth / setFixedHeight
setMinimumSize / setMaximumSize
setMinimumWidth / setMaximumWidth
setMinimumHeight / setMaximumHeight
setSizePolicy
setStretch / setRowStretch / setColumnStretch
min-width / min-height / max-width / max-height
```

本项目的 `configureResponsiveLayout()` 在 `ui->setupUi(this)` 后执行，会覆盖部分 `.ui` 尺寸属性。样式初始化中也存在按钮最小尺寸，因此判断最终效果时必须同时阅读 `widget.ui` 和 `widget.cpp`。

## 4. 标准定位步骤

### 4.1 确认异常对象的 objectName

在 Qt Designer 对象树中选中异常控件，记录：

- 控件名称；
- 直接父对象；
- 直接父布局；
- 控件在父布局中的索引或网格行列；
- 外层直到页面根节点的所有容器。

不要只记录最内层控件。尺寸限制可能位于任意一层父容器。

### 4.2 从 `.ui` 确认真实布局层级

使用 objectName 搜索：

```powershell
rg -n -C 20 'name="目标对象名"' main/widget.ui
```

重点检查：

- `minimumSize` 和 `maximumSize`；
- `sizePolicy`；
- `stretch`、`rowstretch` 和 `columnstretch`；
- `<item row="..." column="...">`；
- 水平或垂直 spacer 的 `sizeHint`；
- 父布局是否确实安装在目标 `QWidget` 或 `QFrame` 上。

### 4.3 从外向内解除冲突约束

推荐修改顺序：

1. 移除页面、`QStackedWidget` 等外层容器的不合理固定尺寸；
2. 将需要跟随窗口变化的容器设为 `Expanding`；
3. 移除子控件冲突的最小或最大尺寸；
4. 最后设置 stretch；
5. 检查 C++ 和样式表是否再次写入尺寸。

不要一开始就把所有控件设为 `Ignored`。`Ignored` 会忽略 size hint，虽然可能强制接近目标比例，但窗口较小时容易造成文字、输入框或按钮被裁剪。

### 4.4 正确设置布局比例

`QHBoxLayout` 和 `QVBoxLayout` 使用项目顺序：

```cpp
layout->setStretch(0, 10);
layout->setStretch(1, 1);
```

`QGridLayout` 使用实际行列编号：

```cpp
gridLayout->setRowStretch(1, 1);
gridLayout->setRowStretch(2, 6);
```

如果第 0 行为空而控件位于第 1、2 行，`.ui` 中的 `rowstretch` 需要保留第 0 行占位，例如：

```xml
rowstretch="0,1,6"
```

### 4.5 检查生成 UI 与 C++ 引用是否一致

删除 `.ui` 控件后，必须同步清理：

- `ui->控件名` 访问；
- `on_控件名_信号名()` 自动连接槽；
- 头文件中的槽声明；
- 响应式控件列表中的指针。

CMake AUTOUIC 生成的头文件位于构建目录，不应把旧的手工生成 `ui_widget.h` 当作当前界面事实。

## 5. 本项目当前布局实例

以下配置已写入当前 `main/widget.ui`：

| 布局 | 当前比例 | 含义 |
| --- | --- | --- |
| `gridLayout_2` | `rowstretch="0,1,6"` | 顶部信息区与 `stackedWidget_2` 为 1:6 |
| `horizontalLayout_29` | `stretch="10,1"` | 开始页曲线区与右侧数据区为 10:1 |
| `horizontalLayout_37`～`40` | `stretch="10,1"` | 四个测试曲线区与右侧选项区为 10:1 |
| `gridLayout_113` | `rowstretch="1,4"` | 测试参数区域的上、下两行权重为 1:4 |
| `verticalLayout_23` | `stretch="3,8"` | 两个直接子项按实际索引分配 3:8 |

`configureResponsiveLayout()` 当前还执行以下工作：

- 为选定控件解除横向最大宽度并设置水平 `Expanding`；
- 对 `gridLayout_120` 的直接控件及其子控件解除宽高限制；
- 将 `gridLayout_120` 的所有行列 stretch 设置为 1。

## 6. 验证方法

### 6.1 静态检查

```powershell
[xml]$ui = Get-Content main/widget.ui -Raw -Encoding UTF8
git diff --check -- main/widget.ui main/widget.cpp main/widget.h
```

确认 XML 可解析，并检查目标控件是否仍存在冲突的尺寸属性。

### 6.2 编译验证

项目当前工具链为 Qt 5.12.11、MinGW GCC 7.3、x64。Debug 构建命令为：

```powershell
cmake --build --preset build-mingw-qt5.12.11-debug
```

编译通过只能证明 `.ui`、生成代码和 C++ 引用一致，不能证明界面比例符合视觉预期。

### 6.3 运行验证

至少检查以下窗口状态：

- 800×600 初始窗口；
- 常用 1080p 工作区；
- 2K 工作区；
- 最大化；
- 从大窗口连续缩小到允许的最小尺寸；
- “开始”“测试”“设置”“帮助”各页面切换后再次缩放。

观察内容：

- 控件是否被裁剪或重叠；
- 曲线区域是否随窗口伸缩；
- 比例是否与布局项索引一致；
- 状态灯是否因非等比拉伸变形；
- 按钮文字和输入框内容是否完整；
- 页面切换后是否出现不同的最小尺寸。

启动当前程序会尝试初始化 ACS 和 Modbus 通讯。运行验证必须与编译验证分开授权，且不得把“程序成功启动”记录为真实设备已经验证。

## 7. 当前验证状态

- 多次 Qt 5.12.11 / MinGW x64 Debug 编译已成功，说明已提交的布局 XML 能被当前 UIC 和编译器处理；
- 尚未完成覆盖 800×600、1080p、2K 和最大化状态的统一视觉验收；
- `verticalLayout_23` 的 3:8 是否符合最终视觉目标仍需运行确认；
- DPI 支持与布局尺寸问题应分别验证，避免把固定像素约束误判为系统缩放失败。
