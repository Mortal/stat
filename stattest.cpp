#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "stat.h"
#include "portability.h"
#include "sysinfo.h"

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
    cout << "Mean:         " << mu << " = " << sum << " / " << n << " = " << xs.sum() << " / " << xs.n() << " = " << xs.mean() << " ~~ N(" << mu << ", " << sigma << "/n)" <<
    endl << "CI for mean: " << xs.ci() <<
    endl << "             " << ssd << " = " << uss << " - " << sum << SQ" / " << n << " = " << xs.uss() << " - " << xs.sum() << SQ" / " << xs.n() << " = " << xs.ssd() <<
    endl << "Variance:    " << var << " = 1 / (" << n << " - 1) * " << ssd << " = " << xs.variance() <<
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
	pair<double,double> m = common_mean(xs, ys);
	cout << endl << string(79, '=') <<
	endl << "Test of common mean:\n" <<
	endl << "p_obs = " << fixed << setprecision(2) << m.second*100 << "% => " << ((m.second < 0.05) ? "REJECTED" : "NOT REJECTED") << '\n';
	if (m.second >= 0.05) {
	    cout << MU" <- m = " << m.first << "\n\n";
	    cout << "Single sample data: " << zs << endl;
	    cout << "95% confidence interval for the mean (biogeostat p. 62): " << setprecision(5) << zs.ci() << '\n';
	}
    }
}

vector<normal_sample> get_observations() {
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

vector<normal_sample> get_input() {
    cout << "Input cumulative sample data." <<
    endl << "n is the number of observations, S is the sum of observations," <<
    endl << "USS is the sum of the squares of the observations." <<
    endl << "End with " << eof_keystroke << '.' <<
    endl;

    vector<normal_sample> samples;
    normal_sample xs;
    size_t i = 1;
    bool prompt = is_interactive();
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

void go_variance() {
    double var_x, var_y;
    size_t freedom_x, freedom_y;
    cout << "Test common variance in two samples." <<
    endl << "First sample." <<
    endl << 's' << SQ << " = " << flush;
    if (!(cin >> var_x)) return;
    cout << "f = " << flush;
    if (!(cin >> freedom_x)) return;
    cout << "Second sample." <<
    endl << 's' << SQ << " = " << flush;
    if (!(cin >> var_y)) return;
    cout << "f = " << flush;
    if (!(cin >> freedom_y)) return;
    double p_obs = common_variance(var_x, var_y, freedom_x, freedom_y);
    cout << "p_obs = " << p_obs << endl;
}

int main(int argc, char ** argv) {
    cout << "This is stattest built from " << git_refspec << " (commit " << git_commit << ")" << endl << endl;
    bool observations = false;
    bool variance = false;
    if (argc > 1) {
	string arg(argv[1]);
	if (arg == "--obs")
	    observations = true;
	else if (arg == "--variance")
	    variance = true;
    }

    if (variance)
	go_variance();
    else
	go(observations ? get_observations() : get_input());

    display_results();
    return 0;
}
// vim: set sw=4 sts=4 ts=8 noet:
