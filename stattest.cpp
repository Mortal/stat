#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "stat.h"
#include "portability.h"
#include "sysinfo.h"
#include "distributions.h"

using namespace std;

bool read_cumulative_sample_data(normal_sample & tgt, size_t i = 1, bool prompt = false) {
    size_t n;
    double sum, uss;
    if (prompt) cout << "n" << sub(i) << " = " << flush;
    if (!(cin >> n)) return false;
    if (prompt) cout << "S" << sub(i) << " = " << flush;
    if (!(cin >> sum)) return false;
    if (prompt) cout << "USS" << sub(i) << " = " << flush;
    if (!(cin >> uss)) return false;
    tgt = normal_sample(n, sum, uss);
    return true;
}

void derive(const normal_sample & xs, string index = "") {
    string mu = MU + index;
    string sum = "S" + index;
    string n = "n" + index;
    string ssd = "SSD" + index;
    string uss = "USS" + index;
    string var = "s" + index + SQ;
    string sigma = SIGMA + index + SQ;
    cout << "Mean:" <<
    endl << mu<<" <- " << sum << " / " << n << "   (3.1 p. 55)" <<
    endl << "      = " << xs.sum() << " / " << xs.n() <<
    endl << "      = " << xs.mean() << "     ~~ N(" << mu << ", " << sigma << "/n)    (3.3 p. 55)" << endl <<

    endl << "Confidence interval for mean:" <<
    endl << "C_95%(" << mu << ") = " << xs.ci() << "    (3.23 p. 63)" << endl <<

    endl << "Sum of squares of deviations:" <<
    endl << ssd <<" = " << uss << " - " << sum << SQ" / " << n << "   (3.25 p. 64)" <<
    endl << "       = " << xs.uss() << " - " << xs.sum() << SQ" / " << xs.n() << " = " << xs.ssd() << endl <<

    endl << "Variance:" <<
    endl << var<<" = 1 / (" << n << " - 1) * " << ssd << "  (3.24 p. 64)" <<
    endl << "      = " << xs.variance() << endl <<

    endl;
    xs.ci_variance();
}

void more_than_two_samples(vector<normal_sample> & samples) {
    cout << endl << string(79, '=') <<
    endl << "Testing the hypothesis of a common variance in more than two samples." <<
    endl << "H_0"SIGMASQ" : "SIGMA << sub("(1)") << SQ" = ... = "SIGMA << char_subscript('(') << sub(samples.size()) << char_subscript(')') << SQ" = "SIGMASQ << endl;

    pair<double,double> v = common_variance(samples);
    cout << "p_obs = " << fixed << setprecision(2) << v.second*100 << "% => " << ((v.second < 0.05) ? "Rejected" : "Not rejected") << '\n';
    if (v.second >= 0.05) {
	cout << SIGMASQ" <- s"SQ" = " << setprecision(5) << v.first << "\n\n";
	normal_sample zs = normal_sample::sum(samples.begin(), samples.end());
	cout << "95% confidence interval for the variance (biogeostat p. 104): " << zs.ci_variance() << '\n';
	cout << endl << string(79, '=') << endl;
	pair<double, double> m = common_mean(samples);
	cout << "p_obs = " << fixed << setprecision(2) << m.second*100 << "% => " << ((m.second < 0.05) ? "Rejected" : "Not rejected") << '\n';
	if (m.second >= 0.05) {
	    cout << MU" <- m = " << m.first << "\n\n";
	    cout << "Model M" << sub(2) << ": X_ij ~ N("MU", "SIGMASQ")" << endl;
	    cout << "Single sample data: " << setprecision(6) << zs << endl;
	    cout << "95% confidence interval for the mean (biogeostat p. 62): " << setprecision(5) << zs.ci() << '\n';
	    cout << "95% confidence interval for the variance: " << setprecision(5) << zs.ci_variance() << '\n';
	}
    }
}

