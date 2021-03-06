#include <string>
#include <cmath>
#include <vector>
#include "format.h"

#ifndef ASCII
#   define SQ "\xC2\xB2"
#   define SIGMA "\xCF\x83"
#   define CHI "\xCF\x87"
#   define MU "\xCE\xBC"
#else
#   define SQ "^2"
#   define SIGMA "sigma"
#   define CHI "chi"
#   define MU "mu"
#endif

#define SIGMASQ SIGMA SQ
#define CHISQ CHI SQ

std::string long_subscript(size_t i);

inline std::string char_subscript(char c) {
#ifndef ASCII
    char s[4] = {'\xE2', '\x82', '\x80', '\x00'};
    if (c >= '1' && c <= '9') {
	s[2] += c-'0';
    } else {
	switch (c) {
	    case '+': s[2] = '\x8A'; break;
	    case '-': s[2] = '\x8B'; break;
	    case '=': s[2] = '\x8C'; break;
	    case '(': s[2] = '\x8D'; break;
	    case ')': s[2] = '\x8E'; break;
	}
    }
    return s;
#else
    char s[3] = {'_', c, '\x00'};
    return s;
#endif
}

std::string sub(std::string s);

inline std::string sub(size_t i) {
#ifndef ASCII
    if (i <= 9) {
	char s[4] = {'\xE2', '\x82', '\x80', '\x00'};
	s[2] += i;
	return s;
    }
#endif
    return long_subscript(i);
}

struct ci_t {
    inline ci_t(std::pair<double, double> ci)
	: first(ci.first)
	, second(ci.second)
    {
    }
    double first;
    double second;
};

inline std::ostream & operator<<(std::ostream & os, const ci_t & ci) {
    return os << '[' << F_SAMPLE(ci.first) << ',' << F_SAMPLE(ci.second) << ']';
}

struct normal_sample {
    inline normal_sample()
	: m_n(0)
	, m_sum(0.0)
	, m_uss(0.0)
    {
    }

    inline normal_sample(size_t n, double sum, double uss)
	: m_n(n)
	, m_sum(sum)
	, m_uss(uss)
    {
    }

    template <typename T>
    inline void operator()(const T & time) {
	++m_n;
	double x = static_cast<double>(time);
	m_uss += x*x;
	m_sum += x;
    }

    // Number of observations
    inline size_t n() const { return m_n; }
    // Degrees of freedom
    inline size_t freedom() const { return n()-1; }
    // Mean of observations
    inline double mean() const { return sum()/n(); }
    // Standard deviation
    inline double stddev() const { return sqrt(m2()/freedom()); }
    // Sum
    inline double sum() const { return m_sum; }
    // Sum of squares
    inline double uss() const { return m_uss; }
    // SSD = sum of (x_i - mean)^2
    inline double ssd() const { return uss()-sum()*sum()/n(); }
    // Estimation of variance
    inline double variance() const { return ssd()/freedom(); }

    // Confidence interval for the mean. biogeostat p. 61
    ci_t ci(double alpha = 0.05) const;

    // Confidence interval for the variance. biogeostat p. 62
    ci_t ci_variance(double alpha = 0.05, bool loud = true) const;

    inline normal_sample operator+(const normal_sample & ys) {
	return normal_sample(n()+ys.n(), sum()+ys.sum(), uss()+ys.uss());
    }

    inline normal_sample operator+=(const normal_sample & ys) {
	m_n += ys.n();
	m_sum += ys.sum();
	m_uss += ys.uss();
	return *this;
    }

    template <typename IT>
    inline static normal_sample sum(IT begin, IT end) {
	normal_sample result;
	while (begin != end) {
	    result += *begin++;
	}
	return result;
    }

    // Deprecated
    inline double m2() const { return ssd(); }

private:
    size_t m_n;
    double m_sum;
    double m_uss;
};

// returns significance probability
double common_variance(double var_x, double var_y, size_t freedom_x, size_t freedom_y, bool loud = true);

// returns <estimation of variance, significance probability>
inline std::pair<double,double> common_variance(const normal_sample & first, const normal_sample & second, bool loud = true) {
    double p_obs = common_variance(first.variance(), second.variance(), first.freedom(), second.freedom(), loud);
    double var = (first.ssd() + second.ssd())/(first.n()+second.n()-2);
    return std::make_pair(var, p_obs);
}

std::pair<double, double> common_variance(const std::vector<normal_sample> & samples, bool loud = true);

std::pair<double, double> common_mean(const normal_sample & first, const normal_sample & second, bool loud = true);

std::pair<double, double> common_mean(const std::vector<normal_sample> & samples, bool loud = true);

// typo
typedef normal_sample normal_samples;

inline std::ostream & operator<<(std::ostream & os, const normal_sample & ns) {
    return os << "[Normal samples with n = " << ns.n() << ", mean = " << F_SAMPLE(ns.mean()) << ", m2 = " << F_SAMPLE_SQ(ns.m2()) << ", stddev = " << F_STDDEV(ns.stddev()) << ", var = " << F_VARIANCE(ns.variance()) << ']';
}
// vim: set sw=4 sts=4 ts=8 noet:
