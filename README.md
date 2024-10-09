.data
array:		.word 9,8,7,6,5,4,10,23,12,100
before:		.asciiz "before sort the array is: \n"
after:		.asciiz "after sort the array is: \n"
control:	.word32 0x10000
data:   	.word32 0x10008
			.space 64
sp:			.word32 0     #堆栈指针初始化
 
.text
main:
	ld r1, control($zero) 
	ld r2, data($zero)	

	daddi r0, $zero, 0
    #输出排序前的提示
	daddi r3, r0, before    #存入提示
	daddi r4, r0, 4   #设置为输出字符串
	sd r3, 0(r2)
	sd r4, 0(r1)
	daddi r3, $zero, 0

print_before:
	ld r4, array(r3)    #数组首地址
	sd r4, 0(r2)    #数组存到数据地址
	daddi r5, $zero, 2    #设置为输出有符号整型
	sd r5, 0(r1)    #输出
	daddi r3, r3, 8    #移动到下一个元素
	daddi r4, r3, -80    #判断元素是否输出完成
	beqz  r4, before_end    #输出完成则退出循环
	j print_before
	
before_end:  
	daddi r0, $zero, 0
	daddi r3, r0, after    #输出提示
	daddi r4, r0, 4   #设置为输出字符串
	sd r3, 0(r2)
	sd r4, 0(r1)
	
	daddi $sp, $zero, sp    #保存栈指针
	daddi r5, $zero, array_size    #保存数组地址
	daddi r6, $zero, 10    #保存数组长度
	jal sort
 
swap:
	dsll r6, r6, 3    #s6 =j*8，计算当前元素地址偏移
	dadd r11, r6, r5    #s11为当前数组地址
	ld r12, 0(r11)    #加载第一个元素
	ld r13, 8(r11)    #加载第一个元素到
	sd r12, 8(r11)    #交换
	sd r13, 0(r11)
	jr $ra    #返回到调用位置
	
sort:
	daddi $sp, $sp, -64    #为寄存器分配栈空间
	sd $ra, 0($sp)    #保存返回地址
	sd r7, 8($sp)     #保存swap需要用到的寄存器
	sd r8, 16($sp) 
	sd r9, 24($sp) 
	sd r10, 32($sp) 
	sd r11, 40($sp) 
	sd r12, 48($sp)
	sd r13, 56($sp)
 
	dadd r8, r6, $zero  #s8保存数组长度
	daddi r9, $zero, 1   #外层循环计数器初始化
	j out_judge

outer_loop:	
	daddi r10, $zero, 0  #内层循环计数器j=0
	j in_judge
inner_loop:
	dsll r7, r10, 3   #s7 =j*8，计算当前元素地址偏移
	dadd r7, r7, r5    #s7为当前数组指针指向的地址
	ld r11, 0(r7)    #加载第一个元素
	ld r12, 8(r7)    #加载第二个元素
	slt r13, r12, r11    #如果r12<r11,则r13=1
	beq r13, $zero, not_swap
	dadd r6, $zero, r10    #保存下标
	jal swap
not_swap:
	daddi r10, r10, 1    #j++

in_judge:
	dadd r3, r9, r10    #i+j
	bne r3, r8, inner_loop    #i+j!=length，即j<len-i，继续内层循环
	daddi r9, r9, 1    #i++
out_judge:
	bne r8, r9, outer_loop    #i!=l数组长度，继续外层循环
	
end:
	ld $ra, 0($sp)    #恢复返回地址
	ld r7, 8($sp)     #恢复寄存器
	ld r8, 16($sp) 
	ld r9, 24($sp) 
	ld r10, 32($sp) 
	ld r11, 40($sp) 
	ld r12, 48($sp)
	ld r13, 56($sp)
	daddi $sp, $sp, 64    #释放栈空间
	
	daddi r3, $zero, 0
print_after:
	ld r4, array(r3)    #数组首地址
	sd r4, 0(r2)    #数组存到数据地址
	daddi r5, $zero, 2    #设置为输出有符号整型
	sd r5, 0(r1)    #输出
	daddi r3, r3, 8    #移动到下一个元素
	daddi r4, r3, -80    #判断元素是否输出完成
	beqz  r4, Halt    #输出完成则退出循环
	j print_after

Halt:
    halt
 
 
# jal Index: 跳转到子程序的地址并且把当前位置所在的下一条
#             指令位置存入 ra 寄存器中，以用于子程序执行完后的返回 
