#pragma once
#include "Core/ServerEngineTypes.hpp"
#include <windows.h>
#include "DB/DBBind.hpp"

{ % -macro gen_procedures() - %} {% include 'Procedure.hpp'%} {% endmacro%}

namespace SP
{
	{ {gen_procedures() | indent}}
};