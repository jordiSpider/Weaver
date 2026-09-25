
#include "App/IO/StorageBridge.h"

#include <fstream>
#include <unordered_map>
#include <map>
#include <oneapi/tbb/parallel_for.h>
#include <oneapi/tbb/parallel_for_each.h>

#include <fmt/format.h>

#include "App/Manager/LogManager.h"
#include "Misc/EnumClass.h"


// Macro portable y segura para escrituras sin bloqueos internos
#if defined(_MSC_VER) || defined(__MINGW32__)
// Entornos Windows (MSVC o MinGW)
#define fwrite_fast _fwrite_nolock
#elif defined(__linux__) || defined(__gnu_linux__)
// Entornos Linux que implementan la extensión POSIX
#define fwrite_fast fwrite_unlocked
#else
// macOS, BSD y otros sistemas sin funciones "unlocked" nativas utilizan el estándar
#define fwrite_fast std::fwrite
#endif


namespace StorageBridge {

    void writeHeaderPackToDisk(const std::filesystem::path& filePath, const std::string& header) {
        fmt::memory_buffer formattedTextBlock;
        writePackToDisk(filePath, header, false, formattedTextBlock);
    }

    void writeContentPackToDisk(const std::filesystem::path& filePath, fmt::memory_buffer& formattedTextBlock) {
        writePackToDisk(filePath, "", false, formattedTextBlock);
    }

    void writeClosePackToDisk(const std::filesystem::path& filePath) {
        fmt::memory_buffer formattedTextBlock;
        writePackToDisk(filePath, "", true, formattedTextBlock);
    }

    void writeFullFilePackToDisk(const std::filesystem::path& filePath, const std::string& header, fmt::memory_buffer& formattedTextBlock) {
		writePackToDisk(filePath, header, true, formattedTextBlock);
    }

    void writePackToDisk(
                const std::filesystem::path& filePath, const std::string& header,
                bool closeAfterWrite, fmt::memory_buffer& formattedTextBlock
            ) {

        if (diskOutputMockEnabled) {
            formattedTextBlock.clear();
            return;
        }

        std::string pathStr = filePath.string();
        std::FILE* f_stream = nullptr;

        // FASE 1: Búsqueda o inserción concurrente (Segura gracias a shared_ptr)
        auto it = openFiles.find(pathStr);
        if (it != openFiles.end()) {
            f_stream = it->second;
        }
        else {
            // Apertura directa y eficiente de C-style files
#if defined(_MSC_VER)
            errno_t err = fopen_s(&f_stream, pathStr.c_str(), "ab");
            if (err != 0 || f_stream == nullptr) {
#else
            f_stream = std::fopen(pathStr.c_str(), "ab");
            if (f_stream == nullptr) {
#endif

                LogManager::emitError(fmt::format("Error opening file: {}\n", pathStr), true, LogManager::Subsystem::IO);
                return;
            }

            // OPTIMIZACIÓN: Asignar un búfer de flujo grande (64 KB) para minimizar llamadas al sistema de archivos
            std::setvbuf(f_stream, nullptr, _IOFBF, 65536);

            openFiles[pathStr] = f_stream;
            LogManager::emit(fmt::format("Opened file for writing: {}\n", pathStr), false, LogManager::Subsystem::IO);

            if (!header.empty()) {
                fwrite_fast(header.data(), sizeof(char), header.size(), f_stream);
                std::fputc('\n', f_stream);
            }
        }

        const size_t blockSize = formattedTextBlock.size();
        if (blockSize > 0 && f_stream != nullptr) {
            fwrite_fast(formattedTextBlock.data(), sizeof(char), blockSize, f_stream);

            LogManager::emit(fmt::format("Flushed block to {} (BS: {})\n", pathStr, blockSize), false, LogManager::Subsystem::IO);

            formattedTextBlock.clear();
        }

        // FASE 3: Cierre y borrado del mapa
        if (closeAfterWrite) {
            if (f_stream) {
                std::fclose(f_stream);
                openFiles.erase(pathStr);
                LogManager::emit(fmt::format("Closed file descriptor (requested): {}\n", pathStr), false, LogManager::Subsystem::IO);
            }
        }
    }
} // namespace StorageBridge
