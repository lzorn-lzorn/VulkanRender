初始化git子模块
```
git submodule update --init --recursive
```


如果出现编译时选择 32bit 的情况, 需要强制指定 MSVC 位 64位
```powershell
Remove-Item -Recurse -Force .\build -ErrorAction SilentlyContinue;
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug `
  -DCMAKE_C_COMPILER="C:/Program Files/.../clang.exe" `
  -DCMAKE_CXX_COMPILER="C:/Program Files/.../clang.exe" `
  -DCMAKE_C_FLAGS="-m64" -DCMAKE_CXX_FLAGS="-m64" `
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON;
cmake --build build --config Debug
```
Windows 下不同位宽（x86 vs x64）对象文件和导入库使用不同的 ABI 和符号约定：
x86 stdcall 名称修饰常出现 @NN 后缀（例如 _vkDestroyInstance@8），x64 则没有这样的 @NN 后缀，调用约定不同且符号命名不一致。
之前的失败是“对象用 32-bit ABI 编译，但链接的 Vulkan lib 是 64-bit（或相反）”造成的 ABI/符号不匹配，最终导致链接器找不到 vk* 的符号。
通过显式设置编译器为 clang 并传 -m64，CMake 和 clang 在配置阶段和编译阶段都将目标架构设为 x64：
CMake 报告 “Detecting Target CPU Architecture - X64”，意味着内部 CMake 变量（例如 CMAKE_SIZEOF_VOID_P）现在为 8，CMake 会选择与之匹配的库和选项（例如 link 到 Lib/vulkan-1.lib 的 x64 形式是正确的）。
生成的对象使用 x64 ABI，符号与 Vulkan x64 import lib 匹配，链接器能够解析 vk* 符号，错误消失。
