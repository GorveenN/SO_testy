#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <cassert>

uint64_t N;

extern "C" uint64_t euron(uint64_t n, char const *prog);

extern "C" uint64_t get_value(uint64_t n) {
    assert(n < N);
    return n;
}

extern "C" void put_value(uint64_t n, uint64_t v) {
    assert(n < N);
}

struct eur {
    uint64_t id;
    const char *s;
    uint64_t result;
    uint64_t expected;
};

void *test_euron(void *s) {
    eur *e = (eur *) s;
    e->result = euron(e->id, e->s);
    return NULL;
}

void test(eur *e) {
    pthread_t t[N];
    for (uint64_t i = 0; i < N; ++i) {
        pthread_create(t + i, NULL, test_euron, (void*)(e + i));
    }
    for (uint64_t i = 0; i < N; ++i) {
        pthread_join(t[i], NULL);
    }
}


int main(int argc, char *argv[]) {
    N = atoll(argv[1]);
    uint64_t test_count = 0;
    eur eurons[N];
    std::string strings[N];

    std::string input = "./tests/" + std::to_string(N) + "/in/";
    std::string output = "./tests/" + std::to_string(N) + "/out/";
    uint64_t failed_n = 0;

    for (const auto &entry : std::filesystem::directory_iterator(input)) {
        std::cout << "test: " << test_count << std::endl;
        test_count++;
        std::string tmp = entry.path().filename();
        std::string filename = tmp.substr(0, tmp.length() - 3);
        std::ifstream in, out;

        in.open(input + tmp);
        out.open(output + filename + ".out");
        for (uint64_t i = 0; i < N; i++) {
            out >> eurons[i].expected;
            in >> strings[i];
            eurons[i].s = strings[i].c_str();
            eurons[i].id = i;
        }
        test(eurons);

        for (uint64_t i = 0; i < N; i++) {
            if (eurons[i].expected != eurons[i].result) {
                failed_n++;
                break;
            }
        }
    }

    std::cout << "passed: " << test_count - failed_n << std::endl;
    std::cout << "failed: " << failed_n << std::endl;
}
