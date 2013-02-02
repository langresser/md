#pragma once

/*  This file is part of Imagine.

	Imagine is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Imagine is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Imagine.  If not, see <http://www.gnu.org/licenses/> */

#include <limits.h>

#ifndef IMAGINE_SRC
	#include <stdio.h>
	#include <stdlib.h>
	#define BRANCH_PRINTF(str, ...) printf(str "\n", ## __VA_ARGS__)
	#define BRANCH_EXIT() abort()
#else
	#include <logger/interface.h>
	#include <base/interface.h>
	#define BRANCH_PRINTF(str, ...) logErr(str, ## __VA_ARGS__)
	#define BRANCH_EXIT() Base::abort()
#endif

#ifdef IMAGINE_SRC
	#define doOrElse(Do, Else) { if(Do != OK) Else; }
	#define doOrReturnVal(Do, Val) doOrElse(Do, return(Val))
	#define doOrReturn(Do) { CallResult _r = Do; if(_r != OK) return(_r); }
	#define doOrExit(Do) doOrElse(Do, Base::exit())
#endif

#ifndef NDEBUG
	#define bug_exit(msg, ...) { BRANCH_PRINTF(msg " in %s", ## __VA_ARGS__, __PRETTY_FUNCTION__); BRANCH_EXIT(); }
#else
	#define bug_exit(msg, ...) {  }
#endif

#define bug_branch(fmtStr, val) bug_exit("Bad branch value " fmtStr, val)

#define bcase break; case
#define bdefault break; default

// call a function pointer if non-null
#define callSafe(funcPtr, ...) { if(likely(funcPtr != 0)) funcPtr(__VA_ARGS__); }
// call a member function if object pointer non-null
#define callMSafe(objPtr, memberFunc, ...) { if(likely(objPtr != 0)) objPtr->memberFunc(__VA_ARGS__); }

// callback func helpers

#define DefCallbackWithCtxVars(type, modifiers, name) \
modifiers type name = 0; \
modifiers void* name ## Ctx = 0

#define DefCallbackWithCtxAssignModifiers(type, modifiers, funcName, name) \
modifiers funcName(type callback, void* ctx) \
{ \
	name = callback; \
	name ## Ctx = ctx; \
}

#define DefCallbackWithCtxAssign(type, funcName, name) \
		DefCallbackWithCtxAssignModifiers(type, void, funcName, name)

#define DefCallbackWithCtxAssignStatic(type, funcName, name) \
		DefCallbackWithCtxAssignModifiers(type, static void, funcName, name)

#define DefCallbackWithCtx(type, funcName, name) \
DefCallbackWithCtxVars(type,  , name); \
DefCallbackWithCtxAssign(type, funcName, name)

#define DefCallbackWithCtxStatic(type, funcName, name) \
DefCallbackWithCtxVars(type, static, name); \
DefCallbackWithCtxAssignStatic(type, funcName, name)
