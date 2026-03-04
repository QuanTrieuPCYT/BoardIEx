#include <Headers/plugin_start.hpp>
#include <Headers/kern_api.hpp>
#include <Headers/kern_patcher.hpp>

static const char *bootargOff[]   = { "-boardiexoff" };
static const char *bootargDebug[] = { "-boardiexdbg" };
static const char *bootargBeta[]  = { "-boardiexbeta" };

static const char *pathAGDPolicy[] = {
    "/System/Library/Extensions/AppleGraphicsControl.kext/Contents/PlugIns/AppleGraphicsDevicePolicy.kext/Contents/MacOS/AppleGraphicsDevicePolicy"
};

static KernelPatcher::KextInfo kextAGDPolicy = {
    "com.apple.driver.AppleGraphicsDevicePolicy",
    pathAGDPolicy,
    arrsize(pathAGDPolicy),
    {true},
    {},
    KernelPatcher::KextInfo::Unloaded
};

static void processKext(void *user, KernelPatcher &patcher, size_t index, mach_vm_address_t address, size_t size) {
    // much more reliable than bootloader's manual kext patching
    if (kextAGDPolicy.loadIndex == index) {
        DBGLOG("boardiex", "AppleGraphicsDevicePolicy loaded, applying kernel patch...");

        KernelPatcher::LookupPatch patch = {
            &kextAGDPolicy,
            reinterpret_cast<const uint8_t *>("board-id"),
            reinterpret_cast<const uint8_t *>("board-ix"),
            sizeof("board-id"),
            1
        };

        patcher.applyLookupPatch(&patch);

        if (patcher.getError() != KernelPatcher::Error::NoError) {
            SYSLOG("boardiex", "Failed to apply kernel patch, error: %d", patcher.getError());
            patcher.clearError();
        } else {
            DBGLOG("boardiex", "Successfully replaced board-id with board-ix");
        }
    }
}

static void pluginStart() {
    DBGLOG("boardiex", "Initializing BoardIEx...");
    lilu.onKextLoadForce(&kextAGDPolicy, 1,
    [](void *user, KernelPatcher &patcher, size_t index, mach_vm_address_t address, size_t size) {
        processKext(user, patcher, index, address, size);
    }, nullptr);
}

PluginConfiguration ADDPR(config) {
    xStringify(PRODUCT_NAME),
    parseModuleVersion(xStringify(MODULE_VERSION)),
    LiluAPI::AllowNormal | LiluAPI::AllowInstallerRecovery | LiluAPI::AllowSafeMode,
    bootargOff,
    arrsize(bootargOff),
    bootargDebug,
    arrsize(bootargDebug),
    bootargBeta,
    arrsize(bootargBeta),
    KernelVersion::Mavericks,
    KernelVersion::Tahoe,
    pluginStart
};
