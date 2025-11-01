#include <jni.h>
#include <android/log.h>
#include <fstream>
#include <string>
#include <ctime>
#include <sstream>
#include <mutex>
#include <sys/stat.h>

#define LOG_TAG "InjectorLogger"
#define LOG_FILE_PATH "/sdcard/Android/data/com.sandboxol.blockymods.official/files/Download/SandboxOL/BlockMan/config/InjectorLogger.txt"

// Mutex for thread-safe file writing
static std::mutex logMutex;

// Get current timestamp
std::string getCurrentTime() {
    time_t now = time(nullptr);
    char buf[80];
    struct tm* tm_info = localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm_info);
    return std::string(buf);
}

// Internal logging function
void writeLog(const char* level, const char* message) {
    std::lock_guard<std::mutex> lock(logMutex);
    
    std::ofstream logFile;
    logFile.open(LOG_FILE_PATH, std::ios::app);
    
    if (logFile.is_open()) {
        std::string timestamp = getCurrentTime();
        logFile << "[" << timestamp << "] [" << level << "] " << message << std::endl;
        logFile.close();
        
        // Also log to Android logcat
        __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "[%s] %s", level, message);
    } else {
        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "Failed to open log file");
    }
}

// Public logging functions
extern "C" {

void InjectorLogger_Info(const char* message) {
    writeLog("INFO", message);
}

void InjectorLogger_Success(const char* message) {
    writeLog("SUCCESS", message);
}

void InjectorLogger_Error(const char* message) {
    writeLog("ERROR", message);
}

void InjectorLogger_Warning(const char* message) {
    writeLog("WARNING", message);
}

void InjectorLogger_Debug(const char* message) {
    writeLog("DEBUG", message);
}

// Clear the log file
void InjectorLogger_Clear() {
    std::lock_guard<std::mutex> lock(logMutex);
    std::ofstream logFile;
    logFile.open(LOG_FILE_PATH, std::ios::trunc);
    if (logFile.is_open()) {
        logFile.close();
        __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "Log file cleared");
    }
}

// Initialize logger (creates file if it doesn't exist)
void InjectorLogger_Init() {
    std::ofstream logFile;
    logFile.open(LOG_FILE_PATH, std::ios::app);
    if (logFile.is_open()) {
        logFile << "\n========== New Session: " << getCurrentTime() << " ==========\n" << std::endl;
        logFile.close();
        __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "Logger initialized");
    }
}

}