/*
  File autogenerated by gengetopt version 2.22.2
  generated with the following command:
  gengetopt --string-parser 

  The developers of gengetopt consider the fixed text that goes in all
  gengetopt output files to be in the public domain:
  we make no copyright claims on it.
*/

/* If we use autoconf.  */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef FIX_UNUSED
#define FIX_UNUSED(X) (void) (X) /* avoid warnings for unused params */
#endif

#include "getopt.h"

#include "cmdline.h"

const char *gengetopt_args_info_purpose = "TwoSpect analysis program";

const char *gengetopt_args_info_usage = "Usage: " CMDLINE_PARSER_PACKAGE " [OPTIONS]...";

const char *gengetopt_args_info_description = "";

const char *gengetopt_args_info_help[] = {
  "  -h, --help                    Print help and exit",
  "  -V, --version                 Print version and exit",
  "      --A=DOUBLE                Amplitude of fake signal",
  "      --f0=DOUBLE               Frequency of fake signal",
  "      --P=DOUBLE                Period of binary orbit of fake signal",
  "      --df=DOUBLE               Modulation depth of the signal due to the \n                                  binary orbit",
  "      --Tobs=DOUBLE             Total observation time",
  "      --Tcoh=DOUBLE             SFT coherence time  (default=`1000')",
  "      --t0=DOUBLE               Start time of the search in GPS seconds",
  "      --fs=DOUBLE               Sampling frequency of time series",
  "      --fmin=DOUBLE             Minimum frequency of band",
  "      --fspan=DOUBLE            Frequency span of band",
  "      --cols=INT                Maximum column width to search",
  "      --ihsfar=DOUBLE           IHS FAR threshold  (default=`0.01')",
  "      --blksize=INT             Blocksize for running median of 1st FFT band  \n                                  (default=`1001')",
  "      --outdirectory=STRING     Output directory",
  "      --ephemDir=STRING         Path to ephemeris files  \n                                  (default=`/opt/lscsoft/lal/share/lal')",
  "      --dopplerMultiplier=DOUBLE\n                                Multiplier for the Doppler velocity  \n                                  (default=`1.0')",
  "      --templateLength=INT      Number of pixels to use in the template  \n                                  (default=`50')",
    0
};

typedef enum {ARG_NO
  , ARG_STRING
  , ARG_INT
  , ARG_DOUBLE
} cmdline_parser_arg_type;

static
void clear_given (struct gengetopt_args_info *args_info);
static
void clear_args (struct gengetopt_args_info *args_info);

static int
cmdline_parser_internal (int argc, char * const *argv, struct gengetopt_args_info *args_info,
                        struct cmdline_parser_params *params, const char *additional_error);

struct line_list
{
  char * string_arg;
  struct line_list * next;
};

static struct line_list *cmd_line_list = 0;
static struct line_list *cmd_line_list_tmp = 0;

static void
free_cmd_list(void)
{
  /* free the list of a previous call */
  if (cmd_line_list)
    {
      while (cmd_line_list) {
        cmd_line_list_tmp = cmd_line_list;
        cmd_line_list = cmd_line_list->next;
        free (cmd_line_list_tmp->string_arg);
        free (cmd_line_list_tmp);
      }
    }
}


static char *
gengetopt_strdup (const char *s);

static
void clear_given (struct gengetopt_args_info *args_info)
{
  args_info->help_given = 0 ;
  args_info->version_given = 0 ;
  args_info->A_given = 0 ;
  args_info->f0_given = 0 ;
  args_info->P_given = 0 ;
  args_info->df_given = 0 ;
  args_info->Tobs_given = 0 ;
  args_info->Tcoh_given = 0 ;
  args_info->t0_given = 0 ;
  args_info->fs_given = 0 ;
  args_info->fmin_given = 0 ;
  args_info->fspan_given = 0 ;
  args_info->cols_given = 0 ;
  args_info->ihsfar_given = 0 ;
  args_info->blksize_given = 0 ;
  args_info->outdirectory_given = 0 ;
  args_info->ephemDir_given = 0 ;
  args_info->dopplerMultiplier_given = 0 ;
  args_info->templateLength_given = 0 ;
}

