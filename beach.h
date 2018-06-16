#ifndef BEACH_H
#define BEACH_H

/* C interface for beach animation routines */

/* board names to numbers */
#define L201 0
#define L202 1
#define L203 2
#define L204 3
#define L205 4
#define L206 5
#define L207 6
#define L208 7
#define L209 8
#define L210 9
#define L211 10
#define L212 11
#define L213 12
#define L214 13
#define L215 14
#define L216 15
#define L217 16
#define L218 17
#define L219 18
#define L220 19
#define L221 20
#define L222 21
#define L223 22
#define L224 23
#define L225 24
#define L226 25
#define L227 26
#define L228 27
#define L229 28
#define L230 29
#define L231 30
#define L232 31
#define L233 32
#define L234 33
#define L235 34
#define L236 35
#define L237 36
#define L238 37
#define L239 38
#define L240 39
#define L301 40
#define L302 41
#define L303 42
#define L304 43
#define L305 44
#define L306 45
#define L307 46
#define L308 47
#define L309 48
#define L310 49
#define L311 50
#define L312 51
#define L313 52
#define L314 53
#define L315 54
#define L316 55
#define L317 56
#define L318 57
#define L319 58
#define L320 59
#define L321 60
#define L322 61
#define L323 62
#define L324 63
#define L325 64
#define L326 65
#define L327 66
#define L328 67
#define L329 68
#define L330 69
#define L331 70
#define L332 71
#define L333 72
#define L334 73
#define L335 74
#define L336 75
#define L337 76
#define L338 77
#define L339 78
#define L340 79
#define L401 80
#define L402 81
#define L403 82
#define L404 83
#define L405 84
#define L406 85
#define L407 86
#define L408 87
#define L409 88
#define L410 89
#define L411 90
#define L412 91
#define L413 92
#define L414 93
#define L415 94
#define L416 95
#define L417 96
#define L418 97
#define L419 98
#define L420 99
#define L421 100
#define L422 101
#define L423 102
#define L424 103
#define L425 104
#define L426 105
#define L427 106
#define L428 107
#define L429 108
#define L430 109
#define L431 110
#define L432 111
#define L433 112
#define L434 113
#define L435 114
#define L436 115
#define L437 116
#define L438 117
#define L439 118
#define L440 119
#define L501 120
#define L502 121
#define L503 122
#define L504 123
#define L505 124
#define L506 125
#define L507 126
#define L508 127
#define L509 128
#define L510 129
#define L511 130
#define L512 131
#define L513 132
#define L514 133
#define L515 134
#define L516 135
#define L517 136
#define L518 137
#define L519 138
#define L520 139
#define L521 140
#define L522 141
#define L523 142
#define L524 143
#define L525 144
#define L526 145
#define L527 146
#define L528 147
#define L529 148
#define L530 149
#define L531 150
#define L532 151
#define L533 152
#define L534 153
#define L535 154
#define L536 155
#define L537 156
#define L538 157
#define L539 158
#define L540 159
#define L601 160
#define L602 161
#define L603 162
#define L604 163
#define L605 164
#define L606 165
#define L607 166
#define L608 167
#define L609 168
#define L610 169
#define L611 170
#define L612 171
#define L613 172

/* some colors */
extern const Pixel Pix_off;
extern const Pixel Pix_sand;
extern const Pixel Pix_water;
extern const Pixel Pix_water2;
extern const Pixel Pix_skin;

/* animation control */
extern int anim_wave_state;

void setpixel(Pixel *scr, const Pixel *col);
void setwave(Pixel *scr, int w, const Pixel *col);
void animate_wave(Pixel *scr);
void setperson(Pixel *scr, int p, Pixel *shirt, Pixel *pants, int left, int right);
#endif
