#include "../IDTF/Converter.h"

int main(int, char *[])
{
	bool res = IDTFConverter::IDTFToU3d("/home/alessandro/tmp/dod.idtf", "/home/alessandro/tmp/dod.u3d");
	return !res;
}
