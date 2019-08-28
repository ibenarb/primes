
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <vector>
//#include <list>
#include <future>
//#include <thread>
#include "time.h"
using namespace std;

clock_t t0, t1;

vector<uint32_t> primes_v;

typedef vector<uint16_t> step_t;

step_t step_v, mstp_v[8];

const uint32_t knownprime[13] = { 1,2,3,5,7,11,13,17,19,23,29,31,37 };
const uint64_t cyclelength[16] = { 1, 1, 2, 8, 48, 480, 5760, 92160, 1658880, 36495360, 1021870080, 30656102400, 1103619686400, 44144787456000, 1854081073152000, 85287729364992000 };

uint32_t cycle;

string lapsed_time(clock_t t0) {
	uint32_t days, hrs, mins, secs, clks;
	string is;
	char cbff[30];
	clock_t t;

	string ts;
	t = clock() - t0;
	days = t / (86400 * CLOCKS_PER_SEC);
	hrs = (t % (86400 * CLOCKS_PER_SEC)) / (3600 * CLOCKS_PER_SEC);
	mins = (t % (3600 * CLOCKS_PER_SEC)) / (60 * CLOCKS_PER_SEC);
	secs = (t % (60 * CLOCKS_PER_SEC)) / CLOCKS_PER_SEC;
	clks = t % CLOCKS_PER_SEC;
	if (days > 0)sprintf_s(cbff, "%03d - %02d:%02d:%02d.%03d", days, hrs, mins, secs, clks);
	else sprintf_s(cbff, "%02d:%02d:%02d.%03d", hrs, mins, secs, clks);
	is = cbff;
	return(is);
}

void show_lapsed_time(clock_t t0, clock_t t1) {
	uint32_t days, hrs, mins, secs, clks;
	clock_t t;
	t = t1 - t0;
	days = t / (86400 * CLOCKS_PER_SEC);
	hrs = (t % (86400 * CLOCKS_PER_SEC)) / (3600 * CLOCKS_PER_SEC);
	mins = (t % (3600 * CLOCKS_PER_SEC)) / (60 * CLOCKS_PER_SEC);
	secs = (t % (60 * CLOCKS_PER_SEC)) / CLOCKS_PER_SEC;
	clks = t % CLOCKS_PER_SEC;
	if (days > 0) printf("%03d - %02d:%02d:%02d.%03d", days, hrs, mins, secs, clks);
	else printf("%02d:%02d:%02d.%03d", hrs, mins, secs, clks);
	cout << "\n";
}

string zstr(uint64_t z) {
	stringstream istr;
	string is, zs;
	int i, len;

	istr << z;
	is = istr.str();
	len = is.length();
	for (i = 0; i < len; ++i) {
		zs += is[i];
		if ((len + 2 - i) % 3 == 0 and i + 3 < len) zs += ".";
	}
	return(zs);
}

void primestxt() {
	string line;
	uint32_t i;
	uint32_t prime;
	ifstream primestxtf;
	ofstream primesbinf;

	primestxtf.open("primenumbers.txt");
	primesbinf.open("primenumbers.bin", ios::out | ios::binary);
	if (primestxtf.is_open() and primesbinf.is_open()) {
		cout << "\n\n loading and converting prime numbers from text to binary";
		t0 = clock();
		i = 0;
		primes_v.clear();
		while (getline(primestxtf, line)) {
			++i;
			prime = stoul(line, nullptr, 0);
			primesbinf.write((char*)& prime, sizeof(prime));
			primes_v.push_back(prime);
			//cout << "\n " << setw(3) << i << setw(5) << line << setw(5) << prime;
		}
		primestxtf.close();
		primesbinf.close();
		cout << "\n finished loading " << zstr(i) << " prime numbers in ";
		show_lapsed_time(t0, clock());
		cout << "\n\n";
	}
	else cout << "either file not open\n";
}

void primesbin() {
	char* buffer;
	uint32_t prime;
	uint64_t filesize, i, n, varsize = sizeof(prime);
	ifstream primesbinf;

	t0 = clock();
	i = 0;
	primes_v.clear();
	primesbinf.open("primenumbers.bin", ios::in | ios::binary | ios::ate);
	if (primesbinf.is_open()) {
		cout << "\n\n loading prime numbers from binary";
		filesize = primesbinf.tellg();
		cout << "\n filesize: " << zstr(filesize) << " varsize: " << zstr(varsize);
		primesbinf.seekg(0, ios::beg);
		buffer = new char[filesize];
		primesbinf.read(buffer, filesize);
		primesbinf.close();
		n = filesize / varsize;
		cout << " number n: " << zstr(n);
		for (i = 0; i < n; ++i) {
			memcpy(&prime, &buffer[varsize * i], varsize);
			primes_v.push_back(prime);
			//cout << "\n " << setw(3) << i << setw(5) << prime;
		}
		primesbinf.close();
		cout << "\n finished loading " << zstr(i) << " prime numbers in ";
		show_lapsed_time(t0, clock());
		cout << "\n\n";
	}
	else cout << " file not open\n";
}

