#pragma once

typedef enum SingleFreqViewMode
{
    VM_S11 = 0,
    VM_S21,
    VM_S11_Z,
    VM_RX_RLC_SERIAL,
    VM_RX_RLC_PARALLEL,
    VM_RX_TX_Z,
    VM_RX_TX_RLC_SERIAL,
    VM_RX_TX_RLC_PARALLEL,
    VM_COUNT
} SingleFreqViewMode;

extern SingleFreqViewMode g_single_freq_view_mode2[2];
extern int g_single_freq_khz;

void SceneSingleFreqStart();
