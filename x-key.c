//
//  x-key.c
//  Mana
//
//  Created by Matthew Hinkle on 2/28/14.
//  Copyright (c) 2014 Matthew Hinkle. All rights reserved.
//

#if defined (__unix__) || defined (__linux__) || (defined (__APPLE__) && defined (__MACH__) && defined (MANA_USE_X11))

#include "x-key.h"

extern mfwKey makeKey(long key)
{
	switch(key) {
	case NoSymbol: return Key_Nil;

	case XK_Left: return Key_Left;
	case XK_Up: return Key_Up;
	case XK_Right: return Key_Right;
	case XK_Down: return Key_Down;

	case XK_BackSpace: return Key_Backspace;
	case XK_Tab: return Key_Tab;
	case XK_Clear: return Key_Clear;
	case XK_Return: return Key_Return;
	case XK_Pause: return Key_Pause;
	case XK_Sys_Req: return Key_SystemRequirements;
	case XK_Escape: return Key_Esc;
	case XK_Delete: return Key_Delete;

	case XK_0: return Key_0;
	case XK_1: return Key_1;
	case XK_2: return Key_2;
	case XK_3: return Key_3;
	case XK_4: return Key_4;
	case XK_5: return Key_5;
	case XK_6: return Key_6;
	case XK_7: return Key_7;
	case XK_8: return Key_8;
	case XK_9: return Key_9;

	case XK_a: return Key_a;
	case XK_b: return Key_b;
	case XK_c: return Key_c;
	case XK_d: return Key_d;
	case XK_e: return Key_e;
	case XK_f: return Key_f;
	case XK_g: return Key_g;
	case XK_h: return Key_h;
	case XK_i: return Key_i;
	case XK_j: return Key_j;
	case XK_k: return Key_k;
	case XK_l: return Key_l;
	case XK_m: return Key_m;
	case XK_n: return Key_n;
	case XK_o: return Key_o;
	case XK_p: return Key_p;
	case XK_q: return Key_q;
	case XK_r: return Key_r;
	case XK_s: return Key_s;
	case XK_t: return Key_t;
	case XK_u: return Key_u;
	case XK_v: return Key_v;
	case XK_w: return Key_w;
	case XK_x: return Key_x;
	case XK_y: return Key_y;
	case XK_z: return Key_z;

	case XK_bracketleft: return Key_LeftBracket;
	case XK_bracketright: return Key_RightBracket;
	case XK_backslash: return Key_BackSlash;
	case XK_underscore: return Key_Underscore;
	case XK_grave: return Key_Grave;

	case XK_A: return Key_A;
	case XK_B: return Key_B;
	case XK_C: return Key_C;
	case XK_D: return Key_D;
	case XK_E: return Key_E;
	case XK_F: return Key_F;
	case XK_G: return Key_G;
	case XK_H: return Key_H;
	case XK_I: return Key_I;
	case XK_J: return Key_J;
	case XK_K: return Key_K;
	case XK_L: return Key_L;
	case XK_M: return Key_M;
	case XK_N: return Key_N;
	case XK_O: return Key_O;
	case XK_P: return Key_P;
	case XK_Q: return Key_Q;
	case XK_R: return Key_R;
	case XK_S: return Key_S;
	case XK_T: return Key_T;
	case XK_U: return Key_U;
	case XK_V: return Key_V;
	case XK_W: return Key_W;
	case XK_X: return Key_X;
	case XK_Y: return Key_Y;
	case XK_Z: return Key_Z;

	case XK_braceleft: return Key_LeftBrace;
	case XK_braceright: return Key_RightBrace;
	case XK_bar: return Key_Bar;
	case XK_asciitilde: return Key_Tilde;

	case XK_F1: return Key_F1;
	case XK_F2: return Key_F2;
	case XK_F3: return Key_F3;
	case XK_F4: return Key_F4;
	case XK_F5: return Key_F5;
	case XK_F6: return Key_F6;
	case XK_F7: return Key_F7;
	case XK_F8: return Key_F8;
	case XK_F9: return Key_F9;
	case XK_F10: return Key_F10;
	case XK_F11: return Key_F11;
	case XK_F12: return Key_F12;

	case XK_Shift_L: return Key_LeftShift;
	case XK_Shift_R: return Key_RightShift;
	case XK_Control_L: return Key_LeftControl;
	case XK_Control_R: return Key_RightControl;
	case XK_Caps_Lock: return Key_CapsLock;
	case XK_Shift_Lock: return Key_ShiftLock;
	case XK_Meta_L: return Key_LeftMeta;
	case XK_Meta_R: return Key_RightMeta;
	case XK_Alt_L: return Key_LeftAlt;
	case XK_Alt_R: return Key_RightAlt;
	case XK_Super_L: return Key_LeftSuper;
	case XK_Super_R: return Key_RightSuper;
	case XK_Hyper_L: return Key_LeftHyper;
	case XK_Hyper_R: return Key_RightHyper;

	case XK_space: return Key_Space;
	case XK_exclam: return Key_ExclamationPoint;
	case XK_at: return Key_At;
	case XK_numbersign: return Key_NumberSign;
	case XK_dollar: return Key_Currency;
	case XK_percent: return Key_Percent;
	case XK_ampersand: return Key_Ampersand;
	case XK_asterisk: return Key_Asterisk;
	case XK_parenleft: return Key_LeftParen;
	case XK_parenright: return Key_RightParen;
	case XK_plus: return Key_Plus;
	case XK_comma: return Key_Comma;
	case XK_minus: return Key_Minus;
	case XK_period: return Key_Period;
	case XK_slash: return Key_Slash;

	case XK_colon: return Key_Colon;
	case XK_semicolon: return Key_Semicolon;
	case XK_less: return Key_Less;
	case XK_equal: return Key_Equal;
	case XK_greater: return Key_Greater;
	case XK_question: return Key_QuestionMark;

	case XK_quotedbl: return Key_DoubleQuote;

	case XK_Home: return Key_Home;
	case XK_Prior: return Key_Prior;
	case XK_Next: return Key_Next;
	case XK_Begin: return Key_Begin;
	case XK_End: return Key_End;

	case XK_Select: return Key_Select;
	case XK_Print: return Key_Print;
	case XK_Execute: return Key_Execute;
	case XK_Insert: return Key_Insert;
	case XK_Undo: return Key_Undo;
	case XK_Redo: return Key_Redo;
	case XK_Menu: return Key_Menu;
	case XK_Find: return Key_Find;
	case XK_Cancel: return Key_Cancel;
	case XK_Help: return Key_Help;
	case XK_Break: return Key_Break;
	case XK_Mode_switch: return Key_ModeSwitch;
	case XK_Num_Lock: return Key_NumLock;

	default: return Key_Unknown;
	}
}

#endif
