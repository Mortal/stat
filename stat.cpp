#include <sstream>
#include <limits>
#include <iostream>
#include <iomanip>

#include "stat.h"
#include "distributions.h"

std::string long_subscript(size_t i) {
    std::stringstream ss;
#ifndef ASCII
    while (i) {
	ss << sub(i % 10);
    }
#else
    ss << i;
#endif
    return ss.str();
}

std::string sub(std::string s) {
#ifndef ASCII
    std::stringstream ss;
    for (size_t i = 0; i < s.size(); ++i) {
	ss << char_subscript(s[i]);
    }
    return ss.str();
#else
    return '_' + s;
#endif
}

// Confidence interval for the mean. biogeostat p. 61
ci_t normal_sample::ci(double alpha) const {
    if (freedom() < 2) return std::make_pair(-std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity());
    double T = quantile_students_t(freedom(), 1 - alpha / 2);
    double w = T * stddev() / sqrt(static_cast<double>(n()));
    return std::make_pair(mean() - w, mean() + w);
}

// Confidence interval for the variance. biogeostat p. 62
ci_t normal_sample::ci_variance(double alpha, bool loud) const {
    double left_quantile = quantile_chi_squared(freedom(), 1-alpha/2);
    double right_quantile = quantile_chi_squared(freedom(), alpha/2);
    double nominator = freedom()*variance();
    double lhs = freedom()*variance()/left_quantile;
    double rhs = freedom()*variance()/right_quantile;
    std::cout << "Confidence interval for the variance:" <<
    std::endl << "C_" << F_PERCENTAGE(100*(1-alpha)) << "%(" << SIGMASQ << ") = [fs_(1)^2 / chi^2_0.975(f), fs_(1)^2 / chi^2_0.025(f)]   (3.15 p. 61)" <<
    std::endl << "= [" << F_FREEDOM(freedom()) << "*" << F_VARIANCE(variance()) << "/" << F_CHI_SQ(left_quantile) << "," <<
    F_FREEDOM(freedom()) << "*" << F_VARIANCE(variance()) << "/" << F_CHI_SQ(right_quantile) << "]" <<
    std::endl << "= [" << F_VARIANCE(lhs) << ", " << F_VARIANCE(rhs) << "]" << std::endl;
    return std::make_pair(lhs, rhs);
}

double common_variance(double var_x, double var_y, size_t freedom_x, size_t freedom_y, bool loud) {
    if (var_x < var_y) return common_variance(var_y, var_x, freedom_y, freedom_x, loud);
    double F = var_x / var_y;
    if (loud)
	std::cout << "Test of common variance: H_0" << SIGMASQ << ": " << SIGMASQ << sub(1) << " = " << SIGMASQ << sub(2) <<
	std::endl << "F = s" << sub(1) << SQ << "/s" << sub(2) << SQ << " (biogeostat p. 87)" <<
	std::endl << "  = " << F_VARIANCE(var_x) << "/" << F_VARIANCE(var_y) << " = " << F_F_STAT(F) <<
	std::endl;
    if (loud) std::cout << "Test statistic: F = " << F_F_STAT(F) << std::endl;
    double cdf = 1-cdf_fisher_f(freedom_x, freedom_y, F);
    double p_obs = 2*cdf;
    if (loud) std::cout << "p_obs = 2*(1-F_(F(" << F_FREEDOM(freedom_x) << ", " << F_FREEDOM(freedom_y) << ")) (F)) = " << F_P_OBS(p_obs) << std::endl;
    return p_obs;
}

std::pair<double, double> common_mean(const normal_sample & first, const normal_sample & second, bool loud) {
    if (first.mean() < second.mean()) return common_mean(second, first);
    double variance = common_variance(first, second, false).first;
    double t = (first.mean() - second.mean())/sqrt(variance*(1.0/first.n() + 1.0/second.n()));
    double p_obs = 2*(1-cdf_students_t(first.n()+second.n()-2, t));
    double mean = (first.mean()*first.n()+second.mean()*second.n())/(first.n()+second.n());
    if (loud)
	std::cout << "Test of common mean: H_0" << MU << ": " << MU << sub(1) << " = " << MU << sub(2) <<
	std::endl << "t(x) = (" << MU << sub(1) << " - " << MU << sub(2) << ")/sqrt(s" << SQ << "*(1/n" << sub(1) << " + 1/n" << sub(2) << "))" <<
	std::endl << "     = (" << F_SAMPLE(first.mean()) << " - " << F_SAMPLE(second.mean()) << ")/sqrt(" << F_VARIANCE(variance) << "*(1/" << F_N(first.n()) << " + 1/" << F_N(second.n()) << "))" <<
	std::endl << "     = " << F_T(t) << " ~~ t(f1) = t(" << first.n()+second.n()-2 << ")" <<
	std::endl << "p_obs = " << F_P_OBS(p_obs) <<
	std::endl << "(biogeostat p. 83)" << std::endl;
    return std::make_pair(mean, p_obs);
}

