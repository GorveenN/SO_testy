#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <cassert>
#include <future>
#include <thread>

uint64_t N;


namespace fs = std::filesystem;

extern "C" uint64_t euron(uint64_t n, char const *prog);

extern "C" uint64_t get_value(uint64_t n) {
    assert(n < N);
    return n;
}

extern "C" void put_value(uint64_t n, uint64_t v) {
    assert(n < N);
}

void test(uint64_t id, const char * com, std::promise<uint64_t> && prom) {
    prom.set_value(euron(id, com));
}


int main(int argc, char *argv[]) {
    N = atoll(argv[1]);
    uint64_t test_count = 0;

    const char * strings[N];
    uint64_t results[N];

    std::string input = "./tests/" + std::to_string(N) + "/in/";
    std::string output = "./tests/" + std::to_string(N) + "/out/";

    std::vector<std::string> failed_name;
    uint64_t failed_n = 0;

    for (const auto & entry : fs::directory_iterator(input)) {
        test_count++;
        std::string tmp = entry.path().filename();
        std::string filename = tmp.substr(0, tmp.length() - 3);
        std::ifstream in, out;

        std::promise<uint64_t> promises[N];
        std::future<uint64_t> futures[N];
        std::thread threads[N];

        in.open (input + tmp);
        out.open (output + filename + ".out");
        for (uint64_t i = 0; i < N; i++) {
            std::string command;

            out >> results[i];
            in >> command;
            futures[i] = promises[i].get_future();
            strings[i] = command.c_str();

            threads[i] = std::thread(&test, i, strings[i], std::move(promises[i]));
            threads[i].join();
        }


        size_t i;
        for (i = 0; i < N; i++) {
            if (results[i] != futures[i].get()) {
                failed_name.push_back(filename);
                failed_n++;
                break;
            }
        }
    }

    std::cout << "passed: " << test_count - failed_n << std::endl;
    std::cout << "failed: " << failed_n << std::endl;
    if (failed_n > 0) {
        for (std::string& s : failed_name) {
            std::cout << s << std::endl;
        }
    }
}