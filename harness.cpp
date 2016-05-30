#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <string>
#include <vector>
#include <queue>
#include <deque>
#include <map>
#include <set>
#include <bitset>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <cassert>
#include <fstream>
#include <future>
#include <thread>
#include <chrono>
#include <iomanip>
#include <numeric>
#include <thread>
#include <atomic>
#include <random>

using namespace std;
typedef int64_t int64;
#define be(coll) std::begin(coll), std::end(coll)


// functional form of next value in a stream
template<typename T>
T nextValue(std::istream &in) {
    T i;
    in >> i;
    return i;
}

// reads a vector (of integral values) from an input stream
template<typename T>
std::vector<T> readVector(std::istream &in, int n) {
    std::vector<T> c;
    std::generate_n(std::back_inserter(c), n, [&in]() { return nextValue<T>(in); });
    return c;
}

std::chrono::time_point<std::chrono::system_clock> now() { return std::chrono::system_clock::now(); }

double since(const std::chrono::time_point<std::chrono::system_clock> &t0) {
    return std::chrono::duration<double>(std::chrono::system_clock::now() - t0).count();
}


// base class: override with a specific problem
class SolverBase {
    std::ostringstream out_;
public:
    virtual void read(std::istream &in) = 0;    // read input for a single test case

    virtual void solve() { out_.str(""); saw(out_); }     // solve a single test case

    virtual void write(std::ostream &out) { out << out_.str(); }    // write output (after "Case: #N: ")

    string str() { return out_.str(); }

    operator string() { return out_.str(); }

protected:
    virtual void saw(std::ostream &out) = 0;    // solve and write
};


class Solver : public SolverBase {
    // fill in member variables

public:
    virtual void read(std::istream &in);

protected:
    virtual void saw(std::ostream &out);
};


void Solver::read(std::istream &in) {
    // read problem data
    // in >> n >> a >> b;
    // v = readVector<int>(in, n);
}


void Solver::saw(std::ostream &out) {
    // solution code
    // out << answer;
}


int main() {
    // comment these out to use console i/o
    freopen("A.in.txt", "r", stdin);
    freopen("A.out.txt", "w", stdout);

    auto t00 = std::chrono::system_clock::now();

    int nTestCases ;
    cin >> nTestCases;

    // number of processors
    const int nThreads = min(1, min(max(int(thread::hardware_concurrency()), 1), nTestCases));

    if (nThreads <= 1) {
        // sequential processing: less memory and useful for debugging
        Solver s;
        for (int it = 0; it < nTestCases; ++it) {
            auto t0 = now();
            s.read(cin);
            s.solve();
            double t = since(t0);
            cout << "Case #" << (it + 1) << ": " << s.str() << endl;
            if (t > 0.01)
                fprintf(stderr, "%3d : %3d / %d = %.2f | %.2f\n", it + 1, int(it + 1), nTestCases, t,
                        since(t00) / (it+1) * nTestCases);
        }
    }
    else {
        // parallel processing with simple thread pool
        vector<Solver> s(nTestCases);
        // read all input sequentially
        for (int it = 0; it < nTestCases; ++it) {
            s[it].read(cin);
        }

        atomic<int> next(0);    // the next case that needs a worker thread
        atomic<int> done(0);    // only used for progress reporting

        auto work = [&next, &done, &s, &nTestCases, t00](void) {
            while (true) {
                int i = next++; // this atomic operation is how the threads synchronize with each other
                if (i >= nTestCases)
                    return;
                //fprintf(stderr, "start %3d\n", i + 1);
                auto t0 = now();
                s[i].solve();
                double t = since(t0);
                done++;
                if (t > 0.01)
                    fprintf(stderr, "%3d : %3d / %d = %.2f | %.2f\n", i + 1, int(done), nTestCases, t,
                            since(t00) / done * nTestCases);
            }
        };
        vector<thread> workers;
        // start worker threads
        for (int i = 0; i < nThreads; ++i) {
            workers.push_back(thread(work));
        }
        // wait for all workers to complete
        for (int i = 0; i < nThreads; ++i) {
            workers[i].join();
        }
        // report results
        for (int it = 0; it < nTestCases; ++it) {
            cout << "Case #" << (it + 1) << ": " << s[it].str() << endl;
        }
    }
    fprintf(stderr, "%.2f\n", since(t00));
    return 0;
}
