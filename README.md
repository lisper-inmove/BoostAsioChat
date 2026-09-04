# gen_test_makefile

  ./gen_test_makefile 自动生成googletest的单元测试

# make debug

	mkdir -p build && cd build && cmake -DCMAKE_BUILD_TYPE=Debug .. && make && cp compile_commands.json ..

# make release

	mkdir -p build && cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && make

# make test

	mkdir -p build && cd build && cmake -DCMAKE_BUILD_TYPE=Test .. && make

# make generate

  generate: test
  	./gen_test_makefile.sh
