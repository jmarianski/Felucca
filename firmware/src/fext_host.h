/* SPDX-License-Identifier: MPL-2.0
 * Copyright (C) 2026 Jacek Mariański */
/* fext_host.c's names the files before it use (felucca.c includes this before params.c and the USB driver when
 * FELUCCA_FEXT is 1), and where Felucca keeps a module: its slot and RAM windows (fext.h). This is the one place
 * they are written down: the linker script (build.py writes build/app_fext.ld from app.ld and these), the loader's
 * check and the host's info (SysEx "FB" op 10, which the installer moves the module by) all take them from here.
 *   slot  [0xE8000, 0xFC000), 80 KiB: FL_XDATA1 (fm1_flash.h: a firmware's own data) above the FM6 voice bank
 *                                   (0xE7000, storage_hw.c ST_VBANK_LO)
 *   NV    [0x96000, 0x97000)  4 KiB: the module's settings and its NV, the top sector of FL_XDATA0. Its bytes
 *                                   0xF00..0xF07 stay erased (the update loader looks for its record there)
 *   CODE  [0x01C06F00, 0x01C07F00)  4 KiB: the top of RAMTEXT's debug room (app.ld; crt0's STATUS area is above it)
 *   DMA   [0x01C08000, 0x01C0E000) 24 KiB: app.ld's RAM bottom
 *   RAM   [0x01C0E000, 0x01C10000)  8 KiB: right above it
 * A radio's DMA reaches only low RAM (0x01C08000 does, 0x01C20000 does not: a module's stacks there and no Bluetooth),
 * so both windows take app.ld's RAM bottom: Felucca's RAM region (72 KiB, .data and .bss) moves up by them, the pool's
 * bottom with it (the pool 328 KiB). The DMA window is SLOOP's socket's too */
#pragma once
#include "fext.h"

#define FEXT_SLOT_OFF 0xE8000u
#define FEXT_SLOT_MAX 0x14000u
#define FL_FEXT_NV 0x96000u
#define FEXT_CODE_LO 0x01C06F00u
#define FEXT_CODE_HI 0x01C07F00u
#define FEXT_DMA_LO 0x01C08000u
#define FEXT_DMA_HI 0x01C0E000u
#define FEXT_RAM_LO 0x01C0E000u
#define FEXT_RAM_HI 0x01C10000u

static int8_t fext_power_fix;                     /* main.c fm1_power_stock(): 1 the DC-DC moved on */
static int8_t fext_clock_fix;                     /* main.c fm1_clock_stock(): 0 stock already, 1 set, -1 failed */
static volatile uint8_t fext_quiet;               /* the module's radio calibrates: the render holds (audio.c) */
static char fext_title[2][8] = {"EXT", "EXT 2"};  /* the module's pages' titles (params.c PAGES): its name's */
static void fext_midi_out(uint32_t pkt);          /* the USB driver's midi_out_try (FELUCCA_MIDI_OUT_TAP, the audio
                                                   * interrupt): a copy to the module */
#define FELUCCA_MIDI_OUT_TAP fext_midi_out
static uint32_t fext_pages(void);                 /* ui.c page_visible: the module's pages (1 without one) */
static int fext_col_used(uint32_t page2, uint32_t col);   /* ui_input.c: a knob of a column the module shows */
static void fext_knob(uint32_t page2, uint32_t col, int32_t steps);   /* ui_input.c edit_param */
static void fext_columns(uint32_t page2);         /* ui_draw.c draw_columns */
static void fext_graph(uint32_t page2);           /* ui_graph.c draw_graph */
static uint32_t fext_graph_sig(uint32_t page2);   /* ui_graph.c graph_signature */