void two_samples(normal_sample & xs, normal_sample & ys) {
    cout << endl << string(79, '=') <<
    endl << "Testing the hypothesis of a common variance in two samples." <<
    endl << "H_0"SIGMASQ" : "SIGMA << sub("(1)") << SQ" = "SIGMA << sub("(2)") << SQ" = "SIGMASQ << endl;

    pair<double,double> v = common_variance(xs, ys);
    cout << "p_obs = " << fixed << setprecision(2) << v.second*100 << "% => " << ((v.second < 0.05) ? "Rejected" : "Not rejected") << '\n';
    if (v.second >= 0.05) {
	cout << "Model M" << sub(1) << ": X_ij = N("MU"_i, "SIGMASQ") for i = 1, ..., n, j = 1, ..., n_i" << endl;
	cout << SIGMASQ" <- s"SQ" = " << setprecision(5) << v.first << "\n\n";
	normal_samples zs = xs+ys;
	cout << "95% confidence interval for the variance (biogeostat p. 61): " << setprecision(5) << zs.ci_variance() << '\n';
	cout << endl << string(79, '=') << endl;
	pair<double,double> m = common_mean(xs, ys);
	cout << "p_obs = " << fixed << setprecision(2) << m.second*100 << "% => " << ((m.second < 0.05) ? "REJECTED" : "NOT REJECTED") << '\n';
	if (m.second >= 0.05) {
	    cout << MU" <- m = " << m.first << "\n\n";
	    cout << "Single sample data: " << zs << endl;
	    cout << "95% confidence interval for the mean (biogeostat p. 62): " << setprecision(5) << zs.ci() << '\n';
	    cout << "95% confidence interval for the variance: " << setprecision(5) << zs.ci_variance() << '\n';
	}
    }
}

vector<normal_sample> get_observations(bool prompt) {
    if (prompt)
	cout << "Input sample observations." <<
	endl << "Separate samples with blank lines, end with " << eof_keystroke << "." <<
	endl;

    vector<normal_sample> samples;
    normal_sample xs;
    string line;
    while (getline(cin, line)) {
	stringstream ss(line);
	size_t seen = 0;
	double observation;
	while (ss >> observation) {
	    ++seen;
	    xs(observation);
	}
	if (!seen) {
	    samples.push_back(xs);
	    xs = normal_sample();
	}
    }
    if (xs.n()) samples.push_back(xs);
    return samples;
}

vector<normal_sample> get_input(bool prompt) {
    if (prompt)
	cout << "Input cumulative sample data." <<
	endl << "n is the number of observations, S is the sum of observations," <<
	endl << "USS is the sum of the squares of the observations." <<
	endl << "End with " << eof_keystroke << '.' <<
	endl;

    vector<normal_sample> samples;
    normal_sample xs;
    size_t i = 1;
    while (read_cumulative_sample_data(xs, i, prompt)) {
	samples.push_back(xs);
	++i;
    }
    return samples;
}

void go(vector<normal_sample> samples) {
    size_t k = samples.size();
    for (size_t i = 0; i < k; ++i) {
	stringstream ss;
	ss << char_subscript('(') << sub(1+i) << char_subscript(')');
	derive(samples[i], ss.str());
	cout << endl;
    }
    cout << "Model M" << sub(0) << ": X_ij = N("MU"_i, "SIGMA"_i"SQ") for i = 1, ..., " << k << ", j = 1, ..., n_i" << endl;
    if (samples.size() < 2) return;
    if (samples.size() == 2) return two_samples(samples[0], samples[1]);
    return more_than_two_samples(samples);
}

void go_variance(bool prompt) {
    double var_x, var_y;
    size_t freedom_x, freedom_y;

    if (prompt)
	cout << "Test common variance in two samples." <<
	endl << "First sample." <<
	endl << 's' << SQ << " = " << flush;
    if (!(cin >> var_x)) return;

    if (prompt)
	cout << "f = " << flush;
    if (!(cin >> freedom_x)) return;

    if (prompt)
	cout << "Second sample." <<
	endl << 's' << SQ << " = " << flush;
    if (!(cin >> var_y)) return;

    if (prompt)
	cout << "f = " << flush;
    if (!(cin >> freedom_y)) return;

    double p_obs = common_variance(var_x, var_y, freedom_x, freedom_y);
    cout << "p_obs = " << p_obs << endl;
}

struct operation {
    operation() : observations(false), variance(false), calculate(false), linear_parameter_ci(false) {}
    bool observations;
    bool variance;
    bool calculate;
    bool linear_parameter_ci;
};

