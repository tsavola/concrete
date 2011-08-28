/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OPCODES_HPP
#define CONCRETE_OPCODES_HPP

namespace concrete {

enum Opcode {
	OpPopTop              = 1,
	OpNop                 = 9,

	OpBinaryAdd           = 23,

	OpReturnValue         = 83,

	OpStoreName           = 90,

	OpLoadConst           = 100,
	OpLoadName            = 101,
	OpLoadAttr            = 106,
	OpCompareOp           = 107,
	OpImportName          = 108,
	OpImportFrom          = 109,

	OpPopJumpIfFalse      = 114,
	OpPopJumpIfTrue       = 115,

	OpLoadFast            = 124,
	OpStoreFast           = 125,

	OpCallFunction        = 131,
	OpMakeFunction        = 132,
};

enum CompareOp {
	CompareLT, CompareLE,
	CompareEQ, CompareNE,
	CompareGT, CompareGE,
	CompareIn, CompareNotIn,
	CompareIs, CompareIsNot,
	CompareException,
};

} // namespace

#endif
