MODULE_1=util
MODULE_2=connect
MODULE_3=ant
MODULE_4=ras

SUBDIR_1=$(MODULE_1)/src
SUBDIR_2=$(MODULE_2)/src
SUBDIR_3=$(MODULE_3)/src
SUBDIR_4=$(MODULE_4)/src

TARGET_1=$(MODULE_1)_
TARGET_2=$(MODULE_2)_
TARGET_3=$(MODULE_3)_
TARGET_4=$(MODULE_4)_


all:
	cd $(SUBDIR_1); make all
	cd $(SUBDIR_2); make all
	cd $(SUBDIR_3); make all
	cd $(SUBDIR_4); make all


clean:
	cd $(SUBDIR_1); make clean
	cd $(SUBDIR_2); make clean
	cd $(SUBDIR_3); make clean
	cd $(SUBDIR_4); make clean


rebuild:
	make clean
	make all


rebuild_clean:
	make rebuild
	make clean


$(TARGET_1):
	cd $(SUBDIR_1); make all


$(TARGET_2):
	cd $(SUBDIR_2); make all


$(TARGET_3):
	cd $(SUBDIR_3); make all


$(TARGET_4):
	cd $(SUBDIR_4); make all