// returns <variance, p_obs>
std::pair<double, double> common_variance(const std::vector<normal_sample> & samples, bool loud) {
    const size_t k = samples.size();
    size_t f1 = 0;
    double ssd1 = 0.0;
    double C = 0.0;
    double teststatsum = 0.0;
    for (size_t i = 0; i < k; ++i) {
	ssd1 += samples[i].ssd();
	f1 += samples[i].freedom();
	C += 1.0/samples[i].freedom();
	teststatsum += log(samples[i].variance()) * samples[i].freedom();
    }
    C -= 1.0/f1;
    C *= 1.0/(3*(k-1));
    C += 1.0;

    const double ss1 = ssd1 / f1;
    const double test_statistic = f1 * log(ss1) - teststatsum;
    const double Ba = test_statistic / C;

    const double p_obs = 1-cdf_chi_squared(k-1, Ba);

    if (loud) std::cout << "s" << sub(1) << SQ << " = SSD" << sub(1) << " / f" << sub(1) << " = " << ss1 << std::endl
	   << std::endl << "-2 ln Q(x) = f" << sub(1) << " ln(s" << sub(1) << SQ << ") - sum_(i=1)^k f_(i) ln s_(i)" << SQ
	   << std::endl << "           = " << f1 << " ln(" << F_SAMPLE_SQ(ss1) << ") - sum_(i=1)^" << k << " f_(i) ln s_(i)" << SQ
	   << std::endl << "           = " << F_TEST_STAT(test_statistic) << std::endl
	   << std::endl << "C = 1 + 1/(3*(k-1))*[(sum_(i=1)^k 1/f_(i)) - 1/f" << sub(1) << "]"
	   << std::endl << "  = 1 + 1/" << (3*k-1) << "*[(sum_(i=1)^" << k << " 1/f_(i)) - 1/" << f1 << "]"
	   << std::endl << "  = " << F_TEST_STAT(C) << std::endl
	   << std::endl << "Ba = -2 ln Q(x) / C = " << F_TEST_STAT(Ba) << std::endl
	   << std::endl << "p_obs = 1 - F_(" << CHISQ << "(k-1))(Ba) = " << F_P_OBS(p_obs) << std::endl << std::endl;

    return std::make_pair(ss1, p_obs);
}

std::pair<double, double> common_mean(const std::vector<normal_sample> & samples, bool loud) {
    std::pair<double, double> vartest = common_variance(samples, false);
    const double ss1 = vartest.first;

    normal_sample sum = samples[0];
    const size_t k = samples.size();
    for (size_t i = 1; i < k; ++i) {
	sum = sum + samples[i];
    }
    double ssd2 = -(sum.sum()*sum.sum())/sum.n();
    for (size_t i = 0; i < k; ++i) {
	ssd2 += samples[i].sum()*samples[i].sum() / samples[i].n();
    }
    const double ss2 = ssd2 / (k-1);
    const double F = ss2 / ss1;
    //const double Q = pow(1.0/(1.0 + ssd2 / sum.ssd()), sum.n()/2.0);
    const double p_obs = 1-cdf_fisher_f(k-1, sum.n()-k, F);

    if (loud) 
	std::cout << "Testing the hypothesis of a common mean in more than two samples." <<
	std::endl << "H_0" << MU << " : " << MU << sub("(1)") << " = ... = " << MU << "_(n) = " << MU << std::endl <<
	std::endl << "SSD" << sub(2) << " = -S." << SQ << "/n. + sum_(i=1)^k S_i" << SQ << " / n_i = " << ssd2 <<
	std::endl << "s" << sub(2) << SQ << " = SSD" << sub(2) << " / (k - 1) = " << ss2 <<
	std::endl << "F(x) = s" << sub(2) << SQ << " / s" << sub(1) << SQ << " = " << F_TEST_STAT(F) << " ~~ F(k-1, n.-k)" <<
	//std::endl << "(p.102) Q(x) = [1/(1 + SSD_2/SSD_1)]^(n/2) = " << Q <<
	std::endl << "p_obs(x) = 1-F_(F(k-1, n. - k))(F(x)) = " << F_P_OBS(p_obs) << std::endl;

    return std::make_pair(sum.mean(), p_obs);
}
// vim: set sw=4 sts=4 ts=8 noet:
