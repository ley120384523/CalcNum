# 目标

从一堆数中，取出N个数相加，使之与给定的数最接近

# 第三方库

C++标准库和boost库

# 使用

将一堆数复制到 `新建文本文档.txt` 中，每一行一个数字。运行本程序一段时间，时间随着数据量的增加而增加。

运行完成后，会生成 `result.csv` 文件。保存了所有最近的可能。按照与给定数据相差的绝对值的大小进行排序，即第一行数据与给定的数最接近。

# 提示

为提高计算效率，程序采用线程池的方案，运行时会跑满CPU。
