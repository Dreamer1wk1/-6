.data
array:      .word 9,8,7,6,5,4,10,23,12,100  # 初始化待排序的数组
STR1:       .asciiz "before sort the array is: \n"  # 输出字符串，表示排序前
STR2:       .asciiz "after sort the array is: \n"   # 输出字符串，表示排序后
CONTROL:    .word32 0x10000  # 控制信息地址
DATA:       .word32 0x10008  # 数据输出地址
            .space 64         # 预留64字节空间
sp:         .word32 0         # 堆栈指针初始化

.text
main:
    ld r1, CONTROL($zero)     # 加载控制地址0x10000到r1
    ld r2, DATA($zero)        # 加载数据地址0x10008到r2
    
    daddi r0, $zero, 0
    daddi r3, r0, STR1        # 将STR1地址加载到r3
    daddi r4, r0, 4           # 将4加载到r4，用于输出控制
    sd r3, 0(r2)              # 将STR1的地址存入数据地址
    sd r4, 0(r1)              # 将输出控制值存入控制地址

    daddi r3, $zero, 0        # 初始化r3为0
print1:
    ld r4, array(r3)          # 加载array中的元素到r4
    sd r4, 0(r2)              # 将元素存入数据地址
    daddi r5, $zero, 2        # 输出控制值为2
    sd r5, 0(r1)              # 更新控制地址的输出值
    daddi r3, r3, 8           # r3自增8，移动到下一个元素
    daddi r4, r3, -80         # 检查是否输出完10个元素
    beqz r4, print_end1       # 如果输出完毕，跳转到print_end1
    j print1                  # 否则继续输出

print_end1:  
    daddi r0, $zero, 0
    daddi r3, r0, STR2        # 将STR2地址加载到r3
    daddi r4, r0, 4           # 将4加载到r4
    sd r3, 0(r2)              # 将STR2的地址存入数据地址
    sd r4, 0(r1)              # 更新输出控制值
    
    daddi $sp, $zero, sp      # 初始化堆栈指针
    daddi r5, $zero, array    # 将array地址存入r5
    daddi r6, $zero, 10       # 将数组长度10存入r6
    jal sort                  # 调用排序函数
    
swap:
    dsll r6, r6, 3            # r6 = j * 8，计算当前元素地址偏移
    dadd r11, r6, r5          # r11指向当前要交换的元素地址
    ld r12, 0(r11)            # 加载第一个元素到r12
    ld r13, 8(r11)            # 加载第二个元素到r13
    sd r12, 8(r11)            # 将第一个元素存入第二个位置
    sd r13, 0(r11)            # 将第二个元素存入第一个位置
    jr $ra                     # 返回到调用位置
    
sort:
    daddi $sp, $sp, -64       # 为保存寄存器分配堆栈空间
    sd $ra, 0($sp)            # 保存返回地址
    sd r7, 8($sp)             # 保存r7
    sd r8, 16($sp)            # 保存r8
    sd r9, 24($sp)            # 保存r9
    sd r10, 32($sp)           # 保存r10
    sd r11, 40($sp)           # 保存r11
    sd r12, 48($sp)           # 保存r12
    sd r13, 56($sp)           # 保存r13

    dadd r8, r6, $zero        # 将数组长度赋值给r8
    daddi r9, $zero, 1        # 初始化外层循环变量i为1
    j mid1

loop1:  
    daddi r10, $zero, 0       # 初始化内层循环变量j为0
    j mid2

loop2:
    dsll r7, r10, 3           # r7 = j * 8，计算当前元素地址偏移
    dadd r7, r7, r5           # r7指向当前数组元素
    ld r11, 0(r7)             # 加载第j个元素到r11
    ld r12, 8(r7)             # 加载第j+1个元素到r12
    slt r13, r12, r11         # 如果r12 < r11，则设置r13=1
    beq r13, $zero, NotSwap   # 如果没有需要交换，跳转到NotSwap
    dadd r6, $zero, r10       # 将j的值赋给r6，用于swap
    jal swap                  # 调用交换函数
NotSwap:
    daddi r10, r10, 1         # j自增1
mid2:
    dadd r3, r9, r10          # 计算i + j
    bne r3, r8, loop2         # 如果i + j != length，继续内层循环
    daddi r9, r9, 1           # i自增1
mid1:
    bne r8, r9, loop1         # 如果i != length，继续外层循环
    
sort_end:
    ld $ra, 0($sp)            # 恢复返回地址
    ld r7, 8($sp)             # 恢复r7
    ld r8, 16($sp)            # 恢复r8
    ld r9, 24($sp)            # 恢复r9
    ld r10, 32($sp)           # 恢复r10
    ld r11, 40($sp)           # 恢复r11
    ld r12, 48($sp)           # 恢复r12
    ld r13, 56($sp)           # 恢复r13
    daddi $sp, $sp, 64        # 释放堆栈空间
    
    daddi r3, $zero, 0
print2:
    ld r4, array(r3)          # 加载已排序的数组元素
    sd r4, 0(r2)              # 存入数据地址
    daddi r5, $zero, 2        # 输出控制值为2
    sd r5, 0(r1)              # 更新控制地址的输出值
    daddi r3, r3, 8           # r3自增8，移动到下一个元素
    daddi r4, r3, -80         # 检查是否输出完10个元素
    beqz r4, ishalt           # 如果输出完毕，跳转到结束
    j print2                  # 否则继续输出

ishalt:
    halt                      # 程序结束
