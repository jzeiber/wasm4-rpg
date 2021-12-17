#include "wasm4.h"
#include "global.h"

void start()
{
	PALETTE[0]=0x472D3C;	// dark brown
	PALETTE[1]=0x38D973;	// green
	PALETTE[2]=0x3CACD7;	// blue
	PALETTE[3]=0xCFC6B8;	// off white

    global::SetupGlobals();

/*
    extern void* __heap_base;
    OutputStringStream ostr;
    ostr.Clear();
    ostr << (uint64_t)((uintptr_t)&__heap_base) << " start of heap  " << (uint64_t)(65536-(uintptr_t)&__heap_base) << " available bytes";
    trace(ostr.Buffer());
*/
    
}

void update()
{
    global::input->Update();
    global::game->HandleInput(global::input);
    global::game->Update(1);
    global::game->Draw();
}