static
void clear_args (struct gengetopt_args_info *args_info)
{
  FIX_UNUSED (args_info);
  args_info->A_orig = NULL;
  args_info->f0_orig = NULL;
  args_info->P_orig = NULL;
  args_info->df_orig = NULL;
  args_info->Tobs_orig = NULL;
  args_info->Tcoh_arg = 1000;
  args_info->Tcoh_orig = NULL;
  args_info->t0_orig = NULL;
  args_info->fs_orig = NULL;
  args_info->fmin_orig = NULL;
  args_info->fspan_orig = NULL;
  args_info->cols_orig = NULL;
  args_info->ihsfar_arg = 0.01;
  args_info->ihsfar_orig = NULL;
  args_info->blksize_arg = 1001;
  args_info->blksize_orig = NULL;
  args_info->outdirectory_arg = NULL;
  args_info->outdirectory_orig = NULL;
  args_info->ephemDir_arg = gengetopt_strdup ("/opt/lscsoft/lal/share/lal");
  args_info->ephemDir_orig = NULL;
  args_info->dopplerMultiplier_arg = 1.0;
  args_info->dopplerMultiplier_orig = NULL;
  args_info->templateLength_arg = 50;
  args_info->templateLength_orig = NULL;
  
}

static
void init_args_info(struct gengetopt_args_info *args_info)
{


  args_info->help_help = gengetopt_args_info_help[0] ;
  args_info->version_help = gengetopt_args_info_help[1] ;
  args_info->A_help = gengetopt_args_info_help[2] ;
  args_info->f0_help = gengetopt_args_info_help[3] ;
  args_info->P_help = gengetopt_args_info_help[4] ;
  args_info->df_help = gengetopt_args_info_help[5] ;
  args_info->Tobs_help = gengetopt_args_info_help[6] ;
  args_info->Tcoh_help = gengetopt_args_info_help[7] ;
  args_info->t0_help = gengetopt_args_info_help[8] ;
  args_info->fs_help = gengetopt_args_info_help[9] ;
  args_info->fmin_help = gengetopt_args_info_help[10] ;
  args_info->fspan_help = gengetopt_args_info_help[11] ;
  args_info->cols_help = gengetopt_args_info_help[12] ;
  args_info->ihsfar_help = gengetopt_args_info_help[13] ;
  args_info->blksize_help = gengetopt_args_info_help[14] ;
  args_info->outdirectory_help = gengetopt_args_info_help[15] ;
  args_info->ephemDir_help = gengetopt_args_info_help[16] ;
  args_info->dopplerMultiplier_help = gengetopt_args_info_help[17] ;
  args_info->templateLength_help = gengetopt_args_info_help[18] ;
  
}

void
cmdline_parser_print_version (void)
{
  printf ("%s %s\n",
     (strlen(CMDLINE_PARSER_PACKAGE_NAME) ? CMDLINE_PARSER_PACKAGE_NAME : CMDLINE_PARSER_PACKAGE),
     CMDLINE_PARSER_VERSION);
}

static void print_help_common(void) {
  cmdline_parser_print_version ();

  if (strlen(gengetopt_args_info_purpose) > 0)
    printf("\n%s\n", gengetopt_args_info_purpose);

  if (strlen(gengetopt_args_info_usage) > 0)
    printf("\n%s\n", gengetopt_args_info_usage);

  printf("\n");

  if (strlen(gengetopt_args_info_description) > 0)
    printf("%s\n\n", gengetopt_args_info_description);
}

void
cmdline_parser_print_help (void)
{
  int i = 0;
  print_help_common();
  while (gengetopt_args_info_help[i])
    printf("%s\n", gengetopt_args_info_help[i++]);
}

void
cmdline_parser_init (struct gengetopt_args_info *args_info)
{
  clear_given (args_info);
  clear_args (args_info);
  init_args_info (args_info);
}

void
cmdline_parser_params_init(struct cmdline_parser_params *params)
{
  if (params)
    { 
      params->override = 0;
      params->initialize = 1;
      params->check_required = 1;
      params->check_ambiguity = 0;
      params->print_errors = 1;
    }
}

