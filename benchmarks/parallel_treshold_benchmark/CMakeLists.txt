
add_executable(weaver_parallel_treshold_benchmark
	ParallelTresholdBenchmark.cpp
)

target_link_libraries(weaver_parallel_treshold_benchmark
	PRIVATE
		weaver_options
		weaver_app_view
		weaver_core_misc
		benchmark::benchmark
)

target_compile_definitions(weaver_parallel_treshold_benchmark
    PRIVATE
        BENCHMARK_CONFIG_DIR="${CMAKE_CURRENT_SOURCE_DIR}/config"
)
