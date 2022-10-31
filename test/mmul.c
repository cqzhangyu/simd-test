#include "util.h"
#include "conf.h"

#include <malloc.h>
#include <x86intrin.h>

int *ma;
int *mb;
int *mc;

static inline void
init_matrix() {
    int n = conf.n;
    for (int i = 0; i < n; i ++) {
        for (int j = 0; j < n; j ++) {
            ma[i*n+j] = 1;
            mb[i*n+j] = 1;
        }
    }
    memset(mc, 0, conf.n*conf.n * sizeof(int));
}

static inline void
verifty_result() {
    int n = conf.n;
    for (int i = 0; i < n; i ++) {
        for (int j = 0; j < n; j ++) {
            int expected = n * conf.epoch;
            if (mc[i*n+j] != expected) {
                LOG_ERRO("result mismatch on (%d,%d)\n", i, j);
                LOG_ERRO("      expected : %d, found : %d\n", expected, mc[i*n+j]);
                exit(1);
            }
        }
    }
}

static inline void
mmul_simple() {
    int n = conf.n;
    for (int ep = 0; ep < conf.epoch; ep ++) {
        for (int i = 0; i < n; i ++) {
            for (int j = 0; j < n; j ++) {
                for (int k = 0; k < n; k ++) {
                    mc[i*n+j] += ma[i*n+k]*mb[k*n+j];
                }
            }
        } 
    }
}

static inline void
mmul_swap() {
    int n = conf.n;
    for (int ep = 0; ep < conf.epoch; ep ++) {
        for (int i = 0; i < n; i ++) {
            for (int k = 0; k < n; k ++) {
                for (int j = 0; j < n; j ++) {
                    mc[i*n+j] += ma[i*n+k]*mb[k*n+j];
                }
            }
        }
    }
}

static inline void
mmul_avx() {
    int n = conf.n;
    const int vec_size = 16;
    for (int ep = 0; ep < conf.epoch; ep ++) {
        for (int i = 0; i < n; i ++) {
            for (int k = 0; k < n; k ++) {
                __m512i va = _mm512_set1_epi32(ma[i*n+k]);
                for (int j = 0; j < n/vec_size; j ++) {
                    __m512i vb = _mm512_loadu_si512(&mb[k*n+j*vec_size]);
                    __m512i vc = _mm512_loadu_si512(&mc[i*n+j*vec_size]);
                    __m512i vt1 = _mm512_mullo_epi32(va, vb);
                    __m512i vt2 = _mm512_add_epi32(vc, vt1);
                    _mm512_storeu_si512((void *)&mc[i*n+j*vec_size], vt2);
                }
                for (int j = n&(~(vec_size - 1)); j < n; j ++) {
                    mc[i*n+j] += ma[i*n+k]*mb[k*n+j];
                }
            }
        }
    }
}

static inline void
mmul_avx_aligned() {
    int n = conf.n;
    const int vec_size = 16;

    for (int ep = 0; ep < conf.epoch; ep ++) {
        for (int i = 0; i < n; i ++) {
            for (int k = 0; k < n; k ++) {
                __m512i va = _mm512_set1_epi32(ma[i*n+k]);
                for (int j = 0; j < n/vec_size; j ++) {
                    __m512i vb = _mm512_load_si512(&mb[k*n+j*vec_size]);
                    __m512i vc = _mm512_load_si512(&mc[i*n+j*vec_size]);
                    __m512i vt1 = _mm512_mullo_epi32(va, vb);
                    __m512i vt2 = _mm512_add_epi32(vc, vt1);
                    _mm512_store_si512((void *)&mc[i*n+j*vec_size], vt2);
                }
            }
        }
    }
}

int
main(int argc, char **argv) {
    int n;
    int epoch;
    struct timeval begin = {0}, end = {0};

    parse_conf(argc, argv);
    n = conf.n;
    epoch = conf.epoch;

    // ma = (int *)malloc(n*n * sizeof(int));
    // mb = (int *)malloc(n*n * sizeof(int));
    // mc = (int *)malloc(n*n * sizeof(int));
    ma = (int*)aligned_alloc(64, n*n * sizeof(int));
    mb = (int*)aligned_alloc(64, n*n * sizeof(int));
    mc = (int*)aligned_alloc(64, n*n * sizeof(int));

    // method 1
    init_matrix();
    gettimeofday(&begin, NULL);
    mmul_simple();
    gettimeofday(&end, NULL);
    LOG_INFO("mmul_simple takes %.2fs\n", time_diff(begin, end));
    verifty_result();

    // method 2
    init_matrix();
    gettimeofday(&begin, NULL);
    mmul_swap();
    gettimeofday(&end, NULL);
    LOG_INFO("mmul_swap takes %.2fs\n", time_diff(begin, end));
    verifty_result();

    // method 3
    init_matrix();
    gettimeofday(&begin, NULL);
    mmul_avx();
    gettimeofday(&end, NULL);
    LOG_INFO("mmul_avx takes %.2fs\n", time_diff(begin, end));
    verifty_result();
    
    // method 4
    // init_matrix();
    // gettimeofday(&begin, NULL);
    // mmul_avx_aligned();
    // gettimeofday(&end, NULL);
    // LOG_INFO("mmul_avx_aligned takes %.2fs\n", time_diff(begin, end));
    // verifty_result();

    free(ma);
    free(mb);
    free(mc);
    
    return 0;
}