struct cmdline_parser_params *
cmdline_parser_params_create(void)
{
  struct cmdline_parser_params *params = 
    (struct cmdline_parser_params *)malloc(sizeof(struct cmdline_parser_params));
  cmdline_parser_params_init(params);  
  return params;
}

static void
free_string_field (char **s)
{
  if (*s)
    {
      free (*s);
      *s = 0;
    }
}


static void
cmdline_parser_release (struct gengetopt_args_info *args_info)
{

  free_string_field (&(args_info->A_orig));
  free_string_field (&(args_info->f0_orig));
  free_string_field (&(args_info->P_orig));
  free_string_field (&(args_info->df_orig));
  free_string_field (&(args_info->Tobs_orig));
  free_string_field (&(args_info->Tcoh_orig));
  free_string_field (&(args_info->t0_orig));
  free_string_field (&(args_info->fs_orig));
  free_string_field (&(args_info->fmin_orig));
  free_string_field (&(args_info->fspan_orig));
  free_string_field (&(args_info->cols_orig));
  free_string_field (&(args_info->ihsfar_orig));
  free_string_field (&(args_info->blksize_orig));
  free_string_field (&(args_info->outdirectory_arg));
  free_string_field (&(args_info->outdirectory_orig));
  free_string_field (&(args_info->ephemDir_arg));
  free_string_field (&(args_info->ephemDir_orig));
  free_string_field (&(args_info->dopplerMultiplier_orig));
  free_string_field (&(args_info->templateLength_orig));
  
  

  clear_given (args_info);
}


static void
write_into_file(FILE *outfile, const char *opt, const char *arg, const char *values[])
{
  FIX_UNUSED (values);
  if (arg) {
    fprintf(outfile, "%s=\"%s\"\n", opt, arg);
  } else {
    fprintf(outfile, "%s\n", opt);
  }
}


int
cmdline_parser_dump(FILE *outfile, struct gengetopt_args_info *args_info)
{
  int i = 0;

  if (!outfile)
    {
      fprintf (stderr, "%s: cannot dump options to stream\n", CMDLINE_PARSER_PACKAGE);
      return EXIT_FAILURE;
    }

  if (args_info->help_given)
    write_into_file(outfile, "help", 0, 0 );
  if (args_info->version_given)
    write_into_file(outfile, "version", 0, 0 );
  if (args_info->A_given)
    write_into_file(outfile, "A", args_info->A_orig, 0);
  if (args_info->f0_given)
    write_into_file(outfile, "f0", args_info->f0_orig, 0);
  if (args_info->P_given)
    write_into_file(outfile, "P", args_info->P_orig, 0);
  if (args_info->df_given)
    write_into_file(outfile, "df", args_info->df_orig, 0);
  if (args_info->Tobs_given)
    write_into_file(outfile, "Tobs", args_info->Tobs_orig, 0);
  if (args_info->Tcoh_given)
    write_into_file(outfile, "Tcoh", args_info->Tcoh_orig, 0);
  if (args_info->t0_given)
    write_into_file(outfile, "t0", args_info->t0_orig, 0);
  if (args_info->fs_given)
    write_into_file(outfile, "fs", args_info->fs_orig, 0);
  if (args_info->fmin_given)
    write_into_file(outfile, "fmin", args_info->fmin_orig, 0);
  if (args_info->fspan_given)
    write_into_file(outfile, "fspan", args_info->fspan_orig, 0);
  if (args_info->cols_given)
    write_into_file(outfile, "cols", args_info->cols_orig, 0);
  if (args_info->ihsfar_given)
    write_into_file(outfile, "ihsfar", args_info->ihsfar_orig, 0);
  if (args_info->blksize_given)
    write_into_file(outfile, "blksize", args_info->blksize_orig, 0);
  if (args_info->outdirectory_given)
    write_into_file(outfile, "outdirectory", args_info->outdirectory_orig, 0);
  if (args_info->ephemDir_given)
    write_into_file(outfile, "ephemDir", args_info->ephemDir_orig, 0);
  if (args_info->dopplerMultiplier_given)
    write_into_file(outfile, "dopplerMultiplier", args_info->dopplerMultiplier_orig, 0);
  if (args_info->templateLength_given)
    write_into_file(outfile, "templateLength", args_info->templateLength_orig, 0);
  

  i = EXIT_SUCCESS;
  return i;
}

int
cmdline_parser_file_save(const char *filename, struct gengetopt_args_info *args_info)
{
  FILE *outfile;
  int i = 0;

  outfile = fopen(filename, "w");

  if (!outfile)
    {
      fprintf (stderr, "%s: cannot open file for writing: %s\n", CMDLINE_PARSER_PACKAGE, filename);
      return EXIT_FAILURE;
    }

  i = cmdline_parser_dump(outfile, args_info);
  fclose (outfile);

  return i;
}

void
cmdline_parser_free (struct gengetopt_args_info *args_info)
{
  cmdline_parser_release (args_info);
}

/** @brief replacement of strdup, which is not standard */
char *
gengetopt_strdup (const char *s)
{
  char *result = 0;
  if (!s)
    return result;

  result = (char*)malloc(strlen(s) + 1);
  if (result == (char*)0)
    return (char*)0;
  strcpy(result, s);
  return result;
}

int
cmdline_parser (int argc, char * const *argv, struct gengetopt_args_info *args_info)
{
  return cmdline_parser2 (argc, argv, args_info, 0, 1, 1);
}

int
cmdline_parser_ext (int argc, char * const *argv, struct gengetopt_args_info *args_info,
                   struct cmdline_parser_params *params)
{
  int result;
  result = cmdline_parser_internal (argc, argv, args_info, params, 0);

  if (result == EXIT_FAILURE)
    {
      cmdline_parser_free (args_info);
      exit (EXIT_FAILURE);
    }
  
  return result;
}

int
cmdline_parser2 (int argc, char * const *argv, struct gengetopt_args_info *args_info, int override, int initialize, int check_required)
{
  int result;
  struct cmdline_parser_params params;
  
  params.override = override;
  params.initialize = initialize;
  params.check_required = check_required;
  params.check_ambiguity = 0;
  params.print_errors = 1;

  result = cmdline_parser_internal (argc, argv, args_info, &params, 0);

  if (result == EXIT_FAILURE)
    {
      cmdline_parser_free (args_info);
      exit (EXIT_FAILURE);
    }
  
  return result;
}

int
cmdline_parser_required (struct gengetopt_args_info *args_info, const char *prog_name)
{
  FIX_UNUSED (args_info);
  FIX_UNUSED (prog_name);
  return EXIT_SUCCESS;
}


static char *package_name = 0;

/**
 * @brief updates an option
 * @param field the generic pointer to the field to update
 * @param orig_field the pointer to the orig field
 * @param field_given the pointer to the number of occurrence of this option
 * @param prev_given the pointer to the number of occurrence already seen
 * @param value the argument for this option (if null no arg was specified)
 * @param possible_values the possible values for this option (if specified)
 * @param default_value the default value (in case the option only accepts fixed values)
 * @param arg_type the type of this option
 * @param check_ambiguity @see cmdline_parser_params.check_ambiguity
 * @param override @see cmdline_parser_params.override
 * @param no_free whether to free a possible previous value
 * @param multiple_option whether this is a multiple option
 * @param long_opt the corresponding long option
 * @param short_opt the corresponding short option (or '-' if none)
 * @param additional_error possible further error specification
 */
static
int update_arg(void *field, char **orig_field,
               unsigned int *field_given, unsigned int *prev_given, 
               char *value, const char *possible_values[],
               const char *default_value,
               cmdline_parser_arg_type arg_type,
               int check_ambiguity, int override,
               int no_free, int multiple_option,
               const char *long_opt, char short_opt,
               const char *additional_error)
{
  FIX_UNUSED (field);
  char *stop_char = 0;
  const char *val = value;
  int found;
  char **string_field;

  stop_char = 0;
  found = 0;

  if (!multiple_option && prev_given && (*prev_given || (check_ambiguity && *field_given)))
    {
      if (short_opt != '-')
        fprintf (stderr, "%s: `--%s' (`-%c') option given more than once%s\n", 
               package_name, long_opt, short_opt,
               (additional_error ? additional_error : ""));
      else
        fprintf (stderr, "%s: `--%s' option given more than once%s\n", 
               package_name, long_opt,
               (additional_error ? additional_error : ""));
      return 1; /* failure */
    }

  FIX_UNUSED (default_value);
    
  if (field_given && *field_given && ! override)
    return 0;
  if (prev_given)
    (*prev_given)++;
  if (field_given)
    (*field_given)++;
  if (possible_values)
    val = possible_values[found];

  switch(arg_type) {
  case ARG_INT:
    if (val) *((int *)field) = strtol (val, &stop_char, 0);
    break;
  case ARG_DOUBLE:
    if (val) *((double *)field) = strtod (val, &stop_char);
    break;
  case ARG_STRING:
    if (val) {
      string_field = (char **)field;
      if (!no_free && *string_field)
        free (*string_field); /* free previous string */
      *string_field = gengetopt_strdup (val);
    }
    break;
  default:
    break;
  };

  /* check numeric conversion */
  switch(arg_type) {
  case ARG_INT:
  case ARG_DOUBLE:
    if (val && !(stop_char && *stop_char == '\0')) {
      fprintf(stderr, "%s: invalid numeric value: %s\n", package_name, val);
      return 1; /* failure */
    }
    break;
  default:
    ;
  };

  /* store the original value */
  switch(arg_type) {
  case ARG_NO:
    break;
  default:
    if (value && orig_field) {
      if (no_free) {
        *orig_field = value;
      } else {
        if (*orig_field)
          free (*orig_field); /* free previous string */
        *orig_field = gengetopt_strdup (value);
      }
    }
  };

  return 0; /* OK */
}


int
cmdline_parser_internal (
  int argc, char * const *argv, struct gengetopt_args_info *args_info,
                        struct cmdline_parser_params *params, const char *additional_error)
{
  int c;	/* Character of the parsed option.  */

  int error = 0;
  struct gengetopt_args_info local_args_info;
  
  int override;
  int initialize;
  int check_required;
  int check_ambiguity;
  
  package_name = argv[0];
  
  override = params->override;
  initialize = params->initialize;
  check_required = params->check_required;
  check_ambiguity = params->check_ambiguity;

  if (initialize)
    cmdline_parser_init (args_info);

  cmdline_parser_init (&local_args_info);

  optarg = 0;
  optind = 0;
  opterr = params->print_errors;
  optopt = '?';

