#!/bin/bash

# 编译 write.c 和 read.c
gcc -o write write.c
if [ $? -ne 0 ]; then
	echo "编译 write.c 失败"
	exit 1
fi

gcc -o read read.c
if [ $? -ne 0 ]; then
	echo "编译 read.c 失败"
	exit 1
fi

# 先运行 write 程序
echo "运行 write 程序..."
./write & # 在后台运行 write

# 等待用户按下回车键
read -p "按回车键以继续并运行 read 程序..."

# 运行 read 程序
echo "运行 read 程序..."
./read
