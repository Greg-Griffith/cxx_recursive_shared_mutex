LINK:=$(CXX)

DEFS=-DBOOST_SPIRIT_THREADSAFE

DEFS += $(addprefix -I,$(CURDIR) $(CURDIR)/rsm $(CURDIR)/rsm/experimental $(CURDIR)/rsm/test $(CURDIR)/rsm/test/experimental $(BOOST_INCLUDE_PATH))
LIBS = $(addprefix -L,$(BOOST_LIB_PATH))

LIBS += \
 -Wl,-Bdynamic \
   -l boost_system$(BOOST_LIB_SUFFIX) \
   -l boost_filesystem$(BOOST_LIB_SUFFIX) \
   -l boost_program_options$(BOOST_LIB_SUFFIX) \
   -l boost_thread$(BOOST_LIB_SUFFIX) \
   -l boost_unit_test_framework$(BOOST_LIB_SUFFIX) \
   -l pthread

xCXXFLAGS=-Werror -std=c++14 $(DEFS) $(CXXFLAGS)

BUILD= \
    build/rsm/recursive_shared_mutex.o \
    build/rsm/experimental/exp_recursive_shared_mutex.o \
    build/rsm/test/rsm_promotion_tests.o \
    build/rsm/test/rsm_simple_tests.o \
    build/rsm/test/rsm_starvation_tests.o \
	build/rsm/test/experimental/exp_rsm_promotion_tests.o \
    build/rsm/test/experimental/exp_rsm_simple_tests.o \
    build/rsm/test/experimental/exp_rsm_starvation_tests.o \
    build/rsm/test/test_cxx_rsm.o \
    build/rsm/test/timer.o

all: directories test_cxx_rsm

.PHONY: directories

directories:
	if [ ! -d "$(CURDIR)/build" ]; then \
		mkdir $(CURDIR)/build; \
	fi; \
	if [ ! -d "$(CURDIR)/build/rsm" ]; then \
		mkdir $(CURDIR)/build/rsm; \
	fi; \
	if [ ! -d "$(CURDIR)/build/rsm/experimental" ]; then \
		mkdir $(CURDIR)/build/rsm/experimental; \
	fi; \
	if [ ! -d "$(CURDIR)/build/rsm/test" ]; then \
		mkdir $(CURDIR)/build/rsm/test; \
	fi;
	if [ ! -d "$(CURDIR)/build/rsm/test/experimental" ]; then \
		mkdir $(CURDIR)/build/rsm/test/experimental; \
	fi;

build/%.o: %.cpp
	$(CXX) -c $(xCXXFLAGS) -MMD -MF $(@:%.o=%.d) -o $@ $<
	@cp $(@:%.o=%.d) $(@:%.o=%.P); \
	  sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	      -e '/^$$/ d' -e 's/$$/ :/' < $(@:%.o=%.d) >> $(@:%.o=%.P); \
	  rm -f $(@:%.o=%.d)

test_cxx_rsm: $(BUILD:build/%=build/%)
	$(CXX) $(xCXXFLAGS) -o $@ $^ $(LIBS)

clean:
	-rm -f test_cxx_rsm
	-rm -f build/rsm/*.o
	-rm -f build/rsm/*.P
	-rm -f build/rsm/experimental/*.o
	-rm -f build/rsm/experimental/*.P
	-rm -f build/rsm/test/*.o
	-rm -f build/rsm/test/*.P
	-rm -rf build

FORCE:
