SHELL=/bin/bash

CC = /opt/OpenCilk-10.0.1-Linux/bin/clang-10
CXX = /opt/OpenCilk-10.0.1-Linux/bin/clang++-10
MSIZE ?= 128

all : sysinfo pyton java c_class c_optimized
	@echo ""
	@echo "########### TEST DONE ############"

clean :
	rm -rf matrix.class matrix_O* matrix*raw 
	rm -rf matrix_cilk matrix_cilk_bs* matrix_cilk_rs* matrix_cilk_vec*
	rm -rf matrix_cilk_avx512_bs*
	rm -rf matrix_mkl/build

sysinfo :
	@if command -v neofetch >/dev/null 2>&1;	then \
		neofetch --off --color_blocks off; \
	else \
		echo "Please install neofetch for detailed info"; \
	fi

python : matrix.py
	@if command -v python3 >/dev/null 2>&1; then \
		echo "########### PYTHON TEST ############"; \
		echo ""; \
	else \
		echo 'Please install python3 for test'; \
		exit 1; \
	fi
	python3 matrix.py ${MSIZE}

java : matrix.java
	@if command -v java >/dev/null 2>&1; then \
		echo "############ JAVA TEST #############"; \
		echo ""; \
	else \
		echo 'Please install JAVA for test'; \
		exit 1; \
	fi
	@cp matrix.java temp_matrix.java
	@sed -i "s/1024/${MSIZE}/g" matrix.java
	@javac matrix.java
	@mv temp_matrix.java matrix.java
	java matrix

c_class : c_raw c_loop_order c_optimizer

c_raw : matrix.c
	@if command -v $(CC) >/dev/null 2>&1; then \
		echo "############ RAW C TEST #############"; \
		echo ""; \
	else \
		echo 'Please install JAVA for test'; \
		exit 1; \
	fi
	@ $(CC) matrix.c -D n=${MSIZE} -o matrix_c_raw 
	@ ./matrix_c_raw

c_loop_order : matrix.c
	@if command -v $(CC) >/dev/null 2>&1; then \
		echo "############ C LOOP ORDER TEST #############"; \
		echo ""; \
	else \
		echo 'Please install JAVA for test'; \
		exit 1; \
	fi
	@for order in "ijk" "ikj" "jik" "jki" "kij" "kji"; \
	do \
		$(CC) matrix.c -D n=${MSIZE} \
			-D _LOOP_1_=$${order:0:1} -D _LOOP_2_=$${order:1:1} -D _LOOP_3_=$${order:2:1} \
			-o matrix_$${order}_raw; \
		ntime=$$(./matrix_$${order}_raw); \
		echo "$${order}: $${ntime}"; \
	done
	
c_optimizer : matrix.c
	@if command -v $(CC) >/dev/null 2>&1; then \
		echo "############ C OPTIMIZER TEST #############"; \
		echo ""; \
	else \
		echo 'Please install JAVA for test'; \
		exit 1; \
	fi
	@for optlevel in "0" "1" "2" "3" "fast"; \
	do \
		$(CC) matrix.c -D n=${MSIZE} \
			-D _LOOP_1_=i -D _LOOP_2_=k -D _LOOP_3_=j \
			-O$${optlevel} \
			-o ./matrix_O$${optlevel}; \
		ntime=$$(./matrix_O$${optlevel}); \
		echo "O$${optlevel}: $${ntime}"; \
	done

c_optimized: cilk_class cilk_super

cilk_class: cilk cilk_block cilk_recu cilk_vec

cilk : matrix_cilk.c
	@ $(CC) matrix_cilk.c -O2 -fopencilk -D n=${MSIZE} -o matrix_cilk; \
	if [ $$? -eq 0 ]; then \
		echo "############ C CILK TEST #############"; \
		echo ""; \
	else \
		echo 'Please USE OPENCILK for your TEST'; \
		exit 1; \
	fi
	@./matrix_cilk

cilk_block: matrix_cilk_block.c
	@ $(CC) matrix_cilk.c -O2 -D n=${MSIZE} -fopencilk -o matrix_cilk; \
	if [ $$? -eq 0 ]; then \
		echo "############ C CILK with BLOCK TEST #############"; \
		echo ""; \
	else \
		echo 'Please USE OPENCILK for your TEST'; \
		exit 1; \
	fi

	@for bsize in "4" "8" "16" "32" "64" "128" "256" "512" "1024" "2048"; \
	do \
		nbin="matrix_cilk_bs$${bsize}"; \
		$(CC) matrix_cilk_block.c -O2 -D ni=${MSIZE} -D THRESHOLD=$${bsize} -fopencilk -o ./$${nbin}; \
		ntimes=($$(./$${nbin}) $$(./$${nbin}) $$(./$${nbin})); \
		ntime=$$(echo $${ntimes} | awk 'BEGIN{min = 65535} {if ($$1 < min) min = $$1;fi} END{printf "Min = %.6f\n",min}'); \
		echo "block$${bsize}: $${ntime}"; \
	done

