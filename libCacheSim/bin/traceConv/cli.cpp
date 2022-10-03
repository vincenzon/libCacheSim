
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <argp.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "../../include/libCacheSim/const.h"
#include "../../utils/include/mysys.h"
#include "../cli_utils.h"
#include "internal.hpp"

namespace cli {
const char *argp_program_version = "traceConv 0.0.1";
const char *argp_program_bug_address = "google group";

enum argp_option_short {
  OPTION_TRACE_TYPE_PARAMS = 't',
  OPTION_OUTPUT_PATH = 'o',
  OPTION_NUM_REQ = 'n',
  OPTION_SAMPLE_RATIO = 's',
  OPTION_IGNORE_OBJ_SIZE = 0x101,
  OPTION_OUTPUT_TXT = 0x102,
  OPTION_REMOVE_SIZE_CHANGE = 0x103
};

/*
   OPTIONS.  Field 1 in ARGP.
   Order of fields: {NAME, KEY, ARG, FLAGS, DOC}.
*/
static struct argp_option options[] = {
    {0, 0, 0, 0, "Options used by all the utilities:"},
    {"trace_type_params", OPTION_TRACE_TYPE_PARAMS,
     "\"obj_id_col=1;header=true\"", 0,
     "Parameters used for csv trace, e.g., \"obj_id_col=1;header=true\"", 2},
    {"num_req", OPTION_NUM_REQ, "-1", 0,
     "Number of requests to process, -1 means all requests in the trace", 2},

    {0, 0, 0, 0, "traceConv options:"},
    {"output_path", OPTION_OUTPUT_PATH, "/path/output", 0,
     "Path to write output", 5},
    {"sample_ratio", OPTION_SAMPLE_RATIO, "1", 0,
     "Sample ratio, 1 means no sampling, 0.01 means sample 1% of objects", 5},
    {"ignore_obj_size", OPTION_IGNORE_OBJ_SIZE, "false", 0,
     "specify to ignore the object size from the trace", 10},
    {"output_txt", OPTION_OUTPUT_TXT, "false", 0,
     "output trace in txt format in addition to binary format", 10},

    // {0, 0, 0, 0, "Other less used options:"},
    // {"warmup_sec", OPTION_WARMUP_SEC, "0", 0, "warm up time in seconds", 10},
    // {"use_ttl", OPTION_USE_TTL, "false", 0, "specify to use ttl from the
    // trace",
    //  11},
    // {"consider_obj_metadata", OPTION_CONSIDER_OBJ_METADATA, "true", 0,
    //  "Whether consider per object metadata size in the simulated cache", 10},

