CXX = g++
INCLUDE_PATH = ~/or-tools_x86_64_Ubuntu-22.04_cpp_v9.11.4210/include/
LIB_PATH = ~/or-tools_x86_64_Ubuntu-22.04_cpp_v9.11.4210/lib/
LIBS = -lortools

#targets
TARGETS = ORexamples ArrayExamples OpticalSolution

#sources
SRCS = ORexamples.cc ArrayExamples.cc OpticalSolution.cc

all: $(TARGETS)

%: %.cc
	$(CXX) -I $(INCLUDE_PATH) $< -L $(LIB_PATH) $(LIBS) -o $@

clean:
	rm -f $(TARGETS)