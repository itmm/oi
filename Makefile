.PHONY: tests clean lines

APP = oi
SOURCEs = $(wildcard *.cpp)
OBJECTs = $(addprefix build/,$(SOURCEs:.cpp=.o))

CXXFLAGS += -g -Wall -std=c++17

tests: $(APP)
	@echo "run tests"
	@./oi Tests.Run

include $(wildcard deps/*.dep)

build/%.o: %.cpp
	@echo "c++ $@"
	@mkdir -p build deps
	@$(CXX) $(CXXFLAGS) -c $(notdir $(@:.o=.cpp)) -o $@ -MMD -MF deps/$(notdir $(@:.o=.dep))

$(APP): $(OBJECTs)
	@echo "link $@"
	@$(CXX) $^ -o $@

clean:
	@echo "clean"
	@rm -Rf $(APP) build deps

lines:
	@cat *.cpp *.h | wc -l
