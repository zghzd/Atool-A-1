# Atool-A-1:sky_candle
# 版本号
Atool-A-1项目使用以下格式的版本号：  
`X.Y.Z-yy.mm-(A|B|G|P)X.Y.Z`  
其中，前导X.Y.Z分别为
主版本号X，次版本号Y，修订号Z；后接yy为发布年，mm为发布月；
后置X.Y.Z为特性版本号。  
A表示程序正处于早期开发阶段，极不稳定。  
B表示程序核心行为不再发生变化，进入测试阶段。  
G表示程序开发已进入后期，主要进行bug修复。  
P表示发布为正式版。  

# 可加载文件
## 自动加载配置
应用程序启动时，总是尝试读取工作目录下的`autoload.atoola1.config`文
件，并尝试将此文件中声明的路径作为可加载文件自动加载。  
使用此方法编写本文件：

- 一行写入一个路径，且路径需要指向有效文件

## 配置文件
以下内容解释了一般的配置文件编写方法，按照以下格式，以纯文本
方式编写$name.config配置文件。
```
无等号行为无效行，可被程序读取，但是不会加载，实际编写文件时，
不建议出现此类行。

scene-code=
大于0的整数，指定场景编号
name-cn=
name-en=
分别是场景中英文名称，可以为空（Unknown或未知）
APBS=
Y/y/T/t均为启用，指定此场景任一点可以直接返回起点
datef=
时间因素，
格式为"YYYY-MM-DDThh:mm:ss+-hhmm;IntNum",当声明的日期与本地日期差可被IntNum（整数）整除时，启用本配置文件
Sdate=
Edate=
分别为起止时间，不指定则一直可用，
格式为"YYYY-MM-DDThh:mm:ss+-hhmm"
LightData=
指向LightData数据库文件，若有多个，用;分隔，可为空
TpData=
指向TpData数据库文件，若有多个，用;分隔，可为空
```

## LightData数据库文件
LightData数据库文件按照以下格式编写(不包括示例中的表头)
，一行一组数据，列之间以
水平制表符（`\t`）分隔  
```
coordinate	light_num	name_cn	name_en
floatX,Y,Z	float	str	str
```

## TpData数据库文件
TpData数据库文件按照以下格式编写(不包括示例中的表头)
，一行一组数据，列之间以
水平制表符（`\t`）分隔  
```
coordinate	next_scene_code	next_scene_coordinate	name_cn	name_en
floatX,Y,Z	int	floatX,Y,Z	str	str
```
