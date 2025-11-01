#include <list>
#include <vector>
#include <cstring>
#include <pthread.h>
#include <thread>
#include <cstring>
#include <string>
#include <jni.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <dlfcn.h>
#include "Includes/Logger.h"
#include "Includes/obfuscate.h"
#include "Includes/Utils.hpp"
#include "Menu/Menu.hpp"
#include "Menu/Jni.hpp"
#include "Includes/Macros.h"
#include "Dobby/dobby.h"
#include "InjectorLogger/InjectorLogger.h"

#define targetLibName OBFUSCATE("libBlockMan.so")

uintptr_t UseHex(const char* hexStr) {
    return strtoul(hexStr, nullptr, 16);
}

#define OBF_HEX(name, hex_str) const uintptr_t name = UseHex(OBFUSCATE(hex_str))

struct MemPatches {
    MemoryPatch;
} gPatches;


JNIEnv* Zenv;
JNIEnv* Aenv;
jclass Aclazz;
jobject Zctx;
JavaVM* g_JavaVM;
const char* defaultScript = nullptr;
bool defaultScriptLoaded = false;




static int (*org_getglobal)(uintptr_t*, const char*) = nullptr;
static void (*org_pushstring)(uintptr_t*, const char*) = nullptr;
static int (*org_pcall)(uintptr_t*, int, int, int) = nullptr;

int (*lua_getglobal)(uintptr_t*, const char*) = nullptr;
void (*lua_pushstring)(uintptr_t*, const char*) = nullptr;
int (*lua_pcall)(uintptr_t*, int, int, int) = nullptr;

uintptr_t* g_luaState = nullptr;

void InjectFunc32bit();
void RunFile(const char* scriptPath);
void RunLua(const char* luaCode);




int hook_getglobal(uintptr_t* L, const char* name) {
    if (L != nullptr) {
        g_luaState = L;
    }
    return org_getglobal ? org_getglobal(L, name) : 0;
}

void hook_pushstring(uintptr_t* L, const char* str) {
    if (L != nullptr) {
        g_luaState = L;
    }
    if (org_pushstring) org_pushstring(L, str);
}

int hook_pcall(uintptr_t* L, int nargs, int nresults, int errfunc) {
    if (L != nullptr) {
        g_luaState = L;
    }
    if (!defaultScriptLoaded && defaultScript != nullptr && nargs == 3) {
        RunFile(defaultScript);
        defaultScriptLoaded = true;
    }
    return org_pcall ? org_pcall(L, nargs, nresults, errfunc) : 0;
}



void RunLua(const char* luaCode) {
    if (!luaCode || strlen(luaCode) == 0) {
        return;
    }
    

    if (!g_luaState) {
        return;
    }

    org_getglobal(g_luaState, "loadstring");
    org_pushstring(g_luaState, luaCode);

    if (org_pcall(g_luaState, 1, 1, 0) != 0) {
        return;
    }

    if (org_pcall(g_luaState, 0, 0, 0) != 0) {
        return;
    }
}


void RunFile(const char* scriptPath) {

    auto readLuaScript = [](const char* path) -> char* {
        FILE* file = fopen(path, "r");
        if (!file) return nullptr;

        fseek(file, 0, SEEK_END);
        size_t size = ftell(file);
        rewind(file);

        char* buffer = (char*)malloc(size + 1);
        if (!buffer) {
            fclose(file);
            return nullptr;
        }

        fread(buffer, 1, size, file);
        buffer[size] = '\0';
        fclose(file);
        return buffer;
    };

    char* luaCode = readLuaScript(scriptPath);
    if (!luaCode || strlen(luaCode) == 0) {
        free(luaCode);
        return;
    }
    

    if (!g_luaState) {
        free(luaCode);
        return;
    }

    org_getglobal(g_luaState, "loadstring");
    org_pushstring(g_luaState, luaCode);
    free(luaCode);

    if (org_pcall(g_luaState, 1, 1, 0) != 0) {
        return;
    }

    if (org_pcall(g_luaState, 0, 0, 0) != 0) {
        return;
    }
    InjectorLogger_Success("Panel loaded Scuccssfully");
}

void InjectFunc32bit() {
sleep(3);

    const char* libName = targetLibName;

    auto getLibraryBaseAddress = [](const char* libName) -> uintptr_t {
        FILE* fp = fopen("/proc/self/maps", "r");
        if (!fp) return 0;

        uintptr_t baseAddr = 0;
        char line[512];

        while (fgets(line, sizeof(line), fp)) {
            if (strstr(line, libName)) {
                char* end = strchr(line, '-');
                if (end) {
                    *end = '\0';
                    baseAddr = strtoul(line, nullptr, 16);
                    break;
                }
            }
        }

        fclose(fp);
        return baseAddr;
    };

    uintptr_t base = getLibraryBaseAddress(libName);
    if (!base) return;

    OBF_HEX(luar_getglobalOffset,  "10CAF3F"); // search string: package
    OBF_HEX(luar_pushstringOffset, "10CADCD"); // search string: error loading module '%s' from file
    OBF_HEX(luar_pcallOffset,      "10CB685"); // luaL_loadbuffer error
    // Note: these strings are helpers to find the new offsets but it's not the offset string itself you'll to scroll until you find the correct offset, it's close to the string.
    
    lua_getglobal = (int (*)(uintptr_t*, const char*))(base + luar_getglobalOffset | 1);
    lua_pushstring = (void (*)(uintptr_t*, const char*))(base + luar_pushstringOffset | 1);
    lua_pcall = (int (*)(uintptr_t*, int, int, int))(base + luar_pcallOffset | 1);


    if (!org_getglobal)
        DobbyHook((void*)lua_getglobal, (void*)hook_getglobal, (void**)&org_getglobal);

    if (!org_pushstring)
        DobbyHook((void*)lua_pushstring, (void*)hook_pushstring, (void**)&org_pushstring);

    if (!org_pcall)
        DobbyHook((void*)lua_pcall, (void*)hook_pcall, (void**)&org_pcall);

    if (!g_luaState) return;
    
}


