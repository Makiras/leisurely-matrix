# 软件系统的性能工程实验

## 性能示例

| 版本  |实现| 运行时间 | 相对上版本加速比 | 相对Python加速比 | GFLOPS | %Peak |
|:-----:|:--:|:--------:|:---------------:|:----------------:|:------:|:-------:|
| 1     |Python| 16972.949575 | 1.000000 | 1.000000 | 0.000000 | 0.000000 |
| 2     |Java8 | 432.705506 | 39.225176 | 39.225176 | 0.000000 | 0.000000 |
| 3     |C | 3287.045410 | 0.131640 | 5.163588 | 0.000000 | 0.000000 |
| 4     |+循环顺序 ikj| 152.977417 | 21.487128 | 110.998000 | 0.000000 | 0.000000 |
| 5     |+编译优化 O3| 54.958866 | 2.999000 | 308.830054 | 0.000000 | 0.000000 |
| 6     |Cilk并行| 1.548951 | 35.481346 | 10957.706001 | 0.000000 | 0.000000 |
| 7     |+循环分块(32)| 1.047338 | 1.478941 | 16205.799441 | 0.000000 | 0.000000 |
| 8     |递归分治 | 0.905799 | 1.156259 |18738.097055 | 0.000000 | 0.000000 |
| 9     |+向量化 | 0.703191 | 1.288127 | 24137.040398| 0.000000 | 0.000000 |
| 10    |+AVX512 | 0.070781 | 9.934742 | 239795.278040 | 0.000000 | 0.000000 |
| 11    |Intel MKL| 0.230618 | 0.306919 | 73597.679170 | 0.000000 | 0.000000 |

// GFLOPS, %Peak TBD

## 上述实验环境

```
OS: CentOS Linux 8 x86_64 
Host: 2288H V5 Purley 
Kernel: 4.18.0-240.el8.x86_64 
Uptime: 1 day, 4 hours, 50 mins 
Packages: 1723 (rpm) 
Shell: zsh 5.5.1 
Terminal: make 
CPU: Intel Xeon Gold 6226R (64) @ 3.900GHz 
GPU: Intelligent Management system chip w/VGA support] 
Memory: 10767MiB / 257261MiB 

Python 3.6.8

openjdk version "1.8.0_312"
OpenJDK Runtime Environment (build 1.8.0_312-b07)
OpenJDK 64-Bit Server VM (build 25.312-b07, mixed mode)

clang version 10.0.1 (https://github.com/OpenCilk/opencilk-project 8435006eea5d32f9cf895a666496b89613e2cbc1)
Target: x86_64-unknown-linux-gnu
Thread model: posix
InstalledDir: /opt/OpenCilk-10.0.1-Linux/bin

Intel(R) oneAPI Math Kernel Library (oneMKL) Link Tool v6.3 help
/opt/intel/oneapi/mkl/2022.1.0/
```

## 如何使用

### 环境配置

#### Cilk

Link: https://github.com/OpenCilk/opencilk-project/releases/tag/opencilk/v1.0  
Extract To: /opt/OpenCilk-10.0.1-Linux

> If not installation is not in /opt, please modify the Makefile


#### Intel MKL (Part of Intel oneAPI)

Page: https://www.intel.com/content/www/us/en/developer/tools/oneapi/onemkl-download.html

- Plz Follow the installation guide to install Intel oneAPI.

> If not installation is not in /opt/intel/oneapi/mkl/2022.1.0/, please modify the Makefile.

### 使用

> 希望同学们可以互帮互助，如果有问题可以在issue中提出，或者在群里讨论。

#### 自行编译运行

0. 修改并填充代码中的空缺部分。
1. 参考Makefile中的编译命令，自行编译运行。所有 **-D \.\*** 宏均可直接在代码中修改，并在编译命令中删去。
2. mkl需要进入`matrix_mkl`目录 `cmake . && camke --build . && ./matrix_mkl` 编译运行。

#### 使用现有Makefile

0. 依据指示修改Makefile中的依赖库路径（Ubuntu和CentOS大概率不用更改）。
1. 修改代码中的空缺部分。
2. `make part_name` 运行某部分代码（请注意依照**代码中已有的宏**填写**循环及阈值参数**）
3. `make` 运行所有测试

> Tips:  
> 1. `make MSIZE=n` 会将矩阵大小设置为n*n。可以使用128进行调试，1024或4096等进行结果生成。
> 2. `make > log` 将会把输出重定向到log文件中，方便查看结果。
> 3. `make clean` 会清除所有生成的文件。
> 4. `make part_name` 将会只运行你指定的修改部分，建议了解简单的Makefile语法。  


>可用的 `part_name` 有：  
>- `python`, `java`  
>- `c_class`  ( include: `c_raw`, `c_loop_order`, `c_optimizer` )  
>- `c_optimized`  ( include: `cilk_class`, `cilk_super` )   
>- `cilk_class`  ( include: `cilk` `cilk_block` `cilk_recu` `cilk_vec` )
>- `cilk_super`  ( include: `cilk_avx` `cilk_mkl` )



