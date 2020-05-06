# 046267 Computer Architecture - Spring 2020 - HW #2

COMPILER := g++
COMPILER_FLAGS := --std=c++11 -Wall
SRCS := cache.cpp cacheSim.cpp
OBJS=$(subst .cpp,.o,$(SRCS))
HDRS := 
CACHE_BIN := cacheSim


$(CACHE_BIN): $(OBJS)
	$(COMPILER) $(COMPILER_FLAGS) $^ -o $@

$(OBJS): %.o: %.cpp
	$(COMPILER) $(COMPILER_FLAGS) -c $^

clean:
	rm -rf $(CACHE_BIN) $(OBJS) 
