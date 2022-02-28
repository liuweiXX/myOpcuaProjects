#ifndef MODBUS_OPERATE_H
#define MODBUS_OPERATE_H


#include <stdio.h>
#include<errno.h>
#include "modbus.h"



#define COIL				1
#define DISCRETE_INPUT		2
#define HOLDING_REGISTER	3
#define INPUT_REGISTER		4


//-------------modbus的部分---------------------

/*
	opcua的节点对应modbus的寄存器
	register种类

	对应读写

	对应单个或多个寄存器
*/
uint8_t *read_coils_value(int register_type, int add, int num);

 // 读取保持寄存器和输入寄存器的值
uint16_t *read_registers_value(int register_type, int add, int num);

 // 写入线圈和保持寄存器，返回是否写入的状态
int write_registers_value(int register_type, int add, int num, uint16_t *data1, uint8_t *data2);



#endif


