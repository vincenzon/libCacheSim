//
// Created by Juncheng Yang on 11/21/19.
//

#include "test_common.h"


/**
 * this one for testing with the plain trace reader, which currently does not have obj size information
 * @param user_data
 */
void test_profiler_trace_no_size(gconstpointer user_data) {
  guint64 cache_size = CACHE_SIZE/MB;
  guint64 bin_size = BIN_SIZE/MB;

  guint64 req_cnt_true = 113872;
  guint64 miss_cnt_true[] = {0, 99411, 96397, 95652, 95370, 95182, 94997, 94891, 94816};

  reader_t *reader = (reader_t *) user_data;
  cache_t *cache = create_cache("LRU", cache_size, reader->base->obj_id_type, NULL);
  g_assert_true(cache != NULL);
  profiler_res_t *res = get_miss_ratio_curve(reader, cache, 4, bin_size);

//  guint64* mc = _get_lru_miss_cnt_seq(reader, get_num_of_req(reader));
//  for (int i=0; i<cache_size/bin_size+1; i++){
//    printf("%lld req %lld miss %lld req_byte %lld miss_byte %lld - LRU %lld\n",
//        res[i].cache_size, res[i].req_cnt, res[i].miss_cnt, res[i].req_byte, res[i].miss_byte, mc[bin_size*i]);
//  }


  g_assert_cmpuint(res[0].cache_size, ==, 0);
  g_assert_cmpuint(res[0].req_cnt, ==, 0);
  g_assert_cmpuint(res[0].miss_cnt, ==, 0);

  for (int i = 1; i < cache_size / bin_size + 1; i++) {
    g_assert_cmpuint(req_cnt_true, ==, res[i].req_cnt);
    g_assert_cmpuint(miss_cnt_true[i], ==, res[i].miss_cnt);
    g_assert_cmpuint(req_cnt_true, ==, res[i].req_byte);
    g_assert_cmpuint(miss_cnt_true[i], ==, res[i].miss_byte);
  }
  cache->core->cache_free(cache);
  g_free(res);
}


/**
 * this one for testing with all other cache readers, which contain obj size information
 * @param user_data
 */
void test_profiler_trace(gconstpointer user_data) {
  guint64 req_cnt_true = 113872, req_byte_true =4205978112;
  guint64 miss_cnt_true[] = {0, 93161, 87795, 82945, 81433, 72250, 72083, 71969, 71716};
  guint64 miss_byte_true[] = {0, 4036642816, 3838236160, 3664065536, 3611544064,
                              3081624576, 3079594496, 3075357184, 3060711936};

  reader_t *reader = (reader_t *) user_data;
  cache_t *cache = create_cache("LRU", CACHE_SIZE, reader->base->obj_id_type, NULL);
  g_assert_true(cache != NULL);
  profiler_res_t *res = get_miss_ratio_curve(reader, cache, 4, BIN_SIZE);

//  guint64* mc = _get_lru_miss_cnt_seq(reader, get_num_of_req(reader));
//  for (int i=0; i<CACHE_SIZE/BIN_SIZE+1; i++){
//    printf("%lld req %lld miss %lld req_byte %lld miss_byte %lld\n",
//    res[i].cache_size, res[i].req_cnt, res[i].miss_cnt, res[i].req_byte, res[i].miss_byte);
//  }

  g_assert_cmpuint(res[0].cache_size, ==, 0);
  g_assert_cmpuint(res[0].req_cnt, ==, 0);
  g_assert_cmpuint(res[0].miss_cnt, ==, 0);
  g_assert_cmpuint(res[0].req_byte, ==, 0);
  g_assert_cmpuint(res[0].miss_byte, ==, 0);

  for (int i = 1; i < CACHE_SIZE / BIN_SIZE + 1; i++) {
    g_assert_cmpuint(req_cnt_true, ==, res[i].req_cnt);
    g_assert_cmpuint(miss_cnt_true[i], ==, res[i].miss_cnt);
    g_assert_cmpuint(req_byte_true, ==, res[i].req_byte);
    g_assert_cmpuint(miss_byte_true[i], ==, res[i].miss_byte);
  }
  cache->core->cache_free(cache);
  g_free(res);
}


int main(int argc, char *argv[]) {
  g_test_init(&argc, &argv, NULL);
  reader_t *reader;

  reader = setup_plaintxt_reader_num();
  g_test_add_data_func_full("/libmimircache/test_profiler_trace_no_size_plain_num", reader, test_profiler_trace_no_size, test_teardown);

  reader = setup_plaintxt_reader_str();
  g_test_add_data_func_full("/libmimircache/test_profiler_trace_no_size_plain_str", reader, test_profiler_trace_no_size, test_teardown);

  reader = setup_csv_reader_obj_num();
  g_test_add_data_func_full("/libmimircache/test_profiler_trace_csv_num", reader, test_profiler_trace, test_teardown);

  reader = setup_csv_reader_obj_str();
  g_test_add_data_func_full("/libmimircache/test_profiler_trace_csv_str", reader, test_profiler_trace, test_teardown);

  reader = setup_binary_reader();
  g_test_add_data_func_full("/libmimircache/test_profiler_trace_binary", reader, test_profiler_trace, test_teardown);

  reader = setup_vscsi_reader();
  g_test_add_data_func_full("/libmimircache/test_profiler_trace_vscsi", reader, test_profiler_trace, test_teardown);

  return g_test_run();
}