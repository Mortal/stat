#include <boost/math/distributions/students_t.hpp>
#include <boost/math/distributions/fisher_f.hpp>
#include <boost/math/distributions/chi_squared.hpp>
#include "distributions.h"

double quantile_students_t(size_t freedom, double p) {
    return quantile(boost::math::students_t(static_cast<double>(freedom)), p);
}
double quantile_chi_squared(size_t freedom, double p) {
    return quantile(boost::math::chi_squared(static_cast<double>(freedom)), p);
}
double cdf_fisher_f(size_t freedom_numerator, size_t freedom_denominator, double x) {
    return boost::math::cdf(boost::math::fisher_f(static_cast<double>(freedom_numerator), static_cast<double>(freedom_denominator)), x);
}
double cdf_students_t(size_t freedom, double x) {
    return boost::math::cdf(boost::math::students_t(static_cast<double>(freedom)), x);
}
double cdf_chi_squared(size_t freedom, double x) {
    return boost::math::cdf(boost::math::chi_squared(static_cast<double>(freedom)), x);
}
// vim: set sw=4 sts=4 ts=8 noet:
