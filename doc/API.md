traceReader:
typedef struct {
int time_field;
int obj_id_field;
int obj_size_field;
int obj_mass_field;
int op_field;
int ttl_field;

// csv reader
gboolean has_header;
char delimiter;

// binary reader
char binary_fmt[MAX_BIN_FMT_STR_LEN];
} reader_init_param_t;

typedef struct reader {
char _mapped_file; /_ mmap the file, this should not change during runtime \*/
uint64_t mmap_offset;

FILE \*file;
size_t file_size;

trace_type_e trace_type; /_ possible types see trace_type_t _/
obj_id_type_e obj_id_type; /_ possible types see obj_id_type_e in request.h _/

size_t item_size; /_ the size of one record, used to
_ locate the memory location of next element,
_ when used in vscsiReaser and binaryReader,
_ it is a const value,
_ when it is used in plainReader or csvReader,
_ it is the size of last record, it does not
_ include LFCR or \0 _/

uint64_t n_total_req; /_ number of requests in the trace _/
uint64_t n_uniq_obj; /_ number of objects in the trace _/

char trace_path[MAX_FILE_PATH_LEN];
reader_init_param_t init_params;

void *reader_params;
void *other_params; /_ currently not used _/

gint ver;

bool cloned; // true if this is a cloned reader, else false

} reader_t;

/\*\*

- setup the reader struct for reading trace
- @param trace_path
- @param trace_type CSV_TRACE, PLAIN_TXT_TRACE, BIN_TRACE, VSCSI_TRACE,
- TWR_BIN_TRACE
- @param obj_id_type OBJ_ID_NUM, OBJ_ID_STR
- @param setup_params
- @return a pointer to reader_t struct, the returned reader needs to be
- explicitly closed by calling close_reader or close_trace
  */
  reader_t *setup_reader(const char *trace_path, const trace_type_e trace_type,
  const obj_id_type_e obj_id_type,
  const reader_init_param_t *const reader_init_param);

/_ this is the same function as setup_reader _/
static inline reader_t *
open_trace(const char *path, const trace_type_e type,
const obj_id_type_e obj_id_type,
const reader_init_param_t \*const reader_init_param) {
return setup_reader(path, type, obj_id_type, reader_init_param);
}

/\*\*

- read one request from reader, and store it in the pre-allocated request_t req
- @param reader
- @param req
  */
  uint64_t get_num_of_req(reader_t *const reader);

/\*\*

- as the name suggests
- @param reader
- @return
  */
  static inline trace_type_e get_trace_type(const reader_t *const reader) {
  return reader->trace_type;
  }

/\*\*

- as the name suggests
- @param reader
- @return
  */
  static inline obj_id_type_e get_obj_id_type(const reader_t *const reader) {
  return reader->obj_id_type;
  }

/\*\*

- read one request from reader/trace, stored the info in pre-allocated req
- @param reader
- @param req
- return 0 on success and 1 if reach end of trace
  */
  int read_one_req(reader_t *const reader, request_t \*const req);

/\*\*

- reset reader, so we can read from the beginning
- @param reader
  */
  void reset_reader(reader_t *const reader);

/\*\*

- close reader and release resources
- @param reader
- @return
  */
  int close_reader(reader_t *const reader);

/\*\*

- clone a reader, mostly used in multithreading
- @param reader
- @return
  */
  reader_t *clone_reader(const reader_t \*const reader);

cache and cacheAlgo:

static inline request_t *new_request();
static inline void copy_request(request_t *req_dest, request_t *req_src);
static inline request_t *clone_request(request_t *req);
static inline void free_request(request_t *req);
static inline void print_request(request_t \*req);

simulator:
sim_res_t *
simulate_at_multi_sizes(reader_t *const reader,
const cache_t *const cache,
const gint num_of_sizes,
const guint64 *const cache_sizes,
reader_t \*const warmup_reader,
const double warmup_perc,
const gint num_of_threads);

sim_res_t *
simulate_at_multi_sizes_with_step_size(reader_t *const reader_in,
const cache_t *const cache_in,
const guint64 step_size,
reader_t *const warmup_reader,
const double warmup_perc,
const gint num_of_threads);

profiler:
