install:
	echo ""; echo "util";    cd util/src;    make install
	echo ""; echo "connect"; cd connect/src; make install
	echo ""; echo "ant";     cd ant/src;     make install
	echo ""; echo "ras";     cd ras/src;     make install
	echo ""

clean:
	echo ""; echo "util";    cd util/src;    make clean
	echo ""; echo "connect"; cd connect/src; make clean
	echo ""; echo "ant";     cd ant/src;     make clean
	echo ""; echo "ras";     cd ras/src;     make clean
	echo ""

clean_all:
	echo ""; echo "util";    cd util/src;    make clean_all
	echo ""; echo "connect"; cd connect/src; make clean_all
	echo ""; echo "ant";     cd ant/src;     make clean_all
	echo ""; echo "ras";     cd ras/src;     make clean_all
	echo ""

rebuild:
	make clean
	make install

rebuild_all:
	make clean_all
	make install

all:
	make clean
	make install

