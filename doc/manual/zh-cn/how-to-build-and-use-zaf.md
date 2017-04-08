# 如何构建和使用zaf

zaf直接在Visual Studio上进行开发，因此只要打开解决方案文件`zaf.sln`，选择合适的构建配置来构建`zaf`项目即可。

zaf是一个静态链接库，构建结果位于`out`文件夹下，例如`out\Debug\zaf.lib`。

为了在你的项目中使用zaf，需要将头文件搜索路径设置到`src`文件夹；以及将库文件搜索路径设置到输出文件夹，例如`out\Debug`。