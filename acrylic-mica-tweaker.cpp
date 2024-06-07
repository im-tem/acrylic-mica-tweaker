// ==WindhawkMod==
// @id              acrylic-mica-tweaker
// @name            Acrylic/Mica Tweaker
// @description     Mod for adjusting blurs present throughout the system.
// @version         0.1
// @author          im-tem
// @github          https://github.com/im-tem/
// @include         explorer.exe
// @include         dwm.exe
// @include         *
// @compilerOptions -lcomdlg32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
This mod allows adjusting the blur radius and optimization mode for both Acrylic and Mica.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
# Here you can define settings, in YAML format, that the mod users will be able
# to configure. Metadata values such as $name and $description are optional.
# Check out the documentation for more information:
# https://github.com/ramensoftware/windhawk/wiki/Creating-a-new-mod#settings
- blurMulti: 100
  $name: Blur multiplier
  $description: Amplifies blur by a given value. (value is specificed in percentages)
- blurMode: default
  $name: Blur mode
  $description: Allows forcing blur's performance-for-quality tradeoff value.
  $options:
  -  default: default
  -  speed: speed
  -  balanced: balanced
  -  quality: quality
*/
// ==/WindhawkModSettings==


#include <windhawk_api.h>

struct {
    float blurMulti;
    char blurMode;   
} settings;

const wchar_t* BlurModeValues[]{
    L"default",
    L"speed",
    L"balanced",
    L"quality",
};

HMODULE wuceffectslib=0x0;

typedef __fastcall void GetBlurParams_t(void* a,unsigned int b,float* c, void* d, void* e);
GetBlurParams_t* GetBlurParams_holder;

tagWH_FIND_SYMBOL_OPTIONS symbolopts={NULL,true};

__fastcall void GetBlurParams_hook(void* self,unsigned int unk2, float* blurRadius, void* blurMode, void* blurBorder){
    GetBlurParams_holder(self,unk2,blurRadius,blurMode,blurBorder);
    *blurRadius*=settings.blurMulti;
    if(settings.blurMode>=0){
        *(int*)blurMode=settings.blurMode; //force speed mode
    };
    return;
};

void LoadWUCHooks(){
    if(wuceffectslib!=0x0){
        return;
    };
    wuceffectslib = LoadLibrary(L"wuceffects.dll");
    tagWH_FIND_SYMBOL sym;

    void* findhandle=Wh_FindFirstSymbol(wuceffectslib, &symbolopts, &sym);
    do{
        if(wcsstr(sym.symbolDecorated,LR"(?GetBlurParams@EffectInstance@Composition@UI@Windows@@UEBAXIPEAMPEAW4D2D1_GAUSSIANBLUR_OPTIMIZATION@@PEAW4D2D1_BORDER_MODE@@@Z)")){
            Wh_SetFunctionHook((void*)sym.address,(void*)GetBlurParams_hook,(void**)&GetBlurParams_holder);
        };
    }while(Wh_FindNextSymbol(findhandle, &sym));
    Wh_FindCloseSymbol(findhandle);
};

void LoadSettings() {
    settings.blurMulti = 0.01*Wh_GetIntSetting(L"blurMulti");
    settings.blurMode=-1;
    const wchar_t* str=Wh_GetStringSetting(L"blurMode");
    for(int i=0;i<4;i++){
        if(wcsstr(BlurModeValues[i],str)){
            settings.blurMode=i-1;
            break;
        };
    };
};

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL Wh_ModInit() {
    Wh_Log(L"Init");
    LoadSettings();
    LoadWUCHooks();
    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit() {
    Wh_Log(L"Uninit");
    if(wuceffectslib){
        FreeLibrary(wuceffectslib);
        wuceffectslib=0x0;
    };
}

// The mod setting were changed, reload them.
void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");

    LoadSettings();
}