operation get_operation(int argc, char ** argv) {
    operation result;
    if (argc > 1) {
	string arg(argv[1]);
	if (arg == "--obs")
	    result.observations = true;
	else if (arg == "--variance")
	    result.variance = true;
    } else if (is_interactive()) {
	cout << endl << "Select mode of operation:" <<
	endl << "  1. Test common variance and mean in two or more samples" <<
	endl << "     given standard calculations." <<
	endl << "  2. Test common variance and mean in two or more samples" <<
	endl << "     given observations." <<
	endl << "  3. Test common variance in two samples" <<
	endl << "     given the observed variance and number of degrees of freedom." <<
	endl << "  4. Lookup distribution fractiles and CDF." << endl <<
	endl << "  5. Confidence interval for parameters in the linear regression model" <<
	endl << "     (biogeostat p. 130 and p. 145)." << endl <<
	endl << "Choose one [12345]: " << flush;
	size_t modus = 1;
	string line;
	if (getline(cin, line)) {
	    stringstream ss(line);
	    ss >> modus;
	    if (modus < 1 || modus > 5) {
		cout << "Modus out of bounds";
		modus = 1;
	    }
	}
	switch (modus) {
	    case 2:
		result.observations = true;
		break;
	    case 3:
		result.variance = true;
		break;
	    case 4:
		result.calculate = true;
		break;
	    case 5:
		result.linear_parameter_ci = true;
	}
    }
    return result;
}

void go_calculate() {
    while (true) {
	cout << "Choose a function." <<
	endl << "  1. Quantile of the Student's t-distribution" <<
	endl << "  2. Quantile of the Chi squared distribution" <<
	endl << "  3. Cumulative distribution function of the Fisher's F-distribution" <<
	endl << "  4. Cumulative distribution function of the Student's t-distribution" <<
	endl << "  5. Cumulative distribution function of the Chi squared distribution" << endl <<
	endl << "Choose one [12345]: " << flush;
	size_t choice;
	if (!(cin >> choice)) return;
	size_t f1, f2;
	if (choice == 3) {
	    cout << "Number of degrees of freedom in the numerator: " << flush;
	    if (!(cin >> f1)) return;
	    cout << "Number of degrees of freedom in the denominator: " << flush;
	    if (!(cin >> f2)) return;
	} else {
	    cout << "Number of degrees of freedom: " << flush;
	    if (!(cin >> f1)) return;
	}
	double x;
	cout << "Input: " << flush;
	if (!(cin >> x)) return;
	double result;
	switch (choice) {
	    case 1:
		result = quantile_students_t(f1, x);
		break;
	    case 2:
		result = quantile_chi_squared(f1, x);
		break;
	    case 3:
		result = cdf_fisher_f(f1, f2, x);
		break;
	    case 4:
		result = cdf_students_t(f1, x);
		break;
	    case 5:
		result = cdf_chi_squared(f1, x);
		break;
	    default:
		return;
	}
	cout << fixed << setprecision(7) << result << endl;
    }
}

void go_linear_parameter_ci() {
    double param;
    double stderror;
    size_t f;
    cout << "Parameter estimate: " << flush;
    if (!(cin >> param)) return;
    cout << "Standard error: " << flush;
    if (!(cin >> stderror)) return;
    cout << "Degrees of freedom: " << flush;
    if (!(cin >> f)) return;
    double d = stderror * quantile_students_t(f, 0.975);
    cout << "The limits of the confidence interval are" <<
    endl << "    Estimate +- Standard Error * t_(0.975)(f)" <<
    endl << "  = " << param << " +- " << stderror << " * " << quantile_students_t(f, 0.975) <<
    endl << "In other words, the confidence interval is" <<
    endl << "    [" << param-d << ", " << param+d << "]" << endl;
}

int main(int argc, char ** argv) {
    cout << "This is stattest " << git_refspec << "\n(commit " << git_commit << ")" << endl << endl;
    operation o = get_operation(argc, argv);

    if (o.variance)
	go_variance(is_interactive());
    else if (o.calculate)
	go_calculate();
    else if (o.linear_parameter_ci)
	go_linear_parameter_ci();
    else
	go(o.observations ? get_observations(is_interactive()) : get_input(is_interactive()));

    display_results();
    return 0;
}
// vim: set sw=4 sts=4 ts=8 noet:
