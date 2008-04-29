#ifndef RECT_PACKER_H
#define RECT_PACKER_H

#include <vector>

#include "point2.h"

class rect_packer
{
	public:

		typedef rect_packer this_type;

		static bool pack(const std::vector<point2i> & sizes, const point2i & max_size, std::vector<point2i> & posiz, point2i & global_size);
};

#endif // RECT_PACKER_H
