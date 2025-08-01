#ifndef INTERVAL_H
#define INTERVAL_H

class interval{
public:
	double min, max;
	interval()
		: min(infinity), max(-infinity){}
	interval(double min, double max)
		: min(min), max(max){}
	interval(const interval& a, const interval& b){
		min = (a.min <= b.min) ? a.min : b.min;
		max = (a.max >= b.max) ? a.max : b.max;
	}
	

	double size() const{
		return max - min;
	}

	bool contains(double x) const{
		return (min <= x && x <= max);
	}

	bool surrounds(double x) const{
		return min < x && x < max;
	}

	interval expand(double delta) const {
		double padding = delta/2.;
		return {min - padding, max + padding};
	}

	double clamp(double x) const{
		if (x < min) return min;
		if (x > max) return max;
		return x;
	}

	static const interval empty, universe;
};

const interval interval::empty = interval(infinity, -infinity);
const interval interval::universe = interval(-infinity, infinity);


interval operator+ (const interval& itvl, double v){
	return interval(itvl.min + v, itvl.max + v);
}

interval operator+ (double v, const interval& itvl){
	return itvl + v;
}

#endif
