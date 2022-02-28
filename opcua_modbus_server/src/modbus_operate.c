#include"modbus_operate.h"



//-------------modbus的部分---------------------

/*
	opcua的节点对应modbus的寄存器
	register种类

	对应读写

	对应单个或多个寄存器
*/
uint8_t *read_coils_value(int register_type, int add, int num)
{

	uint8_t *tab_reg; //定义存放数据的数组
	tab_reg = (uint8_t *)malloc(10);
	modbus_t *ctx = NULL;

	int rc;
	//以串口的方式创建libmobus实例,并设置参数
	ctx = modbus_new_rtu("com3", 9600, 'N', 8, 1);
	if (ctx == NULL)                //使用UART1,对应的设备描述符为com1
	{
		fprintf(stderr, "Unable to allocate libmodbus contex\n");
	}

	modbus_set_debug(ctx, 1);      //设置1可看到调试信息
	modbus_set_slave(ctx, 1);      //设置slave ID

	if (modbus_connect(ctx) == -1) //等待连接设备
	{
		fprintf(stderr, "Connection failed:%s\n", modbus_strerror(errno));
	}

	printf("\n----------------\n");

	switch (register_type)
	{
	case COIL:
		rc = modbus_read_bits(ctx, add, num, tab_reg);
		break;
	case DISCRETE_INPUT:
		rc = modbus_read_input_bits(ctx, add, num, tab_reg);
		break;
	default:
		break;
	}
	if (rc == -1)
	{
		fprintf(stderr, "%s\n", modbus_strerror(errno));
	}
	modbus_close(ctx);  //关闭modbus连接
	modbus_free(ctx);   //释放modbus资源，使用完libmodbus需要释放掉

	return tab_reg; //记得释放内存

}


 // 读取保持寄存器和输入寄存器的值
uint16_t *read_registers_value(int register_type, int add, int num)
{

	uint16_t *tab_reg; //定义存放数据的数组
	tab_reg = (uint16_t *)malloc(10);
	modbus_t *ctx = NULL;

	int rc;
	//以串口的方式创建libmobus实例,并设置参数
	ctx = modbus_new_rtu("com3", 9600, 'N', 8, 1);
	if (ctx == NULL)                //使用UART1,对应的设备描述符为com1
	{
		fprintf(stderr, "Unable to allocate libmodbus contex\n");
	}

	modbus_set_debug(ctx, 1);      //设置1可看到调试信息
	modbus_set_slave(ctx, 1);      //设置slave ID

	if (modbus_connect(ctx) == -1) //等待连接设备
	{
		fprintf(stderr, "Connection failed:%s\n", modbus_strerror(errno));
	}

	printf("\n----------------\n");

	switch (register_type)
	{
	case HOLDING_REGISTER:
		rc = modbus_read_registers(ctx, add, num, tab_reg);
		break;
	case INPUT_REGISTER:
		rc = modbus_read_input_registers(ctx, add, num, tab_reg);
		break;
	default:
		break;
	}
	if (rc == -1)
	{
		fprintf(stderr, "%s\n", modbus_strerror(errno));
	}
	modbus_close(ctx);  //关闭modbus连接
	modbus_free(ctx);   //释放modbus资源，使用完libmodbus需要释放掉

	return tab_reg; //记得释放内存

}


 // 写入线圈和保持寄存器，返回是否写入的状态
int write_registers_value(int register_type, int add, int num, uint16_t *data1, uint8_t *data2)
{

	modbus_t *ctx = NULL;

	int rc;
	//以串口的方式创建libmobus实例,并设置参数
	ctx = modbus_new_rtu("com3", 9600, 'N', 8, 1);
	if (ctx == NULL)                //使用UART1,对应的设备描述符为com1
	{
		fprintf(stderr, "Unable to allocate libmodbus contex\n");
	}

	modbus_set_debug(ctx, 1);      //设置1可看到调试信息
	modbus_set_slave(ctx, 1);      //设置slave ID

	if (modbus_connect(ctx) == -1) //等待连接设备
	{
		fprintf(stderr, "Connection failed:%s\n", modbus_strerror(errno));
	}

	printf("\n----------------\n");
	switch (register_type)
	{
	case COIL:
		rc = modbus_write_bits(ctx, add, num, data2);
		break;
	case HOLDING_REGISTER:
		rc = modbus_write_registers(ctx, add, num, data1);
		break;
	default:
		break;
	}
	if (rc == -1)
	{
		fprintf(stderr, "%s\n", modbus_strerror(errno));
	}
	modbus_close(ctx);  //关闭modbus连接
	modbus_free(ctx);   //释放modbus资源，使用完libmodbus需要释放掉

	return 0;
}
