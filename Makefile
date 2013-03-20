
########################################
## huyanwei add {
########################################
#ifeq ($(SUNMONTECH_DEVICE_COLLECT),yes)
obj-y := device_collector.o
#else
#obj-n := device_collector.o
#endif
#ifeq ($(MTK_PLATFORM),MT6575)
#ccflags-y += -I$(MTK_PATH_PLATFORM)/kernel/drivers/mmc-host/
#endif
########################################
## huyanwei add }
########################################

