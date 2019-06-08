使用STM32F030K6（32KB ROM，4KB RAM，8M内部RC配合PLL主频48）作为平台移植SL_RTE，裁剪情况如下：

1、没有开启RTOS支持；

2、使用LVGL模式动态内存管理；

3、使用BASE模式时间片轮询；

4、没有开启GUI支持；

5、提供了串口、定时器、GPIO、FLASH的标准驱动；

6、KVDB、Shell等模块全部开启。