  while (1)
    {
      int option_index = 0;

      static struct option long_options[] = {
        { "help",	0, NULL, 'h' },
        { "version",	0, NULL, 'V' },
        { "A",	1, NULL, 0 },
        { "f0",	1, NULL, 0 },
        { "P",	1, NULL, 0 },
        { "df",	1, NULL, 0 },
        { "Tobs",	1, NULL, 0 },
        { "Tcoh",	1, NULL, 0 },
        { "t0",	1, NULL, 0 },
        { "fs",	1, NULL, 0 },
        { "fmin",	1, NULL, 0 },
        { "fspan",	1, NULL, 0 },
        { "cols",	1, NULL, 0 },
        { "ihsfar",	1, NULL, 0 },
        { "blksize",	1, NULL, 0 },
        { "outdirectory",	1, NULL, 0 },
        { "ephemDir",	1, NULL, 0 },
        { "dopplerMultiplier",	1, NULL, 0 },
        { "templateLength",	1, NULL, 0 },
        { 0,  0, 0, 0 }
      };

      c = getopt_long (argc, argv, "hV", long_options, &option_index);

      if (c == -1) break;	/* Exit from `while (1)' loop.  */

      switch (c)
        {
        case 'h':	/* Print help and exit.  */
          cmdline_parser_print_help ();
          cmdline_parser_free (&local_args_info);
          exit (EXIT_SUCCESS);

        case 'V':	/* Print version and exit.  */
          cmdline_parser_print_version ();
          cmdline_parser_free (&local_args_info);
          exit (EXIT_SUCCESS);


        case 0:	/* Long option with no short option */
          /* Amplitude of fake signal.  */
          if (strcmp (long_options[option_index].name, "A") == 0)
          {
          
          
            if (update_arg( (void *)&(args_info->A_arg), 
                 &(args_info->A_orig), &(args_info->A_given),
                &(local_args_info.A_given), optarg, 0, 0, ARG_DOUBLE,
                check_ambiguity, override, 0, 0,
                "A", '-',
                additional_error))
              goto failure;
          
          }
          /* Frequency of fake signal.  */
          else if (strcmp (long_options[option_index].name, "f0") == 0)
          {
          
          
            if (update_arg( (void *)&(args_info->f0_arg), 
                 &(args_info->f0_orig), &(args_info->f0_given),
                &(local_args_info.f0_given), optarg, 0, 0, ARG_DOUBLE,
                check_ambiguity, override, 0, 0,
                "f0", '-',
                additional_error))
              goto failure;
          
          }
          /* Period of binary orbit of fake signal.  */
          else if (strcmp (long_options[option_index].name, "P") == 0)
          {
          
          
            if (update_arg( (void *)&(args_info->P_arg), 
                 &(args_info->P_orig), &(args_info->P_given),
                &(local_args_info.P_given), optarg, 0, 0, ARG_DOUBLE,
                check_ambiguity, override, 0, 0,
                "P", '-',
                additional_error))
              goto failure;
          
          }
          /* Modulation depth of the signal due to the binary orbit.  */
          else if (strcmp (long_options[option_index].name, "df") == 0)
          {
          
          
            if (update_arg( (void *)&(args_info->df_arg), 
                 &(args_info->df_orig), &(args_info->df_given),
                &(local_args_info.df_given), optarg, 0, 0, ARG_DOUBLE,
                check_ambiguity, override, 0, 0,
                "df", '-',
                additional_error))
              goto failure;
          
          }
          /* Total observation time.  */
          else if (strcmp (long_options[option_index].name, "Tobs") == 0)
          {
          
          
            if (update_arg( (void *)&(args_info->Tobs_arg), 
                 &(args_info->Tobs_orig), &(args_info->Tobs_given),
                &(local_args_info.Tobs_given), optarg, 0, 0, ARG_DOUBLE,
                check_ambiguity, override, 0, 0,
                "Tobs", '-',
                additional_error))
              goto failure;
          
          }
          /* SFT coherence time.  */
          else if (strcmp (long_options[option_index].name, "Tcoh") == 0)
          {
          
          
            if (update_arg( (void *)&(args_info->Tcoh_arg), 
                 &(args_info->Tcoh_orig), &(args_info->Tcoh_given),
                &(local_args_info.Tcoh_given), optarg, 0, "1000", ARG_DOUBLE,
                check_ambiguity, override, 0, 0,
                "Tcoh", '-',
                additional_error))
              goto failure;
          
          }
          /* Start time of the search in GPS seconds.  */
          else if (strcmp (long_options[option_index].name, "t0") == 0)
          {
          
          
            if (update_arg( (void *)&(args_info->t0_arg), 
                 &(args_info->t0_orig), &(args_info->t0_given),
                &(local_args_info.t0_given), optarg, 0, 0, ARG_DOUBLE,
                check_ambiguity, override, 0, 0,
                "t0", '-',
                additional_error))
              goto failure;
          
          }
          /* Sampling frequency of time series.  */
          else if (strcmp (long_options[option_index].name, "fs") == 0)
          {
          
          
            if (update_arg( (void *)&(args_info->fs_arg), 
                 &(args_info->fs_orig), &(args_info->fs_given),
                &(local_args_info.fs_given), optarg, 0, 0, ARG_DOUBLE,
                check_ambiguity, override, 0, 0,
                "fs", '-',
                additional_error))
              goto failure;
          
          }
          /* Minimum frequency of band.  */
          else if (strcmp (long_options[option_index].name, "fmin") == 0)
          {
          
          
            if (update_arg( (void *)&(args_info->fmin_arg), 
                 &(args_info->fmin_orig), &(args_info->fmin_given),
                &(local_args_info.fmin_given), optarg, 0, 0, ARG_DOUBLE,
                check_ambiguity, override, 0, 0,
                "fmin", '-',
                additional_error))
              goto failure;
          
          }
          /* Frequency span of band.  */
          else if (strcmp (long_options[option_index].name, "fspan") == 0)
          {
          
          
            if (update_arg( (void *)&(args_info->fspan_arg), 
                 &(args_info->fspan_orig), &(args_info->fspan_given),
                &(local_args_info.fspan_given), optarg, 0, 0, ARG_DOUBLE,
                check_ambiguity, override, 0, 0,
                "fspan", '-',
                additional_error))
              goto failure;
          
          }
          /* Maximum column width to search.  */
          else if (strcmp (long_options[option_index].name, "cols") == 0)
          {
          
          
            if (update_arg( (void *)&(args_info->cols_arg), 
                 &(args_info->cols_orig), &(args_info->cols_given),
                &(local_args_info.cols_given), optarg, 0, 0, ARG_INT,
                check_ambiguity, override, 0, 0,
                "cols", '-',
                additional_error))
              goto failure;
          
          }
          /* IHS FAR threshold.  */
          else if (strcmp (long_options[option_index].name, "ihsfar") == 0)
          {
          
          
            if (update_arg( (void *)&(args_info->ihsfar_arg), 
                 &(args_info->ihsfar_orig), &(args_info->ihsfar_given),
                &(local_args_info.ihsfar_given), optarg, 0, "0.01", ARG_DOUBLE,
                check_ambiguity, override, 0, 0,
                "ihsfar", '-',
                additional_error))
              goto failure;
          
          }
          /* Blocksize for running median of 1st FFT band.  */
          else if (strcmp (long_options[option_index].name, "blksize") == 0)
          {
          
          
            if (update_arg( (void *)&(args_info->blksize_arg), 
                 &(args_info->blksize_orig), &(args_info->blksize_given),
                &(local_args_info.blksize_given), optarg, 0, "1001", ARG_INT,
                check_ambiguity, override, 0, 0,
                "blksize", '-',
                additional_error))
              goto failure;
          
          }
          /* Output directory.  */
          else if (strcmp (long_options[option_index].name, "outdirectory") == 0)
          {
          
          
            if (update_arg( (void *)&(args_info->outdirectory_arg), 
                 &(args_info->outdirectory_orig), &(args_info->outdirectory_given),
                &(local_args_info.outdirectory_given), optarg, 0, 0, ARG_STRING,
                check_ambiguity, override, 0, 0,
                "outdirectory", '-',
                additional_error))
              goto failure;
          
          }
          /* Path to ephemeris files.  */
          else if (strcmp (long_options[option_index].name, "ephemDir") == 0)
          {
          
          
            if (update_arg( (void *)&(args_info->ephemDir_arg), 
                 &(args_info->ephemDir_orig), &(args_info->ephemDir_given),
                &(local_args_info.ephemDir_given), optarg, 0, "/opt/lscsoft/lal/share/lal", ARG_STRING,
                check_ambiguity, override, 0, 0,
                "ephemDir", '-',
                additional_error))
              goto failure;
          
          }
          /* Multiplier for the Doppler velocity.  */
          else if (strcmp (long_options[option_index].name, "dopplerMultiplier") == 0)
          {
          
          
            if (update_arg( (void *)&(args_info->dopplerMultiplier_arg), 
                 &(args_info->dopplerMultiplier_orig), &(args_info->dopplerMultiplier_given),
                &(local_args_info.dopplerMultiplier_given), optarg, 0, "1.0", ARG_DOUBLE,
                check_ambiguity, override, 0, 0,
                "dopplerMultiplier", '-',
                additional_error))
              goto failure;
          
          }
          /* Number of pixels to use in the template.  */
          else if (strcmp (long_options[option_index].name, "templateLength") == 0)
          {
          
          
            if (update_arg( (void *)&(args_info->templateLength_arg), 
                 &(args_info->templateLength_orig), &(args_info->templateLength_given),
                &(local_args_info.templateLength_given), optarg, 0, "50", ARG_INT,
                check_ambiguity, override, 0, 0,
                "templateLength", '-',
                additional_error))
              goto failure;
          
          }
          
          break;
        case '?':	/* Invalid option.  */
          /* `getopt_long' already printed an error message.  */
          goto failure;

        default:	/* bug: option not considered.  */
          fprintf (stderr, "%s: option unknown: %c%s\n", CMDLINE_PARSER_PACKAGE, c, (additional_error ? additional_error : ""));
          abort ();
        } /* switch */
    } /* while */




