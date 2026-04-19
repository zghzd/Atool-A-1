# Atool-A-1  
---  
# Atool-A-1
一个外部的理想化《光·遇》（sky:children of the light）数据计算工具

# 运行环境
- 64位Windows操作系统，版本19045.7058及以上
- 至少1GiB空闲磁盘空间
- C++运行时

# 运行前置条件及环境搭建
1. 下载源代码并编译产出可执行文件，或下载预构建的二进制文件（部分版本提供）
2. 在你希望运行程序的位置新建一个空目录
3. 将可执行文件放置在2步骤建立的目录下
4. 启动cmd.exe，并将当前目录设置在上文建立的目录（cd \<path>）
5. 现在，你可以通过cmd命令运行本程序

# 启动与参数
程序命令语法为：  
` Atool-A-1 --total_candle-<float> [--use-<file>] [--owned_candle-<float>] [--max_candle-<float>] [--no-<keyword>] (-en|-cn) `  
其中，--total_candle为你所需的蜡烛总数；
--use指定程序加载的文件；
--owned_candle为你已有的蜡烛数量；
--max_candle指定每日获得的最大蜡烛数量；
--no用于程序管理的名称数据，所有包含keyword的名称对应的数据会被排除；
-en指定英文输出，-cn指定中文。  
示例：` Atool-A-1 --total_candle-45.0 --use-afile --owned_candle-0.0 --max_candle-20 --no-晨岛 -cn `

# 其他文件  
- [sky_candle](doc-sky_candle.md)
- [更新日志](UpdateLog.md)

# 许可协议  
除非特别说明，本存储库使用
[CC-BY-4.0](https://creativecommons.org/licenses/by/4.0/legalcode.txt)
许可协议。  
根据许可协议，你必须提供适当的署名，许可人指定的有效署名是：  

- 你必须在署名中链接到本
[存储库](https://github.com/zghzd/Atool-A-1):
https://github.com/zghzd/Atool-A-1
- 同时链接到开发者邮箱:
dzfg688523@dingtalk.com  

# 外部场景配置文件
用户可以根据需求，自行编写一些可被程序加载的文件，
若要编写这种文件，请严格按照以下格式：
```text
Atool-A-1:config:1.0:sky_candle:scene
{scene_number;scene_name}
{from_scene_number;thisX,Y,Z;fromX,Y,Z/from_scene_number;thisX,Y,Z;fromX,Y,Z/...}
{light_number;X,Y,Z;name}/...
{baseline_time_YYYY-MM-DDThh:mm:ss±hhmm;ableNum}

```
注：  
- 如果允许多个元素的位置中只有一个，那么，严禁添加末尾"/"分隔符，
即"/"永远不能出现在末尾。
- 文件内中文可能导致控制台输出乱码。
- 不符合要求的配置文件将导致程序发生不可恢复的崩溃错误。

