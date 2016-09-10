ifeq ($(CONFIG),)
	CONFIG = release_x64
endif

ifeq ($(VERBOSE),)
	VERBOSE=1
endif


all : bin/nativejson_release_x64_gmake
	cd bin && ./nativejson_release_x64_gmake $(ARGS)
	cd result && make -f makefile

bin/nativejson_%_gmake : build/gmake/nativejson.make bin/nativejson_%_gmake.a
	cd build/gmake && make -f jsonstat.make config=$(CONFIG) verbose=$(VERBOSE)
	cd build/gmake && make -f nativejson.make config=$(CONFIG) verbose=$(VERBOSE)

clean : 
	rm -rf build/gmake
	rm -rf build/vs2005
	rm -rf build/vs2008
	rm -rf build/vs2010
	rm -rf intermediate
	rm -rf src/machine.h
	rm -rf bin
	cd result && make -f makefile clean

setup :
	cd build && ./premake.sh && ./machine.sh
	
	
build/gmake/nativejson.make : setup
build/gmake/benchmark.make : setup
build/gmake/jsonstat.make : setup
	
	
bin/nativejson_release_x64_gmake.a : build/gmake/benchmark.make
	cd build/gmake && make -f benchmark.make config=$(CONFIG) verbose=$(VERBOSE)
	
	
	
clean_status :
	@echo "Filesystem status according to GIT"
	@git clean -dfxn