  cmdline_parser_release (&local_args_info);

  if ( error )
    return (EXIT_FAILURE);

  return 0;

failure:
  
  cmdline_parser_release (&local_args_info);
  return (EXIT_FAILURE);
}

static unsigned int
cmdline_parser_create_argv(const char *cmdline_, char ***argv_ptr, const char *prog_name)
{
  char *cmdline, *p;
  size_t n = 0, j;
  int i;

  if (prog_name) {
    cmd_line_list_tmp = (struct line_list *) malloc (sizeof (struct line_list));
    cmd_line_list_tmp->next = cmd_line_list;
    cmd_line_list = cmd_line_list_tmp;
    cmd_line_list->string_arg = gengetopt_strdup (prog_name);

    ++n;
  }

  cmdline = gengetopt_strdup(cmdline_);
  p = cmdline;

  while (p && strlen(p))
    {
      j = strcspn(p, " \t");
      ++n;
      if (j && j < strlen(p))
        {
          p[j] = '\0';

          cmd_line_list_tmp = (struct line_list *) malloc (sizeof (struct line_list));
          cmd_line_list_tmp->next = cmd_line_list;
          cmd_line_list = cmd_line_list_tmp;
          cmd_line_list->string_arg = gengetopt_strdup (p);

          p += (j+1);
          p += strspn(p, " \t");
        }
      else
        {
          cmd_line_list_tmp = (struct line_list *) malloc (sizeof (struct line_list));
          cmd_line_list_tmp->next = cmd_line_list;
          cmd_line_list = cmd_line_list_tmp;
          cmd_line_list->string_arg = gengetopt_strdup (p);

          break;
        }
    }

  *argv_ptr = (char **) malloc((n + 1) * sizeof(char *));
  cmd_line_list_tmp = cmd_line_list;
  for (i = (n-1); i >= 0; --i)
    {
      (*argv_ptr)[i] = cmd_line_list_tmp->string_arg;
      cmd_line_list_tmp = cmd_line_list_tmp->next;
    }

  (*argv_ptr)[n] = 0;

  free(cmdline);
  return n;
}

