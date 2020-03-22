//
//  generalProfiler.h
//  generalProfiler
//
//  Created by Juncheng on 5/24/16.
//  Copyright © 2016 Juncheng. All rights reserved.
//

#ifndef profiler_h
#define profiler_h

#ifdef __cplusplus
extern "C"
{
#endif


#include "reader.h"
#include "cache.h"
#include "profilerStruct.h"




typedef struct {
  guint64 req_cnt;
  guint64 req_byte;
  guint64 miss_cnt;
  guint64 miss_byte;
  double obj_miss_ratio;
  double byte_miss_ratio;
  gint64 cache_size;
  gpointer other_data;
} profiler_res_t;



typedef enum {
  P_MISS_RATIO = 0,
  P_EVICTION_AGE,
  P_HIT_RESULT,
  P_EVICTION,
} profiler_t;

/**
 * this function performs cache_size/bin_size simulations to obtain miss ratio,
 * the size of simulations are from 0, bin_size, bin_size*2 ... bin_size*n,
 * it returns an array of profiler_res_t*, each element of the array is the result of one simulation
 * the user is responsible for g_free the memory of returned results
 * @param reader_in
 * @param cache_in
 * @param num_of_threads
 * @param bin_size
 * @return an array of profiler_res_t*, each element of the array is the result of one simulation
 */
profiler_res_t *get_miss_ratio_curve(reader_t *reader_in, cache_t *cache_in, int num_of_threads, guint64 bin_size);



#ifdef __cplusplus
}
#endif

#endif /* profiler_h */
