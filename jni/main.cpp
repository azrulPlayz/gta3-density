// GTA III (Android 1.8, ARM32, libR1.so) - Density mod untuk AML
// Naikkan kepadatan mobil & pejalan kaki.
#include <mod/amlmod.h>
#include <mod/logger.h>

MYMOD(net.kamu.gta3density, GTA3 Density, 1.0, Kamu)

// ====== ATUR DI SINI ======
static const float CAR_DENSITY = 3.5f;  // 1.0 = normal
static const float PED_DENSITY = 2.5f;
// ==========================

static float *pCarDensity = nullptr;
static float *pPedDensity = nullptr;

// Hook CPopulation::Update(bool): setelah game hitung multiplier sendiri, kita timpa.
static void (*Population_Update_orig)(bool) = nullptr;
static void Population_Update_hook(bool addPeds) {
    Population_Update_orig(addPeds);
    if (pCarDensity) *pCarDensity = CAR_DENSITY;
    if (pPedDensity) *pPedDensity = PED_DENSITY;
}

extern "C" void OnModLoad() {
    void* h = aml->GetLibHandle("libR1.so");
    if (!h) { logger->Error("libR1.so tidak ketemu"); return; }

    // Nama simbol dari re3, BELUM diverifikasi di APK 1.8. Cek logcat.
    pCarDensity = (float*)aml->GetSym(h, "_ZN8CCarCtrl20CarDensityMultiplierE");
    pPedDensity = (float*)aml->GetSym(h, "_ZN11CPopulation20PedDensityMultiplierE");
    void* upd   = (void*)aml->GetSym(h, "_ZN11CPopulation6UpdateEb");

    logger->Info("CarDensity=%p PedDensity=%p Update=%p", pCarDensity, pPedDensity, upd);

    if (upd && (pCarDensity || pPedDensity))
        aml->Hook(upd, (void*)Population_Update_hook, (void**)&Population_Update_orig);
    else
        logger->Error("Simbol tidak ketemu, kirim log ini ke Claude");
}
