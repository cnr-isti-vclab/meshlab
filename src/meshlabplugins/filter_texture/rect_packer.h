#ifndef RECT_PACKER_H
#define RECT_PACKER_H

#include <vector>

#include <vcg/space/point2.h>

using namespace vcg;

class rect_packer
{
	public:

		typedef rect_packer this_type;

		static bool pack(const std::vector<Point2i> & sizes, const Point2i & max_size, std::vector<Point2i> & posiz, Point2i & global_size);
};

#endif // RECT_PACKER_H
