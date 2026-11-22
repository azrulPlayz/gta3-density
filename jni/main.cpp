// GTA III (Android 1.8, ARM32, libR1.so) - Density mod untuk AML
// Simbol sudah diverifikasi dari libR1.so milik user.
#include <mod/amlmod.h>
#include <mod/logger.h>

MYMOD(net.kamu.gta3density, GTA3 Density, 1.1, Kamu)

// ====== ATUR DI SINI ======
static const float CAR_DENSITY = 10.0f;  // 1.0 = normal
static const float PED_DENSITY = 5.0f;
static const int   MAX_CARS    = 300;    // bawaan game: 12
static const int   MAX_PEDS    = 100;    // bawaan game: 25
// ==========================

static float *pCarDensity = nullptr;
static float *pPedDensity = nullptr;
static int   *pMaxCars    = nullptr;
static int   *pMaxPeds    = nullptr;

static void Apply() {
    if (pCarDensity) *pCarDensity = CAR_DENSITY;
    if (pPedDensity) *pPedDensity = PED_DENSITY;
    if (pMaxCars)    *pMaxCars    = MAX_CARS;
    if (pMaxPeds)    *pMaxPeds    = MAX_PEDS;
}

// CPopulation::Update() - tidak punya parameter di versi 1.8
static void (*Population_Update_orig)() = nullptr;
static void Population_Update_hook() {
    Apply();                  // timpa sebelum game menghitung
    Population_Update_orig();
    Apply();                  // timpa lagi kalau game mengubahnya
}

extern "C" void OnModLoad() {
    void* h = aml->GetLibHandle("libR1.so");
    if (!h) { logger->Error("libR1.so tidak ketemu"); return; }

    pCarDensity = (float*)aml->GetSym(h, "_ZN8CCarCtrl20CarDensityMultiplierE");
    pPedDensity = (float*)aml->GetSym(h, "_ZN11CPopulation20PedDensityMultiplierE");
    pMaxCars    = (int*)  aml->GetSym(h, "_ZN8CCarCtrl20MaxNumberOfCarsInUseE");
    pMaxPeds    = (int*)  aml->GetSym(h, "_ZN11CPopulation20MaxNumberOfPedsInUseE");
    void* upd   = (void*) aml->GetSym(h, "_ZN11CPopulation6UpdateEv");

    logger->Info("car=%p ped=%p maxCars=%p maxPeds=%p update=%p",
                 pCarDensity, pPedDensity, pMaxCars, pMaxPeds, upd);

    if (upd) aml->Hook(upd, (void*)Population_Update_hook, (void**)&Population_Update_orig);
    else     logger->Error("CPopulation::Update tidak ketemu");
}
