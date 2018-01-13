# 2D-car-game

改小游戏由c++纯c++写出，使用[EGE](http://xege.org/)图形库开发

EGE图形库应该只支持在windows下开发，但是支持微软的VC编译器和MinGW编译器，图形库的具体安装就不描述了，EGE官网有[安装教程](http://xege.org/%E6%96%B0%E6%89%8B%E5%85%A5%E9%97%A8%E3%80%8E%E4%B8%80%E3%80%8Fege%E7%9A%84%E4%BB%8B%E7%BB%8D%E3%80%81%E5%AE%89%E8%A3%85%E5%92%8C%E4%BD%BF%E7%94%A8.html)

## 使用
由于就只用一个main.cpp文件，程序使用很简单，在编译器安装EGE图形库后，用vs即可运行，如果使用GCC编译器也可以用命令行来运行，在fork项目mian.cpp文件目录下，运行命令
``` shell
g++ main.cpp -std=c++11 -lgraphics -lgdi32 -limm32 -lmsimg32 -lole32 -loleaut32 -lwinmm -luuid -mwindows -o game
```

编译产生的二进制程序当前目录必须有img,mus目录和log.txt文件

bin.7z中为编译好的二进制文件，运行game.exe即可