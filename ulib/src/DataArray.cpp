/*
* DataArray.cpp
*
*  Created on: 03.06.2011
*      Author: Endre Bock
*/

#include <DataArray>

using namespace u;

int64 u::DataArray::readData(DataArray* bytes, int64 length)
{
	return bytes->writeData(*this, length);
}

