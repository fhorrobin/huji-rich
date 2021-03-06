CC := g++
DUMP_FOLDER := temp_$(TARGET)
LINT_FLAGS := -Wall -Wextra -pedantic -Werror -Wfatal-errors
NUM_PROCESSES := 2
PREXECUTE := 
ENDEMIC_SOURCES := $(shell find $(TARGET) -name '*.cpp')
ENDEMIC_OBJECTS := $(patsubst $(TARGET)/%.cpp,$(DUMP_FOLDER)/%.o,$(ENDEMIC_SOURCES))
ifeq ($(MODE),debug)
	OPTIMIZATION_FLAGS := -O0 -g -pg -std=c++0x
	LFLAGS := -pg -lclipper
else ifeq ($(MODE),intel)
	CC := icpc
	OPTIMIZATION_FLAGS := -O3 -ipo -xHost -fp-model precise -std=c++0x
	LINT_FLAGS := 
	LFLAGS := -lclipper
	ARCHIVER_FUNC := xiar
else ifeq ($(MODE),parallel_intel)
	PREXECUTE := mpiexec -np $(NUM_PROCESSES)
	CC := mpiicpc
	OPTIMIZATION_FLAGS := -O3 -ipo -xHost -fp-model precise -DRICH_MPI -std=c++0x
	LINT_FLAGS := 
	LFLAGS := -lclipper
	ARCHIVER_FUNC := xiar
else ifeq ($(MODE),parallel)
	PREXECUTE := mpirun -np $(NUM_PROCESSES)
	CC := mpiCC
	OPTIMIZATION_FLAGS := -DRICH_MPI -O3 -std=c++0x
	LINT_FLAGS = -Werror -Wall -Wextra -pedantic -Wfatal-errors -Wmissing-declarations -Wno-long-long
	LFLAGS := -lclipper
else ifeq ($(MODE),parallel_profile)
	PREXECUTE := mpirun -np $(NUM_PROCESSES)
	CC := mpiCC
	OPTIMIZATION_FLAGS := -DRICH_MPI -g -pg -std=c++0x
	LINT_FLAGS = -Werror -Wall -Wextra -pedantic -Wfatal-errors -Wmissing-declarations -Wno-long-long
	LFLAGS := -lclipper
else ifeq ($(MODE),debug_parallel)
	PREXECUTE := mpirun -np $(NUM_PROCESSES)
	CC := mpiCC
	OPTIMIZATION_FLAGS := -DRICH_MPI -O0 -g -pg -frecord-gcc-switches -std=c++0x
	LINT_FLAGS := -Wall -Wextra -pedantic -Wfatal-errors -Weffc++ -Wmissing-declarations
	LFLAGS := -pg -lclipper
else
	MODE := production
	OPTIMIZATION_FLAGS := -O3 -std=c++0x
	LFLAGS := -lclipper
endif
CFLAGS := $(LINT_FLAGS) $(OPTIMIZATION_FLAGS)

$(DUMP_FOLDER)/normal_termination.res: $(DUMP_FOLDER)/test.exe
	cp $(TARGET)/*.txt $(DUMP_FOLDER) 2> /dev/null; return 0
	cd $(DUMP_FOLDER) && $(PREXECUTE) ./test.exe

$(DUMP_FOLDER)/test.exe: $(ENDEMIC_OBJECTS) | ../library_$(MODE)/librich.a
	echo "$(ENDEMIC_SOURCES)"
	$(CC) $^ $| -o $@ $(LFLAGS) -lhdf5 -lhdf5_cpp -L $(HDF5_LIB_PATH)

../library_$(MODE)/librich.a:
	$(MAKE) -C .. set_environ_vars.sh
	`cat ../set_environ_vars.sh` && \
	$(MAKE) -C .. MODE=$(MODE)

$(ENDEMIC_OBJECTS): $(DUMP_FOLDER)/%.o: $(TARGET)/%.cpp
	mkdir -p $(DUMP_FOLDER)
	$(MAKE) -C .. set_environ_vars.sh
	`cat ../set_environ_vars.sh` && \
	$(CC) $(CFLAGS) -c $< -o $@ -I ..
