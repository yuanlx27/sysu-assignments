#import "@local/sysu-templates:0.2.0": report

#show: report.with(
  title: "CG-HW2",
  subtitle: "计算机图形学实验报告",
  student: (name: "元朗曦", id: "23336294"),
  institude: "计算机学院",
  major: "计算机科学与技术",
  class: "计八",
)

= 实验目的

+ 理解并掌握直线生成算法（DDA、Bresenham）。

+ 理解并掌握三角形光栅化算法（扫描线/Edge-Walking）。

+ 掌握三维几何变换（模型、视图、投影变换）。

+ 理解并实现 Z-Buffer 消隐算法。

+ 掌握基本的光照模型与着色算法（Gouraud Shading, Phong Shading）。

+ 熟悉使用 C++ 结合 OpenGL (仅用于显示)、GLFW、ImGUI 进行图形学编程。

= 实验环境

- *操作系统*：Windows

- *开发语言*：C++17

- *构建工具*：CMake

- *依赖库*：

    - `GLFW`：用于窗口创建和上下文管理。

    - `GLM`：用于数学运算（向量、矩阵变换）。

    - `ImGUI`：用于构建用户交互界面。

    - `OpenGL 3.3`：仅用于将软件光栅化生成的 Framebuffer 显示到屏幕上。

= 实验原理与实现

本次实验主要分为两个任务：2D 三角形光栅化和 3D 场景渲染。所有的图形绘制（点、线、三角形填充）均通过软件光栅化（Software Rasterization）实现，即直接操作内存中的像素数组 `std::vector<unsigned char> framebuffer`。

== 2D 三角形光栅化

=== 直线生成算法

为了绘制三角形的边框，实现了两种直线生成算法：

- *DDA (Digital Differential Analyzer)*：利用直线的微分特性，通过浮点数增量计算每一步的坐标。虽然逻辑简单，但涉及浮点运算。

- *Bresenham 算法*：仅使用整数加减法和位运算，避免了浮点数计算，效率更高。通过维护一个误差项 $d$ 来判断下一个像素点的选择。

=== 三角形填充 (Edge-Walking)

为了填充三角形内部，采用了扫描线算法（Edge-Walking）的思想：

+ 将三角形的三个顶点按 $y$ 坐标排序。

+ 将三角形分为上半部分（平底）和下半部分（平顶）。

+ 对于每一条扫描线 $y$，计算其与三角形左右两条边的交点 $x_("start")$ 和 $x_("end")$。

+ 在 $[x_("start"), x_("end")]$ 区间内填充像素颜色。

== 3D 场景渲染

=== 坐标变换

实现了标准的图形学管线变换：

$
  P_("clip") = M_("projection") dot M_("view") dot M_("model") dot P_("local")
$

- *Model Matrix*：控制物体的旋转（随时间更新）。

- *View Matrix*：使用 `glm::lookAt` 设置摄像机位置。

- *Projection Matrix*：使用 `glm::perspective` 设置透视投影。

- *Viewport Transform*：将 NDC 坐标映射到屏幕空间 $(0, 0)$ 到 $(W, H)$。

=== Z-Buffer 消隐算法

为了正确处理物体的前后遮挡关系，维护了一个与 Framebuffer 大小相同的深度缓冲区 `std::vector<float> zbuffer`。

- 初始化为 $1.0$（最远）。

- 在画点 `put_pixel` 时，比较当前像素的深度值 $z$ 与 buffer 中的值。

- 如果 $z < $ `zbuffer[index]`，则更新颜色和深度值。

=== 光照与着色

实现了 Blinn-Phong 光照模型，包含环境光、漫反射和镜面反射。

$
  I = I_("ambient") + I_("diffuse") + I_("specular")
$

在此基础上实现了两种着色频率：

- *Gouraud Shading*：在顶点着色器（Vertex Processing 阶段）计算光照颜色，光栅化时对颜色进行插值。计算量小，但高光效果较差。

- *Phong Shading (选做)*：在光栅化时对法线（Normal）和世界坐标（WorldPos）进行插值，对每个像素独立计算光照。高光更细腻真实。

== 交互功能实现 (ImGUI)

使用 ImGUI 实现了完善的控制面板：

- *任务切换*：可在 2D 和 3D 任务间无缝切换。

- *2D 控制*：开启/关闭填充、DDA 边框、Bresenham 边框。

- *3D 控制*：

  - 模型切换（立方体 / 四面体）。

  - 线框模式 (Wireframe)。

  - 着色模式切换 (Gouraud / Phong)。

  - 光源位置调节。

  - *性能监控*：实时显示光栅化耗时 (ms) 和帧率 (FPS)。

= 实验结果

视频见 `report/assets/showcase.mp4`。

== 2D 三角形光栅化

下图展示了 2D 任务的效果。橙色区域为 Edge-Walking 填充效果，青色边框为 Bresenham 算法绘制的边缘。

#figure(
  caption: [2D 三角形光栅化 (Fill + Bresenham Edges)],
  image("assets/2d-triangle-rasterization.png"),
)

== 3D 立方体 (Gouraud Shading)

下图展示了旋转的立方体，使用 Gouraud 着色。可以看到颜色在面上平滑过渡。

#figure(
  caption: [3D 立方体 (Gouraud Shading)],
  image("assets/3d-cube-gouraud-shading.png"),
)

== 3D 四面体 (Phong Shading)

下图展示了四面体模型，开启 Phong Shading。相比 Gouraud，Phong Shading 的高光更加聚拢和真实。

#figure(
  caption: [3D 四面体 (Phong Shading)],
  image("assets/3d-tetrahedron-phong-shading.png"),
)

== 线框模式

开启线框模式后，仅绘制模型的边缘，便于观察几何结构。

#figure(
  caption: [3D 线框模式],
  image("assets/3d-tetrahedron-wireframe-mode.png"),
)

= 实验总结

+ *算法理解*：通过亲手实现 `put_pixel` 和光栅化循环，深刻理解了图形硬件底层的工作原理，特别是重心坐标插值（或扫描线插值）在属性传递（颜色、深度、法线）中的核心作用。

+ *性能优化*：在实现 Bresenham 算法时，体会到了整数运算相比浮点运算的优势。在 3D 渲染中，背面剔除（Backface Culling）显著减少了需要光栅化的三角形数量，提升了性能。

+ *着色差异*：实验直观对比了 Gouraud 和 Phong Shading 的区别。Gouraud 在顶点数较少时会出现明显的马赫带效应或高光丢失，而 Phong Shading 虽然计算量大（每像素计算 `pow`），但画质显著提升。

+ *遇到的问题*：

  - 初期 Z-Buffer 深度方向搞反，导致物体渲染顺序错误。通过检查投影矩阵的 $z$ 范围和比较符号解决。

  - ImGUI 窗口遮挡问题，通过 `SetNextWindowPos` 调整了初始布局。

本次实验成功完成所有必做及选做内容，构建了一个功能完备的软光栅渲染器。