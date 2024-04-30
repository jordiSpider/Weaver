#ifndef IBM_H_
#define IBM_H_


#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstdlib>
#include <sys/time.h>
#include <ctime>
#include <chrono>
#include <ostream>
#include <fstream>
#include <boost/filesystem.hpp>
#include <nlohmann/json.hpp>

#ifdef USE_CPU_PROFILER
	#include "gperftools/profiler.h"
#endif

#ifdef USE_HEAP_PROFILER
	#include "gperftools/heap-profiler.h"
#endif

#include "Exceptions/LineInfoException.h"
#include "Misc/Utilities.h"
#include "Misc/GlobalVariable.h"
#include "IBM/Maths/Random.h"
#include "IBM/World/WorldFactory.h"
#include "IBM/World/WorldInterface.h"


class IBM
{
public:
    static void run(boost::filesystem::path inputConfigPath, boost::filesystem::path outputFolder, bool silentMode, int burnIn);
};

#endif // IBM_H_