    {0}};

/*
   PARSER. Field 2 in ARGP.
   Order of parameters: KEY, ARG, STATE.
*/
static error_t parse_opt(int key, char *arg, struct argp_state *state) {
  struct arguments *arguments =
      reinterpret_cast<struct arguments *>(state->input);

  switch (key) {
    case OPTION_TRACE_TYPE_PARAMS:
      arguments->trace_type_params = arg;
      break;
    case OPTION_IGNORE_OBJ_SIZE:
      arguments->ignore_obj_size = is_true(arg) ? true : false;
      break;
    case OPTION_REMOVE_SIZE_CHANGE:
      arguments->remove_size_change = is_true(arg) ? true : false;
      break;
    case OPTION_OUTPUT_PATH:
      arguments->ofilepath = arg;
      break;
    case OPTION_OUTPUT_TXT:
      arguments->output_txt = is_true(arg) ? true : false;
      break;
    case OPTION_SAMPLE_RATIO:
      arguments->sample_ratio = atof(arg);
      if (arguments->sample_ratio < 0 || arguments->sample_ratio > 1) {
        ERROR("sample ratio should be in (0, 1]\n");
      }
      break;
    case OPTION_NUM_REQ:
      arguments->n_req = atoll(arg);
      break;
    case ARGP_KEY_ARG:
      if (state->arg_num >= N_ARGS) {
        printf("found too many arguments, current %s\n", arg);
        argp_usage(state);
        exit(1);
      }
      arguments->args[state->arg_num] = arg;
      break;
    case ARGP_KEY_END:
      if (state->arg_num < N_ARGS) {
        printf("not enough arguments found\n");
        argp_usage(state);
        exit(1);
      }
      break;
    default:
      return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

/*
   ARGS_DOC. Field 3 in ARGP.
   A description of the non-option command-line arguments
     that we accept.
*/
static char args_doc[] = "trace_path trace_type";

/* Program documentation. */
static char doc[] =
    "example: ./cachesim /trace/path csv -o /path/new_trace.oracleGeneral -t "
    "\"obj_id_col=5; time_col=2; obj_size_col=4\"\n\n";

/**
 * @brief initialize the arguments
 *
 * @param args
 */
static void init_arg(struct arguments *args) {
  memset(args, 0, sizeof(struct arguments));

  args->n_req = -1;
  args->trace_path = NULL;
  args->trace_type_str = NULL;
  args->trace_type_params = NULL;
  args->ignore_obj_size = false;
  args->sample_ratio = 1.0;
  args->ofilepath = NULL;
  args->output_txt = false;
  args->remove_size_change = false;
}

static void print_parsed_arg(struct arguments *args) {
#define OUTPUT_STR_LEN 1024
  int n = 0;
  char output_str[OUTPUT_STR_LEN];
  n = snprintf(output_str, OUTPUT_STR_LEN - 1, "trace path: %s, trace_type %s",
               args->trace_path, trace_type_str[args->trace_type]);

  if (args->trace_type_params != NULL)
    n += snprintf(output_str + n, OUTPUT_STR_LEN - n - 1,
                  ", trace type params: %s", args->trace_type_params);

  if (args->sample_ratio < 1.0)
    n += snprintf(output_str + n, OUTPUT_STR_LEN - n - 1, ", sample ratio: %lf",
                  args->sample_ratio);

  if (args->n_req != -1)
    n += snprintf(output_str + n, OUTPUT_STR_LEN - n - 1,
                  ", num requests to process: %ld", (long)args->n_req);

  if (args->output_txt)
    n += snprintf(output_str + n, OUTPUT_STR_LEN - n - 1,
                  ", output txt trace: true");

  if (args->remove_size_change)
    n += snprintf(output_str + n, OUTPUT_STR_LEN - n - 1,
                  ", remove size change during traceConv");

  if (args->ignore_obj_size)
    n += snprintf(output_str + n, OUTPUT_STR_LEN - n - 1,
                  ", ignore object size");

  snprintf(output_str + n, OUTPUT_STR_LEN - n - 1, "\n");

  INFO("%s", output_str);

#undef OUTPUT_STR_LEN
}

/**
 * @brief parse the command line arguments
 *
 * @param argc
 * @param argv
 */
void parse_cmd(int argc, char *argv[], struct arguments *args) {
  init_arg(args);

  static struct argp argp = {options, parse_opt, args_doc, doc};

  argp_parse(&argp, argc, argv, 0, 0, args);

  args->trace_path = args->args[0];
  args->trace_type_str = args->args[1];
  assert(N_ARGS == 2);

  /* convert trace type string to enum */
  args->trace_type =
      trace_type_str_to_enum(args->trace_type_str, args->trace_path);

  reader_init_param_t reader_init_params;
  memset(&reader_init_params, 0, sizeof(reader_init_params));
  reader_init_params.ignore_obj_size = args->ignore_obj_size;
  reader_init_params.ignore_size_zero_req = true;
  reader_init_params.obj_id_is_num = true;
  reader_init_params.cap_at_n_req = args->n_req;
  reader_init_params.sampler = NULL;

  if (args->sample_ratio > 0 && args->sample_ratio < 1 - 1e-6) {
    sampler_t *sampler = create_spatial_sampler(args->sample_ratio);
    reader_init_params.sampler = sampler;
  }

  parse_reader_params(args->trace_type_params, &reader_init_params);
  if ((args->trace_type == CSV_TRACE || args->trace_type == PLAIN_TXT_TRACE) &&
      reader_init_params.obj_size_field == -1) {
    args->ignore_obj_size = true;
    reader_init_params.ignore_obj_size = true;
  }

  args->reader =
      setup_reader(args->trace_path, args->trace_type, &reader_init_params);

  print_parsed_arg(args);
}
}  // namespace cli
