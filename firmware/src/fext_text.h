/* SPDX-License-Identifier: MPL-2.0
 * Copyright (C) 2026 Jacek Mariański */
/* A module's text (fext.h: UTF-8; fble's carry Bluetooth devices' names, as they advertise them) as Felucca's
 * screen can draw it: its fonts (gfx.c, AF_S / AF_M) have ASCII 32..126 only. Shared by the firmware (fext_host.c:
 * every text from the module) and a test on the computer (tests/fext_test.c), so the folding that runs on the FM-1
 * is the one tested.
 *
 *   fext_name_fold(dst, src, max)   src (0-terminated) into dst (max bytes with the 0):
 *     ASCII                         as it is (control characters left out)
 *     U+00A0..U+017F (Latin-1,      its letter without the mark: French, German, Polish, Czech, Hungarian ...
 *       Latin Extended-A)           (é -> e, Ł -> L, ż -> z, ő -> o; sharp s: "ss"; other signs: '?')
 *     any other code point          '?'
 *     a byte that is no UTF-8       Latin-1 from 0xA0 (a name sent in Latin-1), folded the same, else '?'
 *     a sequence cut at the end     left out when the name has FEXT_NAME_LEN bytes (the module keeps 20: a letter may
 *                                   be cut in two); shorter, its bytes are Latin-1 */
#pragma once
#include <stdint.h>

#define FEXT_NAME_LEN 20u                           /* a name cut by the module (fble: 20 bytes): see above */

static const char FEXT_LATIN_1[97] =                /* U+00A0..U+00FF: a space, signs, then letters without marks */
    " ???????????????????????????????"
    "AAAAAAACEEEEIIIIDNOOOOOxOUUUUYTs" "aaaaaaaceeeeiiiidnooooo?ouuuuyty";
static const char FEXT_LATIN_A[129] =               /* U+0100..U+017F without their marks */
    "AaAaAaCcCcCcCcDd" "DdEeEeEeEeEeGgGg" "GgGgHhHhIiIiIiIi" "IiIiJjKkkLlLlLlL"
    "lLlNnNnNnnNnOoOo" "OoOoRrRrRrSsSsSs" "SsTtTtTtUuUuUuUu" "UuUuWwYyYZzZzZzs";

static void fext_name_fold(char *dst, const char *src, uint32_t max)
{
    const uint8_t *s = (const uint8_t *)src;
    uint32_t o = 0, cp, n, i;
    if (!max)
        return;
    while (*s && o + 1u < max) {
        uint8_t c = *s;
        n = c >= 0xF0u && c < 0xF8u ? 3u : c >= 0xE0u ? (c < 0xF0u ? 2u : 0u) : c >= 0xC2u ? 1u : 0u;
        for (i = 1; i <= n && (s[i] & 0xC0u) == 0x80u; i++)
            ;
        if (n && i <= n && !s[i] && (uint32_t)(s + i - (const uint8_t *)src) >= FEXT_NAME_LEN)
            break;                                  /* cut at the end */
        if (n && i > n) {                           /* a whole sequence */
            cp = c & (0x3Fu >> n);
            for (i = 1; i <= n; i++)
                cp = cp << 6 | (s[i] & 0x3Fu);
            s += n + 1u;
        } else {                                    /* not UTF-8: Latin-1 */
            cp = c >= 0x80u && c < 0xA0u ? '?' : c;
            s++;
        }
        if (cp < 0x20u || cp == 0x7Fu)
            continue;
        if (cp == 0xDFu) {
            dst[o++] = 's';
            if (o + 1u < max)
                dst[o++] = 's';
            continue;
        }
        dst[o++] = (char)(cp < 0x80u ? cp : cp >= 0xA0u && cp < 0x100u ? (uint32_t)(uint8_t)FEXT_LATIN_1[cp - 0xA0u]
                          : cp >= 0x100u && cp < 0x180u ? (uint32_t)(uint8_t)FEXT_LATIN_A[cp - 0x100u] : '?');
    }
    dst[o] = 0;
}
