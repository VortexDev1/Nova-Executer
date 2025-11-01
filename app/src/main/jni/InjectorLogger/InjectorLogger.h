#ifndef INJECTOR_LOGGER_H
#define INJECTOR_LOGGER_H

#ifdef __cplusplus
extern "C" {
#endif

// Initialize the logger (call this once at the start)
void InjectorLogger_Init();

// Log an informational message
void InjectorLogger_Info(const char* message);

// Log a success message
void InjectorLogger_Success(const char* message);

// Log an error message
void InjectorLogger_Error(const char* message);

// Log a warning message
void InjectorLogger_Warning(const char* message);

// Log a debug message
void InjectorLogger_Debug(const char* message);

// Clear the log file
void InjectorLogger_Clear();

#ifdef __cplusplus
}
#endif

#endif // INJECTOR_LOGGER_H