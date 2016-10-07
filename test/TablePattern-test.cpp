/*
 * TablePattern-test.cpp
 *
 *  Created on: Oct 7, 2016
 *      Author: ruanpingcheng
 */
#include <list>
#include "TablePattern.h"
#include "gtest/gtest.h"

TEST(ShareURICountI1, Normal) {
	URI u1 = "a";
	URI u2 = "b";
	URI u3 = "c";

	list<URI> l1;
	l1.push_back(u1);
	l1.push_back(u2);
	l1.push_back(u3);

	URI v1 = "a";
	URI v2 = "b";

	list<URI> l2;
	l2.push_back(v1);
	l2.push_back(v2);

	EXPECT_EQ(shareURICount(l1,l2),2);
}



