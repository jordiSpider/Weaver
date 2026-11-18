
#pragma once

#include <functional>
#include <mutex>
#include <string>

namespace LogManager {
    // Definición de códigos ANSI estándar
    inline const std::string RESET = "\033[0m";
    inline const std::string RED = "\033[31m"; // Errores
    inline const std::string GREEN = "\033[32m"; // Simulación (Información)
    inline const std::string CYAN = "\033[36m"; // IO / Disco
    inline const std::string YELLOW = "\033[33m"; // Advertencias


    enum class Subsystem : unsigned char {
        Simulation,
        IO
    };

    struct LogMessage {
        std::string message;
        bool is_error = false;
        bool ignore_silent = false;
        Subsystem subsystem = Subsystem::Simulation;
        bool is_warning = false;
    };

    using LogHandler = std::function<void(const LogMessage&)>;

    inline LogHandler handler;

    inline void setHandler(LogHandler newHandler) {
        handler = std::move(newHandler);
    }

    // Métodos globales que usará el Modelo (No conocen a la Vista)
    inline void emit(std::string msg, bool ignore_silent = false, Subsystem sub = Subsystem::Simulation) {
        handler(LogMessage{ std::move(msg), false, ignore_silent, sub });
    }

    inline void emitError(std::string msg, bool ignore_silent = false, Subsystem sub = Subsystem::Simulation) {
        handler(LogMessage{ std::move(msg), true, ignore_silent, sub });
    }

    inline void emitWarning(std::string msg, bool ignore_silent = false, Subsystem sub = Subsystem::Simulation) {
        handler(LogMessage{ std::move(msg), false, ignore_silent, sub, true });
    }
}