cilk_recu: matrix_cilk_recu.c
	@ $(CC) matrix_cilk.c -O2 -D ni=${MSIZE} -fopencilk -o matrix_cilk; \
	if [ $$? -eq 0 ]; then \
		echo "######### C CILK with RECURSION TEST ##########"; \
		echo ""; \
	else \
		echo 'Please USE OPENCILK for your TEST'; \
		exit 1; \
	fi

	@for bsize in "4" "8" "16" "32" "64" "128" "256" "512" "1024" "2048"; \
	do \
		nbin="matrix_cilk_rs$${bsize}"; \
		$(CC) matrix_cilk_recu.c -O2 -D ni=${MSIZE} -D THRESHOLD=$${bsize} -fopencilk -o ./$${nbin}; \
		ntimes=($$(./$${nbin}) $$(./$${nbin}) $$(./$${nbin})); \
		ntime=$$(echo $${ntimes} | awk 'BEGIN{min = 65535} {if ($$1 < min) min = $$1;fi} END{printf "Min = %.6f\n",min}'); \
		echo "block$${bsize}: $${ntime}"; \
	done

cilk_vec: matrix_cilk_recu.c
	@ $(CC) matrix_cilk.c -O2 -D ni=${MSIZE} -fopencilk -o matrix_cilk; \
	if [ $$? -eq 0 ]; then \
		echo "######### C CILK with march vec TEST ##########"; \
		echo ""; \
	else \
		echo 'Please USE OPENCILK for your TEST'; \
		exit 1; \
	fi

	@for bsize in "32" "512"; \
	do \
		nbin="matrix_cilk_vec$${bsize}"; \
		$(CC) matrix_cilk_recu.c -O2 -D ni=${MSIZE} -D THRESHOLD=$${bsize} \
			-march=native  -ffast-math -fopencilk -o ./$${nbin}; \
		ntimes=($$(./$${nbin}) $$(./$${nbin}) $$(./$${nbin})); \
		ntime=$$(echo $${ntimes} | awk 'BEGIN{min = 65535} {if ($$1 < min) min = $$1;fi} END{printf "Min = %.6f\n",min}'); \
		echo "block$${bsize}: $${ntime}"; \
	done


cilk_super: warn_super cilk_avx cilk_mkl

warn_super:
	@avxflag=$$(cat /proc/cpuinfo | grep flags | head -n 1 | grep avx512f | grep avx512cd ); \
	if [ -z "$${avxflag}" ]; then \
		echo "########## INSTRUCTION NOT SUPPORT ##########"; \
		echo "  !!! AVX512 and Intel MKL is required !!!"; \
		echo "";\
		echo " Only for Intel CPU Which support AVX512 and Intel MKL,"; \
		echo " Please check your environment"; \
		echo "";\
		exit 1; \
	fi
	@echo "############ C CILK with SUPER TEST #############";
	@echo "";

cilk_avx: matrix_cilk_avx.c
	@ $(CC) matrix_cilk.c -O2 -D ni=${MSIZE} -fopencilk -o matrix_cilk; \
	if [ $$? -eq 0 ]; then \
		echo "######### C CILK with AVX512 TEST ##########"; \
		echo ""; \
	else \
		echo 'Please USE OPENCILK for your TEST'; \
		exit 1; \
	fi

	@for bsize in "32" "64" "128" "256" "512" "1024" "2048"; \
	do \
		nbin="matrix_cilk_avx512_bs$${bsize}"; \
		$(CC) matrix_cilk_avx.c -O2 -D ni=${MSIZE} -D THRESHOLD=$${bsize} \
			-march=native  -ffast-math -mavx512f -mavx512cd -fopencilk -o ./$${nbin}; \
		ntimes=($$(./$${nbin}) $$(./$${nbin}) $$(./$${nbin})); \
		ntime=$$(echo $${ntimes} | awk 'BEGIN{min = 65535} {if ($$1 < min) min = $$1;fi} END{printf "Min = %.6f\n",min}'); \
		echo "block$${bsize}: $${ntime}"; \
	done
	
cilk_mkl:
	@echo "######### C with Intel MKL TEST ##########"
	@cmake matrix_mkl -DNI=${MSIZE} -B matrix_mkl/build
	@cmake --build matrix_mkl/build
	@nbin="matrix_mkl/build/matrix_mkl"; \
	source /opt/intel/oneapi/setvars.sh; \
	ntimes=($$($${nbin}) $$($${nbin}) $$($${nbin})); \
	ntime=$$(echo $${ntimes} | awk 'BEGIN{min = 65535} {if ($$1 < min) min = $$1;fi} END{printf "Min = %.6f\n",min}'); \
	echo "mkl acc: $${ntime}";




		

	

