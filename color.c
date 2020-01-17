/* 
 *  This file is part of Aptdec.
 *  Copyright (c) 2004-2009 Thierry Leconte (F4DWV), Xerbo (xerbo@protonmail.com) 2019-2020
 *
 *  Aptdec is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include "offsets.h"

typedef struct {
    float r, g, b;
} rgb_t;

rgb_t applyPalette(char *palette, int val){
	return (rgb_t){
		palette[(int)CLIP(val, 0, 255)*3 + 0],
		palette[(int)CLIP(val, 0, 255)*3 + 1],
		palette[(int)CLIP(val, 0, 255)*3 + 2]
	};
}

rgb_t RGBcomposite(rgb_t top, float top_a, rgb_t bottom, float bottom_a){
    rgb_t composite;
	composite.r = MCOMPOSITE(top.r, top_a, bottom.r, bottom_a);
	composite.g = MCOMPOSITE(top.g, top_a, bottom.g, bottom_a);
	composite.b = MCOMPOSITE(top.b, top_a, bottom.b, bottom_a);
    return composite;
}

char GviPalette[256*3] = {
    "\230t\17\233x\22\236{\27\241\200\33\244\203\37\247\210#\252\214'\255\220"
    ",\260\2240\264\2305\267\2358\272\240=\274\245A\300\251E\303\255I\306\262"
    "M\311\266Q\314\272V\317\276Z\322\302^\325\306b\330\312g\334\317k\337\323"
    "o\342\330s\344\333w\350\337{\352\344\200\356\350\203\361\354\207\364\360"
    "\213\367\364\220\372\370\224\376\376\230\376\375\230\376\372\227\376\366"
    "\224\376\362\222\376\357\220\376\352\216\376\347\213\376\343\211\376\340"
    "\207\376\334\205\376\330\202\376\325\200\376\321\177\376\315|\376\312z\376"
    "\306y\376\302v\376\277t\376\273q\376\267o\376\263m\376\260k\376\254h\376"
    "\250f\376\245d\376\241b\376\235_\376\232^\376\226[\376\223Y\376\217W\376"
    "\213U\376\207R\376\203Q\376\200N\376}L\376zJ\376vG\376rE\376nB\376k@\376"
    "g>\376d<\376`:\376\\7\376X6\376T3\376Q0\376M/\376J-\376F*\376C(\376>%\376"
    ";$\3767!\3763\37\3760\35\376,\32\376(\31\376$\26\376!\24\376\35\22\376\32"
    "\20\376\26\15\376\22\13\376\16\11\376\12\6\376\7\4\376\0\0\373\2\0\366\3"
    "\0\362\5\0\355\7\0\350\11\0\343\13\0\336\15\0\332\17\0\325\21\0\320\22\0"
    "\313\24\0\307\26\0\303\27\0\275\32\0\270\33\0\264\35\0\257\37\0\253!\0\246"
    "#\0\241%\0\234&\0\227)\0\223+\0\216,\0\212.\0\2050\0\2002\0}4\0w6\0s7\0n"
    "9\0i;\0e>\0`?\0[A\0VC\0QE\0MG\0HH\0CK\0?M\0:N\0""5P\0""0R\0,S\0'V\0\"X\0"
    "\36Z\0\31\\\0\23^\0\17_\0\12a\0\3b\0\6e\2\12i\5\17n\11\23r\14\30v\17\34{"
    "\23\40\200\26$\203\31)\207\35.\215\40""1\221$6\225':\232*?\236.C\2431G\250"
    "4K\2548O\260;U\265>Y\272A\\\276Eb\303Ie\307Lj\313On\320Ss\324Vw\331Y{\336"
    "]\200\342`\203\347d\207\353g\213\357j\217\364n\223\371q\230\376t\222\375"
    "{\207\374\205}\373\216r\371\230g\371\241\\\370\254Q\367\265F\365\300;\364"
    "\3110\363\323%\362\334\32\361\347\17\357\361\3\356\371\4\353\370\3\347\365"
    "\4\342\361\4\335\357\4\331\354\4\323\351\3\317\346\3\313\343\4\306\340\3"
    "\301\335\3\275\332\3\270\327\3\263\324\2\257\322\3\252\316\3\245\314\3\241"
    "\311\2\234\306\2\230\303\2\223\300\3\216\275\3\211\273\2\205\267\2\200\265"
    "\2|\262\2w\257\2s\254\2n\251\2j\246\2e\243\2`\240\2[\235\2W\232\1S\230\2"
    "M\225\1I\221\2E\217\1@\214\1;\211\1""7\206\1""1\203\1-\200\0(~\1${\0\37y"
    "\0\33u\0\25r\0\21p\0\14l\0\7j\0\3"
};
char TempPalette[256*3] = {
    "\376\376\376\376\376\376\375\375\376\374\375\376\374\375\375\374\373\375"
    "\373\373\375\372\373\375\372\373\374\372\372\374\371\372\374\371\371\375"
    "\370\371\374\367\370\375\367\370\374\367\367\374\366\367\373\366\366\373"
    "\365\366\373\364\366\373\364\365\374\363\365\373\363\364\373\363\364\373"
    "\362\363\372\361\363\372\361\362\372\361\361\372\360\361\372\357\361\372"
    "\357\361\372\356\360\372\355\357\371\355\357\371\355\356\372\354\356\371"
    "\354\355\371\354\355\371\353\355\371\353\354\370\352\353\370\351\353\370"
    "\351\353\370\350\352\370\350\351\370\347\351\370\347\351\370\346\350\367"
    "\346\350\367\346\350\367\345\346\367\344\346\367\344\346\367\344\345\366"
    "\342\346\366\343\344\366\342\345\366\341\344\366\341\343\365\340\343\366"
    "\337\342\365\337\341\366\336\341\365\337\341\365\336\340\365\335\340\365"
    "\335\337\364\334\337\364\334\336\364\334\336\364\333\336\364\333\335\364"
    "\332\334\364\331\334\363\331\334\364\330\333\363\330\333\363\327\332\363"
    "\326\332\363\327\331\362\326\331\362\325\331\362\325\330\362\324\330\363"
    "\323\327\362\323\327\362\322\326\362\322\326\362\322\325\361\321\325\361"
    "\321\325\361\320\323\361\320\323\361\317\323\361\317\322\360\316\322\361"
    "\316\322\360\315\321\360\314\321\360\314\320\360\313\320\360\313\317\360"
    "\312\317\357\311\317\357\312\316\357\311\316\357\310\315\356\310\314\356"
    "\307\314\356\307\313\357\306\313\357\306\313\356\305\312\356\305\312\355"
    "\305\311\356\304\311\356\303\311\355\302\310\355\302\307\355\302\307\355"
    "\301\306\355\301\306\355\300\305\354\300\305\354\277\305\354\277\304\354"
    "\276\304\354\276\304\354\276\302\353\275\303\354\274\302\353\274\301\353"
    "\274\301\353\273\301\353\272\300\353\272\277\353\271\277\352\271\276\352"
    "\270\276\352\270\276\352\267\275\351\267\274\352\266\275\352\265\273\352"
    "\265\273\351\264\273\351\265\272\351\263\272\351\263\272\351\263\271\350"
    "\263\271\351\262\271\350\261\267\350\260\267\350\260\266\350\257\267\350"
    "\256\270\350\255\272\350\254\273\351\252\275\351\251\275\351\247\300\351"
    "\247\301\351\244\303\352\243\306\352\242\310\353\240\312\352\237\315\352"
    "\236\317\352\234\322\353\233\324\353\232\330\353\230\332\354\227\336\354"
    "\225\341\354\224\345\354\223\350\354\222\354\355\220\355\353\217\355\347"
    "\216\355\344\214\355\337\212\356\334\211\356\330\210\357\324\207\356\317"
    "\205\357\313\204\357\307\202\360\302\200\357\275\200\360\270\177\360\264"
    "~\360\256|\361\251z\360\244y\361\237x\361\230v\362\223u\362\215s\362\207"
    "r\362\200p\362|o\362un\362np\363ls\363kw\363i|\364g\200\363f\204\363d\210"
    "\364b\215\364a\222\364`\227\365^\234\365]\241\365\\\246\365Z\254\366Y\261"
    "\365W\267\366U\275\366T\303\366S\311\367Q\317\367O\325\367N\334\370L\343"
    "\370J\352\370I\360\370G\367\371F\370\362E\371\354C\370\345A\371\336@\371"
    "\326?\372\316=\372\307;\372\277:\372\2679\373\2577\373\2475\373\2373\373"
    "\2261\373\2161\373\206.\373}-\374u,\375k*\374b(\375Y'\375O%\375E#\375;\""
    "\3762\40\376(\37\376\35"
};
char PrecipPalette[256*3] = {
    "\xe0\x98\x8\xec\x84\x10\xf5\x70\x1b\xfc\x5c\x29\xff\x49\x38\xff\x37\x4a"
    "\xfb\x28\x5d\xf5\x1a\x71\xeb\xf\x85\xdf\x8\x99\xd0\x3\xad\xc0\x1\xbf"
    "\xad\x3\xd0\x9a\x7\xdf\x86\xf\xeb\x72\x1a\xf5\x5e\x27\xfb\x4b\x37\xfe"
    "\x39\x48\xff\x29\x5b\xfc\x1b\x6f\xf5\x10\x83\xec\x8\x98\xe0\x3\xab\xd2"
    "\x1\xbe\xc1\x2\xcf\xaf\x7\xdd\x9c\xe\xea\x88\x19\xf4\x73\x26\xfb\x5f"
    "\x35\xfe\x4c\x47\xff\x3a\x5a\xfc\x2a\x6d\xf6\x1d\x82\xed\x11\x96\xe1\x9"
    "\xa9\xd3\x3\xbc\xc3\x1\xcd\xb1\x2\xdc\x9e\x6\xe9\x89\xe\xf3\x75\x18"
    "\xfa\x61\x25\xfe\x4e\x34\xff\x3c\x45\xfc\x2c\x58\xf7\x1e\x6c\xee\x12\x80"
    "\xe3\x9\x94\xd5\x4\xa8\xc4\x1\xba\xb2\x2\xcc\x9f\x6\xdb\x8b\xd\xe8"
    "\x77\x17\xf2\x63\x23\xfa\x50\x33\xfe\x3d\x44\xff\x2d\x56\xfd\x1f\x6a\xf7"
    "\x13\x7e\xef\xa\x92\xe4\x4\xa6\xd6\x1\xb9\xc6\x2\xca\xb4\x5\xda\xa1"
    "\xc\xe7\x8d\x16\xf1\x79\x22\xf9\x65\x31\xfe\x51\x42\xff\x3f\x54\xfd\x2f"
    "\x68\xf8\x20\x7c\xf0\x14\x90\xe5\xb\xa4\xd7\x4\xb7\xc7\x1\xc9\xb6\x1"
    "\xd8\xa3\x5\xe6\x8f\xb\xf1\x7b\x15\xf9\x66\x21\xfd\x53\x30\xff\x41\x40"
    "\xfd\x30\x53\xf9\x21\x66\xf1\x15\x7a\xe6\xb\x8f\xd9\x5\xa3\xc9\x1\xb6"
    "\xb7\x1\xc7\xa5\x4\xd7\x91\xb\xe5\x7c\x14\xf0\x68\x20\xf8\x55\x2e\xfd"
    "\x42\x3f\xff\x31\x51\xfe\x22\x64\xf9\x16\x78\xf2\xc\x8d\xe7\x5\xa1\xda"
    "\x2\xb4\xca\x1\xc6\xb9\x4\xd6\xa6\xa\xe4\x93\x13\xef\x7e\x1f\xf7\x6a"
    "\x2d\xfd\x56\x3d\xff\x44\x4f\xfe\x33\x63\xfa\x24\x77\xf2\x17\x8b\xe8\xd"
    "\x9f\xdb\x6\xb2\xcc\x2\xc4\xbb\x1\xd4\xa8\x4\xe2\x94\x9\xee\x80\x12"
    "\xf7\x6c\x1d\xfc\x58\x2b\xff\x45\x3c\xfe\x34\x4e\xfa\x25\x61\xf3\x18\x75"
    "\xe9\xe\x89\xdc\x6\x9d\xcd\x2\xb1\xbc\x1\xc3\xaa\x3\xd3\x96\x9\xe1"
    "\x82\x11\xed\x6e\x1c\xf6\x5a\x2a\xfc\x47\x3a\xff\x36\x4c\xfe\x26\x5f\xfb"
    "\x19\x73\xf4\xf\x87\xea\x7\x9b\xde\x2\xaf\xcf\x1\xc1\xbe\x3\xd2\xac"
    "\x8\xe0\x98\x10\xec\x84\x1b\xf5\x6f\x29\xfc\x5c\x39\xff\x49\x4a\xff\x37"
    "\x5d\xfb\x28\x71\xf5\x1a\x85\xeb\xf\x9a\xdf\x7\xad\xd0\x3\xbf\xc0\x1"
    "\xd0\xad\x3\xdf\x9a\x7\xeb\x86\xf\xf5\x71\x1a\xfb\x5d\x27\xff\x4a\x37"
    "\xff\x39\x49\xfc\x29\x5c\xf5\x1b\x6f\xec\x10\x84\xe0\x8\x98\xd2\x3\xab"
    "\xc1\x1\xbe\xaf\x2\xcf\x9c\x7\xde\x87\xe\xea\x73\x19\xf4\x5f\x26\xfb"
    "\x4c\x36\xfe\x3a\x47\xff\x2a\x5a\xfc\x1c\x6e\xf6\x11\x82\xed\x9\x96\xe1"
    "\x3\xaa\xd3\x1\xbc\xc3\x2\xcd\xb1\x6\xdc\x9d\xe\xe9\x89\x18\xf3\x75"
    "\x25\xfa\x61\x34\xfe\x4e\x45\xff\x3c\x58\xfc\x2c\x6c\xf7\x1e\x80\xee\x12"
    "\x94\xe2\x9\xa8\xd4\x4\xbb\xc4\x1\xcc\xb2\x2\xdb\x9f\x6\xe8\x8b\xd"
    "\xf2\x77\x17\xfa\x63\x24\xfe\x4f\x33\xff\x3d\x44\xfd\x2d\x56\xf7\x1f\x6a"
    "\xef\x13\x7e\xe4\xa\x92\xd6\x4\xa6\xc6\x1\xb9\xb4\x2\xca\xa1\x5\xda"
    "\x8d\xc\xe7\x79\x16\xf2\x64\x22\xf9\x51\x31\xfe\x3f\x42\xff\x2e\x55\xfd"
    "\x20\x68\xf8\x14\x7c\xf0\xb\x91\xe5\x4\xa4\xd7\x1\xb7\xc7\x1\xc9\xb6"
    "\x5\xd9\xa3\xb\xe6\x8f\x15\xf1\x7a\x21\xf9\x66\x30\xfd\x53\x41\xff\x40"
    "\x53\xfd\x30\x66\xf9\x21\x7b\xf1\x15\x8f\xe6\xb\xa3\xd8\x5\xb6\xc9\x1"
    "\xc7\xb7\x1\xd7\xa4\x4\xe5\x91\xb\xf0\x7c\x14\xf8\x68\x20\xfd\x54\x2e"
    "\xff\x42\x3f\xfe\x31\x51\xf9\x22\x65\xf1\x16\x79\xe7\xc\x8d\xda\x5\xa1"
    "\xca\x2\xb4\xb9\x1\xc6\xa6\x4\xd6\x92\xa\xe4\x7e\x13\xef\x6a\x1f\xf7"
    "\x56\x2d\xfd\x44\x3d\xff\x33\x4f\xfe\x24\x63\xfa"
};