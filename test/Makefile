CPPUNIT_CONTRIB=../../cppunit_contrib
TIME_LIB=../../time
COORDINATE_LIB = ../../coordinates
CPPXML_LIB = ../../cppxml
OBJS = run_tests.o test_env_manager.o test_env_memento.o test_env_var.o flysight_test.o sp3_reader_test.o ge_object_test.o gpx_factory_test.o
EXE = run_tests
CFLAGS += -g --std=c++11 -I.. -I${CPPXML_LIB} -I${CPPUNIT_CONTRIB} -I${TIME_LIB} -I${COORDINATE_LIB}
LFLAGS = -lcppunit -lexpat

.cpp.o:
	g++ -c ${CFLAGS} $<

all: ${OBJS}
	g++ -o ${EXE} ${OBJS} ${LFLAGS}

clean:
	rm -f *~ ${EXE} ${OBJS} core