int
cmdline_parser_string(const char *cmdline, struct gengetopt_args_info *args_info, const char *prog_name)
{
  return cmdline_parser_string2(cmdline, args_info, prog_name, 0, 1, 1);
}

int
cmdline_parser_string2(const char *cmdline, struct gengetopt_args_info *args_info, const char *prog_name,
    int override, int initialize, int check_required)
{
  struct cmdline_parser_params params;

  params.override = override;
  params.initialize = initialize;
  params.check_required = check_required;
  params.check_ambiguity = 0;
  params.print_errors = 1;

  return cmdline_parser_string_ext(cmdline, args_info, prog_name, &params);
}

int
cmdline_parser_string_ext(const char *cmdline, struct gengetopt_args_info *args_info, const char *prog_name,
    struct cmdline_parser_params *params)
{
  char **argv_ptr = 0;
  int result;
  unsigned int argc;
  
  argc = cmdline_parser_create_argv(cmdline, &argv_ptr, prog_name);
  
  result =
    cmdline_parser_internal (argc, argv_ptr, args_info, params, 0);
  
  if (argv_ptr)
    {
      free (argv_ptr);
    }

  free_cmd_list();
  
  if (result == EXIT_FAILURE)
    {
      cmdline_parser_free (args_info);
      exit (EXIT_FAILURE);
    }
  
  return result;
}

