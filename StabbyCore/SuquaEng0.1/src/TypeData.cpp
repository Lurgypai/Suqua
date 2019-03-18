#include "stdafx.h"
#include "TypeData.h"

type_id increm() {
	static type_id ctr = 0;
	return ++ctr;
}