/* 
	Authors
	*  M. Bilal khan     
  *  (BCS02153260)
*/

#include <iostream>
#include <string>
#include <algorithm>
#include <complex>
#include <cstdlib>
#include "skip_list.hpp"

struct Point {
	double x;
	double y;
	
	Point(double x, double y) : x(x), y(y) {}
	
	friend bool operator<(const Point& lhs, const Point& rhs) {
		double dist_lhs = std::sqrt(lhs.x * lhs.x + lhs.y * lhs.y);
		double dist_rhs = std::sqrt(rhs.x * rhs.x + rhs.y * rhs.y);
		return dist_lhs < dist_rhs;
	}
	
	friend std::ostream& operator<<(std::ostream& os, const Point& ob) {
		os << "(" << ob.x << "," << ob.y << ")";
		return os;
	}
};



int main() {
	#if 0
	skip_list<std::string> s(10);
	s.insert("c");
	s.insert("a");
	s.insert("d");
	s.insert("b");
	s.insert("f");
	s.insert("e");
	
	std::cout << "s1:" << std::endl;
	
	skip_list<int> s2(5);
	s2.insert(9);
	s2.insert(7);
	s2.insert(8);
	s2.insert(1);
	s2.insert(4);
	s2.insert(3);
	s2.insert(5);
	#endif

	#if 1
	Point p1(0,20);
	Point p2(0, 20);
	
	std::cout << p1 << std::endl;
	std::cout << p2 << std::endl;
	std::cout << (!(p2 < p1) && !(p1 < p2))	 << std::endl;
	
	srand(100);
	
	skip_list<Point> s(5);
	for(int i = 0; i < 10; i++) {
		s.insert(Point(std::rand()%100, std::rand()%100));
	}
	s.pretty_print();

	#endif
	
	//s.pretty_print();
	//skip_list<std::string>::Iterator i1 = s.begin();
	//skip_list<int>::Iterator i2 = s.end();
	//while(i1 != i2) {
	//	int a = *i1;
	//	std::cout << a << std::endl;
	//	++i1;
	//}
	//std::cout << "Done" << std::endl;
	
	//std::for_each(s.begin(), s.end(), [](auto e) {
	//	std::cout << e << std::endl;
	//});
	//std::cout << skip_list<int>::comp(3,4) << std::endl;
	
	//std::cout << *s.find(1) << "\n";
	//s.remove(1);
	//std::for_each(s.begin(), s.end(), [](auto e) {
	//std::cout << e << std::endl;
	//s});
	
	//s.pretty_print();
	return 0;
}
