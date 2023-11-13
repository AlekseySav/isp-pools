pools: main.cpp pool.cpp
	$(CXX) $(CXXFLAG) -O3 $^ -o $@

run: pools
	./$<
