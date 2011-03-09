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

namespace opcodes {
	enum Opcode {
		PopTop              = 1,
		Nop                 = 9,
		BinaryAdd           = 23,
		ReturnValue         = 83,
		StoreName           = 90,
		LoadConst           = 100,
		LoadName            = 101,
		LoadAttr            = 106,
		ImportName          = 108,
		ImportFrom          = 109,
		LoadFast            = 124,
		CallFunction        = 131,
		MakeFunction        = 132,
	};
}

} // namespace

#endif