void make_steps(uint32_t n) {
	uint64_t last;
	bool relprime;
	uint32_t cycle, i, p, product;
	uint16_t stp = 0, maxstp = 0;
	clock_t t0;
	t0 = clock();
	cout << "\n repeat " << setw(4) << n;
	step_v.clear();
	step_v.push_back(0);
	cycle = 0;
	last = 1;
	product = 1;
	for (p = 1; p <= n; ++p) product *= knownprime[p];
	cout << " product " << setw(14) << zstr(product);
	for (i = 2; i <= product + 1; ++i) {
		relprime = 1;
		for (p = 1; p <= n; ++p) {
			relprime = relprime && (i % knownprime[p]);
		}
		if (relprime) {
			stp = i - last;
			step_v.push_back(stp);
			if (stp > maxstp) maxstp = stp;
			last = i;
			++cycle;
		}
	}
	step_v[0] = 0;
	cout << " done in " << lapsed_time(t0) << " cycle = " << zstr(cycle) << " max step " << setw(4) << zstr(maxstp) << "\n";
	if (cycle < 500) for (auto s : step_v) cout << setw(3) << s;
	//else cout << " exceeding cycle length, numbers not shown ";
	//cout << "\n";

}

void make_msteps(int np, int nt) { // np : number of known primes to be multiplied, nt : number of threads
	vector<uint32_t> cand_v;
	bool relprime;
	uint32_t i, p, product, t;
	uint32_t mstp = 0, step = 0;
	clock_t t0;

	t0 = clock();
	//cout << "\n repeat " << setw(4) << np;
	cand_v.clear();
	cand_v.push_back(1);
	cycle = 0;
	product = 1;
	for (p = 1; p <= np; ++p) product *= knownprime[p];
	for (i = 2; i <= product + 1; ++i) {
		relprime = 1;
		for (p = 1; p <= np; ++p) {
			relprime = relprime && (i % knownprime[p]);
		}
		if (relprime) {
			cand_v.push_back(i);
			++cycle;
		}
	}
	//cout << " product " << setw(14) << zstr(product) << " cycle " << setw(14) << zstr(cycle) << " last cand " << setw(14) << zstr(cand_v[cycle-1]);
	for (i = 0; i <= cycle; i++) {
		for (t = 0; t < nt; t++) {
			if (i == 0) {
				mstp_v[t].clear();
				mstp_v[t].push_back(cand_v[t]);
			}
			else {
				mstp = product * ((nt * i + t) / cycle - (nt * (i - 1) + t) / cycle) + cand_v[(nt * i + t) % cycle] - cand_v[(nt * (i - 1) + t) % cycle];
				mstp_v[t].push_back(mstp);
			}
		}
	}
	for (i = 0; i <= cycle; i++) {
		if (i == 0) {
			step_v.clear();
			step_v.push_back(cand_v[0]);
		}
		else { // t = 0, nt = 1
			step = product * (i / cycle - (i - 1) / cycle) + cand_v[i % cycle] - cand_v[(i - 1) % cycle];
			step_v.push_back(step);
		}
	}
	//cout<< " in " << lapsed_time(t0) << "\n\n";
	/*
	for (i = 0; i <= cycle; ++i) {
		cout << "\n " << setw(5) << i << ": ";
		for (t = 0; t < nt; ++t) cout << setw(5) << mstp_v[t][i];
		cout << setw(5) << step_v[i];
	}*/
}

void make_primes(uint32_t n, int np, int nt) {
	uint32_t divisor, di, candidate, ci, i, pn, start = knownprime[np + 1];
	bool divides;
	clock_t t0 = clock();

	make_msteps(np, nt);
	//cout << "\n\n np : " << np << " cycle of " << setw(14) << zstr(cycle) << " searching primes up to " << setw(14) << zstr(n) << "\n";
	primes_v.clear();
	for (i = 1; i <= np; ++i)
		primes_v.push_back(knownprime[i]);
	pn = np;
	candidate = start;
	ci = 2;
	while (candidate <= n) {
		divides = 0;
		//cout << "\n ci"<<setw(5)<<ci<<" step[ci] "<<setw(5)<<step_v[ci]<<" candidate : " << setw(5) << candidate;
		di = 2;
		divisor = start;
		//cout << " divisor " << setw(5) << divisor;
		while (divisor <= sqrt(candidate)) {
			//cout <<"."<< setw(5) << divisor;
			divides = (candidate % divisor == 0);
			if (divides) break;
			if (di > cycle) di = 1;
			divisor += step_v[di];
			++di;
		}
		if (!divides) {
			++pn;
			primes_v.push_back(candidate);
			//cout << " -- add " << candidate;
		}
		candidate += step_v[ci];
		//cout << " new candidate " << setw(5) << candidate;
		++ci;
		if (ci > cycle) ci = 1;
		//cout << " new ci  " << setw(5) << ci;
	}
	cout << "\n " << setw(15) << zstr(n) << setw(4) << np << setw(4) << nt << " finished with " << setw(14) << zstr(pn) << " prime numbers found, last one is ";
	cout << setw(14) << zstr(primes_v[pn - 1]) << " in " << lapsed_time(t0) << "\n\n";
	if (pn < 10000) {
		for (auto s : primes_v) cout << setw(5) << s;
		cout << "\n\n";
	}
}

void loop() {
	//for (int i = 2; i < 10; ++i) make_primes(pow(2,24)-1, i, 1);
	for (int i = 9; i > 6; --i) make_primes(pow(2, 31), i, 1);
	cout << "\n\n";
}

int main() {
	cout << "\n\n " << zstr(step_v.max_size());
	for (int i = 0; i < 8; ++i) cout << setw(15) << zstr(mstp_v[i].max_size());
	cout << "\n ";
	loop();
	return(0);
}