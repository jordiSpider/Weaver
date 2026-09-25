
#pragma once

#include <oneapi/tbb/global_control.h>
#include <oneapi/tbb/task_arena.h>
#include <oneapi/tbb/task_group.h>
#include <oneapi/tbb/task_scheduler_observer.h>
#include <oneapi/tbb/parallel_for.h>
#include <string>
#include <atomic>
#include <thread>
#include <algorithm>

#if defined(_WIN32)
#include <windows.h>
#elif defined(__linux__)
#include <sys/prctl.h>
#endif

namespace TbbManager {

    class NamedObserver : public tbb::task_scheduler_observer {
    private:
        std::string prefijo;

    public:
        NamedObserver(tbb::task_arena& arena, std::string p)
            : tbb::task_scheduler_observer(arena), prefijo(p)
        {
            observe(true);
        }

        NamedObserver(const NamedObserver&) = delete;
        NamedObserver& operator=(const NamedObserver&) = delete;
        NamedObserver(NamedObserver&&) = delete;
        NamedObserver& operator=(NamedObserver&&) = delete;

        ~NamedObserver() {
            observe(false);
        }

        void on_scheduler_entry(bool is_worker) override {
            int id = tbb::this_task_arena::current_thread_index() + 1;
            std::string name;

            // Corrección de nombres: Si el índice es 1 y NO es un hilo worker puro de TBB,
            // significa que es nuestro hilo principal ayudando en la arena.
            if (id == 1 && !is_worker) {
                name = "Main_IO_Worker";
            }
            else {
                name = prefijo + "_" + std::to_string(id);
            }

#if defined(_WIN32)
            std::wstring wname(name.begin(), name.end());
            SetThreadDescription(GetCurrentThread(), wname.c_str());
#elif defined(__linux__)
            if (name.length() > 15) name = name.substr(0, 15);
            prctl(PR_SET_NAME, name.c_str(), 0, 0, 0);
#endif
        }
    };

    inline const unsigned int hardware_cores = std::max(1u, std::thread::hardware_concurrency());
    inline std::unique_ptr<tbb::global_control> global_pool = nullptr;

    inline unsigned int simulation_threads = 1u;
    inline std::unique_ptr<tbb::task_arena> simulation_arena = nullptr;
    inline std::unique_ptr<NamedObserver> sim_namer = nullptr;
    inline std::atomic<bool> is_simulation_running{ true };

    // FUNCIÓN DE CONFIGURACIÓN DINÁMICA ACTUALIZADA
    inline void configure_pool(unsigned int user_requested_threads, bool reserveOneThreadForUI = false) {
        unsigned int max_allowed_threads = hardware_cores;
        if (user_requested_threads > 0) {
            max_allowed_threads = std::min(hardware_cores, user_requested_threads);
        }

        if (reserveOneThreadForUI && max_allowed_threads < 2) {
            throwLineInfoException("At least 2 threads are required when reserving one thread for UI.");
        }

        // Establecemos el control global con el número total de hilos lógicos (8 en tu máquina)
        global_pool = std::make_unique<tbb::global_control>(
            tbb::global_control::max_allowed_parallelism,
            max_allowed_threads
        );

        simulation_threads = reserveOneThreadForUI ? (max_allowed_threads - 1u) : max_allowed_threads;

        simulation_arena = std::make_unique<tbb::task_arena>(simulation_threads);
        sim_namer = std::make_unique<NamedObserver>(*simulation_arena, "Sim_Worker");
    }

    inline void initialize_and_name_pool() {
        if (simulation_arena) {
            simulation_arena->execute([]() {
                tbb::parallel_for(0, 100, [](int) {});
                });
        }
    }
}