void BypassFunc32bit() {
    const char* libName = targetLibName;

    auto getLibraryBaseAddress = [](const char* libName) -> uintptr_t {
        FILE* fp = fopen("/proc/self/maps", "r");
        if (!fp) return 0;
        uintptr_t baseAddr = 0;
        char line[512];
        while (fgets(line, sizeof(line), fp)) {
            if (strstr(line, libName)) {
                char* end = strchr(line, '-');
                if (end) {
                    *end = '\0';
                    baseAddr = strtoul(line, nullptr, 16);
                    break;
                }
            }
        }
        fclose(fp);
        return baseAddr;
    };

    uintptr_t base = getLibraryBaseAddress(libName);
    if (!base) {
        return;
    }

    OBF_HEX(luar_detectionOffset, "DE79E2");
    uintptr_t patchAddr = base + luar_detectionOffset;
    
    uint8_t movw_patch[] = {0x40, 0xF2, 0x00, 0x02};
    uint8_t movt_patch[] = {0xC0, 0xF2, 0x00, 0x02};
    
    size_t pageSize = sysconf(_SC_PAGESIZE);
    void* pageStart = (void*)(patchAddr & ~(pageSize - 1));
    
    if (mprotect(pageStart, pageSize, PROT_READ | PROT_WRITE | PROT_EXEC) == 0) {
        memcpy((void*)patchAddr, movw_patch, 4);
        memcpy((void*)(patchAddr + 4), movt_patch, 4);
        
        __builtin___clear_cache((char*)patchAddr, (char*)(patchAddr + 8));
        
        mprotect(pageStart, pageSize, PROT_READ | PROT_EXEC);
        InjectorLogger_Success("Integrity check Bypassed Scuccssfully");
    } else {
        InjectorLogger_Error("Integrity check bypassing fail");
    }
}


jobjectArray GetFeatureList(JNIEnv *env, jobject context) {
    jobjectArray ret;

    const char *features[] = {
            OBFUSCATE("Toggle_No death"),
    };

    int Total_Feature = (sizeof features / sizeof features[0]);
    ret = (jobjectArray)
            env->NewObjectArray(Total_Feature, env->FindClass(OBFUSCATE("java/lang/String")),
                                env->NewStringUTF(""));

    for (int i = 0; i < Total_Feature; i++)
        env->SetObjectArrayElement(ret, i, env->NewStringUTF(features[i]));

    return (ret);
}


void Changes(JNIEnv* env, jclass clazz, jobject obj, jint featNum, jstring featName,
             jint value, jlong Lvalue, jboolean boolean, jstring str) {
    LOGD(OBFUSCATE("Feature name: %d - %s | Value: = %d | LValue: %lld | Bool: = %d | Text: = %s"),
         featNum, env->GetStringUTFChars(featName, 0), value, Lvalue, boolean,
         str != nullptr ? env->GetStringUTFChars(str, 0) : "");
         
    jclass local = env->FindClass("com/sandboxol/blockmango/EchoesRenderer");
    if (local) {
        Aclazz = (jclass)env->NewGlobalRef(local);
        env->DeleteLocalRef(local);
    }
    
    switch (featNum) {
        case 27:
            RunLua(env->GetStringUTFChars(str, 0));
            break;
        case 28:
            RunFile(env->GetStringUTFChars(str, 0));
            break;
        case 29:
            defaultScript = env->GetStringUTFChars(str, 0);
            break;
    }
}


//Target lib here

ElfScanner g_il2cppELF;

// we will run our hacks in a new thread so our while loop doesn't block process main thread
void *hack_thread(void *) {
    while (!isLibraryLoaded(targetLibName)) {
        sleep(1);
    }
    InjectorLogger_Info("libBlockMan.so is loaded");
    InjectorLogger_Info("Loading....");
    BypassFunc32bit();
    std::thread([] {
        usleep(500000);
        InjectFunc32bit();
    }).detach();
    
    do {
        sleep(1);
        g_il2cppELF = ElfScanner::createWithPath(targetLibName);
    } while (!g_il2cppELF.isValid());
}

__attribute__((constructor))
void lib_main() {
    // Create a new thread so it does not block the main thread, means the game would not freeze
    pthread_t ptid;
    pthread_create(&ptid, NULL, hack_thread, NULL);
}
