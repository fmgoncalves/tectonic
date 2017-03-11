#include <tectonic/internals.h>

/* (Re)Allocate N items of type T using xmalloc/xrealloc.  */
#define XTALLOC(n, t) ((t *) xmalloc ((n) * sizeof (t)))

#define BIB_XRETALLOC_NOSET(array_name, array_var, type, size_var, new_size) \
  (array_var) = (type *) xrealloc((array_var), (new_size + 1) * sizeof(type))

#define BIB_XRETALLOC(array_name, array_var, type, size_var, new_size) do { \
  BIB_XRETALLOC_NOSET(array_name, array_var, type, size_var, new_size); \
  size_var = new_size; \
} while (0)

#define BIB_XRETALLOC_STRING(array_name, array_var, length, size_var, new_size) \
  (array_var) = (ASCII_code *) xrealloc((array_var), (new_size) * (length + 1) * sizeof(ASCII_code))

/* duplicated from xetexd.h: */

#define xmalloc_array(type,size) ((type*)xmalloc((size+1)*sizeof(type)))


/* eofeoln.c */

/* Return true if we're at the end of FILE, else false.  This implements
   Pascal's `eof' builtin.  */

static boolean
eof (FILE *file)
{
    int c;

    /* If FILE doesn't exist, return true. This happens, for example,
       when a user does `mft foo.mf' -- there's no change file,
       so we never open it, so we end up calling this with a null pointer. */
    if (!file)
        return true;

    /* Maybe we're already at the end?  */
    if (feof (file))
        return true;

    if ((c = getc (file)) == EOF)
        return true;

    /* We weren't at the end.  Back up.  */
    ungetc (c, file);

    return false;
}


/* Return true on end-of-line in FILE or at the end of FILE, else false.  */
/* Accept both CR and LF as end-of-line. */

static boolean
eoln (FILE *file)
{
    int c;

    if (feof (file))
        return true;

    c = getc (file);

    if (c != EOF)
        ungetc (c, file);

    return c == '\n' || c == '\r' || c == EOF;
}


/* end eofeoln.c */

#include <setjmp.h>

static jmp_buf error_jmpbuf, recover_jmpbuf;

/*14:*/

#define hash_base 1 /*empty 1*/
#define quote_next_fn (hash_base - 1)
#define BUF_SIZE 20000
#define min_print_line 3
#define max_print_line 79
#define aux_stack_size 20
#define MAX_BIB_FILES 20
#define POOL_SIZE 65000L
#define MAX_STRINGS 4000
#define MAX_CITES 750
#define WIZ_FN_SPACE 3000
#define SINGLE_FN_SPACE 50
#define ENT_STR_SIZE 100
#define GLOB_STR_SIZE 1000
#define MAX_GLOB_STRS 10
#define MAX_FIELDS 5000
#define LIT_STK_SIZE 50
#define num_blt_in_fns 37

/*22: */

typedef unsigned char ASCII_code;
typedef unsigned char /*last_lex */ lex_type;
typedef unsigned char id_type;
typedef integer buf_pointer;
typedef ASCII_code *buf_type;
typedef integer pool_pointer;
typedef integer str_number;
typedef integer hash_loc;
typedef integer hash_pointer;
typedef unsigned char /*last_ilk */ str_ilk;
typedef unsigned char /*longest_pds */ pds_loc;
typedef unsigned char /*longest_pds */ pds_len;
typedef const_string pds_type;
typedef integer aux_number;
typedef integer bib_number;
typedef integer cite_number;
typedef unsigned char /*last_fn_class */ fn_class;
typedef integer wiz_fn_loc;
typedef integer int_ent_loc;
typedef integer str_ent_loc;
typedef integer str_glob_loc;
typedef integer field_loc;
typedef integer hash_ptr2;
typedef integer lit_stk_loc;
typedef unsigned char /*last_lit_type */ stk_type;
typedef integer blt_in_range;

static FILE *standard_output;
static integer pool_size;
static integer max_bib_files;
static integer max_cites;
static integer wiz_fn_space;
static integer ent_str_size;
static integer glob_str_size;
static integer max_glob_strs;
static integer max_fields;
static integer lit_stk_size;
static integer max_strings;
static integer hash_size;
static integer hash_prime;
static integer hash_max;
static integer end_of_def;
static integer undefined;
static integer bad;
static unsigned char /*fatal_message */ history;
static integer err_count;
static lex_type lex_class[256];
static id_type id_class[256];
static integer char_width[256];
static integer string_width;
static ASCII_code *name_of_file;
static integer name_length;
static integer name_ptr;
static integer buf_size;
static buf_type buffer;
static buf_pointer last;
static buf_type sv_buffer;
static buf_pointer sv_ptr1;
static buf_pointer sv_ptr2;
static integer tmp_ptr, tmp_end_ptr;
static ASCII_code *str_pool;
static pool_pointer *str_start;
static pool_pointer pool_ptr;
static str_number str_ptr;
static str_number str_num;
static pool_pointer p_ptr1, p_ptr2;
static hash_pointer *hash_next;
static str_number *hash_text;
static str_ilk *hash_ilk;
static integer *ilk_info;
static integer hash_used;
static boolean hash_found;
static hash_loc dummy_loc;
static str_number s_aux_extension;
static str_number s_log_extension;
static str_number s_bbl_extension;
static str_number s_bst_extension;
static str_number s_bib_extension;
static str_number s_bst_area;
static str_number s_bib_area;
static hash_loc pre_def_loc;
static integer command_num;
static buf_pointer buf_ptr1;
static buf_pointer buf_ptr2;
static unsigned char /*white_adjacent */ scan_result;
static integer token_value;
static integer aux_name_length;
static FILE *aux_file[aux_stack_size + 1];
static str_number aux_list[aux_stack_size + 1];
static aux_number aux_ptr;
static integer aux_ln_stack[aux_stack_size + 1];
static str_number top_lev_str;
static FILE *log_file;
static FILE *bbl_file;
static str_number *bib_list;
static bib_number bib_ptr;
static bib_number num_bib_files;
static boolean bib_seen;
static FILE **bib_file;
static boolean bst_seen;
static str_number bst_str;
static FILE *bst_file;
static str_number *cite_list;
static cite_number cite_ptr;
static cite_number entry_cite_ptr;
static cite_number num_cites;
static cite_number old_num_cites;
static boolean citation_seen;
static hash_loc cite_loc;
static hash_loc lc_cite_loc;
static hash_loc lc_xcite_loc;
static boolean cite_found;
static boolean all_entries;
static cite_number all_marker;
static integer bbl_line_num;
static integer bst_line_num;
static hash_loc fn_loc;
static hash_loc wiz_loc;
static hash_loc literal_loc;
static hash_loc macro_name_loc;
static hash_loc macro_def_loc;
static fn_class *fn_type;
static wiz_fn_loc wiz_def_ptr;
static wiz_fn_loc wiz_fn_ptr;
static hash_ptr2 *wiz_functions;
static int_ent_loc int_ent_ptr;
static integer *entry_ints;
static int_ent_loc num_ent_ints;
static str_ent_loc str_ent_ptr;
static ASCII_code *entry_strs;
static str_ent_loc num_ent_strs;
static integer str_glb_ptr;
static str_number *glb_str_ptr;
static ASCII_code *global_strs;
static integer *glb_str_end;
static integer num_glb_strs;
static field_loc field_ptr;
static field_loc field_parent_ptr, field_end_ptr;
static cite_number cite_parent_ptr, cite_xptr;
static str_number *field_info;
static field_loc num_fields;
static field_loc num_pre_defined_fields;
static field_loc crossref_num;
static boolean no_fields;
static boolean entry_seen;
static boolean read_seen;
static boolean read_performed;
static boolean reading_completed;
static boolean read_completed;
static integer impl_fn_num;
static integer bib_line_num;
static hash_loc entry_type_loc;
static hash_ptr2 *type_list;
static boolean type_exists;
static boolean *entry_exists;
static boolean store_entry;
static hash_loc field_name_loc;
static hash_loc field_val_loc;
static boolean store_field;
static boolean store_token;
static ASCII_code right_outer_delim;
static ASCII_code right_str_delim;
static boolean at_bib_command;
static hash_loc cur_macro_loc;
static str_number *cite_info;
static boolean cite_hash_found;
static bib_number preamble_ptr;
static bib_number num_preamble_strings;
static integer bib_brace_level;
static integer *lit_stack;
static stk_type *lit_stk_type;
static lit_stk_loc lit_stk_ptr;
static str_number cmd_str_ptr;
static integer ent_chr_ptr;
static integer glob_chr_ptr;
static buf_type ex_buf;
static buf_pointer ex_buf_ptr;
static buf_pointer ex_buf_length;
static buf_type out_buf;
static buf_pointer out_buf_ptr;
static buf_pointer out_buf_length;
static boolean mess_with_entries;
static cite_number sort_cite_ptr;
static str_ent_loc sort_key_num;
static integer brace_level;
static hash_loc b_equals;
static hash_loc b_greater_than;
static hash_loc b_less_than;
static hash_loc b_plus;
static hash_loc b_minus;
static hash_loc b_concatenate;
static hash_loc b_gets;
static hash_loc b_add_period;
static hash_loc b_call_type;
static hash_loc b_change_case;
static hash_loc b_chr_to_int;
static hash_loc b_cite;
static hash_loc b_duplicate;
static hash_loc b_empty;
static hash_loc b_format_name;
static hash_loc b_if;
static hash_loc b_int_to_chr;
static hash_loc b_int_to_str;
static hash_loc b_missing;
static hash_loc b_newline;
static hash_loc b_num_names;
static hash_loc b_pop;
static hash_loc b_preamble;
static hash_loc b_purify;
static hash_loc b_quote;
static hash_loc b_skip;
static hash_loc b_stack;
static hash_loc b_substring;
static hash_loc b_swap;
static hash_loc b_text_length;
static hash_loc b_text_prefix;
static hash_loc b_top_stack;
static hash_loc b_type;
static hash_loc b_warning;
static hash_loc b_while;
static hash_loc b_width;
static hash_loc b_write;
static hash_loc b_default;

static str_number s_null;
static str_number s_default;
static str_number s_t;
static str_number s_l;
static str_number s_u;
static str_number *s_preamble;
static integer pop_lit1, pop_lit2, pop_lit3;
static stk_type pop_typ1, pop_typ2, pop_typ3;
static pool_pointer sp_ptr;
static pool_pointer sp_xptr1, sp_xptr2;
static pool_pointer sp_end;
static pool_pointer sp_length, sp2_length;
static integer sp_brace_level;
static buf_pointer ex_buf_xptr, ex_buf_yptr;
static hash_loc control_seq_loc;
static boolean preceding_white;
static boolean and_found;
static integer num_names;
static buf_pointer name_bf_ptr;
static buf_pointer name_bf_xptr, name_bf_yptr;
static integer nm_brace_level;
static buf_pointer *name_tok;
static ASCII_code *name_sep_char;
static buf_pointer num_tokens;
static boolean token_starting;
static boolean alpha_found;
static boolean double_letter, end_of_group, to_be_written;
static buf_pointer first_start;
static buf_pointer first_end;
static buf_pointer last_end;
static buf_pointer von_start;
static buf_pointer von_end;
static buf_pointer jr_end;
static buf_pointer cur_token, last_token;
static boolean use_default;
static buf_pointer num_commas;
static buf_pointer comma1, comma2;
static buf_pointer num_text_chars;
static unsigned char /*bad_conversion */ conversion_type;
static boolean prev_colon;
static int verbose;
static integer min_crossrefs;

/*:473*//*12: *//*3: */

static void
putc_log(const int c)
{
    putc(c, log_file);
    putc(c, standard_output);
}

static void
puts_log(const char *s)
{
    fputs(s, log_file);
    fputs(s, standard_output);
}

PRINTF_FUNC(1,2) static void
printf_log(const char *fmt, ...)
{
    va_list ap;

    va_start (ap, fmt);
    vfprintf(log_file, fmt, ap);
    vfprintf(standard_output, fmt, ap);
    va_end (ap);
}


static void
mark_warning(void)
{
    if (history == HISTORY_WARNING_ISSUED)
        err_count++;
    else if (history == HISTORY_SPOTLESS) {
        history = HISTORY_WARNING_ISSUED;
        err_count = 1;
    }
}

static void
mark_error(void)
{
    if (history < HISTORY_ERROR_ISSUED) {
        history = HISTORY_ERROR_ISSUED;
        err_count = 1;
    } else
        err_count++;
}

static void
mark_fatal(void)
{
    history = HISTORY_FATAL_ERROR;
}

static void
print_overflow(void)
{
    puts_log("Sorry---you've exceeded BibTeX's ");
    mark_fatal();
}

static void
print_confusion(void)
{
    puts_log("---this can't happen\n");
    puts_log("*Please notify the BibTeX maintainer*\n");
    mark_fatal();
}

static void
buffer_overflow(void)
{
    BIB_XRETALLOC_NOSET("buffer", buffer, ASCII_code, buf_size, buf_size + BUF_SIZE);
    BIB_XRETALLOC_NOSET("sv_buffer", sv_buffer, ASCII_code, buf_size, buf_size + BUF_SIZE);
    BIB_XRETALLOC_NOSET("ex_buf", ex_buf, ASCII_code, buf_size, buf_size + BUF_SIZE);
    BIB_XRETALLOC_NOSET("out_buf", out_buf, ASCII_code, buf_size, buf_size + BUF_SIZE);
    BIB_XRETALLOC_NOSET("name_tok", name_tok, buf_pointer, buf_size, buf_size + BUF_SIZE);
    BIB_XRETALLOC("name_sep_char", name_sep_char, ASCII_code, buf_size, buf_size + BUF_SIZE);
}


static boolean
input_ln(FILE *f)
{
    last = 0; /* note: global! */

    if (eof(f))
        return false;

    while (!eoln(f)) {
        if (last >= buf_size)
            buffer_overflow();

        buffer[last] = getc(f);
        last++;
    }

    getc(f);

    while (last > 0) {
        if (lex_class[buffer[last - 1]] == 1 /*white_space */ )
            last--;
        else
            break;
    }

    return true;
}


static void
out_pool_str(FILE *f, str_number s)
{
    pool_pointer i;

    if (s < 0 || s >= str_ptr + 3 || s >= max_strings) {
        printf_log("Illegal string number:%ld", (long) s);
        print_confusion();
        longjmp(error_jmpbuf, 1);
    }

    for (i = str_start[s]; i < str_start[s + 1]; i++)
        putc(str_pool[i], f);
}


static void
print_a_pool_str(str_number s)
{
    out_pool_str(standard_output, s);
    out_pool_str(log_file, s);
}


static void
pool_overflow(void)
{
    BIB_XRETALLOC("str_pool", str_pool, ASCII_code, pool_size, pool_size + POOL_SIZE);
}


static void
out_token(FILE *f)
{
    buf_pointer i = buf_ptr1;

    while (i < buf_ptr2) {
        putc(buffer[i], f);
        i++;
    }
}


static void
print_a_token(void)
{
    out_token(standard_output);
    out_token(log_file);
}

static void
print_bad_input_line(void)
{
    buf_pointer bf_ptr;

    puts_log(" : ");
    bf_ptr = 0;

    while ((bf_ptr < buf_ptr2)) {

        if ((lex_class[buffer[bf_ptr]] == 1 /*white_space */ )) {
            putc_log(' ');
        } else {
            putc_log(buffer[bf_ptr]);
        }
        bf_ptr = bf_ptr + 1;
    }

    putc_log('\n');
    puts_log(" : ");
    bf_ptr = 0;

    while ((bf_ptr < buf_ptr2)) {
        putc_log(' ');
        bf_ptr = bf_ptr + 1;
    }
    bf_ptr = buf_ptr2;
    while ((bf_ptr < last)) {

        if ((lex_class[buffer[bf_ptr]] == 1 /*white_space */ )) {
            putc_log(' ');
        } else {
            putc_log(buffer[bf_ptr]);
        }
        bf_ptr = bf_ptr + 1;
    }
    putc_log('\n');
    bf_ptr = 0;
    while (((bf_ptr < buf_ptr2) && (lex_class[buffer[bf_ptr]] == 1 /*white_space */ )))
        bf_ptr = bf_ptr + 1;
    if ((bf_ptr == buf_ptr2)) {
        puts_log("(Error may have been on previous line)\n");
    }
    mark_error();
}

static void
print_skipping_whatever_remains(void)
{
    puts_log("I'm skipping whatever remains of this ");
}

static void
sam_too_long_file_name_print(void)
{
    fputs("File name `", standard_output);
    name_ptr = 1;
    while ((name_ptr <= aux_name_length)) {

        putc(name_of_file[name_ptr], standard_output);
        name_ptr = name_ptr + 1;
    }
    fprintf(standard_output, "%s\n", "' is too long");
}

static void
sam_wrong_file_name_print(void)
{
    fputs("I couldn't open file name `", standard_output);
    name_ptr = 1;
    while ((name_ptr <= name_length)) {

        putc(name_of_file[name_ptr], standard_output);
        name_ptr = name_ptr + 1;
    }
    {
        putc('\'', standard_output);
        putc('\n', standard_output);
    }
}

void print_aux_name(void)
{
    print_a_pool_str(aux_list[aux_ptr]);
    putc_log('\n');
}

void log_pr_aux_name(void)
{
    {
        out_pool_str(log_file, aux_list[aux_ptr]);
    }
    {
        putc('\n', log_file);
    }
}

void aux_err_print(void)
{
    printf_log("---line %ld of file ", (long) aux_ln_stack[aux_ptr]);
    print_aux_name();
    print_bad_input_line();
    print_skipping_whatever_remains();
    puts_log("command\n");
}

void aux_err_illegal_another_print(integer cmd_num)
{
    puts_log("Illegal, another \\bib");

    switch ((cmd_num)) {
    case 0:
        puts_log("data");
        break;
    case 1:
        puts_log("style");
        break;
    default:
        puts_log("Illegal auxiliary-file command");
        print_confusion();
        longjmp(error_jmpbuf, 1);
        break;
    }

    puts_log(" command");
}

void aux_err_no_right_brace_print(void)
{
    puts_log("No \"}\"");
}

void aux_err_stuff_after_right_brace_print(void)
{
    puts_log("Stuff after \"}\"");
}

void aux_err_white_space_in_argument_print(void)
{
    puts_log("White space in argument");
}

void print_bib_name(void)
{
    print_a_pool_str(bib_list[bib_ptr]);
    print_a_pool_str(s_bib_extension);
    putc_log('\n');
}

void log_pr_bib_name(void)
{
    {
        out_pool_str(log_file, bib_list[bib_ptr]);
    }
    {
        out_pool_str(log_file, s_bib_extension);
    }
    {
        putc('\n', log_file);
    }
}

void print_bst_name(void)
{
    print_a_pool_str(bst_str);
    print_a_pool_str(s_bst_extension);
    putc_log('\n');
}

void log_pr_bst_name(void)
{
    {
        out_pool_str(log_file, bst_str);
    }
    {
        out_pool_str(log_file, s_bst_extension);
    }
    {
        putc('\n', log_file);
    }
}

void hash_cite_confusion(void)
{
    puts_log("Cite hash error");
    print_confusion();
    longjmp(error_jmpbuf, 1);
}

void check_cite_overflow(cite_number last_cite)
{
    if ((last_cite == max_cites)) {
        BIB_XRETALLOC_NOSET("cite_list", cite_list, str_number, max_cites, max_cites + MAX_CITES);
        BIB_XRETALLOC_NOSET("type_list", type_list, hash_ptr2, max_cites, max_cites + MAX_CITES);
        BIB_XRETALLOC_NOSET("entry_exists", entry_exists, boolean, max_cites, max_cites + MAX_CITES);
        BIB_XRETALLOC("cite_info", cite_info, str_number, max_cites, max_cites + MAX_CITES);
        while ((last_cite < max_cites)) {

            type_list[last_cite] = 0 /*empty */ ;
            cite_info[last_cite] = 0 /*any_value */ ;
            last_cite = last_cite + 1;
        }
    }
}

void aux_end1_err_print(void)
{
    puts_log("I found no ");
}

void aux_end2_err_print(void)
{
    puts_log("---while reading file ");
    print_aux_name();
    mark_error();
}

void bst_ln_num_print(void)
{
    printf_log("--line %ld of file ", (long) bst_line_num);
    print_bst_name();
}

void bst_err_print_and_look_for_blank_line(void)
{
    putc_log('-');
    bst_ln_num_print();
    print_bad_input_line();
    while ((last != 0))
        if ((!input_ln(bst_file)))
            longjmp(recover_jmpbuf, 1);
        else
            bst_line_num = bst_line_num + 1;
    buf_ptr2 = last;
}

void bst_warn_print(void)
{
    bst_ln_num_print();
    mark_warning();
}

void eat_bst_print(void)
{
    puts_log("Illegal end of style file in command: ");
}

void unknwn_function_class_confusion(void)
{
    puts_log("Unknown function class");
    print_confusion();
    longjmp(error_jmpbuf, 1);
}

void print_fn_class(hash_loc fn_loc)
{
    switch ((fn_type[fn_loc])) {
    case 0:
        puts_log("built-in");
        break;
    case 1:
        puts_log("wizard-defined");
        break;
    case 2:
        puts_log("integer-literal");
        break;
    case 3:
        puts_log("string-literal");
        break;
    case 4:
        puts_log("field");
        break;
    case 5:
        puts_log("integer-entry-variable");
        break;
    case 6:
        puts_log("string-entry-variable");
        break;
    case 7:
        puts_log("integer-global-variable");
        break;
    case 8:
        puts_log("string-global-variable");
        break;
    default:
        unknwn_function_class_confusion();
        break;
    }
}

/*:159*//*160: */

void id_scanning_confusion(void)
{
    puts_log("Identifier scanning error");
    print_confusion();
    longjmp(error_jmpbuf, 1);
}

void bst_id_print(void)
{
    if ((scan_result == 0 /*id_null */ )) {
        printf_log("\"%c\" begins identifier, command: ", buffer[buf_ptr2]);
    } else if ((scan_result == 2 /*other_char_adjacent */ )) {
        printf_log("\"%c\" immediately follows identifier, command: ", buffer[buf_ptr2]);
    } else
        id_scanning_confusion();
}

void bst_left_brace_print(void)
{
    puts_log("\"{\" is missing in command: ");
}

void bst_right_brace_print(void)
{
    puts_log("\"}\" is missing in command: ");
}

void already_seen_function_print(hash_loc seen_fn_loc)
{
    print_a_pool_str(hash_text[seen_fn_loc]);
    puts_log(" is already a type \"");
    print_fn_class(seen_fn_loc);
    puts_log("\" function name\n");
    bst_err_print_and_look_for_blank_line();
}

void bib_ln_num_print(void)
{
    printf_log("--line %ld of file", (long) bib_line_num);
    print_bib_name();
}

void bib_err_print(void)
{
    putc_log('-');
    bib_ln_num_print();
    print_bad_input_line();
    print_skipping_whatever_remains();
    if ((at_bib_command)) {
        puts_log("command\n");
    } else {
        puts_log("entry\n");
    }
}

void bib_warn_print(void)
{
    bib_ln_num_print();
    mark_warning();
}

void check_field_overflow(integer total_fields)
{
    field_loc f_ptr;
    field_loc start_fields;
    if ((total_fields > max_fields)) {
        start_fields = max_fields;
        BIB_XRETALLOC("field_info", field_info, str_number, max_fields, total_fields + MAX_FIELDS);
        {
            register integer for_end;
            f_ptr = start_fields;
            for_end = max_fields - 1;
            if (f_ptr <= for_end)
                do {
                    field_info[f_ptr] = 0 /*missing */ ;
                }
                while (f_ptr++ < for_end);
        }
    }
}

void eat_bib_print(void)
{
    puts_log("Illegal end of database file");
    bib_err_print();
}

void bib_one_of_two_print(ASCII_code char1, ASCII_code char2)
{
    printf_log("I was expecting a `%c' or a `%c'", char1, char2);
    bib_err_print();
}

void bib_equals_sign_print(void)
{
    printf_log("I was expecting an \"=\"");
    bib_err_print();
}

void bib_unbalanced_braces_print(void)
{
    puts_log("Unbalanced braces");
    bib_err_print();
}

void bib_field_too_long_print(void)
{
    printf_log("Your field is more than %ld characters", (long) buf_size);
    bib_err_print();
}

void macro_warn_print(void)
{
    puts_log("Warning--string name \"");
    print_a_token();
    puts_log("\" is ");
}

void bib_id_print(void)
{
    if ((scan_result == 0 /*id_null */ )) {
        puts_log("You're missing ");
    } else if ((scan_result == 2 /*other_char_adjacent */ )) {
        printf_log("\"%c\" immediately follows ", buffer[buf_ptr2]);
    } else
        id_scanning_confusion();
}

void bib_cmd_confusion(void)
{
    puts_log("Unknown database-file command");
    print_confusion();
    longjmp(error_jmpbuf, 1);
}

void cite_key_disappeared_confusion(void)
{
    puts_log("A cite key disappeared");
    print_confusion();
    longjmp(error_jmpbuf, 1);
}

void bad_cross_reference_print(str_number s)
{
    puts_log("--entry \"");
    print_a_pool_str(cite_list[cite_ptr]);
    putc_log('"');
    putc_log('\n');
    puts_log("refers to entry \"");
    print_a_pool_str(s);
}

void nonexistent_cross_reference_error(void)
{
    puts_log("A bad cross reference-");
    bad_cross_reference_print(field_info[field_ptr]);
    puts_log("\", which doesn't exist\n");
    mark_error();
}

void print_missing_entry(str_number s)
{
    puts_log("Warning--I didn't find a database entry for \"");
    print_a_pool_str(s);
    putc_log('"');
    putc_log('\n');
    mark_warning();
}

void bst_ex_warn_print(void)
{
    if ((mess_with_entries)) {
        puts_log(" for entry ");
        print_a_pool_str(cite_list[cite_ptr]);
    }

    putc_log('\n');
    puts_log("while executing-");
    bst_ln_num_print();
    mark_error();
}

void bst_mild_ex_warn_print(void)
{
    if ((mess_with_entries)) {
        puts_log(" for entry ");
        print_a_pool_str(cite_list[cite_ptr]);
    }

    putc_log('\n');
    puts_log("while executing");
    bst_warn_print();
}

void bst_cant_mess_with_entries_print(void)
{
    puts_log("You can't mess with entries here");
    bst_ex_warn_print();
}

void illegl_literal_confusion(void)
{
    puts_log("Illegal literal type");
    print_confusion();
    longjmp(error_jmpbuf, 1);
}

void unknwn_literal_confusion(void)
{
    puts_log("Unknown literal type");
    print_confusion();
    longjmp(error_jmpbuf, 1);
}

void print_stk_lit(integer stk_lt, stk_type stk_tp)
{
    switch ((stk_tp)) {
    case 0:
        printf_log("%ld is an integer literal", (long) stk_lt);
        break;
    case 1:
        putc_log('"');
        print_a_pool_str(stk_lt);
        puts_log("\" is a string literal");
        break;
    case 2:
        putc_log('`');
        print_a_pool_str(hash_text[stk_lt]);
        puts_log("' is a function literal");
        break;
    case 3:
        putc_log('`');
        print_a_pool_str(stk_lt);
        puts_log("' is a missing field");
        break;
    case 4:
        illegl_literal_confusion();
        break;
    default:
        unknwn_literal_confusion();
        break;
    }
}

void print_lit(integer stk_lt, stk_type stk_tp)
{
    switch ((stk_tp)) {
    case 0:
        printf_log("%ld\n", (long) stk_lt);
        break;
    case 1:
        {
            print_a_pool_str(stk_lt);
            putc_log('\n');
        }
        break;
    case 2:
        {
            print_a_pool_str(hash_text[stk_lt]);
            putc_log('\n');
        }
        break;
    case 3:
        {
            print_a_pool_str(stk_lt);
            putc_log('\n');
        }
        break;
    case 4:
        illegl_literal_confusion();
        break;
    default:
        unknwn_literal_confusion();
        break;
    }
}

void output_bbl_line(void)
{
    if ((out_buf_length != 0)) {
        while ((out_buf_length > 0))
            if ((lex_class[out_buf[out_buf_length - 1]] == 1 /*white_space */ ))
                out_buf_length = out_buf_length - 1;
            else
                goto loop_exit;
 loop_exit:
        if ((out_buf_length == 0))
            goto exit;
        out_buf_ptr = 0;
        while ((out_buf_ptr < out_buf_length)) {

            putc(out_buf[out_buf_ptr], bbl_file);
            out_buf_ptr = out_buf_ptr + 1;
        }
    }
    putc('\n', bbl_file);
    bbl_line_num = bbl_line_num + 1;
    out_buf_length = 0;
 exit: ;
}

void bst_1print_string_size_exceeded(void)
{
    puts_log("Warning--you've exceeded ");
}

void bst_2print_string_size_exceeded(void)
{
    puts_log("-string-size,");
    bst_mild_ex_warn_print();
    puts_log("*Please notify the bibstyle designer*\n");
}

void braces_unbalanced_complaint(str_number pop_lit_var)
{
    puts_log("Warning--\"");
    print_a_pool_str(pop_lit_var);
    puts_log("\" isn't a brace-balanced string");
    bst_mild_ex_warn_print();
}

void case_conversion_confusion(void)
{
    puts_log("Unknown type of case conversion");
    print_confusion();
    longjmp(error_jmpbuf, 1);
}

void start_name(str_number file_name)
{
    pool_pointer p_ptr;
    free(name_of_file);
    name_of_file = xmalloc_array(ASCII_code, (str_start[file_name + 1] - str_start[file_name]) + 1);
    name_ptr = 1;
    p_ptr = str_start[file_name];
    while ((p_ptr < str_start[file_name + 1])) {

        name_of_file[name_ptr] = str_pool[p_ptr];
        name_ptr = name_ptr + 1;
        p_ptr = p_ptr + 1;
    }
    name_length = (str_start[file_name + 1] - str_start[file_name]);
    name_of_file[name_length + 1] = 0;
}

void add_extension(str_number ext)
{
    pool_pointer p_ptr;
    name_ptr = name_length + 1;
    p_ptr = str_start[ext];
    while ((p_ptr < str_start[ext + 1])) {

        name_of_file[name_ptr] = str_pool[p_ptr];
        name_ptr = name_ptr + 1;
        p_ptr = p_ptr + 1;
    }
    name_length = name_length + (str_start[ext + 1] - str_start[ext]);
    name_of_file[name_length + 1] = 0;
}

static str_number make_string(void)
{
    register str_number Result;
    if ((str_ptr == max_strings)) {
        print_overflow();
        printf_log("number of strings %ld\n", (long) max_strings);
        longjmp(error_jmpbuf, 1);
    }
    str_ptr = str_ptr + 1;
    str_start[str_ptr] = pool_ptr;
    Result = str_ptr - 1;
    return Result;
}

static boolean str_eq_buf(str_number s, buf_type buf, buf_pointer bf_ptr, buf_pointer len)
{
    register boolean Result;
    buf_pointer i;
    pool_pointer j;
    if (((str_start[s + 1] - str_start[s]) != len)) {
        Result = false;
        goto exit;
    }
    i = bf_ptr;
    j = str_start[s];
    while ((j < str_start[s + 1])) {

        if ((str_pool[j] != buf[i])) {
            Result = false;
            goto exit;
        }
        i = i + 1;
        j = j + 1;
    }
    Result = true;
 exit: ;
    return Result;
}

static boolean str_eq_str(str_number s1, str_number s2)
{
    register boolean Result;
    if (((str_start[s1 + 1] - str_start[s1]) != (str_start[s2 + 1] - str_start[s2]))) {
        Result = false;
        goto exit;
    }
    p_ptr1 = str_start[s1];
    p_ptr2 = str_start[s2];
    while ((p_ptr1 < str_start[s1 + 1])) {

        if ((str_pool[p_ptr1] != str_pool[p_ptr2])) {
            Result = false;
            goto exit;
        }
        p_ptr1 = p_ptr1 + 1;
        p_ptr2 = p_ptr2 + 1;
    }
    Result = true;
 exit: ;
    return Result;
}

void lower_case(buf_type buf, buf_pointer bf_ptr, buf_pointer len)
{
    buf_pointer i;
    if ((len > 0)) {
        register integer for_end;
        i = bf_ptr;
        for_end = bf_ptr + len - 1;
        if (i <= for_end)
            do
                if (((buf[i] >= 65 /*"A" */ ) && (buf[i] <= 90 /*"Z" */ )))
                    buf[i] = buf[i] + 32;
            while (i++ < for_end) ;
    }
}

void upper_case(buf_type buf, buf_pointer bf_ptr, buf_pointer len)
{
    buf_pointer i;
    if ((len > 0)) {
        register integer for_end;
        i = bf_ptr;
        for_end = bf_ptr + len - 1;
        if (i <= for_end)
            do
                if (((buf[i] >= 97 /*"a" */ ) && (buf[i] <= 122 /*"z" */ )))
                    buf[i] = buf[i] - 32;
            while (i++ < for_end) ;
    }
}

hash_loc str_lookup(buf_type buf, buf_pointer j, buf_pointer l, str_ilk ilk, boolean insert_it)
{
    register hash_loc Result;
    integer h;
    hash_loc p;
    buf_pointer k;
    str_number str_num;
    {
        h = 0;
        k = j;
        while ((k < j + l)) {

            h = h + h + buf[k];
            while ((h >= hash_prime))
                h = h - hash_prime;
            k = k + 1;
        }
    }
    p = h + hash_base;
    hash_found = false;
    str_num = 0;
    while (true) {

        {
            if ((hash_text[p] > 0)) {

                if ((str_eq_buf(hash_text[p], buf, j, l))) {

                    if ((hash_ilk[p] == ilk)) {
                        hash_found = true;
                        goto lab40;
                    } else {

                        str_num = hash_text[p];
                    }
                }
            }
        }
        if ((hash_next[p] == 0 /*empty */ )) {
            if ((!insert_it))
                goto lab45;
            {
                if ((hash_text[p] > 0)) {
                    do {
                        if (((hash_used == hash_base))) {
                            print_overflow();
                            printf_log("hash size %ld\n", (long) hash_size);
                            longjmp(error_jmpbuf, 1);
                        }
                        hash_used = hash_used - 1;
                    } while (!((hash_text[hash_used] == 0)));
                    hash_next[p] = hash_used;
                    p = hash_used;
                }
                if ((str_num > 0))
                    hash_text[p] = str_num;
                else {

                    {
                        while ((pool_ptr + l > pool_size))
                            pool_overflow();
                    }
                    k = j;
                    while ((k < j + l)) {

                        {
                            str_pool[pool_ptr] = buf[k];
                            pool_ptr = pool_ptr + 1;
                        }
                        k = k + 1;
                    }
                    hash_text[p] = make_string();
                }
                hash_ilk[p] = ilk;
            }
            goto lab40;
        }
        p = hash_next[p];
    }
 lab45:                        /*str_not_found */ ;
 lab40:                        /*str_found */ Result = p;
    return Result;
}

void pre_define(pds_type pds, pds_len len, str_ilk ilk)
{
    pds_len i;
    {
        register integer for_end;
        i = 1;
        for_end = len;
        if (i <= for_end)
            do
                buffer[i] = (unsigned char) pds[i - 1];
            while (i++ < for_end);
    }
    pre_def_loc = str_lookup(buffer, 1, len, ilk, true);
}

void int_to_ASCII(integer the_int, buf_type int_buf, buf_pointer int_begin, buf_pointer * int_end)
{
    buf_pointer int_ptr, int_xptr;
    ASCII_code int_tmp_val;
    int_ptr = int_begin;
    if ((the_int < 0)) {
        {
            if ((int_ptr == buf_size))
                buffer_overflow();
            int_buf[int_ptr] = 45 /*minus_sign */ ;
            int_ptr = int_ptr + 1;
        }
        the_int = -(integer) the_int;
    }
    int_xptr = int_ptr;
    do {
        {
            if ((int_ptr == buf_size))
                buffer_overflow();
            int_buf[int_ptr] = 48 /*"0" */  + (the_int % 10);
            int_ptr = int_ptr + 1;
        }
        the_int = the_int / 10;
    } while (!((the_int == 0)));
    *int_end = int_ptr;
    int_ptr = int_ptr - 1;
    while ((int_xptr < int_ptr)) {

        int_tmp_val = int_buf[int_xptr];
        int_buf[int_xptr] = int_buf[int_ptr];
        int_buf[int_ptr] = int_tmp_val;
        int_ptr = int_ptr - 1;
        int_xptr = int_xptr + 1;
    }
}

void add_database_cite(cite_number * new_cite)
{
    check_cite_overflow(*new_cite);
    check_field_overflow(num_fields * (*new_cite + 1));
    cite_list[*new_cite] = hash_text[cite_loc];
    ilk_info[cite_loc] = *new_cite;
    ilk_info[lc_cite_loc] = cite_loc;
    *new_cite = *new_cite + 1;
}

boolean find_cite_locs_for_this_cite_key(str_number cite_str)
{
    register boolean Result;
    ex_buf_ptr = 0;
    tmp_ptr = str_start[cite_str];
    tmp_end_ptr = str_start[cite_str + 1];
    while ((tmp_ptr < tmp_end_ptr)) {

        ex_buf[ex_buf_ptr] = str_pool[tmp_ptr];
        ex_buf_ptr = ex_buf_ptr + 1;
        tmp_ptr = tmp_ptr + 1;
    }
    cite_loc = str_lookup(ex_buf, 0, (str_start[cite_str + 1] - str_start[cite_str]), 9 /*cite_ilk */ , false);
    cite_hash_found = hash_found;
    lower_case(ex_buf, 0, (str_start[cite_str + 1] - str_start[cite_str]));
    lc_cite_loc = str_lookup(ex_buf, 0, (str_start[cite_str + 1] - str_start[cite_str]), 10 /*lc_cite_ilk */ , false);
    if ((hash_found))
        Result = true;
    else
        Result = false;
    return Result;
}

void swap(cite_number swap1, cite_number swap2)
{
    cite_number innocent_bystander;
    innocent_bystander = cite_info[swap2];
    cite_info[swap2] = cite_info[swap1];
    cite_info[swap1] = innocent_bystander;
}

boolean less_than(cite_number arg1, cite_number arg2)
{
    register boolean Result;
    integer char_ptr;
    str_ent_loc ptr1, ptr2;
    ASCII_code char1, char2;
    ptr1 = arg1 * num_ent_strs + sort_key_num;
    ptr2 = arg2 * num_ent_strs + sort_key_num;
    char_ptr = 0;
    while (true) {

        char1 = entry_strs[(ptr1) * (ent_str_size + 1) + (char_ptr)];
        char2 = entry_strs[(ptr2) * (ent_str_size + 1) + (char_ptr)];
        if ((char1 == 127 /*end_of_string */ )) {

            if ((char2 == 127 /*end_of_string */ )) {

                if ((arg1 < arg2)) {
                    Result = true;
                    goto exit;
                } else if ((arg1 > arg2)) {
                    Result = false;
                    goto exit;
                } else {
                    puts_log("Duplicate sort key");
                    print_confusion();
                    longjmp(error_jmpbuf, 1);
                }
            } else {

                Result = true;
                goto exit;
            }
        } else if ((char2 == 127 /*end_of_string */ )) {
            Result = false;
            goto exit;
        } else if ((char1 < char2)) {
            Result = true;
            goto exit;
        } else if ((char1 > char2)) {
            Result = false;
            goto exit;
        }
        char_ptr = char_ptr + 1;
    }
 exit: ;
    return Result;
}

void quick_sort(cite_number left_end, cite_number right_end)
{
    cite_number left, right;
    cite_number insert_ptr;
    cite_number middle;
    cite_number partition;
    ;

    if ((right_end - left_end < 10 /*short_list */ )) { /*305: */
        {
            register integer for_end;
            insert_ptr = left_end + 1;
            for_end = right_end;
            if (insert_ptr <= for_end)
                do {
                    {
                        register integer for_end;
                        right = insert_ptr;
                        for_end = left_end + 1;
                        if (right >= for_end)
                            do {
                                if ((less_than(cite_info[right - 1], cite_info[right])))
                                    goto lab24;
                                swap(right - 1, right);
                            }
                            while (right-- > for_end);
                    }
 lab24:                        /*next_insert */ ;
                }
                while (insert_ptr++ < for_end);
        }
    } else {

        {
            left = left_end + 4;
            middle = (left_end + right_end) / 2;
            right = right_end - 4;
            if ((less_than(cite_info[left], cite_info[middle]))) {

                if ((less_than(cite_info[middle], cite_info[right])))
                    swap(left_end, middle);
                else if ((less_than(cite_info[left], cite_info[right])))
                    swap(left_end, right);
                else
                    swap(left_end, left);
            } else if ((less_than(cite_info[right], cite_info[middle])))
                swap(left_end, middle);
            else if ((less_than(cite_info[right], cite_info[left])))
                swap(left_end, right);
            else
                swap(left_end, left);
        }
        {
            partition = cite_info[left_end];
            left = left_end + 1;
            right = right_end;
            do {
                while ((less_than(cite_info[left], partition)))
                    left = left + 1;
                while ((less_than(partition, cite_info[right])))
                    right = right - 1;
                if ((left < right)) {
                    swap(left, right);
                    left = left + 1;
                    right = right - 1;
                }
            } while (!((left == right + 1)));
            swap(left_end, right);
            quick_sort(left_end, right - 1);
            quick_sort(left, right_end);
        }
    }
}

void build_in(pds_type pds, pds_len len, hash_loc * fn_hash_loc, blt_in_range blt_in_num)
{
    pre_define(pds, len, 11 /*bst_fn_ilk */ );
    *fn_hash_loc = pre_def_loc;
    fn_type[*fn_hash_loc] = 0 /*built_in */ ;
    ilk_info[*fn_hash_loc] = blt_in_num;
    ;

}

void pre_def_certain_strings(void)
{
    pre_define(".aux        ", 4, 7 /*file_ext_ilk */ );
    s_aux_extension = hash_text[pre_def_loc];
    pre_define(".bbl        ", 4, 7 /*file_ext_ilk */ );
    s_bbl_extension = hash_text[pre_def_loc];
    pre_define(".blg        ", 4, 7 /*file_ext_ilk */ );
    s_log_extension = hash_text[pre_def_loc];
    pre_define(".bst        ", 4, 7 /*file_ext_ilk */ );
    s_bst_extension = hash_text[pre_def_loc];
    pre_define(".bib        ", 4, 7 /*file_ext_ilk */ );
    s_bib_extension = hash_text[pre_def_loc];
    pre_define("texinputs:  ", 10, 8 /*file_area_ilk */ );
    s_bst_area = hash_text[pre_def_loc];
    pre_define("texbib:     ", 7, 8 /*file_area_ilk */ );
    s_bib_area = hash_text[pre_def_loc];
    pre_define("\\citation   ", 9, 2 /*aux_command_ilk */ );
    ilk_info[pre_def_loc] = 2 /*n_aux_citation */ ;
    pre_define("\\bibdata    ", 8, 2 /*aux_command_ilk */ );
    ilk_info[pre_def_loc] = 0 /*n_aux_bibdata */ ;
    pre_define("\\bibstyle   ", 9, 2 /*aux_command_ilk */ );
    ilk_info[pre_def_loc] = 1 /*n_aux_bibstyle */ ;
    pre_define("\\@input     ", 7, 2 /*aux_command_ilk */ );
    ilk_info[pre_def_loc] = 3 /*n_aux_input */ ;
    pre_define("entry       ", 5, 4 /*bst_command_ilk */ );
    ilk_info[pre_def_loc] = 0 /*n_bst_entry */ ;
    pre_define("execute     ", 7, 4 /*bst_command_ilk */ );
    ilk_info[pre_def_loc] = 1 /*n_bst_execute */ ;
    pre_define("function    ", 8, 4 /*bst_command_ilk */ );
    ilk_info[pre_def_loc] = 2 /*n_bst_function */ ;
    pre_define("integers    ", 8, 4 /*bst_command_ilk */ );
    ilk_info[pre_def_loc] = 3 /*n_bst_integers */ ;
    pre_define("iterate     ", 7, 4 /*bst_command_ilk */ );
    ilk_info[pre_def_loc] = 4 /*n_bst_iterate */ ;
    pre_define("macro       ", 5, 4 /*bst_command_ilk */ );
    ilk_info[pre_def_loc] = 5 /*n_bst_macro */ ;
    pre_define("read        ", 4, 4 /*bst_command_ilk */ );
    ilk_info[pre_def_loc] = 6 /*n_bst_read */ ;
    pre_define("reverse     ", 7, 4 /*bst_command_ilk */ );
    ilk_info[pre_def_loc] = 7 /*n_bst_reverse */ ;
    pre_define("sort        ", 4, 4 /*bst_command_ilk */ );
    ilk_info[pre_def_loc] = 8 /*n_bst_sort */ ;
    pre_define("strings     ", 7, 4 /*bst_command_ilk */ );
    ilk_info[pre_def_loc] = 9 /*n_bst_strings */ ;
    pre_define("comment     ", 7, 12 /*bib_command_ilk */ );
    ilk_info[pre_def_loc] = 0 /*n_bib_comment */ ;
    pre_define("preamble    ", 8, 12 /*bib_command_ilk */ );
    ilk_info[pre_def_loc] = 1 /*n_bib_preamble */ ;
    pre_define("string      ", 6, 12 /*bib_command_ilk */ );
    ilk_info[pre_def_loc] = 2 /*n_bib_string */ ;
    build_in("=           ", 1, &b_equals, 0 /*n_equals */ );
    build_in(">           ", 1, &b_greater_than, 1 /*n_greater_than */ );
    build_in("<           ", 1, &b_less_than, 2 /*n_less_than */ );
    build_in("+           ", 1, &b_plus, 3 /*n_plus */ );
    build_in("-           ", 1, &b_minus, 4 /*n_minus */ );
    build_in("*           ", 1, &b_concatenate, 5 /*n_concatenate */ );
    build_in(":=          ", 2, &b_gets, 6 /*n_gets */ );
    build_in("add.period$ ", 11, &b_add_period, 7 /*n_add_period */ );
    build_in("call.type$  ", 10, &b_call_type, 8 /*n_call_type */ );
    build_in("change.case$", 12, &b_change_case, 9 /*n_change_case */ );
    build_in("chr.to.int$ ", 11, &b_chr_to_int, 10 /*n_chr_to_int */ );
    build_in("cite$       ", 5, &b_cite, 11 /*n_cite */ );
    build_in("duplicate$  ", 10, &b_duplicate, 12 /*n_duplicate */ );
    build_in("empty$      ", 6, &b_empty, 13 /*n_empty */ );
    build_in("format.name$", 12, &b_format_name, 14 /*n_format_name */ );
    build_in("if$         ", 3, &b_if, 15 /*n_if */ );
    build_in("int.to.chr$ ", 11, &b_int_to_chr, 16 /*n_int_to_chr */ );
    build_in("int.to.str$ ", 11, &b_int_to_str, 17 /*n_int_to_str */ );
    build_in("missing$    ", 8, &b_missing, 18 /*n_missing */ );
    build_in("newline$    ", 8, &b_newline, 19 /*n_newline */ );
    build_in("num.names$  ", 10, &b_num_names, 20 /*n_num_names */ );
    build_in("pop$        ", 4, &b_pop, 21 /*n_pop */ );
    build_in("preamble$   ", 9, &b_preamble, 22 /*n_preamble */ );
    build_in("purify$     ", 7, &b_purify, 23 /*n_purify */ );
    build_in("quote$      ", 6, &b_quote, 24 /*n_quote */ );
    build_in("skip$       ", 5, &b_skip, 25 /*n_skip */ );
    build_in("stack$      ", 6, &b_stack, 26 /*n_stack */ );
    build_in("substring$  ", 10, &b_substring, 27 /*n_substring */ );
    build_in("swap$       ", 5, &b_swap, 28 /*n_swap */ );
    build_in("text.length$", 12, &b_text_length, 29 /*n_text_length */ );
    build_in("text.prefix$", 12, &b_text_prefix, 30 /*n_text_prefix */ );
    build_in("top$        ", 4, &b_top_stack, 31 /*n_top_stack */ );
    build_in("type$       ", 5, &b_type, 32 /*n_type */ );
    build_in("warning$    ", 8, &b_warning, 33 /*n_warning */ );
    build_in("while$      ", 6, &b_while, 34 /*n_while */ );
    build_in("width$      ", 6, &b_width, 35 /*n_width */ );
    build_in("write$      ", 6, &b_write, 36 /*n_write */ );
    pre_define("            ", 0, 0 /*text_ilk */ );
    s_null = hash_text[pre_def_loc];
    fn_type[pre_def_loc] = 3 /*str_literal */ ;
    pre_define("default.type", 12, 0 /*text_ilk */ );
    s_default = hash_text[pre_def_loc];
    fn_type[pre_def_loc] = 3 /*str_literal */ ;
    b_default = b_skip;
    preamble_ptr = 0;
    pre_define("i           ", 1, 14 /*control_seq_ilk */ );
    ilk_info[pre_def_loc] = 0 /*n_i */ ;
    pre_define("j           ", 1, 14 /*control_seq_ilk */ );
    ilk_info[pre_def_loc] = 1 /*n_j */ ;
    pre_define("oe          ", 2, 14 /*control_seq_ilk */ );
    ilk_info[pre_def_loc] = 2 /*n_oe */ ;
    pre_define("OE          ", 2, 14 /*control_seq_ilk */ );
    ilk_info[pre_def_loc] = 3 /*n_oe_upper */ ;
    pre_define("ae          ", 2, 14 /*control_seq_ilk */ );
    ilk_info[pre_def_loc] = 4 /*n_ae */ ;
    pre_define("AE          ", 2, 14 /*control_seq_ilk */ );
    ilk_info[pre_def_loc] = 5 /*n_ae_upper */ ;
    pre_define("aa          ", 2, 14 /*control_seq_ilk */ );
    ilk_info[pre_def_loc] = 6 /*n_aa */ ;
    pre_define("AA          ", 2, 14 /*control_seq_ilk */ );
    ilk_info[pre_def_loc] = 7 /*n_aa_upper */ ;
    pre_define("o           ", 1, 14 /*control_seq_ilk */ );
    ilk_info[pre_def_loc] = 8 /*n_o */ ;
    pre_define("O           ", 1, 14 /*control_seq_ilk */ );
    ilk_info[pre_def_loc] = 9 /*n_o_upper */ ;
    pre_define("l           ", 1, 14 /*control_seq_ilk */ );
    ilk_info[pre_def_loc] = 10 /*n_l */ ;
    pre_define("L           ", 1, 14 /*control_seq_ilk */ );
    ilk_info[pre_def_loc] = 11 /*n_l_upper */ ;
    pre_define("ss          ", 2, 14 /*control_seq_ilk */ );
    ilk_info[pre_def_loc] = 12 /*n_ss */ ;
    pre_define("crossref    ", 8, 11 /*bst_fn_ilk */ );
    fn_type[pre_def_loc] = 4 /*field */ ;
    ilk_info[pre_def_loc] = num_fields;
    crossref_num = num_fields;
    num_fields = num_fields + 1;
    num_pre_defined_fields = num_fields;
    pre_define("sort.key$   ", 9, 11 /*bst_fn_ilk */ );
    fn_type[pre_def_loc] = 6 /*str_entry_var */ ;
    ilk_info[pre_def_loc] = num_ent_strs;
    sort_key_num = num_ent_strs;
    num_ent_strs = num_ent_strs + 1;
    pre_define("entry.max$  ", 10, 11 /*bst_fn_ilk */ );
    fn_type[pre_def_loc] = 7 /*int_global_var */ ;
    ilk_info[pre_def_loc] = ent_str_size;
    pre_define("global.max$ ", 11, 11 /*bst_fn_ilk */ );
    fn_type[pre_def_loc] = 7 /*int_global_var */ ;
    ilk_info[pre_def_loc] = glob_str_size;
}

boolean scan1(ASCII_code char1)
{
    register boolean Result;
    buf_ptr1 = buf_ptr2;
    while (((buffer[buf_ptr2] != char1) && (buf_ptr2 < last)))
        buf_ptr2 = buf_ptr2 + 1;
    if ((buf_ptr2 < last))
        Result = true;
    else
        Result = false;
    return Result;
}

boolean scan1_white(ASCII_code char1)
{
    register boolean Result;
    buf_ptr1 = buf_ptr2;
    while (((lex_class[buffer[buf_ptr2]] != 1 /*white_space */ ) && (buffer[buf_ptr2] != char1) && (buf_ptr2 < last)))
        buf_ptr2 = buf_ptr2 + 1;
    if ((buf_ptr2 < last))
        Result = true;
    else
        Result = false;
    return Result;
}

boolean scan2(ASCII_code char1, ASCII_code char2)
{
    register boolean Result;
    buf_ptr1 = buf_ptr2;
    while (((buffer[buf_ptr2] != char1) && (buffer[buf_ptr2] != char2) && (buf_ptr2 < last)))
        buf_ptr2 = buf_ptr2 + 1;
    if ((buf_ptr2 < last))
        Result = true;
    else
        Result = false;
    return Result;
}

boolean scan2_white(ASCII_code char1, ASCII_code char2)
{
    register boolean Result;
    buf_ptr1 = buf_ptr2;
    while (((buffer[buf_ptr2] != char1) && (buffer[buf_ptr2] != char2)
            && (lex_class[buffer[buf_ptr2]] != 1 /*white_space */ ) && (buf_ptr2 < last)))
        buf_ptr2 = buf_ptr2 + 1;
    if ((buf_ptr2 < last))
        Result = true;
    else
        Result = false;
    return Result;
}

boolean scan3(ASCII_code char1, ASCII_code char2, ASCII_code char3)
{
    register boolean Result;
    buf_ptr1 = buf_ptr2;
    while (((buffer[buf_ptr2] != char1) && (buffer[buf_ptr2] != char2) && (buffer[buf_ptr2] != char3)
            && (buf_ptr2 < last)))
        buf_ptr2 = buf_ptr2 + 1;
    if ((buf_ptr2 < last))
        Result = true;
    else
        Result = false;
    return Result;
}

boolean scan_alpha(void)
{
    register boolean Result;
    buf_ptr1 = buf_ptr2;
    while (((lex_class[buffer[buf_ptr2]] == 2 /*alpha */ ) && (buf_ptr2 < last)))
        buf_ptr2 = buf_ptr2 + 1;
    if (((buf_ptr2 - buf_ptr1) == 0))
        Result = false;
    else
        Result = true;
    return Result;
}

void scan_identifier(ASCII_code char1, ASCII_code char2, ASCII_code char3)
{
    buf_ptr1 = buf_ptr2;
    if ((lex_class[buffer[buf_ptr2]] != 3 /*numeric */ ))
        while (((id_class[buffer[buf_ptr2]] == 1 /*legal_id_char */ ) && (buf_ptr2 < last)))
            buf_ptr2 = buf_ptr2 + 1;
    if (((buf_ptr2 - buf_ptr1) == 0))
        scan_result = 0 /*id_null */ ;
    else if (((lex_class[buffer[buf_ptr2]] == 1 /*white_space */ ) || (buf_ptr2 == last)))
        scan_result = 3 /*white_adjacent */ ;
    else if (((buffer[buf_ptr2] == char1) || (buffer[buf_ptr2] == char2) || (buffer[buf_ptr2] == char3)))
        scan_result = 1 /*specified_char_adjacent */ ;
    else
        scan_result = 2 /*other_char_adjacent */ ;
}

boolean scan_nonneg_integer(void)
{
    register boolean Result;
    buf_ptr1 = buf_ptr2;
    token_value = 0;
    while (((lex_class[buffer[buf_ptr2]] == 3 /*numeric */ ) && (buf_ptr2 < last))) {

        token_value = token_value * 10 + (buffer[buf_ptr2] - 48);
        buf_ptr2 = buf_ptr2 + 1;
    }
    if (((buf_ptr2 - buf_ptr1) == 0))
        Result = false;
    else
        Result = true;
    return Result;
}

boolean scan_integer(void)
{
    register boolean Result;
    unsigned char sign_length;
    buf_ptr1 = buf_ptr2;
    if ((buffer[buf_ptr2] == 45 /*minus_sign */ )) {
        sign_length = 1;
        buf_ptr2 = buf_ptr2 + 1;
    } else
        sign_length = 0;
    token_value = 0;
    while (((lex_class[buffer[buf_ptr2]] == 3 /*numeric */ ) && (buf_ptr2 < last))) {

        token_value = token_value * 10 + (buffer[buf_ptr2] - 48);
        buf_ptr2 = buf_ptr2 + 1;
    }
    if (((sign_length == 1)))
        token_value = -(integer) token_value;
    if (((buf_ptr2 - buf_ptr1) == sign_length))
        Result = false;
    else
        Result = true;
    return Result;
}

boolean scan_white_space(void)
{
    register boolean Result;
    while (((lex_class[buffer[buf_ptr2]] == 1 /*white_space */ ) && (buf_ptr2 < last)))
        buf_ptr2 = buf_ptr2 + 1;
    if ((buf_ptr2 < last))
        Result = true;
    else
        Result = false;
    return Result;
}

boolean eat_bst_white_space(void)
{
    register boolean Result;
    while (true) {

        if ((scan_white_space())) {

            if ((buffer[buf_ptr2] != 37 /*comment */ )) {
                Result = true;
                goto exit;
            }
        }
        if ((!input_ln(bst_file))) {
            Result = false;
            goto exit;
        }
        bst_line_num = bst_line_num + 1;
        buf_ptr2 = 0;
    }
 exit: ;
    return Result;
}

void skip_token_print(void)
{
    putc_log('-');
    bst_ln_num_print();
    mark_error();
    if ((scan2_white(125 /*right_brace */ , 37 /*comment */ ))) ;
}

void print_recursion_illegal(void)
{
    puts_log("Curse you, wizard, before you recurse me:\n");
    puts_log("function ");
    print_a_token();
    puts_log(" is illegal in its own definition\n");
    skip_token_print();
}

void skp_token_unknown_function_print(void)
{
    print_a_token();
    puts_log(" is an unknown function");
    skip_token_print();
}

void skip_illegal_stuff_after_token_print(void)
{
    printf_log("\"%c\" can't follow a literal", buffer[buf_ptr2]);
    skip_token_print();
}

void scan_fn_def(hash_loc fn_hash_loc)
{
    typedef integer fn_def_loc;
    hash_ptr2 *singl_function;
    integer single_fn_space;
    fn_def_loc single_ptr;
    fn_def_loc copy_ptr;
    buf_pointer end_of_num;
    hash_loc impl_fn_loc;
    single_fn_space = SINGLE_FN_SPACE;
    singl_function = XTALLOC(single_fn_space + 1, hash_ptr2);
    {
        if ((!eat_bst_white_space())) {
            eat_bst_print();
            puts_log("function");
            bst_err_print_and_look_for_blank_line();
            goto exit;
        }
    }
    single_ptr = 0;
    while ((buffer[buf_ptr2] != 125 /*right_brace */ )) {

        switch ((buffer[buf_ptr2])) {
        case 35:
            {
                buf_ptr2 = buf_ptr2 + 1;
                if ((!scan_integer())) {
                    puts_log("Illegal integer in integer literal");
                    skip_token_print();
                    goto lab25;
                };

                literal_loc = str_lookup(buffer, buf_ptr1, (buf_ptr2 - buf_ptr1), 1 /*integer_ilk */ , true);
                if ((!hash_found)) {
                    fn_type[literal_loc] = 2 /*int_literal */ ;
                    ilk_info[literal_loc] = token_value;
                }
                if (((lex_class[buffer[buf_ptr2]] != 1 /*white_space */ ) && (buf_ptr2 < last)
                     && (buffer[buf_ptr2] != 125 /*right_brace */ ) && (buffer[buf_ptr2] != 37 /*comment */ ))) {
                    skip_illegal_stuff_after_token_print();
                    goto lab25;
                }
                {
                    singl_function[single_ptr] = literal_loc;
                    if ((single_ptr == single_fn_space)) {
                        BIB_XRETALLOC("singl_function", singl_function, hash_ptr2, single_fn_space,
                                      single_fn_space + SINGLE_FN_SPACE);
                    }
                    single_ptr = single_ptr + 1;
                }
            }
            break;
        case 34:
            {
                buf_ptr2 = buf_ptr2 + 1;
                if ((!scan1(34 /*double_quote */ ))) {
                    printf_log("No `\"' to end string literal");
                    skip_token_print();
                    goto lab25;
                };

                literal_loc = str_lookup(buffer, buf_ptr1, (buf_ptr2 - buf_ptr1), 0 /*text_ilk */ , true);
                fn_type[literal_loc] = 3 /*str_literal */ ;
                buf_ptr2 = buf_ptr2 + 1;
                if (((lex_class[buffer[buf_ptr2]] != 1 /*white_space */ ) && (buf_ptr2 < last)
                     && (buffer[buf_ptr2] != 125 /*right_brace */ ) && (buffer[buf_ptr2] != 37 /*comment */ ))) {
                    skip_illegal_stuff_after_token_print();
                    goto lab25;
                }
                {
                    singl_function[single_ptr] = literal_loc;
                    if ((single_ptr == single_fn_space)) {
                        BIB_XRETALLOC("singl_function", singl_function, hash_ptr2, single_fn_space,
                                      single_fn_space + SINGLE_FN_SPACE);
                    }
                    single_ptr = single_ptr + 1;
                }
            }
            break;
        case 39:
            {
                buf_ptr2 = buf_ptr2 + 1;
                if ((scan2_white(125 /*right_brace */ , 37 /*comment */ ))) ;
                ;

                lower_case(buffer, buf_ptr1, (buf_ptr2 - buf_ptr1));
                fn_loc = str_lookup(buffer, buf_ptr1, (buf_ptr2 - buf_ptr1), 11 /*bst_fn_ilk */ , false);
                if ((!hash_found)) {
                    skp_token_unknown_function_print();
                    goto lab25;
                } else {        /*194: */

                    if ((fn_loc == wiz_loc)) {
                        print_recursion_illegal();
                        goto lab25;
                    } else {

                        ;

                        {
                            singl_function[single_ptr] = quote_next_fn;
                            if ((single_ptr == single_fn_space)) {
                                BIB_XRETALLOC("singl_function", singl_function, hash_ptr2, single_fn_space,
                                              single_fn_space + SINGLE_FN_SPACE);
                            }
                            single_ptr = single_ptr + 1;
                        }
                        {
                            singl_function[single_ptr] = fn_loc;
                            if ((single_ptr == single_fn_space)) {
                                BIB_XRETALLOC("singl_function", singl_function, hash_ptr2, single_fn_space,
                                              single_fn_space + SINGLE_FN_SPACE);
                            }
                            single_ptr = single_ptr + 1;
                        }
                    }
                }
            }
            break;
        case 123:
            {
                ex_buf[0] = 39 /*single_quote */ ;
                int_to_ASCII(impl_fn_num, ex_buf, 1, &end_of_num);
                impl_fn_loc = str_lookup(ex_buf, 0, end_of_num, 11 /*bst_fn_ilk */ , true);
                if ((hash_found)) {
                    puts_log("Already encountered implicit function");
                    print_confusion();
                    longjmp(error_jmpbuf, 1);
                };

                impl_fn_num = impl_fn_num + 1;
                fn_type[impl_fn_loc] = 1 /*wiz_defined */ ;
                {
                    singl_function[single_ptr] = quote_next_fn;
                    if ((single_ptr == single_fn_space)) {
                        BIB_XRETALLOC("singl_function", singl_function, hash_ptr2, single_fn_space,
                                      single_fn_space + SINGLE_FN_SPACE);
                    }
                    single_ptr = single_ptr + 1;
                }
                {
                    singl_function[single_ptr] = impl_fn_loc;
                    if ((single_ptr == single_fn_space)) {
                        BIB_XRETALLOC("singl_function", singl_function, hash_ptr2, single_fn_space,
                                      single_fn_space + SINGLE_FN_SPACE);
                    }
                    single_ptr = single_ptr + 1;
                }
                buf_ptr2 = buf_ptr2 + 1;
                scan_fn_def(impl_fn_loc);
            }
            break;
        default:
            {
                if ((scan2_white(125 /*right_brace */ , 37 /*comment */ ))) ;
                ;

                lower_case(buffer, buf_ptr1, (buf_ptr2 - buf_ptr1));
                fn_loc = str_lookup(buffer, buf_ptr1, (buf_ptr2 - buf_ptr1), 11 /*bst_fn_ilk */ , false);
                if ((!hash_found)) {
                    skp_token_unknown_function_print();
                    goto lab25;
                } else if ((fn_loc == wiz_loc)) {
                    print_recursion_illegal();
                    goto lab25;
                } else {

                    ;

                    {
                        singl_function[single_ptr] = fn_loc;
                        if ((single_ptr == single_fn_space)) {
                            BIB_XRETALLOC("singl_function", singl_function, hash_ptr2, single_fn_space,
                                          single_fn_space + SINGLE_FN_SPACE);
                        }
                        single_ptr = single_ptr + 1;
                    }
                }
            }
            break;
        }
 lab25:                        /*next_token */  {

            if ((!eat_bst_white_space())) {
                eat_bst_print();
                puts_log("function");
                bst_err_print_and_look_for_blank_line();
                goto exit;
            }
        }
    }
    {
        {
            singl_function[single_ptr] = end_of_def;
            if ((single_ptr == single_fn_space)) {
                BIB_XRETALLOC("singl_function", singl_function, hash_ptr2, single_fn_space,
                              single_fn_space + SINGLE_FN_SPACE);
            }
            single_ptr = single_ptr + 1;
        }
        while ((single_ptr + wiz_def_ptr > wiz_fn_space)) {

            BIB_XRETALLOC("wiz_functions", wiz_functions, hash_ptr2, wiz_fn_space, wiz_fn_space + WIZ_FN_SPACE);
        }
        ilk_info[fn_hash_loc] = wiz_def_ptr;
        copy_ptr = 0;
        while ((copy_ptr < single_ptr)) {

            wiz_functions[wiz_def_ptr] = singl_function[copy_ptr];
            copy_ptr = copy_ptr + 1;
            wiz_def_ptr = wiz_def_ptr + 1;
        }
    }
    buf_ptr2 = buf_ptr2 + 1;
 exit: free(singl_function);
}

boolean eat_bib_white_space(void)
{
    register boolean Result;
    while ((!scan_white_space())) {

        if ((!input_ln(bib_file[bib_ptr]))) {
            Result = false;
            goto exit;
        }
        bib_line_num = bib_line_num + 1;
        buf_ptr2 = 0;
    }
    Result = true;
 exit: ;
    return Result;
}

boolean compress_bib_white(void)
{
    register boolean Result;
    Result = false;
    {
        if ((ex_buf_ptr == buf_size)) {
            bib_field_too_long_print();
            goto exit;
        } else {

            ex_buf[ex_buf_ptr] = 32 /*space */ ;
            ex_buf_ptr = ex_buf_ptr + 1;
        }
    }
    while ((!scan_white_space())) {

        if ((!input_ln(bib_file[bib_ptr]))) {
            eat_bib_print();
            goto exit;
        }
        bib_line_num = bib_line_num + 1;
        buf_ptr2 = 0;
    }
    Result = true;
 exit: ;
    return Result;
}

boolean scan_balanced_braces(void)
{
    register boolean Result;
    Result = false;
    buf_ptr2 = buf_ptr2 + 1;
    {
        if (((lex_class[buffer[buf_ptr2]] == 1 /*white_space */ ) || (buf_ptr2 == last))) {

            if ((!compress_bib_white()))
                goto exit;
        }
    }
    if ((ex_buf_ptr > 1)) {

        if ((ex_buf[ex_buf_ptr - 1] == 32 /*space */ )) {

            if ((ex_buf[ex_buf_ptr - 2] == 32 /*space */ ))
                ex_buf_ptr = ex_buf_ptr - 1;
        }
    }
    bib_brace_level = 0;
    if ((store_field)) {        /*257: */
        while ((buffer[buf_ptr2] != right_str_delim))
            switch ((buffer[buf_ptr2])) {
            case 123:
                {
                    bib_brace_level = bib_brace_level + 1;
                    {
                        if ((ex_buf_ptr == buf_size)) {
                            bib_field_too_long_print();
                            goto exit;
                        } else {

                            ex_buf[ex_buf_ptr] = 123 /*left_brace */ ;
                            ex_buf_ptr = ex_buf_ptr + 1;
                        }
                    }
                    buf_ptr2 = buf_ptr2 + 1;
                    {
                        if (((lex_class[buffer[buf_ptr2]] == 1 /*white_space */ ) || (buf_ptr2 == last))) {

                            if ((!compress_bib_white()))
                                goto exit;
                        }
                    }
                    {
                        while (true)
                            switch ((buffer[buf_ptr2])) {
                            case 125:
                                {
                                    bib_brace_level = bib_brace_level - 1;
                                    {
                                        if ((ex_buf_ptr == buf_size)) {
                                            bib_field_too_long_print();
                                            goto exit;
                                        } else {

                                            ex_buf[ex_buf_ptr] = 125 /*right_brace */ ;
                                            ex_buf_ptr = ex_buf_ptr + 1;
                                        }
                                    }
                                    buf_ptr2 = buf_ptr2 + 1;
                                    {
                                        if (((lex_class[buffer[buf_ptr2]] == 1 /*white_space */ )
                                             || (buf_ptr2 == last))) {

                                            if ((!compress_bib_white()))
                                                goto exit;
                                        }
                                    }
                                    if ((bib_brace_level == 0))
                                        goto loop_exit;
                                }
                                break;
                            case 123:
                                {
                                    bib_brace_level = bib_brace_level + 1;
                                    {
                                        if ((ex_buf_ptr == buf_size)) {
                                            bib_field_too_long_print();
                                            goto exit;
                                        } else {

                                            ex_buf[ex_buf_ptr] = 123 /*left_brace */ ;
                                            ex_buf_ptr = ex_buf_ptr + 1;
                                        }
                                    }
                                    buf_ptr2 = buf_ptr2 + 1;
                                    {
                                        if (((lex_class[buffer[buf_ptr2]] == 1 /*white_space */ )
                                             || (buf_ptr2 == last))) {

                                            if ((!compress_bib_white()))
                                                goto exit;
                                        }
                                    }
                                }
                                break;
                            default:
                                {
                                    {
                                        if ((ex_buf_ptr == buf_size)) {
                                            bib_field_too_long_print();
                                            goto exit;
                                        } else {

                                            ex_buf[ex_buf_ptr] = buffer[buf_ptr2];
                                            ex_buf_ptr = ex_buf_ptr + 1;
                                        }
                                    }
                                    buf_ptr2 = buf_ptr2 + 1;
                                    {
                                        if (((lex_class[buffer[buf_ptr2]] == 1 /*white_space */ )
                                             || (buf_ptr2 == last))) {

                                            if ((!compress_bib_white()))
                                                goto exit;
                                        }
                                    }
                                }
                                break;
                            }
                    loop_exit:
                        ;
                    }
                }
                break;
            case 125:
                {
                    bib_unbalanced_braces_print();
                    goto exit;
                }
                break;
            default:
                {
                    {
                        if ((ex_buf_ptr == buf_size)) {
                            bib_field_too_long_print();
                            goto exit;
                        } else {

                            ex_buf[ex_buf_ptr] = buffer[buf_ptr2];
                            ex_buf_ptr = ex_buf_ptr + 1;
                        }
                    }
                    buf_ptr2 = buf_ptr2 + 1;
                    {
                        if (((lex_class[buffer[buf_ptr2]] == 1 /*white_space */ ) || (buf_ptr2 == last))) {

                            if ((!compress_bib_white()))
                                goto exit;
                        }
                    }
                }
                break;
            }
    } else {                    /*255: */

        while ((buffer[buf_ptr2] != right_str_delim))
            if ((buffer[buf_ptr2] == 123 /*left_brace */ )) {
                bib_brace_level = bib_brace_level + 1;
                buf_ptr2 = buf_ptr2 + 1;
                {
                    if ((!eat_bib_white_space())) {
                        eat_bib_print();
                        goto exit;
                    }
                }
                while ((bib_brace_level > 0)) { /*256: */

                    if ((buffer[buf_ptr2] == 125 /*right_brace */ )) {
                        bib_brace_level = bib_brace_level - 1;
                        buf_ptr2 = buf_ptr2 + 1;
                        {
                            if ((!eat_bib_white_space())) {
                                eat_bib_print();
                                goto exit;
                            }
                        }
                    } else if ((buffer[buf_ptr2] == 123 /*left_brace */ )) {
                        bib_brace_level = bib_brace_level + 1;
                        buf_ptr2 = buf_ptr2 + 1;
                        {
                            if ((!eat_bib_white_space())) {
                                eat_bib_print();
                                goto exit;
                            }
                        }
                    } else {

                        buf_ptr2 = buf_ptr2 + 1;
                        if ((!scan2(125 /*right_brace */ , 123 /*left_brace */ ))) {
                            if ((!eat_bib_white_space())) {
                                eat_bib_print();
                                goto exit;
                            }
                        }
                    }
                }
            } else if ((buffer[buf_ptr2] == 125 /*right_brace */ )) {
                bib_unbalanced_braces_print();
                goto exit;
            } else {

                buf_ptr2 = buf_ptr2 + 1;
                if ((!scan3(right_str_delim, 123 /*left_brace */ , 125 /*right_brace */ ))) {
                    if ((!eat_bib_white_space())) {
                        eat_bib_print();
                        goto exit;
                    }
                }
            }
    }
    buf_ptr2 = buf_ptr2 + 1;
    Result = true;
 exit: ;
    return Result;
}

boolean scan_a_field_token_and_eat_white(void)
{
    register boolean Result;
    Result = false;
    switch ((buffer[buf_ptr2])) {
    case 123:
        {
            right_str_delim = 125 /*right_brace */ ;
            if ((!scan_balanced_braces()))
                goto exit;
        }
        break;
    case 34:
        {
            right_str_delim = 34 /*double_quote */ ;
            if ((!scan_balanced_braces()))
                goto exit;
        }
        break;
    case 48:
    case 49:
    case 50:
    case 51:
    case 52:
    case 53:
    case 54:
    case 55:
    case 56:
    case 57:
        {
            if ((!scan_nonneg_integer())) {
                puts_log("A digit disappeared");
                print_confusion();
                longjmp(error_jmpbuf, 1);
            }
            if ((store_field)) {
                tmp_ptr = buf_ptr1;
                while ((tmp_ptr < buf_ptr2)) {

                    {
                        if ((ex_buf_ptr == buf_size)) {
                            bib_field_too_long_print();
                            goto exit;
                        } else {

                            ex_buf[ex_buf_ptr] = buffer[tmp_ptr];
                            ex_buf_ptr = ex_buf_ptr + 1;
                        }
                    }
                    tmp_ptr = tmp_ptr + 1;
                }
            }
        }
        break;
    default:
        {
            scan_identifier(44 /*comma */ , right_outer_delim, 35 /*concat_char */ );
            {
                if (((scan_result == 3 /*white_adjacent */ ) || (scan_result == 1 /*specified_char_adjacent */ ))) ;
                else {
                    bib_id_print();
                    puts_log("a field part");
                    bib_err_print();
                    goto exit;
                }
            }
            if ((store_field)) {
                lower_case(buffer, buf_ptr1, (buf_ptr2 - buf_ptr1));
                macro_name_loc = str_lookup(buffer, buf_ptr1, (buf_ptr2 - buf_ptr1), 13 /*macro_ilk */ , false);
                store_token = true;
                if ((at_bib_command)) {

                    if ((command_num == 2 /*n_bib_string */ )) {

                        if ((macro_name_loc == cur_macro_loc)) {
                            store_token = false;
                            macro_warn_print();
                            puts_log("used in its own definition\n");
                            bib_warn_print();
                        }
                    }
                }
                if ((!hash_found)) {
                    store_token = false;
                    macro_warn_print();
                    puts_log("undefined\n");
                    bib_warn_print();
                }
                if ((store_token)) {    /*261: */
                    tmp_ptr = str_start[ilk_info[macro_name_loc]];
                    tmp_end_ptr = str_start[ilk_info[macro_name_loc] + 1];
                    if ((ex_buf_ptr == 0)) {

                        if (((lex_class[str_pool[tmp_ptr]] == 1 /*white_space */ ) && (tmp_ptr < tmp_end_ptr))) {
                            {
                                if ((ex_buf_ptr == buf_size)) {
                                    bib_field_too_long_print();
                                    goto exit;
                                } else {

                                    ex_buf[ex_buf_ptr] = 32 /*space */ ;
                                    ex_buf_ptr = ex_buf_ptr + 1;
                                }
                            }
                            tmp_ptr = tmp_ptr + 1;
                            while (((lex_class[str_pool[tmp_ptr]] == 1 /*white_space */ ) && (tmp_ptr < tmp_end_ptr)))
                                tmp_ptr = tmp_ptr + 1;
                        }
                    }
                    while ((tmp_ptr < tmp_end_ptr)) {

                        if ((lex_class[str_pool[tmp_ptr]] != 1 /*white_space */ )) {
                            if ((ex_buf_ptr == buf_size)) {
                                bib_field_too_long_print();
                                goto exit;
                            } else {

                                ex_buf[ex_buf_ptr] = str_pool[tmp_ptr];
                                ex_buf_ptr = ex_buf_ptr + 1;
                            }
                        } else if ((ex_buf[ex_buf_ptr - 1] != 32 /*space */ )) {
                            if ((ex_buf_ptr == buf_size)) {
                                bib_field_too_long_print();
                                goto exit;
                            } else {

                                ex_buf[ex_buf_ptr] = 32 /*space */ ;
                                ex_buf_ptr = ex_buf_ptr + 1;
                            }
                        }
                        tmp_ptr = tmp_ptr + 1;
                    }
                }
            }
        }
        break;
    }
    {
        if ((!eat_bib_white_space())) {
            eat_bib_print();
            goto exit;
        }
    }
    Result = true;
 exit: ;
    return Result;
}

boolean scan_and_store_the_field_value_and_eat_white(void)
{
    register boolean Result;
    Result = false;
    ex_buf_ptr = 0;
    if ((!scan_a_field_token_and_eat_white()))
        goto exit;
    while ((buffer[buf_ptr2] == 35 /*concat_char */ )) {

        buf_ptr2 = buf_ptr2 + 1;
        {
            if ((!eat_bib_white_space())) {
                eat_bib_print();
                goto exit;
            }
        }
        if ((!scan_a_field_token_and_eat_white()))
            goto exit;
    }
    if ((store_field)) {        /*262: */
        if ((!at_bib_command)) {

            if ((ex_buf_ptr > 0)) {

                if ((ex_buf[ex_buf_ptr - 1] == 32 /*space */ ))
                    ex_buf_ptr = ex_buf_ptr - 1;
            }
        }
        if (((!at_bib_command) && (ex_buf[0] == 32 /*space */ ) && (ex_buf_ptr > 0)))
            ex_buf_xptr = 1;
        else
            ex_buf_xptr = 0;
        field_val_loc = str_lookup(ex_buf, ex_buf_xptr, ex_buf_ptr - ex_buf_xptr, 0 /*text_ilk */ , true);
        fn_type[field_val_loc] = 3 /*str_literal */ ;
        ;

        if ((at_bib_command)) { /*263: */
            switch ((command_num)) {
            case 1:
                {
                    s_preamble[preamble_ptr] = hash_text[field_val_loc];
                    preamble_ptr = preamble_ptr + 1;
                }
                break;
            case 2:
                ilk_info[cur_macro_loc] = hash_text[field_val_loc];
                break;
            default:
                bib_cmd_confusion();
                break;
            }
        } else {                /*264: */

            field_ptr = entry_cite_ptr * num_fields + ilk_info[field_name_loc];
            if ((field_ptr >= max_fields)) {
                puts_log("field_info index is out of range");
                print_confusion();
                longjmp(error_jmpbuf, 1);
            }
            if ((field_info[field_ptr] != 0 /*missing */ )) {
                puts_log("Warning--I'm ignoring ");
                print_a_pool_str(cite_list[entry_cite_ptr]);
                puts_log("'s extra \"");
                print_a_pool_str(hash_text[field_name_loc]);
                puts_log("\" field\n");
                bib_warn_print();
            } else {

                field_info[field_ptr] = hash_text[field_val_loc];
                if (((ilk_info[field_name_loc] == crossref_num) && (!all_entries))) {   /*265: */
                    tmp_ptr = ex_buf_xptr;
                    while ((tmp_ptr < ex_buf_ptr)) {

                        out_buf[tmp_ptr] = ex_buf[tmp_ptr];
                        tmp_ptr = tmp_ptr + 1;
                    }
                    lower_case(out_buf, ex_buf_xptr, ex_buf_ptr - ex_buf_xptr);
                    lc_cite_loc =
                        str_lookup(out_buf, ex_buf_xptr, ex_buf_ptr - ex_buf_xptr, 10 /*lc_cite_ilk */ , true);
                    if ((hash_found)) {
                        cite_loc = ilk_info[lc_cite_loc];
                        if ((ilk_info[cite_loc] >= old_num_cites))
                            cite_info[ilk_info[cite_loc]] = cite_info[ilk_info[cite_loc]] + 1;
                    } else {

                        cite_loc = str_lookup(ex_buf, ex_buf_xptr, ex_buf_ptr - ex_buf_xptr, 9 /*cite_ilk */ , true);
                        if ((hash_found))
                            hash_cite_confusion();
                        add_database_cite(&cite_ptr);
                        cite_info[ilk_info[cite_loc]] = 1;
                    }
                }
            }
        }
    }
    Result = true;
 exit: ;
    return Result;
}

void decr_brace_level(str_number pop_lit_var)
{
    if ((brace_level == 0))
        braces_unbalanced_complaint(pop_lit_var);
    else
        brace_level = brace_level - 1;
}

void check_brace_level(str_number pop_lit_var)
{
    if ((brace_level > 0))
        braces_unbalanced_complaint(pop_lit_var);
}

void name_scan_for_and(str_number pop_lit_var)
{
    brace_level = 0;
    preceding_white = false;
    and_found = false;
    while (((!and_found) && (ex_buf_ptr < ex_buf_length)))
        switch ((ex_buf[ex_buf_ptr])) {
        case 97:
        case 65:
            {
                ex_buf_ptr = ex_buf_ptr + 1;
                if ((preceding_white)) {        /*387: */
                    if ((ex_buf_ptr <= (ex_buf_length - 3))) {

                        if (((ex_buf[ex_buf_ptr] == 110 /*"n" */ ) || (ex_buf[ex_buf_ptr] == 78 /*"N" */ ))) {

                            if (((ex_buf[ex_buf_ptr + 1] == 100 /*"d" */ ) || (ex_buf[ex_buf_ptr + 1] == 68 /*"D" */ ))) {

                                if ((lex_class[ex_buf[ex_buf_ptr + 2]] == 1 /*white_space */ )) {
                                    ex_buf_ptr = ex_buf_ptr + 2;
                                    and_found = true;
                                }
                            }
                        }
                    }
                }
                preceding_white = false;
            }
            break;
        case 123:
            {
                brace_level = brace_level + 1;
                ex_buf_ptr = ex_buf_ptr + 1;
                while (((brace_level > 0) && (ex_buf_ptr < ex_buf_length))) {

                    if ((ex_buf[ex_buf_ptr] == 125 /*right_brace */ ))
                        brace_level = brace_level - 1;
                    else if ((ex_buf[ex_buf_ptr] == 123 /*left_brace */ ))
                        brace_level = brace_level + 1;
                    ex_buf_ptr = ex_buf_ptr + 1;
                }
                preceding_white = false;
            }
            break;
        case 125:
            {
                decr_brace_level(pop_lit_var);
                ex_buf_ptr = ex_buf_ptr + 1;
                preceding_white = false;
            }
            break;
        default:
            if ((lex_class[ex_buf[ex_buf_ptr]] == 1 /*white_space */ )) {
                ex_buf_ptr = ex_buf_ptr + 1;
                preceding_white = true;
            } else {

                ex_buf_ptr = ex_buf_ptr + 1;
                preceding_white = false;
            }
            break;
        }
    check_brace_level(pop_lit_var);
}

boolean von_token_found(void)
{
    register boolean Result;
    nm_brace_level = 0;
    Result = false;
    while ((name_bf_ptr < name_bf_xptr))
        if (((sv_buffer[name_bf_ptr] >= 65 /*"A" */ ) && (sv_buffer[name_bf_ptr] <= 90 /*"Z" */ )))
            goto exit;
        else if (((sv_buffer[name_bf_ptr] >= 97 /*"a" */ ) && (sv_buffer[name_bf_ptr] <= 122 /*"z" */ ))) {
            Result = true;
            goto exit;
        } else if ((sv_buffer[name_bf_ptr] == 123 /*left_brace */ )) {
            nm_brace_level = nm_brace_level + 1;
            name_bf_ptr = name_bf_ptr + 1;
            if (((name_bf_ptr + 2 < name_bf_xptr) && (sv_buffer[name_bf_ptr] == 92 /*backslash */ ))) { /*399: */
                name_bf_ptr = name_bf_ptr + 1;
                name_bf_yptr = name_bf_ptr;
                while (((name_bf_ptr < name_bf_xptr) && (lex_class[sv_buffer[name_bf_ptr]] == 2 /*alpha */ )))
                    name_bf_ptr = name_bf_ptr + 1;
                control_seq_loc =
                    str_lookup(sv_buffer, name_bf_yptr, name_bf_ptr - name_bf_yptr, 14 /*control_seq_ilk */ , false);
                if ((hash_found)) {     /*400: */
                    switch ((ilk_info[control_seq_loc])) {
                    case 3:
                    case 5:
                    case 7:
                    case 9:
                    case 11:
                        goto exit;
                        break;
                    case 0:
                    case 1:
                    case 2:
                    case 4:
                    case 6:
                    case 8:
                    case 10:
                    case 12:
                        {
                            Result = true;
                            goto exit;
                        }
                        break;
                    default:
                        puts_log("Control-sequence hash error");
                        print_confusion();
                        longjmp(error_jmpbuf, 1);
                        break;
                    }
                }
                while (((name_bf_ptr < name_bf_xptr) && (nm_brace_level > 0))) {

                    if (((sv_buffer[name_bf_ptr] >= 65 /*"A" */ ) && (sv_buffer[name_bf_ptr] <= 90 /*"Z" */ )))
                        goto exit;
                    else if (((sv_buffer[name_bf_ptr] >= 97 /*"a" */ ) && (sv_buffer[name_bf_ptr] <= 122 /*"z" */ ))) {
                        Result = true;
                        goto exit;
                    } else if ((sv_buffer[name_bf_ptr] == 125 /*right_brace */ ))
                        nm_brace_level = nm_brace_level - 1;
                    else if ((sv_buffer[name_bf_ptr] == 123 /*left_brace */ ))
                        nm_brace_level = nm_brace_level + 1;
                    name_bf_ptr = name_bf_ptr + 1;
                }
                goto exit;
            } else              /*401: */
                while (((nm_brace_level > 0) && (name_bf_ptr < name_bf_xptr))) {

                    if ((sv_buffer[name_bf_ptr] == 125 /*right_brace */ ))
                        nm_brace_level = nm_brace_level - 1;
                    else if ((sv_buffer[name_bf_ptr] == 123 /*left_brace */ ))
                        nm_brace_level = nm_brace_level + 1;
                    name_bf_ptr = name_bf_ptr + 1;
                }
        } else
            name_bf_ptr = name_bf_ptr + 1;
 exit: ;
    return Result;
}

void von_name_ends_and_last_name_starts_stuff(void)
{
    von_end = last_end - 1;
    while ((von_end > von_start)) {

        name_bf_ptr = name_tok[von_end - 1];
        name_bf_xptr = name_tok[von_end];
        if ((von_token_found()))
            goto exit;
        von_end = von_end - 1;
    }
 exit: ;
}

void skip_stuff_at_sp_brace_level_greater_than_one(void)
{
    while (((sp_brace_level > 1) && (sp_ptr < sp_end))) {

        if ((str_pool[sp_ptr] == 125 /*right_brace */ ))
            sp_brace_level = sp_brace_level - 1;
        else if ((str_pool[sp_ptr] == 123 /*left_brace */ ))
            sp_brace_level = sp_brace_level + 1;
        sp_ptr = sp_ptr + 1;
    }
}

void brace_lvl_one_letters_complaint(void)
{
    puts_log("The format string \"");
    print_a_pool_str(pop_lit1);
    puts_log("\" has an illegal brace-level-1 letter");
    bst_ex_warn_print();
}

boolean enough_text_chars(buf_pointer enough_chars)
{
    register boolean Result;
    num_text_chars = 0;
    ex_buf_yptr = ex_buf_xptr;
    while (((ex_buf_yptr < ex_buf_ptr) && (num_text_chars < enough_chars))) {

        ex_buf_yptr = ex_buf_yptr + 1;
        if ((ex_buf[ex_buf_yptr - 1] == 123 /*left_brace */ )) {
            brace_level = brace_level + 1;
            if (((brace_level == 1) && (ex_buf_yptr < ex_buf_ptr))) {

                if ((ex_buf[ex_buf_yptr] == 92 /*backslash */ )) {
                    ex_buf_yptr = ex_buf_yptr + 1;
                    while (((ex_buf_yptr < ex_buf_ptr) && (brace_level > 0))) {

                        if ((ex_buf[ex_buf_yptr] == 125 /*right_brace */ ))
                            brace_level = brace_level - 1;
                        else if ((ex_buf[ex_buf_yptr] == 123 /*left_brace */ ))
                            brace_level = brace_level + 1;
                        ex_buf_yptr = ex_buf_yptr + 1;
                    }
                }
            }
        } else if ((ex_buf[ex_buf_yptr - 1] == 125 /*right_brace */ ))
            brace_level = brace_level - 1;
        num_text_chars = num_text_chars + 1;
    }
    if ((num_text_chars < enough_chars))
        Result = false;
    else
        Result = true;
    return Result;
}

void figure_out_the_formatted_name(void)
{
    {
        ex_buf_ptr = 0;
        sp_brace_level = 0;
        sp_ptr = str_start[pop_lit1];
        sp_end = str_start[pop_lit1 + 1];
        while ((sp_ptr < sp_end))
            if ((str_pool[sp_ptr] == 123 /*left_brace */ )) {
                sp_brace_level = sp_brace_level + 1;
                sp_ptr = sp_ptr + 1;
                {
                    sp_xptr1 = sp_ptr;
                    alpha_found = false;
                    double_letter = false;
                    end_of_group = false;
                    to_be_written = true;
                    while (((!end_of_group) && (sp_ptr < sp_end)))
                        if ((lex_class[str_pool[sp_ptr]] == 2 /*alpha */ )) {
                            sp_ptr = sp_ptr + 1;
                            {
                                if ((alpha_found)) {
                                    brace_lvl_one_letters_complaint();
                                    to_be_written = false;
                                } else {

                                    switch ((str_pool[sp_ptr - 1])) {
                                    case 102:
                                    case 70:
                                        {
                                            cur_token = first_start;
                                            last_token = first_end;
                                            if ((cur_token == last_token))
                                                to_be_written = false;
                                            if (((str_pool[sp_ptr] == 102 /*"f" */ )
                                                 || (str_pool[sp_ptr] == 70 /*"F" */ )))
                                                double_letter = true;
                                        }
                                        break;
                                    case 118:
                                    case 86:
                                        {
                                            cur_token = von_start;
                                            last_token = von_end;
                                            if ((cur_token == last_token))
                                                to_be_written = false;
                                            if (((str_pool[sp_ptr] == 118 /*"v" */ )
                                                 || (str_pool[sp_ptr] == 86 /*"V" */ )))
                                                double_letter = true;
                                        }
                                        break;
                                    case 108:
                                    case 76:
                                        {
                                            cur_token = von_end;
                                            last_token = last_end;
                                            if ((cur_token == last_token))
                                                to_be_written = false;
                                            if (((str_pool[sp_ptr] == 108 /*"l" */ )
                                                 || (str_pool[sp_ptr] == 76 /*"L" */ )))
                                                double_letter = true;
                                        }
                                        break;
                                    case 106:
                                    case 74:
                                        {
                                            cur_token = last_end;
                                            last_token = jr_end;
                                            if ((cur_token == last_token))
                                                to_be_written = false;
                                            if (((str_pool[sp_ptr] == 106 /*"j" */ )
                                                 || (str_pool[sp_ptr] == 74 /*"J" */ )))
                                                double_letter = true;
                                        }
                                        break;
                                    default:
                                        {
                                            brace_lvl_one_letters_complaint();
                                            to_be_written = false;
                                        }
                                        break;
                                    }
                                    if ((double_letter))
                                        sp_ptr = sp_ptr + 1;
                                }
                                alpha_found = true;
                            }
                        } else if ((str_pool[sp_ptr] == 125 /*right_brace */ )) {
                            sp_brace_level = sp_brace_level - 1;
                            sp_ptr = sp_ptr + 1;
                            end_of_group = true;
                        } else if ((str_pool[sp_ptr] == 123 /*left_brace */ )) {
                            sp_brace_level = sp_brace_level + 1;
                            sp_ptr = sp_ptr + 1;
                            skip_stuff_at_sp_brace_level_greater_than_one();
                        } else
                            sp_ptr = sp_ptr + 1;
                    if (((end_of_group) && (to_be_written))) {  /*412: */
                        ex_buf_xptr = ex_buf_ptr;
                        sp_ptr = sp_xptr1;
                        sp_brace_level = 1;
                        while ((sp_brace_level > 0))
                            if (((lex_class[str_pool[sp_ptr]] == 2 /*alpha */ ) && (sp_brace_level == 1))) {
                                sp_ptr = sp_ptr + 1;
                                {
                                    if ((double_letter))
                                        sp_ptr = sp_ptr + 1;
                                    use_default = true;
                                    sp_xptr2 = sp_ptr;
                                    if ((str_pool[sp_ptr] == 123 /*left_brace */ )) {
                                        use_default = false;
                                        sp_brace_level = sp_brace_level + 1;
                                        sp_ptr = sp_ptr + 1;
                                        sp_xptr1 = sp_ptr;
                                        skip_stuff_at_sp_brace_level_greater_than_one();
                                        sp_xptr2 = sp_ptr - 1;
                                    }
                                    while ((cur_token < last_token)) {

                                        if ((double_letter)) {  /*415: */
                                            name_bf_ptr = name_tok[cur_token];
                                            name_bf_xptr = name_tok[cur_token + 1];
                                            if ((ex_buf_length + (name_bf_xptr - name_bf_ptr) > buf_size))
                                                buffer_overflow();
                                            while ((name_bf_ptr < name_bf_xptr)) {

                                                {
                                                    ex_buf[ex_buf_ptr] = sv_buffer[name_bf_ptr];
                                                    ex_buf_ptr = ex_buf_ptr + 1;
                                                }
                                                name_bf_ptr = name_bf_ptr + 1;
                                            }
                                        } else {        /*416: */

                                            name_bf_ptr = name_tok[cur_token];
                                            name_bf_xptr = name_tok[cur_token + 1];
                                            while ((name_bf_ptr < name_bf_xptr)) {

                                                if ((lex_class[sv_buffer[name_bf_ptr]] == 2 /*alpha */ )) {
                                                    {
                                                        if ((ex_buf_ptr == buf_size))
                                                            buffer_overflow();
                                                        {
                                                            ex_buf[ex_buf_ptr] = sv_buffer[name_bf_ptr];
                                                            ex_buf_ptr = ex_buf_ptr + 1;
                                                        }
                                                    }
                                                    goto loop_exit;
                                                } else
                                                    if (((sv_buffer[name_bf_ptr] == 123 /*left_brace */ )
                                                         && (name_bf_ptr + 1 < name_bf_xptr))) {

                                                    if ((sv_buffer[name_bf_ptr + 1] == 92 /*backslash */ )) {   /*417: */
                                                        if ((ex_buf_ptr + 2 > buf_size))
                                                            buffer_overflow();
                                                        {
                                                            ex_buf[ex_buf_ptr] = 123 /*left_brace */ ;
                                                            ex_buf_ptr = ex_buf_ptr + 1;
                                                        }
                                                        {
                                                            ex_buf[ex_buf_ptr] = 92 /*backslash */ ;
                                                            ex_buf_ptr = ex_buf_ptr + 1;
                                                        }
                                                        name_bf_ptr = name_bf_ptr + 2;
                                                        nm_brace_level = 1;
                                                        while (((name_bf_ptr < name_bf_xptr) && (nm_brace_level > 0))) {

                                                            if ((sv_buffer[name_bf_ptr] == 125 /*right_brace */ ))
                                                                nm_brace_level = nm_brace_level - 1;
                                                            else if ((sv_buffer[name_bf_ptr] == 123 /*left_brace */ ))
                                                                nm_brace_level = nm_brace_level + 1;
                                                            {
                                                                if ((ex_buf_ptr == buf_size))
                                                                    buffer_overflow();
                                                                {
                                                                    ex_buf[ex_buf_ptr] = sv_buffer[name_bf_ptr];
                                                                    ex_buf_ptr = ex_buf_ptr + 1;
                                                                }
                                                            }
                                                            name_bf_ptr = name_bf_ptr + 1;
                                                        }
                                                        goto loop_exit;
                                                    }
                                                }
                                                name_bf_ptr = name_bf_ptr + 1;
                                            }
                                        loop_exit:
                                            ;
                                        }
                                        cur_token = cur_token + 1;
                                        if ((cur_token < last_token)) { /*418: */
                                            if ((use_default)) {
                                                if ((!double_letter)) {
                                                    if ((ex_buf_ptr == buf_size))
                                                        buffer_overflow();
                                                    {
                                                        ex_buf[ex_buf_ptr] = 46 /*period */ ;
                                                        ex_buf_ptr = ex_buf_ptr + 1;
                                                    }
                                                }
                                                if ((lex_class[name_sep_char[cur_token]] == 4 /*sep_char */ )) {
                                                    if ((ex_buf_ptr == buf_size))
                                                        buffer_overflow();
                                                    {
                                                        ex_buf[ex_buf_ptr] = name_sep_char[cur_token];
                                                        ex_buf_ptr = ex_buf_ptr + 1;
                                                    }
                                                } else
                                                    if (((cur_token == last_token - 1)
                                                         || (!enough_text_chars(3 /*long_token */ )))) {
                                                    if ((ex_buf_ptr == buf_size))
                                                        buffer_overflow();
                                                    {
                                                        ex_buf[ex_buf_ptr] = 126 /*tie */ ;
                                                        ex_buf_ptr = ex_buf_ptr + 1;
                                                    }
                                                } else {

                                                    if ((ex_buf_ptr == buf_size))
                                                        buffer_overflow();
                                                    {
                                                        ex_buf[ex_buf_ptr] = 32 /*space */ ;
                                                        ex_buf_ptr = ex_buf_ptr + 1;
                                                    }
                                                }
                                            } else {

                                                if ((ex_buf_length + (sp_xptr2 - sp_xptr1) > buf_size))
                                                    buffer_overflow();
                                                sp_ptr = sp_xptr1;
                                                while ((sp_ptr < sp_xptr2)) {

                                                    {
                                                        ex_buf[ex_buf_ptr] = str_pool[sp_ptr];
                                                        ex_buf_ptr = ex_buf_ptr + 1;
                                                    }
                                                    sp_ptr = sp_ptr + 1;
                                                }
                                            }
                                        }
                                    }
                                    if ((!use_default))
                                        sp_ptr = sp_xptr2 + 1;
                                }
                            } else if ((str_pool[sp_ptr] == 125 /*right_brace */ )) {
                                sp_brace_level = sp_brace_level - 1;
                                sp_ptr = sp_ptr + 1;
                                if ((sp_brace_level > 0)) {
                                    if ((ex_buf_ptr == buf_size))
                                        buffer_overflow();
                                    {
                                        ex_buf[ex_buf_ptr] = 125 /*right_brace */ ;
                                        ex_buf_ptr = ex_buf_ptr + 1;
                                    }
                                }
                            } else if ((str_pool[sp_ptr] == 123 /*left_brace */ )) {
                                sp_brace_level = sp_brace_level + 1;
                                sp_ptr = sp_ptr + 1;
                                {
                                    if ((ex_buf_ptr == buf_size))
                                        buffer_overflow();
                                    {
                                        ex_buf[ex_buf_ptr] = 123 /*left_brace */ ;
                                        ex_buf_ptr = ex_buf_ptr + 1;
                                    }
                                }
                            } else {

                                {
                                    if ((ex_buf_ptr == buf_size))
                                        buffer_overflow();
                                    {
                                        ex_buf[ex_buf_ptr] = str_pool[sp_ptr];
                                        ex_buf_ptr = ex_buf_ptr + 1;
                                    }
                                }
                                sp_ptr = sp_ptr + 1;
                            }
                        if ((ex_buf_ptr > 0)) {

                            if ((ex_buf[ex_buf_ptr - 1] == 126 /*tie */ )) {    /*420: */
                                ex_buf_ptr = ex_buf_ptr - 1;
                                if ((ex_buf[ex_buf_ptr - 1] == 126 /*tie */ )) ;
                                else if ((!enough_text_chars(3 /*long_name */ )))
                                    ex_buf_ptr = ex_buf_ptr + 1;
                                else {

                                    ex_buf[ex_buf_ptr] = 32 /*space */ ;
                                    ex_buf_ptr = ex_buf_ptr + 1;
                                }
                            }
                        }
                    }
                }
            } else if ((str_pool[sp_ptr] == 125 /*right_brace */ )) {
                braces_unbalanced_complaint(pop_lit1);
                sp_ptr = sp_ptr + 1;
            } else {

                {
                    if ((ex_buf_ptr == buf_size))
                        buffer_overflow();
                    {
                        ex_buf[ex_buf_ptr] = str_pool[sp_ptr];
                        ex_buf_ptr = ex_buf_ptr + 1;
                    }
                }
                sp_ptr = sp_ptr + 1;
            }
        if ((sp_brace_level > 0))
            braces_unbalanced_complaint(pop_lit1);
        ex_buf_length = ex_buf_ptr;
    }
}

void push_lit_stk(integer push_lt, stk_type push_type)
{

    lit_stack[lit_stk_ptr] = push_lt;
    lit_stk_type[lit_stk_ptr] = push_type;
    ;

    if ((lit_stk_ptr == lit_stk_size)) {
        BIB_XRETALLOC_NOSET("lit_stack", lit_stack, integer, lit_stk_size, lit_stk_size + LIT_STK_SIZE);
        BIB_XRETALLOC("lit_stk_type", lit_stk_type, stk_type, lit_stk_size, lit_stk_size + LIT_STK_SIZE);
    }
    lit_stk_ptr = lit_stk_ptr + 1;
}

void pop_lit_stk(integer * pop_lit, stk_type * pop_type)
{
    if ((lit_stk_ptr == 0)) {
        puts_log("You can't pop an empty literal stack");
        bst_ex_warn_print();
        *pop_type = 4 /*stk_empty */ ;
    } else {

        lit_stk_ptr = lit_stk_ptr - 1;
        *pop_lit = lit_stack[lit_stk_ptr];
        *pop_type = lit_stk_type[lit_stk_ptr];
        if ((*pop_type == 1 /*stk_str */ )) {

            if ((*pop_lit >= cmd_str_ptr)) {
                if ((*pop_lit != str_ptr - 1)) {
                    puts_log("Nontop top of string stack");
                    print_confusion();
                    longjmp(error_jmpbuf, 1);
                }
                {
                    str_ptr = str_ptr - 1;
                    pool_ptr = str_start[str_ptr];
                }
            }
        }
    }
}

void print_wrong_stk_lit(integer stk_lt, stk_type stk_tp1, stk_type stk_tp2)
{
    if ((stk_tp1 != 4 /*stk_empty */ )) {
        print_stk_lit(stk_lt, stk_tp1);
        switch ((stk_tp2)) {
        case 0:
            puts_log(", not an integer,");
            break;
        case 1:
            puts_log(", not a string,");
            break;
        case 2:
            puts_log(", not a function,");
            break;
        case 3:
        case 4:
            illegl_literal_confusion();
            break;
        default:
            unknwn_literal_confusion();
            break;
        }
        bst_ex_warn_print();
    }
}

void pop_top_and_print(void)
{
    integer stk_lt;
    stk_type stk_tp;
    pop_lit_stk(&stk_lt, &stk_tp);
    if ((stk_tp == 4 /*stk_empty */ )) {
        puts_log("Empty literal\n");
    } else
        print_lit(stk_lt, stk_tp);
}

void pop_whole_stack(void)
{
    while ((lit_stk_ptr > 0))
        pop_top_and_print();
}

void init_command_execution(void)
{
    lit_stk_ptr = 0;
    cmd_str_ptr = str_ptr;
}

void check_command_execution(void)
{
    if ((lit_stk_ptr != 0)) {
        printf_log("ptr=%ld, stack=\n", (long) lit_stk_ptr);
        pop_whole_stack();
        puts_log("---the literal stack isn't empty");
        bst_ex_warn_print();
    }
    if ((cmd_str_ptr != str_ptr)) {
        puts_log("Nonempty empty string stack");
        print_confusion();
        longjmp(error_jmpbuf, 1);
    }
}

void add_pool_buf_and_push(void)
{
    {
        while ((pool_ptr + ex_buf_length > pool_size))
            pool_overflow();
    }
    ex_buf_ptr = 0;
    while ((ex_buf_ptr < ex_buf_length)) {

        {
            str_pool[pool_ptr] = ex_buf[ex_buf_ptr];
            pool_ptr = pool_ptr + 1;
        }
        ex_buf_ptr = ex_buf_ptr + 1;
    }
    push_lit_stk(make_string(), 1 /*stk_str */ );
}

void add_buf_pool(str_number p_str)
{
    p_ptr1 = str_start[p_str];
    p_ptr2 = str_start[p_str + 1];
    if ((ex_buf_length + (p_ptr2 - p_ptr1) > buf_size))
        buffer_overflow();
    ex_buf_ptr = ex_buf_length;
    while ((p_ptr1 < p_ptr2)) {

        {
            ex_buf[ex_buf_ptr] = str_pool[p_ptr1];
            ex_buf_ptr = ex_buf_ptr + 1;
        }
        p_ptr1 = p_ptr1 + 1;
    }
    ex_buf_length = ex_buf_ptr;
}

void add_out_pool(str_number p_str)
{
    buf_pointer break_ptr;
    buf_pointer end_ptr;
    boolean break_pt_found;
    boolean unbreakable_tail;
    p_ptr1 = str_start[p_str];
    p_ptr2 = str_start[p_str + 1];
    while ((out_buf_length + (p_ptr2 - p_ptr1) > buf_size))
        buffer_overflow();
    out_buf_ptr = out_buf_length;
    while ((p_ptr1 < p_ptr2)) {

        out_buf[out_buf_ptr] = str_pool[p_ptr1];
        p_ptr1 = p_ptr1 + 1;
        out_buf_ptr = out_buf_ptr + 1;
    }
    out_buf_length = out_buf_ptr;
    unbreakable_tail = false;
    while (((out_buf_length > max_print_line) && (!unbreakable_tail))) {        /*324: */

        end_ptr = out_buf_length;
        out_buf_ptr = max_print_line;
        break_pt_found = false;
        while (((lex_class[out_buf[out_buf_ptr]] != 1 /*white_space */ ) && (out_buf_ptr >= min_print_line)))
            out_buf_ptr = out_buf_ptr - 1;
        if ((out_buf_ptr == min_print_line - 1)) {      /*325: */
            out_buf_ptr = max_print_line + 1;
            while ((out_buf_ptr < end_ptr))
                if ((lex_class[out_buf[out_buf_ptr]] != 1 /*white_space */ ))
                    out_buf_ptr = out_buf_ptr + 1;
                else
                    goto lab16;
 lab16:                        /*loop1_exit */ if ((out_buf_ptr == end_ptr))
                unbreakable_tail = true;
            else {

                break_pt_found = true;
                while ((out_buf_ptr + 1 < end_ptr))
                    if ((lex_class[out_buf[out_buf_ptr + 1]] == 1 /*white_space */ ))
                        out_buf_ptr = out_buf_ptr + 1;
                    else
                        goto lab17;
 lab17:                        /*loop2_exit */ ;
            }
        } else
            break_pt_found = true;
        if ((break_pt_found)) {
            out_buf_length = out_buf_ptr;
            break_ptr = out_buf_length + 1;
            output_bbl_line();
            out_buf[0] = 32 /*space */ ;
            out_buf[1] = 32 /*space */ ;
            out_buf_ptr = 2;
            tmp_ptr = break_ptr;
            while ((tmp_ptr < end_ptr)) {

                out_buf[out_buf_ptr] = out_buf[tmp_ptr];
                out_buf_ptr = out_buf_ptr + 1;
                tmp_ptr = tmp_ptr + 1;
            }
            out_buf_length = end_ptr - break_ptr + 2;
        }
    }
}

void x_equals(void)
{
    pop_lit_stk(&pop_lit1, &pop_typ1);
    pop_lit_stk(&pop_lit2, &pop_typ2);
    if ((pop_typ1 != pop_typ2)) {
        if (((pop_typ1 != 4 /*stk_empty */ ) && (pop_typ2 != 4 /*stk_empty */ ))) {
            print_stk_lit(pop_lit1, pop_typ1);
            puts_log(", ");
            print_stk_lit(pop_lit2, pop_typ2);
            putc_log('\n');
            puts_log("---they aren't the same literal types");
            bst_ex_warn_print();
        }
        push_lit_stk(0, 0 /*stk_int */ );
    } else if (((pop_typ1 != 0 /*stk_int */ ) && (pop_typ1 != 1 /*stk_str */ ))) {
        if ((pop_typ1 != 4 /*stk_empty */ )) {
            print_stk_lit(pop_lit1, pop_typ1);
            puts_log(", not an integer or a string,");
            bst_ex_warn_print();
        }
        push_lit_stk(0, 0 /*stk_int */ );
    } else if ((pop_typ1 == 0 /*stk_int */ )) {

        if ((pop_lit2 == pop_lit1))
            push_lit_stk(1, 0 /*stk_int */ );
        else
            push_lit_stk(0, 0 /*stk_int */ );
    } else if ((str_eq_str(pop_lit2, pop_lit1)))
        push_lit_stk(1, 0 /*stk_int */ );
    else
        push_lit_stk(0, 0 /*stk_int */ );
}

void x_greater_than(void)
{
    pop_lit_stk(&pop_lit1, &pop_typ1);
    pop_lit_stk(&pop_lit2, &pop_typ2);
    if ((pop_typ1 != 0 /*stk_int */ )) {
        print_wrong_stk_lit(pop_lit1, pop_typ1, 0 /*stk_int */ );
        push_lit_stk(0, 0 /*stk_int */ );
    } else if ((pop_typ2 != 0 /*stk_int */ )) {
        print_wrong_stk_lit(pop_lit2, pop_typ2, 0 /*stk_int */ );
        push_lit_stk(0, 0 /*stk_int */ );
    } else if ((pop_lit2 > pop_lit1))
        push_lit_stk(1, 0 /*stk_int */ );
    else
        push_lit_stk(0, 0 /*stk_int */ );
}

void x_less_than(void)
{
    pop_lit_stk(&pop_lit1, &pop_typ1);
    pop_lit_stk(&pop_lit2, &pop_typ2);
    if ((pop_typ1 != 0 /*stk_int */ )) {
        print_wrong_stk_lit(pop_lit1, pop_typ1, 0 /*stk_int */ );
        push_lit_stk(0, 0 /*stk_int */ );
    } else if ((pop_typ2 != 0 /*stk_int */ )) {
        print_wrong_stk_lit(pop_lit2, pop_typ2, 0 /*stk_int */ );
        push_lit_stk(0, 0 /*stk_int */ );
    } else if ((pop_lit2 < pop_lit1))
        push_lit_stk(1, 0 /*stk_int */ );
    else
        push_lit_stk(0, 0 /*stk_int */ );
}

void x_plus(void)
{
    pop_lit_stk(&pop_lit1, &pop_typ1);
    pop_lit_stk(&pop_lit2, &pop_typ2);
    if ((pop_typ1 != 0 /*stk_int */ )) {
        print_wrong_stk_lit(pop_lit1, pop_typ1, 0 /*stk_int */ );
        push_lit_stk(0, 0 /*stk_int */ );
    } else if ((pop_typ2 != 0 /*stk_int */ )) {
        print_wrong_stk_lit(pop_lit2, pop_typ2, 0 /*stk_int */ );
        push_lit_stk(0, 0 /*stk_int */ );
    } else
        push_lit_stk(pop_lit2 + pop_lit1, 0 /*stk_int */ );
}

void x_minus(void)
{
    pop_lit_stk(&pop_lit1, &pop_typ1);
    pop_lit_stk(&pop_lit2, &pop_typ2);
    if ((pop_typ1 != 0 /*stk_int */ )) {
        print_wrong_stk_lit(pop_lit1, pop_typ1, 0 /*stk_int */ );
        push_lit_stk(0, 0 /*stk_int */ );
    } else if ((pop_typ2 != 0 /*stk_int */ )) {
        print_wrong_stk_lit(pop_lit2, pop_typ2, 0 /*stk_int */ );
        push_lit_stk(0, 0 /*stk_int */ );
    } else
        push_lit_stk(pop_lit2 - pop_lit1, 0 /*stk_int */ );
}

void x_concatenate(void)
{
    pop_lit_stk(&pop_lit1, &pop_typ1);
    pop_lit_stk(&pop_lit2, &pop_typ2);
    if ((pop_typ1 != 1 /*stk_str */ )) {
        print_wrong_stk_lit(pop_lit1, pop_typ1, 1 /*stk_str */ );
        push_lit_stk(s_null, 1 /*stk_str */ );
    } else if ((pop_typ2 != 1 /*stk_str */ )) {
        print_wrong_stk_lit(pop_lit2, pop_typ2, 1 /*stk_str */ );
        push_lit_stk(s_null, 1 /*stk_str */ );
    } else {                    /*352: */

        if ((pop_lit2 >= cmd_str_ptr)) {

            if ((pop_lit1 >= cmd_str_ptr)) {
                str_start[pop_lit1] = str_start[pop_lit1 + 1];
                {
                    str_ptr = str_ptr + 1;
                    pool_ptr = str_start[str_ptr];
                }
                lit_stk_ptr = lit_stk_ptr + 1;
            } else if (((str_start[pop_lit2 + 1] - str_start[pop_lit2]) == 0))
                push_lit_stk(pop_lit1, 1 /*stk_str */ );
            else {

                pool_ptr = str_start[pop_lit2 + 1];
                {
                    while ((pool_ptr + (str_start[pop_lit1 + 1] - str_start[pop_lit1]) > pool_size))
                        pool_overflow();
                }
                sp_ptr = str_start[pop_lit1];
                sp_end = str_start[pop_lit1 + 1];
                while ((sp_ptr < sp_end)) {

                    {
                        str_pool[pool_ptr] = str_pool[sp_ptr];
                        pool_ptr = pool_ptr + 1;
                    }
                    sp_ptr = sp_ptr + 1;
                }
                push_lit_stk(make_string(), 1 /*stk_str */ );
            }
        } else {                /*353: */

            if ((pop_lit1 >= cmd_str_ptr)) {

                if (((str_start[pop_lit2 + 1] - str_start[pop_lit2]) == 0)) {
                    {
                        str_ptr = str_ptr + 1;
                        pool_ptr = str_start[str_ptr];
                    }
                    lit_stack[lit_stk_ptr] = pop_lit1;
                    lit_stk_ptr = lit_stk_ptr + 1;
                } else if (((str_start[pop_lit1 + 1] - str_start[pop_lit1]) == 0))
                    lit_stk_ptr = lit_stk_ptr + 1;
                else {

                    sp_length = (str_start[pop_lit1 + 1] - str_start[pop_lit1]);
                    sp2_length = (str_start[pop_lit2 + 1] - str_start[pop_lit2]);
                    {
                        while ((pool_ptr + sp_length + sp2_length > pool_size))
                            pool_overflow();
                    }
                    sp_ptr = str_start[pop_lit1 + 1];
                    sp_end = str_start[pop_lit1];
                    sp_xptr1 = sp_ptr + sp2_length;
                    while ((sp_ptr > sp_end)) {

                        sp_ptr = sp_ptr - 1;
                        sp_xptr1 = sp_xptr1 - 1;
                        str_pool[sp_xptr1] = str_pool[sp_ptr];
                    }
                    sp_ptr = str_start[pop_lit2];
                    sp_end = str_start[pop_lit2 + 1];
                    while ((sp_ptr < sp_end)) {

                        {
                            str_pool[pool_ptr] = str_pool[sp_ptr];
                            pool_ptr = pool_ptr + 1;
                        }
                        sp_ptr = sp_ptr + 1;
                    }
                    pool_ptr = pool_ptr + sp_length;
                    push_lit_stk(make_string(), 1 /*stk_str */ );
                }
            } else {            /*354: */

                if (((str_start[pop_lit1 + 1] - str_start[pop_lit1]) == 0))
                    lit_stk_ptr = lit_stk_ptr + 1;
                else if (((str_start[pop_lit2 + 1] - str_start[pop_lit2]) == 0))
                    push_lit_stk(pop_lit1, 1 /*stk_str */ );
                else {

                    {
                        while ((pool_ptr + (str_start[pop_lit1 + 1] - str_start[pop_lit1]) +
                                (str_start[pop_lit2 + 1] - str_start[pop_lit2]) > pool_size))
                            pool_overflow();
                    }
                    sp_ptr = str_start[pop_lit2];
                    sp_end = str_start[pop_lit2 + 1];
                    while ((sp_ptr < sp_end)) {

                        {
                            str_pool[pool_ptr] = str_pool[sp_ptr];
                            pool_ptr = pool_ptr + 1;
                        }
                        sp_ptr = sp_ptr + 1;
                    }
                    sp_ptr = str_start[pop_lit1];
                    sp_end = str_start[pop_lit1 + 1];
                    while ((sp_ptr < sp_end)) {

                        {
                            str_pool[pool_ptr] = str_pool[sp_ptr];
                            pool_ptr = pool_ptr + 1;
                        }
                        sp_ptr = sp_ptr + 1;
                    }
                    push_lit_stk(make_string(), 1 /*stk_str */ );
                }
            }
        }
    }
}

void x_gets(void)
{
    pop_lit_stk(&pop_lit1, &pop_typ1);
    pop_lit_stk(&pop_lit2, &pop_typ2);
    if ((pop_typ1 != 2 /*stk_fn */ ))
        print_wrong_stk_lit(pop_lit1, pop_typ1, 2 /*stk_fn */ );
    else if (((!mess_with_entries)
              && ((fn_type[pop_lit1] == 6 /*str_entry_var */ ) || (fn_type[pop_lit1] == 5 /*int_entry_var */ ))))
        bst_cant_mess_with_entries_print();
    else
        switch ((fn_type[pop_lit1])) {
        case 5:
            /*
               356: */ if ((pop_typ2 != 0 /*stk_int */ ))
                print_wrong_stk_lit(pop_lit2, pop_typ2, 0 /*stk_int */ );
            else
                entry_ints[cite_ptr * num_ent_ints + ilk_info[pop_lit1]] = /*:356 */ pop_lit2;
            break;
        case 6:
            {
                if ((pop_typ2 != 1 /*stk_str */ ))
                    print_wrong_stk_lit(pop_lit2, pop_typ2, 1 /*stk_str */ );
                else {

                    str_ent_ptr = cite_ptr * num_ent_strs + ilk_info[pop_lit1];
                    ent_chr_ptr = 0;
                    sp_ptr = str_start[pop_lit2];
                    sp_xptr1 = str_start[pop_lit2 + 1];
                    if ((sp_xptr1 - sp_ptr > ent_str_size)) {
                        {
                            bst_1print_string_size_exceeded();
                            printf_log("%ld, the entry", (long) ent_str_size);
                            bst_2print_string_size_exceeded();
                        }
                        sp_xptr1 = sp_ptr + ent_str_size;
                    }
                    while ((sp_ptr < sp_xptr1)) {

                        entry_strs[(str_ent_ptr) * (ent_str_size + 1) + (ent_chr_ptr)] = str_pool[sp_ptr];
                        ent_chr_ptr = ent_chr_ptr + 1;
                        sp_ptr = sp_ptr + 1;
                    }
                    entry_strs[(str_ent_ptr) * (ent_str_size + 1) + (ent_chr_ptr)] = 127 /*end_of_string */ ;
                }
            }
            break;
        case 7:
            if ((pop_typ2 != 0 /*stk_int */ ))
                print_wrong_stk_lit(pop_lit2, pop_typ2, 0 /*stk_int */ );
            else
                ilk_info[pop_lit1] = /*:359 */ pop_lit2;
            break;
        case 8:
            {
                if ((pop_typ2 != 1 /*stk_str */ ))
                    print_wrong_stk_lit(pop_lit2, pop_typ2, 1 /*stk_str */ );
                else {

                    str_glb_ptr = ilk_info[pop_lit1];
                    if ((pop_lit2 < cmd_str_ptr))
                        glb_str_ptr[str_glb_ptr] = pop_lit2;
                    else {

                        glb_str_ptr[str_glb_ptr] = 0;
                        glob_chr_ptr = 0;
                        sp_ptr = str_start[pop_lit2];
                        sp_end = str_start[pop_lit2 + 1];
                        if ((sp_end - sp_ptr > glob_str_size)) {
                            {
                                bst_1print_string_size_exceeded();
                                printf_log("%ld, the global", (long) glob_str_size);
                                bst_2print_string_size_exceeded();
                            }
                            sp_end = sp_ptr + glob_str_size;
                        }
                        while ((sp_ptr < sp_end)) {

                            global_strs[(str_glb_ptr) * (glob_str_size + 1) + (glob_chr_ptr)] = str_pool[sp_ptr];
                            glob_chr_ptr = glob_chr_ptr + 1;
                            sp_ptr = sp_ptr + 1;
                        }
                        glb_str_end[str_glb_ptr] = glob_chr_ptr;
                    }
                }
            }
            break;
        default:
            puts_log("You can't assign to type ");
            print_fn_class(pop_lit1);
            puts_log(", a nonvariable function class");
            bst_ex_warn_print();
            break;
        }
}

void x_add_period(void)
{
    pop_lit_stk(&pop_lit1, &pop_typ1);
    if ((pop_typ1 != 1 /*stk_str */ )) {
        print_wrong_stk_lit(pop_lit1, pop_typ1, 1 /*stk_str */ );
        push_lit_stk(s_null, 1 /*stk_str */ );
    } else if (((str_start[pop_lit1 + 1] - str_start[pop_lit1]) == 0))
        push_lit_stk(s_null, 1 /*stk_str */ );
    else {                      /*362: */

        sp_ptr = str_start[pop_lit1 + 1];
        sp_end = str_start[pop_lit1];
        while ((sp_ptr > sp_end)) {

            sp_ptr = sp_ptr - 1;
            if ((str_pool[sp_ptr] != 125 /*right_brace */ ))
                goto loop_exit;
        }
 loop_exit:
        switch ((str_pool[sp_ptr])) {
        case 46:
        case 63:
        case 33:
            {
                if ((lit_stack[lit_stk_ptr] >= cmd_str_ptr)) {
                    str_ptr = str_ptr + 1;
                    pool_ptr = str_start[str_ptr];
                }
                lit_stk_ptr = lit_stk_ptr + 1;
            }
            break;
        default:
            {
                if ((pop_lit1 < cmd_str_ptr)) {
                    {
                        while ((pool_ptr + (str_start[pop_lit1 + 1] - str_start[pop_lit1]) + 1 > pool_size))
                            pool_overflow();
                    }
                    sp_ptr = str_start[pop_lit1];
                    sp_end = str_start[pop_lit1 + 1];
                    while ((sp_ptr < sp_end)) {

                        {
                            str_pool[pool_ptr] = str_pool[sp_ptr];
                            pool_ptr = pool_ptr + 1;
                        }
                        sp_ptr = sp_ptr + 1;
                    }
                } else {

                    pool_ptr = str_start[pop_lit1 + 1];
                    {
                        while ((pool_ptr + 1 > pool_size))
                            pool_overflow();
                    }
                }
                {
                    str_pool[pool_ptr] = 46 /*period */ ;
                    pool_ptr = pool_ptr + 1;
                }
                push_lit_stk(make_string(), 1 /*stk_str */ );
            }
            break;
        }
    }
}

void x_change_case(void)
{
    pop_lit_stk(&pop_lit1, &pop_typ1);
    pop_lit_stk(&pop_lit2, &pop_typ2);
    if ((pop_typ1 != 1 /*stk_str */ )) {
        print_wrong_stk_lit(pop_lit1, pop_typ1, 1 /*stk_str */ );
        push_lit_stk(s_null, 1 /*stk_str */ );
    } else if ((pop_typ2 != 1 /*stk_str */ )) {
        print_wrong_stk_lit(pop_lit2, pop_typ2, 1 /*stk_str */ );
        push_lit_stk(s_null, 1 /*stk_str */ );
    } else {

        {
            switch ((str_pool[str_start[pop_lit1]])) {
            case 116:
            case 84:
                conversion_type = 0 /*title_lowers */ ;
                break;
            case 108:
            case 76:
                conversion_type = 1 /*all_lowers */ ;
                break;
            case 117:
            case 85:
                conversion_type = 2 /*all_uppers */ ;
                break;
            default:
                conversion_type = 3 /*bad_conversion */ ;
                break;
            }
            if ((((str_start[pop_lit1 + 1] - str_start[pop_lit1]) != 1) || (conversion_type == 3 /*bad_conversion */ ))) {
                conversion_type = 3 /*bad_conversion */ ;
                print_a_pool_str(pop_lit1);
                puts_log(" is an illegal case-conversion string");
                bst_ex_warn_print();
            }
        }
        ex_buf_length = 0;
        add_buf_pool(pop_lit2);
        {
            brace_level = 0;
            ex_buf_ptr = 0;
            while ((ex_buf_ptr < ex_buf_length)) {

                if ((ex_buf[ex_buf_ptr] == 123 /*left_brace */ )) {
                    brace_level = brace_level + 1;
                    if ((brace_level != 1))
                        goto lab21;
                    if ((ex_buf_ptr + 4 > ex_buf_length))
                        goto lab21;
                    else if ((ex_buf[ex_buf_ptr + 1] != 92 /*backslash */ ))
                        goto lab21;
                    if ((conversion_type == 0 /*title_lowers */ )) {

                        if ((ex_buf_ptr == 0))
                            goto lab21;
                        else if (((prev_colon) && (lex_class[ex_buf[ex_buf_ptr - 1]] == 1 /*white_space */ )))
                            goto lab21;
                    }
                    {
                        ex_buf_ptr = ex_buf_ptr + 1;
                        while (((ex_buf_ptr < ex_buf_length) && (brace_level > 0))) {

                            ex_buf_ptr = ex_buf_ptr + 1;
                            ex_buf_xptr = ex_buf_ptr;
                            while (((ex_buf_ptr < ex_buf_length) && (lex_class[ex_buf[ex_buf_ptr]] == 2 /*alpha */ )))
                                ex_buf_ptr = ex_buf_ptr + 1;
                            control_seq_loc =
                                str_lookup(ex_buf, ex_buf_xptr, ex_buf_ptr - ex_buf_xptr, 14 /*control_seq_ilk */ ,
                                           false);
                            if ((hash_found)) { /*373: */
                                switch ((conversion_type)) {
                                case 0:
                                case 1:
                                    switch ((ilk_info[control_seq_loc])) {
                                    case 11:
                                    case 9:
                                    case 3:
                                    case 5:
                                    case 7:
                                        lower_case(ex_buf, ex_buf_xptr, ex_buf_ptr - ex_buf_xptr);
                                        break;
                                    default:
                                        ;
                                        break;
                                    }
                                    break;
                                case 2:
                                    switch ((ilk_info[control_seq_loc])) {
                                    case 10:
                                    case 8:
                                    case 2:
                                    case 4:
                                    case 6:
                                        upper_case(ex_buf, ex_buf_xptr, ex_buf_ptr - ex_buf_xptr);
                                        break;
                                    case 0:
                                    case 1:
                                    case 12:
                                        {
                                            upper_case(ex_buf, ex_buf_xptr, ex_buf_ptr - ex_buf_xptr);
                                            while ((ex_buf_xptr < ex_buf_ptr)) {

                                                ex_buf[ex_buf_xptr - 1] = ex_buf[ex_buf_xptr];
                                                ex_buf_xptr = ex_buf_xptr + 1;
                                            }
                                            ex_buf_xptr = ex_buf_xptr - 1;
                                            while (((ex_buf_ptr < ex_buf_length)
                                                    && (lex_class[ex_buf[ex_buf_ptr]] == 1 /*white_space */ )))
                                                ex_buf_ptr = ex_buf_ptr + 1;
                                            tmp_ptr = ex_buf_ptr;
                                            while ((tmp_ptr < ex_buf_length)) {

                                                ex_buf[tmp_ptr - (ex_buf_ptr - ex_buf_xptr)] = ex_buf[tmp_ptr];
                                                tmp_ptr = tmp_ptr + 1;
                                            }
                                            ex_buf_length = tmp_ptr - (ex_buf_ptr - ex_buf_xptr);
                                            ex_buf_ptr = ex_buf_xptr;
                                        }
                                        break;
                                    default:
                                        ;
                                        break;
                                    }
                                    break;
                                case 3:
                                    ;
                                    break;
                                default:
                                    case_conversion_confusion();
                                    break;
                                }
                            }
                            ex_buf_xptr = ex_buf_ptr;
                            while (((ex_buf_ptr < ex_buf_length) && (brace_level > 0)
                                    && (ex_buf[ex_buf_ptr] != 92 /*backslash */ ))) {

                                if ((ex_buf[ex_buf_ptr] == 125 /*right_brace */ ))
                                    brace_level = brace_level - 1;
                                else if ((ex_buf[ex_buf_ptr] == 123 /*left_brace */ ))
                                    brace_level = brace_level + 1;
                                ex_buf_ptr = ex_buf_ptr + 1;
                            }
                            {
                                switch ((conversion_type)) {
                                case 0:
                                case 1:
                                    lower_case(ex_buf, ex_buf_xptr, ex_buf_ptr - ex_buf_xptr);
                                    break;
                                case 2:
                                    upper_case(ex_buf, ex_buf_xptr, ex_buf_ptr - ex_buf_xptr);
                                    break;
                                case 3:
                                    ;
                                    break;
                                default:
                                    case_conversion_confusion();
                                    break;
                                }
                            }
                        }
                        ex_buf_ptr = ex_buf_ptr - 1;
                    }
 lab21:                        /*ok_pascal_i_give_up */ prev_colon = false;
                } else if ((ex_buf[ex_buf_ptr] == 125 /*right_brace */ )) {
                    decr_brace_level(pop_lit2);
                    prev_colon = false;
                } else if ((brace_level == 0)) {        /*377: */
                    switch ((conversion_type)) {
                    case 0:
                        {
                            if ((ex_buf_ptr == 0)) ;
                            else if (((prev_colon) && (lex_class[ex_buf[ex_buf_ptr - 1]] == 1 /*white_space */ ))) ;
                            else
                                lower_case(ex_buf, ex_buf_ptr, 1);
                            if ((ex_buf[ex_buf_ptr] == 58 /*colon */ ))
                                prev_colon = true;
                            else if ((lex_class[ex_buf[ex_buf_ptr]] != 1 /*white_space */ ))
                                prev_colon = false;
                        }
                        break;
                    case 1:
                        lower_case(ex_buf, ex_buf_ptr, 1);
                        break;
                    case 2:
                        upper_case(ex_buf, ex_buf_ptr, 1);
                        break;
                    case 3:
                        ;
                        break;
                    default:
                        case_conversion_confusion();
                        break;
                    }
                }
                ex_buf_ptr = ex_buf_ptr + 1;
            }
            check_brace_level(pop_lit2);
        }
        add_pool_buf_and_push();
    }
}

void x_chr_to_int(void)
{
    pop_lit_stk(&pop_lit1, &pop_typ1);
    if ((pop_typ1 != 1 /*stk_str */ )) {
        print_wrong_stk_lit(pop_lit1, pop_typ1, 1 /*stk_str */ );
        push_lit_stk(0, 0 /*stk_int */ );
    } else if (((str_start[pop_lit1 + 1] - str_start[pop_lit1]) != 1)) {
        putc_log('"');
        print_a_pool_str(pop_lit1);
        puts_log("\" isn't a single character");
        bst_ex_warn_print();
        push_lit_stk(0, 0 /*stk_int */ );
    } else
        push_lit_stk(str_pool[str_start[pop_lit1]], 0 /*stk_int */ );
}

void x_cite(void)
{
    if ((!mess_with_entries))
        bst_cant_mess_with_entries_print();
    else
        push_lit_stk(cite_list[cite_ptr], 1 /*stk_str */ );
}

void x_duplicate(void)
{
    pop_lit_stk(&pop_lit1, &pop_typ1);
    if ((pop_typ1 != 1 /*stk_str */ )) {
        push_lit_stk(pop_lit1, pop_typ1);
        push_lit_stk(pop_lit1, pop_typ1);
    } else {

        {
            if ((lit_stack[lit_stk_ptr] >= cmd_str_ptr)) {
                str_ptr = str_ptr + 1;
                pool_ptr = str_start[str_ptr];
            }
            lit_stk_ptr = lit_stk_ptr + 1;
        }
        if ((pop_lit1 < cmd_str_ptr))
            push_lit_stk(pop_lit1, pop_typ1);
        else {

            {
                while ((pool_ptr + (str_start[pop_lit1 + 1] - str_start[pop_lit1]) > pool_size))
                    pool_overflow();
            }
            sp_ptr = str_start[pop_lit1];
            sp_end = str_start[pop_lit1 + 1];
            while ((sp_ptr < sp_end)) {

                {
                    str_pool[pool_ptr] = str_pool[sp_ptr];
                    pool_ptr = pool_ptr + 1;
                }
                sp_ptr = sp_ptr + 1;
            }
            push_lit_stk(make_string(), 1 /*stk_str */ );
        }
    }
}

void x_empty(void)
{
    pop_lit_stk(&pop_lit1, &pop_typ1);
    switch ((pop_typ1)) {
    case 1:
        {
            sp_ptr = str_start[pop_lit1];
            sp_end = str_start[pop_lit1 + 1];
            while ((sp_ptr < sp_end)) {

                if ((lex_class[str_pool[sp_ptr]] != 1 /*white_space */ )) {
                    push_lit_stk(0, 0 /*stk_int */ );
                    goto exit;
                }
                sp_ptr = sp_ptr + 1;
            }
            push_lit_stk(1, 0 /*stk_int */ );
        }
        break;
    case 3:
        push_lit_stk(1, 0 /*stk_int */ );
        break;
    case 4:
        push_lit_stk(0, 0 /*stk_int */ );
        break;
    default:
        print_stk_lit(pop_lit1, pop_typ1);
        puts_log(", not a string or missing field,");
        bst_ex_warn_print();
        push_lit_stk(0, 0 /*stk_int */ );
        break;
    }
 exit: ;
}

void x_format_name(void)
{
    pop_lit_stk(&pop_lit1, &pop_typ1);
    pop_lit_stk(&pop_lit2, &pop_typ2);
    pop_lit_stk(&pop_lit3, &pop_typ3);
    if ((pop_typ1 != 1 /*stk_str */ )) {
        print_wrong_stk_lit(pop_lit1, pop_typ1, 1 /*stk_str */ );
        push_lit_stk(s_null, 1 /*stk_str */ );
    } else if ((pop_typ2 != 0 /*stk_int */ )) {
        print_wrong_stk_lit(pop_lit2, pop_typ2, 0 /*stk_int */ );
        push_lit_stk(s_null, 1 /*stk_str */ );
    } else if ((pop_typ3 != 1 /*stk_str */ )) {
        print_wrong_stk_lit(pop_lit3, pop_typ3, 1 /*stk_str */ );
        push_lit_stk(s_null, 1 /*stk_str */ );
    } else {

        ex_buf_length = 0;
        add_buf_pool(pop_lit3);
        {
            ex_buf_ptr = 0;
            num_names = 0;
            while (((num_names < pop_lit2) && (ex_buf_ptr < ex_buf_length))) {

                num_names = num_names + 1;
                ex_buf_xptr = ex_buf_ptr;
                name_scan_for_and(pop_lit3);
            }
            if ((ex_buf_ptr < ex_buf_length))
                ex_buf_ptr = ex_buf_ptr - 4;
            if ((num_names < pop_lit2)) {
                if ((pop_lit2 == 1)) {
                    puts_log("There is no name in \"");
                } else {
                    printf_log("There aren't %ld names in \"", (long) pop_lit2);
                }
                print_a_pool_str(pop_lit3);
                {
                    putc_log('"');
                    bst_ex_warn_print();
                }
            }
        }
        {
            {
                while ((ex_buf_ptr > ex_buf_xptr))
                    switch ((lex_class[ex_buf[ex_buf_ptr - 1]])) {
                    case 1:
                    case 4:
                        ex_buf_ptr = ex_buf_ptr - 1;
                        break;
                    default:
                        if ((ex_buf[ex_buf_ptr - 1] == 44 /*comma */ )) {
                            printf_log("Name %ld in \"", (long) pop_lit2);
                            print_a_pool_str(pop_lit3);
                            puts_log("\" has a comma at the end");
                            bst_ex_warn_print();
                            ex_buf_ptr = ex_buf_ptr - 1;
                        } else
                            goto lab16;
                        break;
                    }
 lab16:                        /*loop1_exit */ ;
            }
            name_bf_ptr = 0;
            num_commas = 0;
            num_tokens = 0;
            token_starting = true;
            while ((ex_buf_xptr < ex_buf_ptr))
                switch ((ex_buf[ex_buf_xptr])) {
                case 44:
                    {
                        if ((num_commas == 2)) {
                            printf_log("Too many commas in name %ld of \"", (long) pop_lit2);
                            print_a_pool_str(pop_lit3);
                            putc_log('"');
                            bst_ex_warn_print();
                        } else {

                            num_commas = num_commas + 1;
                            if ((num_commas == 1))
                                comma1 = num_tokens;
                            else
                                comma2 = num_tokens;
                            name_sep_char[num_tokens] = 44 /*comma */ ;
                        }
                        ex_buf_xptr = ex_buf_xptr + 1;
                        token_starting = true;
                    }
                    break;
                case 123:
                    {
                        brace_level = brace_level + 1;
                        if ((token_starting)) {
                            name_tok[num_tokens] = name_bf_ptr;
                            num_tokens = num_tokens + 1;
                        }
                        sv_buffer[name_bf_ptr] = ex_buf[ex_buf_xptr];
                        name_bf_ptr = name_bf_ptr + 1;
                        ex_buf_xptr = ex_buf_xptr + 1;
                        while (((brace_level > 0) && (ex_buf_xptr < ex_buf_ptr))) {

                            if ((ex_buf[ex_buf_xptr] == 125 /*right_brace */ ))
                                brace_level = brace_level - 1;
                            else if ((ex_buf[ex_buf_xptr] == 123 /*left_brace */ ))
                                brace_level = brace_level + 1;
                            sv_buffer[name_bf_ptr] = ex_buf[ex_buf_xptr];
                            name_bf_ptr = name_bf_ptr + 1;
                            ex_buf_xptr = ex_buf_xptr + 1;
                        }
                        token_starting = false;
                    }
                    break;
                case 125:
                    {
                        if ((token_starting)) {
                            name_tok[num_tokens] = name_bf_ptr;
                            num_tokens = num_tokens + 1;
                        }

                        printf_log("Name %ld of \"", (long) pop_lit2);
                        print_a_pool_str(pop_lit3);
                        puts_log("\" isn't brace balanced");
                        bst_ex_warn_print();
                        ex_buf_xptr = ex_buf_xptr + 1;
                        token_starting = false;
                    }
                    break;
                default:
                    switch ((lex_class[ex_buf[ex_buf_xptr]])) {
                    case 1:
                        {
                            if ((!token_starting))
                                name_sep_char[num_tokens] = 32 /*space */ ;
                            ex_buf_xptr = ex_buf_xptr + 1;
                            token_starting = true;
                        }
                        break;
                    case 4:
                        {
                            if ((!token_starting))
                                name_sep_char[num_tokens] = ex_buf[ex_buf_xptr];
                            ex_buf_xptr = ex_buf_xptr + 1;
                            token_starting = true;
                        }
                        break;
                    default:
                        {
                            if ((token_starting)) {
                                name_tok[num_tokens] = name_bf_ptr;
                                num_tokens = num_tokens + 1;
                            }
                            sv_buffer[name_bf_ptr] = ex_buf[ex_buf_xptr];
                            name_bf_ptr = name_bf_ptr + 1;
                            ex_buf_xptr = ex_buf_xptr + 1;
                            token_starting = false;
                        }
                        break;
                    }
                    break;
                }
            name_tok[num_tokens] = name_bf_ptr;
        }
        {
            if ((num_commas == 0)) {
                first_start = 0;
                last_end = num_tokens;
                jr_end = last_end;
                {
                    von_start = 0;
                    while ((von_start < last_end - 1)) {

                        name_bf_ptr = name_tok[von_start];
                        name_bf_xptr = name_tok[von_start + 1];
                        if ((von_token_found())) {
                            von_name_ends_and_last_name_starts_stuff();
                            goto lab52;
                        }
                        von_start = von_start + 1;
                    }
                    while ((von_start > 0)) {

                        if (((lex_class[name_sep_char[von_start]] != 4 /*sep_char */ )
                             || (name_sep_char[von_start] == 126 /*tie */ )))
                            goto lab17;
                        von_start = von_start - 1;
                    }
 lab17:                        /*loop2_exit */ von_end = von_start;
 lab52:                        /*von_found */ first_end = von_start;
                }
            } else if ((num_commas == 1)) {
                von_start = 0;
                last_end = comma1;
                jr_end = last_end;
                first_start = jr_end;
                first_end = num_tokens;
                von_name_ends_and_last_name_starts_stuff();
            } else if ((num_commas == 2)) {
                von_start = 0;
                last_end = comma1;
                jr_end = comma2;
                first_start = jr_end;
                first_end = num_tokens;
                von_name_ends_and_last_name_starts_stuff();
            } else {
                puts_log("Illegal number of comma,s");
                print_confusion();
                longjmp(error_jmpbuf, 1);
            }
        }
        ex_buf_length = 0;
        add_buf_pool(pop_lit1);
        figure_out_the_formatted_name();
        add_pool_buf_and_push();
    }
}

void x_int_to_chr(void)
{
    pop_lit_stk(&pop_lit1, &pop_typ1);
    if ((pop_typ1 != 0 /*stk_int */ )) {
        print_wrong_stk_lit(pop_lit1, pop_typ1, 0 /*stk_int */ );
        push_lit_stk(s_null, 1 /*stk_str */ );
    } else if (((pop_lit1 < 0) || (pop_lit1 > 127))) {
        printf_log("%ld isn't valid ASCII", (long) pop_lit1);
        bst_ex_warn_print();
        push_lit_stk(s_null, 1 /*stk_str */ );
    } else {

        {
            while ((pool_ptr + 1 > pool_size))
                pool_overflow();
        }
        {
            str_pool[pool_ptr] = pop_lit1;
            pool_ptr = pool_ptr + 1;
        }
        push_lit_stk(make_string(), 1 /*stk_str */ );
    }
}

void x_int_to_str(void)
{
    pop_lit_stk(&pop_lit1, &pop_typ1);
    if ((pop_typ1 != 0 /*stk_int */ )) {
        print_wrong_stk_lit(pop_lit1, pop_typ1, 0 /*stk_int */ );
        push_lit_stk(s_null, 1 /*stk_str */ );
    } else {

        int_to_ASCII(pop_lit1, ex_buf, 0, &ex_buf_length);
        add_pool_buf_and_push();
    }
}

void x_missing(void)
{
    pop_lit_stk(&pop_lit1, &pop_typ1);
    if ((!mess_with_entries))
        bst_cant_mess_with_entries_print();
    else if (((pop_typ1 != 1 /*stk_str */ ) && (pop_typ1 != 3 /*stk_field_missing */ ))) {
        if ((pop_typ1 != 4 /*stk_empty */ )) {
            print_stk_lit(pop_lit1, pop_typ1);
            puts_log(", not a string or missing field,");
            bst_ex_warn_print();
        }
        push_lit_stk(0, 0 /*stk_int */ );
    } else if ((pop_typ1 == 3 /*stk_field_missing */ ))
        push_lit_stk(1, 0 /*stk_int */ );
    else
        push_lit_stk(0, 0 /*stk_int */ );
}

void x_num_names(void)
{
    pop_lit_stk(&pop_lit1, &pop_typ1);
    if ((pop_typ1 != 1 /*stk_str */ )) {
        print_wrong_stk_lit(pop_lit1, pop_typ1, 1 /*stk_str */ );
        push_lit_stk(0, 0 /*stk_int */ );
    } else {

        ex_buf_length = 0;
        add_buf_pool(pop_lit1);
        {
            ex_buf_ptr = 0;
            num_names = 0;
            while ((ex_buf_ptr < ex_buf_length)) {

                name_scan_for_and(pop_lit1);
                num_names = num_names + 1;
            }
        }
        push_lit_stk(num_names, 0 /*stk_int */ );
    }
}

void x_preamble(void)
{
    ex_buf_length = 0;
    preamble_ptr = 0;
    while ((preamble_ptr < num_preamble_strings)) {

        add_buf_pool(s_preamble[preamble_ptr]);
        preamble_ptr = preamble_ptr + 1;
    }
    add_pool_buf_and_push();
}

void x_purify(void)
{
    pop_lit_stk(&pop_lit1, &pop_typ1);
    if ((pop_typ1 != 1 /*stk_str */ )) {
        print_wrong_stk_lit(pop_lit1, pop_typ1, 1 /*stk_str */ );
        push_lit_stk(s_null, 1 /*stk_str */ );
    } else {

        ex_buf_length = 0;
        add_buf_pool(pop_lit1);
        {
            brace_level = 0;
            ex_buf_xptr = 0;
            ex_buf_ptr = 0;
            while ((ex_buf_ptr < ex_buf_length)) {

                switch ((lex_class[ex_buf[ex_buf_ptr]])) {
                case 1:
                case 4:
                    {
                        ex_buf[ex_buf_xptr] = 32 /*space */ ;
                        ex_buf_xptr = ex_buf_xptr + 1;
                    }
                    break;
                case 2:
                case 3:
                    {
                        ex_buf[ex_buf_xptr] = ex_buf[ex_buf_ptr];
                        ex_buf_xptr = ex_buf_xptr + 1;
                    }
                    break;
                default:
                    if ((ex_buf[ex_buf_ptr] == 123 /*left_brace */ )) {
                        brace_level = brace_level + 1;
                        if (((brace_level == 1) && (ex_buf_ptr + 1 < ex_buf_length))) {

                            if ((ex_buf[ex_buf_ptr + 1] == 92 /*backslash */ )) {       /*433: */
                                ex_buf_ptr = ex_buf_ptr + 1;
                                while (((ex_buf_ptr < ex_buf_length) && (brace_level > 0))) {

                                    ex_buf_ptr = ex_buf_ptr + 1;
                                    ex_buf_yptr = ex_buf_ptr;
                                    while (((ex_buf_ptr < ex_buf_length)
                                            && (lex_class[ex_buf[ex_buf_ptr]] == 2 /*alpha */ )))
                                        ex_buf_ptr = ex_buf_ptr + 1;
                                    control_seq_loc =
                                        str_lookup(ex_buf, ex_buf_yptr, ex_buf_ptr - ex_buf_yptr,
                                                   14 /*control_seq_ilk */ , false);
                                    if ((hash_found)) { /*434: */
                                        ex_buf[ex_buf_xptr] = ex_buf[ex_buf_yptr];
                                        ex_buf_xptr = ex_buf_xptr + 1;
                                        switch ((ilk_info[control_seq_loc])) {
                                        case 2:
                                        case 3:
                                        case 4:
                                        case 5:
                                        case 12:
                                            {
                                                ex_buf[ex_buf_xptr] = ex_buf[ex_buf_yptr + 1];
                                                ex_buf_xptr = ex_buf_xptr + 1;
                                            }
                                            break;
                                        default:
                                            ;
                                            break;
                                        }
                                    }
                                    while (((ex_buf_ptr < ex_buf_length) && (brace_level > 0)
                                            && (ex_buf[ex_buf_ptr] != 92 /*backslash */ ))) {

                                        switch ((lex_class[ex_buf[ex_buf_ptr]])) {
                                        case 2:
                                        case 3:
                                            {
                                                ex_buf[ex_buf_xptr] = ex_buf[ex_buf_ptr];
                                                ex_buf_xptr = ex_buf_xptr + 1;
                                            }
                                            break;
                                        default:
                                            if ((ex_buf[ex_buf_ptr] == 125 /*right_brace */ ))
                                                brace_level = brace_level - 1;
                                            else if ((ex_buf[ex_buf_ptr] == 123 /*left_brace */ ))
                                                brace_level = brace_level + 1;
                                            break;
                                        }
                                        ex_buf_ptr = ex_buf_ptr + 1;
                                    }
                                }
                                ex_buf_ptr = ex_buf_ptr - 1;
                            }
                        }
                    } else if ((ex_buf[ex_buf_ptr] == 125 /*right_brace */ )) {

                        if ((brace_level > 0))
                            brace_level = brace_level - 1;
                    }
                    break;
                }
                ex_buf_ptr = ex_buf_ptr + 1;
            }
            ex_buf_length = ex_buf_xptr;
        }
        add_pool_buf_and_push();
    }
}

void x_quote(void)
{
    {
        while ((pool_ptr + 1 > pool_size))
            pool_overflow();
    }
    {
        str_pool[pool_ptr] = 34 /*double_quote */ ;
        pool_ptr = pool_ptr + 1;
    }
    push_lit_stk(make_string(), 1 /*stk_str */ );
}

void x_substring(void)
{
    pop_lit_stk(&pop_lit1, &pop_typ1);
    pop_lit_stk(&pop_lit2, &pop_typ2);
    pop_lit_stk(&pop_lit3, &pop_typ3);
    if ((pop_typ1 != 0 /*stk_int */ )) {
        print_wrong_stk_lit(pop_lit1, pop_typ1, 0 /*stk_int */ );
        push_lit_stk(s_null, 1 /*stk_str */ );
    } else if ((pop_typ2 != 0 /*stk_int */ )) {
        print_wrong_stk_lit(pop_lit2, pop_typ2, 0 /*stk_int */ );
        push_lit_stk(s_null, 1 /*stk_str */ );
    } else if ((pop_typ3 != 1 /*stk_str */ )) {
        print_wrong_stk_lit(pop_lit3, pop_typ3, 1 /*stk_str */ );
        push_lit_stk(s_null, 1 /*stk_str */ );
    } else {

        sp_length = (str_start[pop_lit3 + 1] - str_start[pop_lit3]);
        if ((pop_lit1 >= sp_length)) {

            if (((pop_lit2 == 1) || (pop_lit2 == -1))) {
                {
                    if ((lit_stack[lit_stk_ptr] >= cmd_str_ptr)) {
                        str_ptr = str_ptr + 1;
                        pool_ptr = str_start[str_ptr];
                    }
                    lit_stk_ptr = lit_stk_ptr + 1;
                }
                goto exit;
            }
        }
        if (((pop_lit1 <= 0) || (pop_lit2 == 0) || (pop_lit2 > sp_length) || (pop_lit2 < -(integer) sp_length))) {
            push_lit_stk(s_null, 1 /*stk_str */ );
            goto exit;
        } else {                /*439: */

            if ((pop_lit2 > 0)) {
                if ((pop_lit1 > sp_length - (pop_lit2 - 1)))
                    pop_lit1 = sp_length - (pop_lit2 - 1);
                sp_ptr = str_start[pop_lit3] + (pop_lit2 - 1);
                sp_end = sp_ptr + pop_lit1;
                if ((pop_lit2 == 1)) {

                    if ((pop_lit3 >= cmd_str_ptr)) {
                        str_start[pop_lit3 + 1] = sp_end;
                        {
                            str_ptr = str_ptr + 1;
                            pool_ptr = str_start[str_ptr];
                        }
                        lit_stk_ptr = lit_stk_ptr + 1;
                        goto exit;
                    }
                }
            } else {

                pop_lit2 = -(integer) pop_lit2;
                if ((pop_lit1 > sp_length - (pop_lit2 - 1)))
                    pop_lit1 = sp_length - (pop_lit2 - 1);
                sp_end = str_start[pop_lit3 + 1] - (pop_lit2 - 1);
                sp_ptr = sp_end - pop_lit1;
            }
            {
                while ((pool_ptr + sp_end - sp_ptr > pool_size))
                    pool_overflow();
            }
            while ((sp_ptr < sp_end)) {

                {
                    str_pool[pool_ptr] = str_pool[sp_ptr];
                    pool_ptr = pool_ptr + 1;
                }
                sp_ptr = sp_ptr + 1;
            }
            push_lit_stk(make_string(), 1 /*stk_str */ );
        }
    }
 exit: ;
}

void x_swap(void)
{
    pop_lit_stk(&pop_lit1, &pop_typ1);
    pop_lit_stk(&pop_lit2, &pop_typ2);
    if (((pop_typ1 != 1 /*stk_str */ ) || (pop_lit1 < cmd_str_ptr))) {
        push_lit_stk(pop_lit1, pop_typ1);
        if (((pop_typ2 == 1 /*stk_str */ ) && (pop_lit2 >= cmd_str_ptr))) {
            str_ptr = str_ptr + 1;
            pool_ptr = str_start[str_ptr];
        }
        push_lit_stk(pop_lit2, pop_typ2);
    } else if (((pop_typ2 != 1 /*stk_str */ ) || (pop_lit2 < cmd_str_ptr))) {
        {
            str_ptr = str_ptr + 1;
            pool_ptr = str_start[str_ptr];
        }
        push_lit_stk(pop_lit1, 1 /*stk_str */ );
        push_lit_stk(pop_lit2, pop_typ2);
    } else {                    /*441: */

        ex_buf_length = 0;
        add_buf_pool(pop_lit2);
        sp_ptr = str_start[pop_lit1];
        sp_end = str_start[pop_lit1 + 1];
        while ((sp_ptr < sp_end)) {

            {
                str_pool[pool_ptr] = str_pool[sp_ptr];
                pool_ptr = pool_ptr + 1;
            }
            sp_ptr = sp_ptr + 1;
        }
        push_lit_stk(make_string(), 1 /*stk_str */ );
        add_pool_buf_and_push();
    }
}

void x_text_length(void)
{
    pop_lit_stk(&pop_lit1, &pop_typ1);
    if ((pop_typ1 != 1 /*stk_str */ )) {
        print_wrong_stk_lit(pop_lit1, pop_typ1, 1 /*stk_str */ );
        push_lit_stk(s_null, 1 /*stk_str */ );
    } else {

        num_text_chars = 0;
        {
            sp_ptr = str_start[pop_lit1];
            sp_end = str_start[pop_lit1 + 1];
            sp_brace_level = 0;
            while ((sp_ptr < sp_end)) {

                sp_ptr = sp_ptr + 1;
                if ((str_pool[sp_ptr - 1] == 123 /*left_brace */ )) {
                    sp_brace_level = sp_brace_level + 1;
                    if (((sp_brace_level == 1) && (sp_ptr < sp_end))) {

                        if ((str_pool[sp_ptr] == 92 /*backslash */ )) {
                            sp_ptr = sp_ptr + 1;
                            while (((sp_ptr < sp_end) && (sp_brace_level > 0))) {

                                if ((str_pool[sp_ptr] == 125 /*right_brace */ ))
                                    sp_brace_level = sp_brace_level - 1;
                                else if ((str_pool[sp_ptr] == 123 /*left_brace */ ))
                                    sp_brace_level = sp_brace_level + 1;
                                sp_ptr = sp_ptr + 1;
                            }
                            num_text_chars = num_text_chars + 1;
                        }
                    }
                } else if ((str_pool[sp_ptr - 1] == 125 /*right_brace */ )) {
                    if ((sp_brace_level > 0))
                        sp_brace_level = sp_brace_level - 1;
                } else
                    num_text_chars = num_text_chars + 1;
            }
        }
        push_lit_stk(num_text_chars, 0 /*stk_int */ );
    }
}

void x_text_prefix(void)
{
    pop_lit_stk(&pop_lit1, &pop_typ1);
    pop_lit_stk(&pop_lit2, &pop_typ2);
    if ((pop_typ1 != 0 /*stk_int */ )) {
        print_wrong_stk_lit(pop_lit1, pop_typ1, 0 /*stk_int */ );
        push_lit_stk(s_null, 1 /*stk_str */ );
    } else if ((pop_typ2 != 1 /*stk_str */ )) {
        print_wrong_stk_lit(pop_lit2, pop_typ2, 1 /*stk_str */ );
        push_lit_stk(s_null, 1 /*stk_str */ );
    } else if ((pop_lit1 <= 0)) {
        push_lit_stk(s_null, 1 /*stk_str */ );
        goto exit;
    } else {                    /*445: */

        sp_ptr = str_start[pop_lit2];
        sp_end = str_start[pop_lit2 + 1];
        {
            num_text_chars = 0;
            sp_brace_level = 0;
            sp_xptr1 = sp_ptr;
            while (((sp_xptr1 < sp_end) && (num_text_chars < pop_lit1))) {

                sp_xptr1 = sp_xptr1 + 1;
                if ((str_pool[sp_xptr1 - 1] == 123 /*left_brace */ )) {
                    sp_brace_level = sp_brace_level + 1;
                    if (((sp_brace_level == 1) && (sp_xptr1 < sp_end))) {

                        if ((str_pool[sp_xptr1] == 92 /*backslash */ )) {
                            sp_xptr1 = sp_xptr1 + 1;
                            while (((sp_xptr1 < sp_end) && (sp_brace_level > 0))) {

                                if ((str_pool[sp_xptr1] == 125 /*right_brace */ ))
                                    sp_brace_level = sp_brace_level - 1;
                                else if ((str_pool[sp_xptr1] == 123 /*left_brace */ ))
                                    sp_brace_level = sp_brace_level + 1;
                                sp_xptr1 = sp_xptr1 + 1;
                            }
                            num_text_chars = num_text_chars + 1;
                        }
                    }
                } else if ((str_pool[sp_xptr1 - 1] == 125 /*right_brace */ )) {
                    if ((sp_brace_level > 0))
                        sp_brace_level = sp_brace_level - 1;
                } else
                    num_text_chars = num_text_chars + 1;
            }
            sp_end = sp_xptr1;
        }
        {
            while ((pool_ptr + sp_brace_level + sp_end - sp_ptr > pool_size))
                pool_overflow();
        }
        if ((pop_lit2 >= cmd_str_ptr))
            pool_ptr = sp_end;
        else
            while ((sp_ptr < sp_end)) {

                {
                    str_pool[pool_ptr] = str_pool[sp_ptr];
                    pool_ptr = pool_ptr + 1;
                }
                sp_ptr = sp_ptr + 1;
            }
        while ((sp_brace_level > 0)) {

            {
                str_pool[pool_ptr] = 125 /*right_brace */ ;
                pool_ptr = pool_ptr + 1;
            }
            sp_brace_level = sp_brace_level - 1;
        }
        push_lit_stk(make_string(), 1 /*stk_str */ );
    }
 exit: ;
}

void x_type(void)
{
    if ((!mess_with_entries))
        bst_cant_mess_with_entries_print();
    else if (((type_list[cite_ptr] == undefined) || (type_list[cite_ptr] == 0 /*empty */ )))
        push_lit_stk(s_null, 1 /*stk_str */ );
    else
        push_lit_stk(hash_text[type_list[cite_ptr]], 1 /*stk_str */ );
}

void x_warning(void)
{
    pop_lit_stk(&pop_lit1, &pop_typ1);
    if ((pop_typ1 != 1 /*stk_str */ ))
        print_wrong_stk_lit(pop_lit1, pop_typ1, 1 /*stk_str */ );
    else {
        puts_log("Warning--");
        print_lit(pop_lit1, pop_typ1);
        mark_warning();
    }
}

void x_width(void)
{
    pop_lit_stk(&pop_lit1, &pop_typ1);
    if ((pop_typ1 != 1 /*stk_str */ )) {
        print_wrong_stk_lit(pop_lit1, pop_typ1, 1 /*stk_str */ );
        push_lit_stk(0, 0 /*stk_int */ );
    } else {

        ex_buf_length = 0;
        add_buf_pool(pop_lit1);
        string_width = 0;
        {
            brace_level = 0;
            ex_buf_ptr = 0;
            while ((ex_buf_ptr < ex_buf_length)) {

                if ((ex_buf[ex_buf_ptr] == 123 /*left_brace */ )) {
                    brace_level = brace_level + 1;
                    if (((brace_level == 1) && (ex_buf_ptr + 1 < ex_buf_length))) {

                        if ((ex_buf[ex_buf_ptr + 1] == 92 /*backslash */ )) {   /*453: */
                            ex_buf_ptr = ex_buf_ptr + 1;
                            while (((ex_buf_ptr < ex_buf_length) && (brace_level > 0))) {

                                ex_buf_ptr = ex_buf_ptr + 1;
                                ex_buf_xptr = ex_buf_ptr;
                                while (((ex_buf_ptr < ex_buf_length)
                                        && (lex_class[ex_buf[ex_buf_ptr]] == 2 /*alpha */ )))
                                    ex_buf_ptr = ex_buf_ptr + 1;
                                if (((ex_buf_ptr < ex_buf_length) && (ex_buf_ptr == ex_buf_xptr)))
                                    ex_buf_ptr = ex_buf_ptr + 1;
                                else {

                                    control_seq_loc =
                                        str_lookup(ex_buf, ex_buf_xptr, ex_buf_ptr - ex_buf_xptr,
                                                   14 /*control_seq_ilk */ , false);
                                    if ((hash_found)) { /*454: */
                                        switch ((ilk_info[control_seq_loc])) {
                                        case 12:
                                            string_width = string_width + 500;
                                            break;
                                        case 4:
                                            string_width = string_width + 722;
                                            break;
                                        case 2:
                                            string_width = string_width + 778;
                                            break;
                                        case 5:
                                            string_width = string_width + 903;
                                            break;
                                        case 3:
                                            string_width = string_width + 1014;
                                            break;
                                        default:
                                            string_width = string_width + char_width[ex_buf[ex_buf_xptr]];
                                            break;
                                        }
                                    }
                                }
                                while (((ex_buf_ptr < ex_buf_length)
                                        && (lex_class[ex_buf[ex_buf_ptr]] == 1 /*white_space */ )))
                                    ex_buf_ptr = ex_buf_ptr + 1;
                                while (((ex_buf_ptr < ex_buf_length) && (brace_level > 0)
                                        && (ex_buf[ex_buf_ptr] != 92 /*backslash */ ))) {

                                    if ((ex_buf[ex_buf_ptr] == 125 /*right_brace */ ))
                                        brace_level = brace_level - 1;
                                    else if ((ex_buf[ex_buf_ptr] == 123 /*left_brace */ ))
                                        brace_level = brace_level + 1;
                                    else
                                        string_width = string_width + char_width[ex_buf[ex_buf_ptr]];
                                    ex_buf_ptr = ex_buf_ptr + 1;
                                }
                            }
                            ex_buf_ptr = ex_buf_ptr - 1;
                        } else
                            string_width = string_width + char_width[123 /*left_brace */ ];
                    } else
                        string_width = string_width + char_width[123 /*left_brace */ ];
                } else if ((ex_buf[ex_buf_ptr] == 125 /*right_brace */ )) {
                    decr_brace_level(pop_lit1);
                    string_width = string_width + char_width[125 /*right_brace */ ];
                } else
                    string_width = string_width + char_width[ex_buf[ex_buf_ptr]];
                ex_buf_ptr = ex_buf_ptr + 1;
            }
            check_brace_level(pop_lit1);
        }
        push_lit_stk(string_width, 0 /*stk_int */ );
    }
}

void x_write(void)
{
    pop_lit_stk(&pop_lit1, &pop_typ1);
    if ((pop_typ1 != 1 /*stk_str */ ))
        print_wrong_stk_lit(pop_lit1, pop_typ1, 1 /*stk_str */ );
    else
        add_out_pool(pop_lit1);
}

void execute_fn(hash_loc ex_fn_loc)
{
    integer r_pop_lt1, r_pop_lt2;
    stk_type r_pop_tp1, r_pop_tp2;
    wiz_fn_loc wiz_ptr;
    ;

    switch ((fn_type[ex_fn_loc])) {
    case 0:
        {
            ;

            switch ((ilk_info[ex_fn_loc])) {
            case 0:
                x_equals();
                break;
            case 1:
                x_greater_than();
                break;
            case 2:
                x_less_than();
                break;
            case 3:
                x_plus();
                break;
            case 4:
                x_minus();
                break;
            case 5:
                x_concatenate();
                break;
            case 6:
                x_gets();
                break;
            case 7:
                x_add_period();
                break;
            case 8:
                {
                    if ((!mess_with_entries))
                        bst_cant_mess_with_entries_print();
                    else if ((type_list[cite_ptr] == undefined))
                        execute_fn(b_default);
                    else if ((type_list[cite_ptr] == 0 /*empty */ )) ;
                    else
                        execute_fn(type_list[cite_ptr]);
                }
                break;
            case 9:
                x_change_case();
                break;
            case 10:
                x_chr_to_int();
                break;
            case 11:
                x_cite();
                break;
            case 12:
                x_duplicate();
                break;
            case 13:
                x_empty();
                break;
            case 14:
                x_format_name();
                break;
            case 15:
                {
                    pop_lit_stk(&pop_lit1, &pop_typ1);
                    pop_lit_stk(&pop_lit2, &pop_typ2);
                    pop_lit_stk(&pop_lit3, &pop_typ3);
                    if ((pop_typ1 != 2 /*stk_fn */ ))
                        print_wrong_stk_lit(pop_lit1, pop_typ1, 2 /*stk_fn */ );
                    else if ((pop_typ2 != 2 /*stk_fn */ ))
                        print_wrong_stk_lit(pop_lit2, pop_typ2, 2 /*stk_fn */ );
                    else if ((pop_typ3 != 0 /*stk_int */ ))
                        print_wrong_stk_lit(pop_lit3, pop_typ3, 0 /*stk_int */ );
                    else if ((pop_lit3 > 0))
                        execute_fn(pop_lit2);
                    else
                        execute_fn(pop_lit1);
                }
                break;
            case 16:
                x_int_to_chr();
                break;
            case 17:
                x_int_to_str();
                break;
            case 18:
                x_missing();
                break;
            case 19:
                {
                    output_bbl_line();
                }
                break;
            case 20:
                x_num_names();
                break;
            case 21:
                {
                    pop_lit_stk(&pop_lit1, &pop_typ1);
                }
                break;
            case 22:
                x_preamble();
                break;
            case 23:
                x_purify();
                break;
            case 24:
                x_quote();
                break;
            case 25:
                {
                    ;
                }
                break;
            case 26:
                {
                    pop_whole_stack();
                }
                break;
            case 27:
                x_substring();
                break;
            case 28:
                x_swap();
                break;
            case 29:
                x_text_length();
                break;
            case 30:
                x_text_prefix();
                break;
            case 31:
                {
                    pop_top_and_print();
                }
                break;
            case 32:
                x_type();
                break;
            case 33:
                x_warning();
                break;
            case 34:
                {
                    pop_lit_stk(&r_pop_lt1, &r_pop_tp1);
                    pop_lit_stk(&r_pop_lt2, &r_pop_tp2);
                    if ((r_pop_tp1 != 2 /*stk_fn */ ))
                        print_wrong_stk_lit(r_pop_lt1, r_pop_tp1, 2 /*stk_fn */ );
                    else if ((r_pop_tp2 != 2 /*stk_fn */ ))
                        print_wrong_stk_lit(r_pop_lt2, r_pop_tp2, 2 /*stk_fn */ );
                    else
                        while (true) {

                            execute_fn(r_pop_lt2);
                            pop_lit_stk(&pop_lit1, &pop_typ1);
                            if ((pop_typ1 != 0 /*stk_int */ )) {
                                print_wrong_stk_lit(pop_lit1, pop_typ1, 0 /*stk_int */ );
                                goto lab51;
                            } else if ((pop_lit1 > 0))
                                execute_fn(r_pop_lt1);
                            else
                                goto lab51;
                        }
 lab51:                        /*end_while */ ;
                }
                break;
            case 35:
                x_width();
                break;
            case 36:
                x_write();
                break;
            default:
                puts_log("Unknown built-in function");
                print_confusion();
                longjmp(error_jmpbuf, 1);
                break;
            }
        }
        break;
    case 1:
        {
            wiz_ptr = ilk_info[ex_fn_loc];
            while ((wiz_functions[wiz_ptr] != end_of_def)) {

                if ((wiz_functions[wiz_ptr] != quote_next_fn))
                    execute_fn(wiz_functions[wiz_ptr]);
                else {

                    wiz_ptr = wiz_ptr + 1;
                    push_lit_stk(wiz_functions[wiz_ptr], 2 /*stk_fn */ );
                }
                wiz_ptr = wiz_ptr + 1;
            }
        }
        break;
    case 2:
        push_lit_stk(ilk_info[ex_fn_loc], 0 /*stk_int */ );
        break;
    case 3:
        push_lit_stk(hash_text[ex_fn_loc], 1 /*stk_str */ );
        break;
    case 4:
        {
            if ((!mess_with_entries))
                bst_cant_mess_with_entries_print();
            else {

                field_ptr = cite_ptr * num_fields + ilk_info[ex_fn_loc];
                if ((field_ptr >= max_fields)) {
                    puts_log("field_info index is out of range");
                    print_confusion();
                    longjmp(error_jmpbuf, 1);
                }
                if ((field_info[field_ptr] == 0 /*missing */ ))
                    push_lit_stk(hash_text[ex_fn_loc], 3 /*stk_field_missing */ );
                else
                    push_lit_stk(field_info[field_ptr], 1 /*stk_str */ );
            }
        }
        break;
    case 5:
        {
            if ((!mess_with_entries))
                bst_cant_mess_with_entries_print();
            else
                push_lit_stk(entry_ints[cite_ptr * num_ent_ints + ilk_info[ex_fn_loc]], 0 /*stk_int */ );
        }
        break;
    case 6:
        {
            if ((!mess_with_entries))
                bst_cant_mess_with_entries_print();
            else {

                str_ent_ptr = cite_ptr * num_ent_strs + ilk_info[ex_fn_loc];
                ex_buf_ptr = 0;
                while ((entry_strs[(str_ent_ptr) * (ent_str_size + 1) + (ex_buf_ptr)] != 127 /*end_of_string */ )) {

                    ex_buf[ex_buf_ptr] = entry_strs[(str_ent_ptr) * (ent_str_size + 1) + (ex_buf_ptr)];
                    ex_buf_ptr = ex_buf_ptr + 1;
                }
                ex_buf_length = ex_buf_ptr;
                add_pool_buf_and_push();
            }
        }
        break;
    case 7:
        push_lit_stk(ilk_info[ex_fn_loc], 0 /*stk_int */ );
        break;
    case 8:
        {
            str_glb_ptr = ilk_info[ex_fn_loc];
            if ((glb_str_ptr[str_glb_ptr] > 0))
                push_lit_stk(glb_str_ptr[str_glb_ptr], 1 /*stk_str */ );
            else {

                {
                    while ((pool_ptr + glb_str_end[str_glb_ptr] > pool_size))
                        pool_overflow();
                }
                glob_chr_ptr = 0;
                while ((glob_chr_ptr < glb_str_end[str_glb_ptr])) {

                    {
                        str_pool[pool_ptr] = global_strs[(str_glb_ptr) * (glob_str_size + 1) + (glob_chr_ptr)];
                        pool_ptr = pool_ptr + 1;
                    }
                    glob_chr_ptr = glob_chr_ptr + 1;
                }
                push_lit_stk(make_string(), 1 /*stk_str */ );
            }
        }
        break;
    default:
        unknwn_function_class_confusion();
        break;
    }
}

void get_the_top_level_aux_file_name(void)
{
    /*
    parse_arguments();
    name_of_file = xmalloc_array(ASCII_code, strlen(cmdline(optind)) + 5);
    strcpy(stringcast(name_of_file + 1), cmdline(optind));
    aux_name_length = strlen(stringcast(name_of_file + 1));
    {
        if (((aux_name_length + (str_start[s_aux_extension + 1] - str_start[s_aux_extension]) > maxint)
             || (aux_name_length + (str_start[s_log_extension + 1] - str_start[s_log_extension]) > maxint)
             || (aux_name_length + (str_start[s_bbl_extension + 1] - str_start[s_bbl_extension]) > maxint))) {
            sam_too_long_file_name_print();
            goto lab46;
        }
        {
            name_length = aux_name_length;
            if ((name_length < 4) || (strcmp(stringcast(name_of_file + 1 + name_length - 4), ".aux") != 0))
                add_extension(s_aux_extension);
            else
                aux_name_length = aux_name_length - 4;
            aux_ptr = 0;
            if ((!kpse_in_name_ok(stringcast(name_of_file + 1))
                 || !a_open_in(aux_file[aux_ptr], -1 {{no_file_path}} ))) {
                sam_wrong_file_name_print();
                goto lab46;
            }
            name_length = aux_name_length;
            add_extension(s_log_extension);
            if ((!kpse_out_name_ok(stringcast(name_of_file + 1)) || !a_open_out(log_file))) {
                sam_wrong_file_name_print();
                goto lab46;
            }
            name_length = aux_name_length;
            add_extension(s_bbl_extension);
            if ((!kpse_out_name_ok(stringcast(name_of_file + 1)) || !a_open_out(bbl_file))) {
                sam_wrong_file_name_print();
                goto lab46;
            }
        }
        {
            name_length = aux_name_length;
            add_extension(s_aux_extension);
            name_ptr = 1;
            while ((name_ptr <= name_length)) {

                buffer[name_ptr] = name_of_file[name_ptr];
                name_ptr = name_ptr + 1;
            }
            top_lev_str = hash_text[str_lookup(buffer, 1, aux_name_length, 0 {{text_ilk}}, true)];
            aux_list[aux_ptr] = hash_text[str_lookup(buffer, 1, name_length, 3 {{aux_file_ilk}}, true)];
            if ((hash_found)) {
                ;
                {
                    {
                        fputs("Already encountered auxiliary file", log_file);
                        fputs("Already encountered auxiliary file", standard_output);
                    }
                    print_confusion();
                    longjmp(error_jmpbuf, 1);
                }
            }
            aux_ln_stack[aux_ptr] = 0;
        }
        goto lab41;
    }
 lab46:                        {{aux_not_found}} exit(1);
 lab41:                        {{aux_found}} ;
    */
}

void aux_bib_data_command(void)
{
    if ((bib_seen)) {
        aux_err_illegal_another_print(0 /*n_aux_bibdata */ );
        {
            aux_err_print();
            goto exit;
        }
    }
    bib_seen = true;
    while ((buffer[buf_ptr2] != 125 /*right_brace */ )) {

        buf_ptr2 = buf_ptr2 + 1;
        if ((!scan2_white(125 /*right_brace */ , 44 /*comma */ ))) {
            aux_err_no_right_brace_print();
            {
                aux_err_print();
                goto exit;
            }
        }
        if ((lex_class[buffer[buf_ptr2]] == 1 /*white_space */ )) {
            aux_err_white_space_in_argument_print();
            {
                aux_err_print();
                goto exit;
            }
        }
        if (((last > buf_ptr2 + 1) && (buffer[buf_ptr2] == 125 /*right_brace */ ))) {
            aux_err_stuff_after_right_brace_print();
            {
                aux_err_print();
                goto exit;
            }
        }
        {
            if ((bib_ptr == max_bib_files)) {
                BIB_XRETALLOC_NOSET("bib_list", bib_list, str_number, max_bib_files, max_bib_files + MAX_BIB_FILES);
                BIB_XRETALLOC_NOSET("bib_file", bib_file, FILE *, max_bib_files, max_bib_files + MAX_BIB_FILES);
                BIB_XRETALLOC("s_preamble", s_preamble, str_number, max_bib_files, max_bib_files + MAX_BIB_FILES);
            }
            bib_list[bib_ptr] =
                hash_text[str_lookup(buffer, buf_ptr1, (buf_ptr2 - buf_ptr1), 6 /*bib_file_ilk */ , true)];
            if ((hash_found)) {
                puts_log("This database file appears more than once: ");
                print_bib_name();
                aux_err_print();
                goto exit;
            }
            start_name(bib_list[bib_ptr]);
            if ((!a_open_in(bib_file[bib_ptr], kpse_bib_format))) {
                puts_log("I couldn't open database file ");
                print_bib_name();
                aux_err_print();
                goto exit;
            }

            bib_ptr = bib_ptr + 1;
        }
    }
 exit: ;
}

void aux_bib_style_command(void)
{
    if ((bst_seen)) {
        aux_err_illegal_another_print(1 /*n_aux_bibstyle */ );
        {
            aux_err_print();
            goto exit;
        }
    }
    bst_seen = true;
    buf_ptr2 = buf_ptr2 + 1;
    if ((!scan1_white(125 /*right_brace */ ))) {
        aux_err_no_right_brace_print();
        {
            aux_err_print();
            goto exit;
        }
    }
    if ((lex_class[buffer[buf_ptr2]] == 1 /*white_space */ )) {
        aux_err_white_space_in_argument_print();
        {
            aux_err_print();
            goto exit;
        }
    }
    if ((last > buf_ptr2 + 1)) {
        aux_err_stuff_after_right_brace_print();
        {
            aux_err_print();
            goto exit;
        }
    }
    {
        bst_str = hash_text[str_lookup(buffer, buf_ptr1, (buf_ptr2 - buf_ptr1), 5 /*bst_file_ilk */ , true)];
        if ((hash_found)) {
            puts_log("Already encountered style file");
            print_confusion();
            longjmp(error_jmpbuf, 1);
        }
        start_name(bst_str);
        if ((!a_open_in(bst_file, kpse_bst_format))) {
            puts_log("I couldn't open style file ");
            print_bst_name();
            bst_str = 0;
            aux_err_print();
            goto exit;
        }
        if (verbose) {
            puts_log("The style file: ");
            print_bst_name();
        } else {
            fputs("The style file: ", log_file);
            log_pr_bst_name();
        }
    }
 exit: ;
}

void aux_citation_command(void)
{
    citation_seen = true;
    while ((buffer[buf_ptr2] != 125 /*right_brace */ )) {

        buf_ptr2 = buf_ptr2 + 1;
        if ((!scan2_white(125 /*right_brace */ , 44 /*comma */ ))) {
            aux_err_no_right_brace_print();
            {
                aux_err_print();
                goto exit;
            }
        }
        if ((lex_class[buffer[buf_ptr2]] == 1 /*white_space */ )) {
            aux_err_white_space_in_argument_print();
            {
                aux_err_print();
                goto exit;
            }
        }
        if (((last > buf_ptr2 + 1) && (buffer[buf_ptr2] == 125 /*right_brace */ ))) {
            aux_err_stuff_after_right_brace_print();
            {
                aux_err_print();
                goto exit;
            }
        }
        {
            {
                if (((buf_ptr2 - buf_ptr1) == 1)) {

                    if ((buffer[buf_ptr1] == 42 /*star */ )) {
                        if ((all_entries)) {
                            puts_log("Multiple inclusions of entire database\n");
                            aux_err_print();
                            goto exit;
                        } else {

                            all_entries = true;
                            all_marker = cite_ptr;
                            goto lab23;
                        }
                    }
                }
            }
            tmp_ptr = buf_ptr1;
            while ((tmp_ptr < buf_ptr2)) {

                ex_buf[tmp_ptr] = buffer[tmp_ptr];
                tmp_ptr = tmp_ptr + 1;
            }
            lower_case(ex_buf, buf_ptr1, (buf_ptr2 - buf_ptr1));
            lc_cite_loc = str_lookup(ex_buf, buf_ptr1, (buf_ptr2 - buf_ptr1), 10 /*lc_cite_ilk */ , true);
            if ((hash_found)) { /*136: */
                ;

                dummy_loc = str_lookup(buffer, buf_ptr1, (buf_ptr2 - buf_ptr1), 9 /*cite_ilk */ , false);
                if ((!hash_found)) {
                    puts_log("Case mismatch error between cite keys ");
                    print_a_token();
                    puts_log(" and ");
                    print_a_pool_str(cite_list[ilk_info[ilk_info[lc_cite_loc]]]);
                    putc_log('\n');
                    aux_err_print();
                    goto exit;
                }
            } else {            /*137: */

                ;

                cite_loc = str_lookup(buffer, buf_ptr1, (buf_ptr2 - buf_ptr1), 9 /*cite_ilk */ , true);
                if ((hash_found))
                    hash_cite_confusion();
                check_cite_overflow(cite_ptr);
                cite_list[cite_ptr] = hash_text[cite_loc];
                ilk_info[cite_loc] = cite_ptr;
                ilk_info[lc_cite_loc] = cite_loc;
                cite_ptr = cite_ptr + 1;
            }
        }
 lab23:                        /*next_cite */ ;
    }
 exit: ;
}

void aux_input_command(void)
{
    boolean aux_extension_ok;
    buf_ptr2 = buf_ptr2 + 1;
    if ((!scan1_white(125 /*right_brace */ ))) {
        aux_err_no_right_brace_print();
        {
            aux_err_print();
            goto exit;
        }
    }
    if ((lex_class[buffer[buf_ptr2]] == 1 /*white_space */ )) {
        aux_err_white_space_in_argument_print();
        {
            aux_err_print();
            goto exit;
        }
    }
    if ((last > buf_ptr2 + 1)) {
        aux_err_stuff_after_right_brace_print();
        {
            aux_err_print();
            goto exit;
        }
    }
    {
        aux_ptr = aux_ptr + 1;
        if ((aux_ptr == aux_stack_size)) {
            print_a_token();
            puts_log(": ");
            print_overflow();
            printf_log("auxiliary file depth %ld\n", (long) aux_stack_size);
            longjmp(error_jmpbuf, 1);
        }
        aux_extension_ok = true;
        if (((buf_ptr2 - buf_ptr1) < (str_start[s_aux_extension + 1] - str_start[s_aux_extension])))
            aux_extension_ok = false;
        else if ((!str_eq_buf
                  (s_aux_extension, buffer, buf_ptr2 - (str_start[s_aux_extension + 1] - str_start[s_aux_extension]),
                   (str_start[s_aux_extension + 1] - str_start[s_aux_extension]))))
            aux_extension_ok = false;
        if ((!aux_extension_ok)) {
            print_a_token();
            puts_log(" has a wrong extension");
            aux_ptr = aux_ptr - 1;
            aux_err_print();
            goto exit;
        }
        aux_list[aux_ptr] = hash_text[str_lookup(buffer, buf_ptr1, (buf_ptr2 - buf_ptr1), 3 /*aux_file_ilk */ , true)];
        if ((hash_found)) {
            puts_log("Already encountered file ");
            print_aux_name();
            aux_ptr = aux_ptr - 1;
            aux_err_print();
            goto exit;
        }
        {
            start_name(aux_list[aux_ptr]);
            name_ptr = name_length + 1;
            name_of_file[name_ptr] = 0;
            if ((!a_open_in(aux_file[aux_ptr], -1 /*no_file_path */ ))) {
                puts_log("I couldn't open auxiliary file ");
                print_aux_name();
                aux_ptr = aux_ptr - 1;
                aux_err_print();
                goto exit;
            }

            printf_log("A level-%ld auxiliary file: ", (long) aux_ptr);
            print_aux_name();
            aux_ln_stack[aux_ptr] = 0;
        }
    }
 exit: ;
}

static void
pop_the_aux_stack(void)
{
    a_close(aux_file[aux_ptr]);

    if (aux_ptr == 0)
        return;

    aux_ptr--;
}

void get_aux_command_and_process(void)
{
    buf_ptr2 = 0;
    if ((!scan1(123 /*left_brace */ )))
        goto exit;
    command_num = ilk_info[str_lookup(buffer, buf_ptr1, (buf_ptr2 - buf_ptr1), 2 /*aux_command_ilk */ , false)];
    if ((hash_found))
        switch ((command_num)) {
        case 0:
            aux_bib_data_command();
            break;
        case 1:
            aux_bib_style_command();
            break;
        case 2:
            aux_citation_command();
            break;
        case 3:
            aux_input_command();
            break;
        default:
            puts_log("Unknown auxiliary-file command");
            print_confusion();
            longjmp(error_jmpbuf, 1);
            break;
        }
 exit: ;
}

void last_check_for_aux_errors(void)
{
    num_cites = cite_ptr;
    num_bib_files = bib_ptr;
    if ((!citation_seen)) {
        aux_end1_err_print();
        puts_log("\\citation commands");
        aux_end2_err_print();
    } else if (((num_cites == 0) && (!all_entries))) {
        aux_end1_err_print();
        puts_log("cite keys");
        aux_end2_err_print();
    }
    if ((!bib_seen)) {
        aux_end1_err_print();
        puts_log("\\bibdata command");
        aux_end2_err_print();
    } else if ((num_bib_files == 0)) {
        aux_end1_err_print();
        puts_log("database files");
        aux_end2_err_print();
    }
    if ((!bst_seen)) {
        aux_end1_err_print();
        puts_log("\\bibstyle command");
        aux_end2_err_print();
    } else if ((bst_str == 0)) {
        aux_end1_err_print();
        puts_log("style file");
        aux_end2_err_print();
    }
}

void bst_entry_command(void)
{
    if ((entry_seen)) {
        puts_log("Illegal, another entry command");
        bst_err_print_and_look_for_blank_line();
        goto exit;
    }
    entry_seen = true;
    {
        if ((!eat_bst_white_space())) {
            eat_bst_print();
            puts_log("entry");
            bst_err_print_and_look_for_blank_line();
            goto exit;
        }
    }
    {
        {
            if ((buffer[buf_ptr2] != 123 /*left_brace */ )) {
                bst_left_brace_print();
                puts_log("entry");
                bst_err_print_and_look_for_blank_line();
                goto exit;
            }
            buf_ptr2 = buf_ptr2 + 1;
        }
        {
            if ((!eat_bst_white_space())) {
                eat_bst_print();
                puts_log("entry");
                bst_err_print_and_look_for_blank_line();
                goto exit;
            }
        }
        while ((buffer[buf_ptr2] != 125 /*right_brace */ )) {

            {
                scan_identifier(125 /*right_brace */ , 37 /*comment */ , 37 /*comment */ );
                if (((scan_result == 3 /*white_adjacent */ ) || (scan_result == 1 /*specified_char_adjacent */ ))) ;
                else {
                    bst_id_print();
                    puts_log("entry");
                    bst_err_print_and_look_for_blank_line();
                    goto exit;
                }
            }
            {
                ;

                lower_case(buffer, buf_ptr1, (buf_ptr2 - buf_ptr1));
                fn_loc = str_lookup(buffer, buf_ptr1, (buf_ptr2 - buf_ptr1), 11 /*bst_fn_ilk */ , true);
                {
                    if ((hash_found)) {
                        already_seen_function_print(fn_loc);
                        goto exit;
                    }
                }
                fn_type[fn_loc] = 4 /*field */ ;
                ilk_info[fn_loc] = num_fields;
                num_fields = num_fields + 1;
            }
            {
                if ((!eat_bst_white_space())) {
                    eat_bst_print();
                    puts_log("entry");
                    bst_err_print_and_look_for_blank_line();
                    goto exit;
                }
            }
        }
        buf_ptr2 = buf_ptr2 + 1;
    }
    {
        if ((!eat_bst_white_space())) {
            eat_bst_print();
            puts_log("entry");
            bst_err_print_and_look_for_blank_line();
            goto exit;
        }
    }
    if ((num_fields == num_pre_defined_fields)) {
        puts_log("Warning--I didn't find any fields");
        bst_warn_print();
    }
    {
        {
            if ((buffer[buf_ptr2] != 123 /*left_brace */ )) {
                bst_left_brace_print();
                puts_log("entry");
                bst_err_print_and_look_for_blank_line();
                goto exit;
            }
            buf_ptr2 = buf_ptr2 + 1;
        }
        {
            if ((!eat_bst_white_space())) {
                eat_bst_print();
                puts_log("entry");
                bst_err_print_and_look_for_blank_line();
                goto exit;
            }
        }
        while ((buffer[buf_ptr2] != 125 /*right_brace */ )) {

            {
                scan_identifier(125 /*right_brace */ , 37 /*comment */ , 37 /*comment */ );
                if (((scan_result == 3 /*white_adjacent */ ) || (scan_result == 1 /*specified_char_adjacent */ ))) ;
                else {
                    bst_id_print();
                    puts_log("entry");
                    bst_err_print_and_look_for_blank_line();
                    goto exit;
                }
            }
            {
                ;

                lower_case(buffer, buf_ptr1, (buf_ptr2 - buf_ptr1));
                fn_loc = str_lookup(buffer, buf_ptr1, (buf_ptr2 - buf_ptr1), 11 /*bst_fn_ilk */ , true);
                {
                    if ((hash_found)) {
                        already_seen_function_print(fn_loc);
                        goto exit;
                    }
                }
                fn_type[fn_loc] = 5 /*int_entry_var */ ;
                ilk_info[fn_loc] = num_ent_ints;
                num_ent_ints = num_ent_ints + 1;
            }
            {
                if ((!eat_bst_white_space())) {
                    eat_bst_print();
                    puts_log("entry");
                    bst_err_print_and_look_for_blank_line();
                    goto exit;
                }
            }
        }
        buf_ptr2 = buf_ptr2 + 1;
    }
    {
        if ((!eat_bst_white_space())) {
            eat_bst_print();
            puts_log("entry");
            bst_err_print_and_look_for_blank_line();
            goto exit;
        }
    }
    {
        {
            if ((buffer[buf_ptr2] != 123 /*left_brace */ )) {
                bst_left_brace_print();
                puts_log("entry");
                bst_err_print_and_look_for_blank_line();
                goto exit;
            }
            buf_ptr2 = buf_ptr2 + 1;
        }
        {
            if ((!eat_bst_white_space())) {
                eat_bst_print();
                puts_log("entry");
                bst_err_print_and_look_for_blank_line();
                goto exit;
            }
        }
        while ((buffer[buf_ptr2] != 125 /*right_brace */ )) {

            {
                scan_identifier(125 /*right_brace */ , 37 /*comment */ , 37 /*comment */ );
                if (((scan_result == 3 /*white_adjacent */ ) || (scan_result == 1 /*specified_char_adjacent */ ))) ;
                else {
                    bst_id_print();
                    puts_log("entry");
                    bst_err_print_and_look_for_blank_line();
                    goto exit;
                }
            }
            {
                ;

                lower_case(buffer, buf_ptr1, (buf_ptr2 - buf_ptr1));
                fn_loc = str_lookup(buffer, buf_ptr1, (buf_ptr2 - buf_ptr1), 11 /*bst_fn_ilk */ , true);
                {
                    if ((hash_found)) {
                        already_seen_function_print(fn_loc);
                        goto exit;
                    }
                }
                fn_type[fn_loc] = 6 /*str_entry_var */ ;
                ilk_info[fn_loc] = num_ent_strs;
                num_ent_strs = num_ent_strs + 1;
            }
            {
                if ((!eat_bst_white_space())) {
                    eat_bst_print();
                    puts_log("entry");
                    bst_err_print_and_look_for_blank_line();
                    goto exit;
                }
            }
        }
        buf_ptr2 = buf_ptr2 + 1;
    }
 exit: ;
}

boolean bad_argument_token(void)
{
    register boolean Result;
    Result = true;
    lower_case(buffer, buf_ptr1, (buf_ptr2 - buf_ptr1));
    fn_loc = str_lookup(buffer, buf_ptr1, (buf_ptr2 - buf_ptr1), 11 /*bst_fn_ilk */ , false);
    if ((!hash_found)) {
        print_a_token();
        puts_log(" is an unknown function");
        bst_err_print_and_look_for_blank_line();
        goto exit;
    } else if (((fn_type[fn_loc] != 0 /*built_in */ ) && (fn_type[fn_loc] != 1 /*wiz_defined */ ))) {
        print_a_token();
        puts_log(" has bad function type ");
        print_fn_class(fn_loc);
        bst_err_print_and_look_for_blank_line();
        goto exit;
    }
    Result = false;
 exit: ;
    return Result;
}

void bst_execute_command(void)
{
    if ((!read_seen)) {
        puts_log("Illegal, execute command before read command");
        bst_err_print_and_look_for_blank_line();
        goto exit;
    }
    {
        if ((!eat_bst_white_space())) {
            eat_bst_print();
            puts_log("execute");
            bst_err_print_and_look_for_blank_line();
            goto exit;
        }
    }
    {
        if ((buffer[buf_ptr2] != 123 /*left_brace */ )) {
            bst_left_brace_print();
            puts_log("execute");
            bst_err_print_and_look_for_blank_line();
            goto exit;
        }
        buf_ptr2 = buf_ptr2 + 1;
    }
    {
        if ((!eat_bst_white_space())) {
            eat_bst_print();
            puts_log("execute");
            bst_err_print_and_look_for_blank_line();
            goto exit;
        }
    }
    {
        scan_identifier(125 /*right_brace */ , 37 /*comment */ , 37 /*comment */ );
        if (((scan_result == 3 /*white_adjacent */ ) || (scan_result == 1 /*specified_char_adjacent */ ))) ;
        else {
            bst_id_print();
            puts_log("execute");
            bst_err_print_and_look_for_blank_line();
            goto exit;
        }
    }
    {
        ;

        if ((bad_argument_token()))
            goto exit;
    }
    {
        if ((!eat_bst_white_space())) {
            eat_bst_print();
            puts_log("execute");
            bst_err_print_and_look_for_blank_line();
            goto exit;
        }
    }
    {
        if ((buffer[buf_ptr2] != 125 /*right_brace */ )) {
            bst_right_brace_print();
            puts_log("execute");
            bst_err_print_and_look_for_blank_line();
            goto exit;
        }
        buf_ptr2 = buf_ptr2 + 1;
    }
    {
        init_command_execution();
        mess_with_entries = false;
        execute_fn(fn_loc);
        check_command_execution();
    }
 exit: ;
}

void bst_function_command(void)
{
    {
        if ((!eat_bst_white_space())) {
            eat_bst_print();
            puts_log("function");
            bst_err_print_and_look_for_blank_line();
            goto exit;
        }
    }
    {
        {
            if ((buffer[buf_ptr2] != 123 /*left_brace */ )) {
                bst_left_brace_print();
                puts_log("function");
                bst_err_print_and_look_for_blank_line();
                goto exit;
            }
            buf_ptr2 = buf_ptr2 + 1;
        }
        {
            if ((!eat_bst_white_space())) {
                eat_bst_print();
                puts_log("function");
                bst_err_print_and_look_for_blank_line();
                goto exit;
            }
        }
        {
            scan_identifier(125 /*right_brace */ , 37 /*comment */ , 37 /*comment */ );
            if (((scan_result == 3 /*white_adjacent */ ) || (scan_result == 1 /*specified_char_adjacent */ ))) ;
            else {
                bst_id_print();
                puts_log("function");
                bst_err_print_and_look_for_blank_line();
                goto exit;
            }
        }
        {
            ;

            lower_case(buffer, buf_ptr1, (buf_ptr2 - buf_ptr1));
            wiz_loc = str_lookup(buffer, buf_ptr1, (buf_ptr2 - buf_ptr1), 11 /*bst_fn_ilk */ , true);
            {
                if ((hash_found)) {
                    already_seen_function_print(wiz_loc);
                    goto exit;
                }
            }
            fn_type[wiz_loc] = 1 /*wiz_defined */ ;
            if ((hash_text[wiz_loc] == s_default))
                b_default = wiz_loc;
        }
        {
            if ((!eat_bst_white_space())) {
                eat_bst_print();
                puts_log("function");
                bst_err_print_and_look_for_blank_line();
                goto exit;
            }
        }
        {
            if ((buffer[buf_ptr2] != 125 /*right_brace */ )) {
                bst_right_brace_print();
                puts_log("function");
                bst_err_print_and_look_for_blank_line();
                goto exit;
            }
            buf_ptr2 = buf_ptr2 + 1;
        }
    }
    {
        if ((!eat_bst_white_space())) {
            eat_bst_print();
            puts_log("function");
            bst_err_print_and_look_for_blank_line();
            goto exit;
        }
    }
    {
        if ((buffer[buf_ptr2] != 123 /*left_brace */ )) {
            bst_left_brace_print();
            puts_log("function");
            bst_err_print_and_look_for_blank_line();
            goto exit;
        }
        buf_ptr2 = buf_ptr2 + 1;
    }
    scan_fn_def(wiz_loc);
 exit: ;
}

void bst_integers_command(void)
{
    {
        if ((!eat_bst_white_space())) {
            eat_bst_print();
            puts_log("integers");
            bst_err_print_and_look_for_blank_line();
            goto exit;
        }
    }
    {
        if ((buffer[buf_ptr2] != 123 /*left_brace */ )) {
            bst_left_brace_print();
            puts_log("integers");
            bst_err_print_and_look_for_blank_line();
            goto exit;
        }
        buf_ptr2 = buf_ptr2 + 1;
    }
    {
        if ((!eat_bst_white_space())) {
            eat_bst_print();
            puts_log("integers");
            bst_err_print_and_look_for_blank_line();
            goto exit;
        }
    }
    while ((buffer[buf_ptr2] != 125 /*right_brace */ )) {

        {
            scan_identifier(125 /*right_brace */ , 37 /*comment */ , 37 /*comment */ );
            if (((scan_result == 3 /*white_adjacent */ ) || (scan_result == 1 /*specified_char_adjacent */ ))) ;
            else {
                bst_id_print();
                puts_log("integers");
                bst_err_print_and_look_for_blank_line();
                goto exit;
            }
        }
        {
            ;

            lower_case(buffer, buf_ptr1, (buf_ptr2 - buf_ptr1));
            fn_loc = str_lookup(buffer, buf_ptr1, (buf_ptr2 - buf_ptr1), 11 /*bst_fn_ilk */ , true);
            {
                if ((hash_found)) {
                    already_seen_function_print(fn_loc);
                    goto exit;
                }
            }
            fn_type[fn_loc] = 7 /*int_global_var */ ;
            ilk_info[fn_loc] = 0;
        }
        {
            if ((!eat_bst_white_space())) {
                eat_bst_print();
                puts_log("integers");
                bst_err_print_and_look_for_blank_line();
                goto exit;
            }
        }
    }
    buf_ptr2 = buf_ptr2 + 1;
 exit: ;
}

void bst_iterate_command(void)
{
    if ((!read_seen)) {
        puts_log("Illegal, iterate command before read command");
        bst_err_print_and_look_for_blank_line();
        goto exit;
    }
    {
        if ((!eat_bst_white_space())) {
            eat_bst_print();
            puts_log("iterate");
            bst_err_print_and_look_for_blank_line();
            goto exit;
        }
    }
    {
        if ((buffer[buf_ptr2] != 123 /*left_brace */ )) {
            bst_left_brace_print();
            puts_log("iterate");
            bst_err_print_and_look_for_blank_line();
            goto exit;
        }
        buf_ptr2 = buf_ptr2 + 1;
    }
    {
        if ((!eat_bst_white_space())) {
            eat_bst_print();
            puts_log("iterate");
            bst_err_print_and_look_for_blank_line();
            goto exit;
        }
    }
    {
        scan_identifier(125 /*right_brace */ , 37 /*comment */ , 37 /*comment */ );
        if (((scan_result == 3 /*white_adjacent */ ) || (scan_result == 1 /*specified_char_adjacent */ ))) ;
        else {
            bst_id_print();
            puts_log("iterate");
            bst_err_print_and_look_for_blank_line();
            goto exit;
        }
    }
    {
        ;

        if ((bad_argument_token()))
            goto exit;
    }
    {
        if ((!eat_bst_white_space())) {
            eat_bst_print();
            puts_log("iterate");
            bst_err_print_and_look_for_blank_line();
            goto exit;
        }
    }
    {
        if ((buffer[buf_ptr2] != 125 /*right_brace */ )) {
            bst_right_brace_print();
            puts_log("iterate");
            bst_err_print_and_look_for_blank_line();
            goto exit;
        }
        buf_ptr2 = buf_ptr2 + 1;
    }
    {
        init_command_execution();
        mess_with_entries = true;
        sort_cite_ptr = 0;
        while ((sort_cite_ptr < num_cites)) {

            cite_ptr = cite_info[sort_cite_ptr];
            ;

            execute_fn(fn_loc);
            check_command_execution();
            sort_cite_ptr = sort_cite_ptr + 1;
        }
    }
 exit: ;
}

void bst_macro_command(void)
{
    if ((read_seen)) {
        puts_log("Illegal, macro command after read command");
        bst_err_print_and_look_for_blank_line();
        goto exit;
    }
    {
        if ((!eat_bst_white_space())) {
            eat_bst_print();
            puts_log("macro");
            bst_err_print_and_look_for_blank_line();
            goto exit;
        }
    }
    {
        {
            if ((buffer[buf_ptr2] != 123 /*left_brace */ )) {
                bst_left_brace_print();
                puts_log("macro");
                bst_err_print_and_look_for_blank_line();
                goto exit;
            }
            buf_ptr2 = buf_ptr2 + 1;
        }
        {
            if ((!eat_bst_white_space())) {
                eat_bst_print();
                puts_log("macro");
                bst_err_print_and_look_for_blank_line();
                goto exit;
            }
        }
        {
            scan_identifier(125 /*right_brace */ , 37 /*comment */ , 37 /*comment */ );
            if (((scan_result == 3 /*white_adjacent */ ) || (scan_result == 1 /*specified_char_adjacent */ ))) ;
            else {
                bst_id_print();
                puts_log("macro");
                bst_err_print_and_look_for_blank_line();
                goto exit;
            }
        }
        {
            ;

            lower_case(buffer, buf_ptr1, (buf_ptr2 - buf_ptr1));
            macro_name_loc = str_lookup(buffer, buf_ptr1, (buf_ptr2 - buf_ptr1), 13 /*macro_ilk */ , true);
            if ((hash_found)) {
                print_a_token();
                puts_log(" is already defined as a macro");
                bst_err_print_and_look_for_blank_line();
                goto exit;
            }
            ilk_info[macro_name_loc] = hash_text[macro_name_loc];
        }
        {
            if ((!eat_bst_white_space())) {
                eat_bst_print();
                puts_log("macro");
                bst_err_print_and_look_for_blank_line();
                goto exit;
            }
        }
        {
            if ((buffer[buf_ptr2] != 125 /*right_brace */ )) {
                bst_right_brace_print();
                puts_log("macro");
                bst_err_print_and_look_for_blank_line();
                goto exit;
            }
            buf_ptr2 = buf_ptr2 + 1;
        }
    }
    {
        if ((!eat_bst_white_space())) {
            eat_bst_print();
            puts_log("macro");
            bst_err_print_and_look_for_blank_line();
            goto exit;
        }
    }
    {
        {
            if ((buffer[buf_ptr2] != 123 /*left_brace */ )) {
                bst_left_brace_print();
                puts_log("macro");
                bst_err_print_and_look_for_blank_line();
                goto exit;
            }
            buf_ptr2 = buf_ptr2 + 1;
        }
        {
            if ((!eat_bst_white_space())) {
                eat_bst_print();
                puts_log("macro");
                bst_err_print_and_look_for_blank_line();
                goto exit;
            }
        }
        if ((buffer[buf_ptr2] != 34 /*double_quote */ )) {
            puts_log("A macro definition must be \"-delimited");
            bst_err_print_and_look_for_blank_line();
            goto exit;
        }
        {
            buf_ptr2 = buf_ptr2 + 1;
            if ((!scan1(34 /*double_quote */ ))) {
                puts_log("There's no `\"' to end macro definition");
                bst_err_print_and_look_for_blank_line();
                goto exit;
            }

            macro_def_loc = str_lookup(buffer, buf_ptr1, (buf_ptr2 - buf_ptr1), 0 /*text_ilk */ , true);
            fn_type[macro_def_loc] = 3 /*str_literal */ ;
            ilk_info[macro_name_loc] = hash_text[macro_def_loc];
            buf_ptr2 = buf_ptr2 + 1;
        }
        {
            if ((!eat_bst_white_space())) {
                eat_bst_print();
                puts_log("macro");
                bst_err_print_and_look_for_blank_line();
                goto exit;
            }
        }
        {
            if ((buffer[buf_ptr2] != 125 /*right_brace */ )) {
                bst_right_brace_print();
                puts_log("macro");
                bst_err_print_and_look_for_blank_line();
                goto exit;
            }
            buf_ptr2 = buf_ptr2 + 1;
        }
    }
 exit: ;
}

void get_bib_command_or_entry_and_process(void)
{
    at_bib_command = false;
    while ((!scan1(64 /*at_sign */ ))) {

        if ((!input_ln(bib_file[bib_ptr])))
            goto exit;
        bib_line_num = bib_line_num + 1;
        buf_ptr2 = 0;
    }
    {
        if ((buffer[buf_ptr2] != 64 /*at_sign */ )) {
            puts_log("An \"@\" disappeared");
            print_confusion();
            longjmp(error_jmpbuf, 1);
        }
        buf_ptr2 = buf_ptr2 + 1;
        {
            if ((!eat_bib_white_space())) {
                eat_bib_print();
                goto exit;
            }
        }
        scan_identifier(123 /*left_brace */ , 40 /*left_paren */ , 40 /*left_paren */ );
        {
            if (((scan_result == 3 /*white_adjacent */ ) || (scan_result == 1 /*specified_char_adjacent */ ))) ;
            else {
                bib_id_print();
                puts_log("an entry type");
                bib_err_print();
                goto exit;
            }
        }
        ;

        lower_case(buffer, buf_ptr1, (buf_ptr2 - buf_ptr1));
        command_num = ilk_info[str_lookup(buffer, buf_ptr1, (buf_ptr2 - buf_ptr1), 12 /*bib_command_ilk */ , false)];
        if ((hash_found)) {     /*240: */
            at_bib_command = true;
            switch ((command_num)) {
            case 0:
                {
                    goto exit;
                }
                break;
            case 1:
                {
                    if ((preamble_ptr == max_bib_files)) {
                        BIB_XRETALLOC_NOSET("bib_list", bib_list, str_number, max_bib_files,
                                            max_bib_files + MAX_BIB_FILES);
                        BIB_XRETALLOC_NOSET("bib_file", bib_file, FILE *, max_bib_files,
                                            max_bib_files + MAX_BIB_FILES);
                        BIB_XRETALLOC("s_preamble", s_preamble, str_number, max_bib_files,
                                      max_bib_files + MAX_BIB_FILES);
                    }
                    {
                        if ((!eat_bib_white_space())) {
                            eat_bib_print();
                            goto exit;
                        }
                    }
                    if ((buffer[buf_ptr2] == 123 /*left_brace */ ))
                        right_outer_delim = 125 /*right_brace */ ;
                    else if ((buffer[buf_ptr2] == 40 /*left_paren */ ))
                        right_outer_delim = 41 /*right_paren */ ;
                    else {

                        bib_one_of_two_print(123 /*left_brace */ , 40 /*left_paren */ );
                        goto exit;
                    }
                    buf_ptr2 = buf_ptr2 + 1;
                    {
                        if ((!eat_bib_white_space())) {
                            eat_bib_print();
                            goto exit;
                        }
                    }
                    store_field = true;
                    if ((!scan_and_store_the_field_value_and_eat_white()))
                        goto exit;
                    if ((buffer[buf_ptr2] != right_outer_delim)) {
                        printf_log("Missing \"%c\" in preamble command", right_outer_delim);
                        bib_err_print();
                        goto exit;
                    }
                    buf_ptr2 = buf_ptr2 + 1;
                    goto exit;
                }
                break;
            case 2:
                {
                    {
                        if ((!eat_bib_white_space())) {
                            eat_bib_print();
                            goto exit;
                        }
                    }
                    {
                        if ((buffer[buf_ptr2] == 123 /*left_brace */ ))
                            right_outer_delim = 125 /*right_brace */ ;
                        else if ((buffer[buf_ptr2] == 40 /*left_paren */ ))
                            right_outer_delim = 41 /*right_paren */ ;
                        else {

                            bib_one_of_two_print(123 /*left_brace */ , 40 /*left_paren */ );
                            goto exit;
                        }
                        buf_ptr2 = buf_ptr2 + 1;
                        {
                            if ((!eat_bib_white_space())) {
                                eat_bib_print();
                                goto exit;
                            }
                        }
                        scan_identifier(61 /*equals_sign */ , 61 /*equals_sign */ , 61 /*equals_sign */ );
                        {
                            if (((scan_result == 3 /*white_adjacent */ )
                                 || (scan_result == 1 /*specified_char_adjacent */ ))) ;
                            else {
                                bib_id_print();
                                puts_log("a string name");
                                bib_err_print();
                                goto exit;
                            }
                        }
                        {
                            ;

                            lower_case(buffer, buf_ptr1, (buf_ptr2 - buf_ptr1));
                            cur_macro_loc =
                                str_lookup(buffer, buf_ptr1, (buf_ptr2 - buf_ptr1), 13 /*macro_ilk */ , true);
                            ilk_info[cur_macro_loc] = hash_text[cur_macro_loc];
/*if(hash_found)then begin macro_warn_print;
begin begin write_ln(log_file,'having its definition overwritten');write_ln(standard_output,'having its definition overwritten');end;
bib_warn_print;end;end;*/ }
                    }
                    {
                        if ((!eat_bib_white_space())) {
                            eat_bib_print();
                            goto exit;
                        }
                    }
                    {
                        if ((buffer[buf_ptr2] != 61 /*equals_sign */ )) {
                            bib_equals_sign_print();
                            goto exit;
                        }
                        buf_ptr2 = buf_ptr2 + 1;
                        {
                            if ((!eat_bib_white_space())) {
                                eat_bib_print();
                                goto exit;
                            }
                        }
                        store_field = true;
                        if ((!scan_and_store_the_field_value_and_eat_white()))
                            goto exit;
                        if ((buffer[buf_ptr2] != right_outer_delim)) {
                            printf_log("Missing \"%c\" in string command", right_outer_delim);
                            bib_err_print();
                            goto exit;
                        }
                        buf_ptr2 = buf_ptr2 + 1;
                    }
                    goto exit;
                }
                break;
            default:
                bib_cmd_confusion();
                break;
            }
        } else {

            entry_type_loc = str_lookup(buffer, buf_ptr1, (buf_ptr2 - buf_ptr1), 11 /*bst_fn_ilk */ , false);
            if (((!hash_found) || (fn_type[entry_type_loc] != 1 /*wiz_defined */ )))
                type_exists = false;
            else
                type_exists = true;
        }
    }
    {
        if ((!eat_bib_white_space())) {
            eat_bib_print();
            goto exit;
        }
    }
    {
        if ((buffer[buf_ptr2] == 123 /*left_brace */ ))
            right_outer_delim = 125 /*right_brace */ ;
        else if ((buffer[buf_ptr2] == 40 /*left_paren */ ))
            right_outer_delim = 41 /*right_paren */ ;
        else {

            bib_one_of_two_print(123 /*left_brace */ , 40 /*left_paren */ );
            goto exit;
        }
        buf_ptr2 = buf_ptr2 + 1;
        {
            if ((!eat_bib_white_space())) {
                eat_bib_print();
                goto exit;
            }
        }
        if ((right_outer_delim == 41 /*right_paren */ )) {
            if ((scan1_white(44 /*comma */ ))) ;
        } else if ((scan2_white(44 /*comma */ , 125 /*right_brace */ ))) ;
        {
            ;

            tmp_ptr = buf_ptr1;
            while ((tmp_ptr < buf_ptr2)) {

                ex_buf[tmp_ptr] = buffer[tmp_ptr];
                tmp_ptr = tmp_ptr + 1;
            }
            lower_case(ex_buf, buf_ptr1, (buf_ptr2 - buf_ptr1));
            if ((all_entries))
                lc_cite_loc = str_lookup(ex_buf, buf_ptr1, (buf_ptr2 - buf_ptr1), 10 /*lc_cite_ilk */ , true);
            else
                lc_cite_loc = str_lookup(ex_buf, buf_ptr1, (buf_ptr2 - buf_ptr1), 10 /*lc_cite_ilk */ , false);
            if ((hash_found)) {
                entry_cite_ptr = ilk_info[ilk_info[lc_cite_loc]];
                {
                    if (((!all_entries) || (entry_cite_ptr < all_marker) || (entry_cite_ptr >= old_num_cites))) {
                        if ((type_list[entry_cite_ptr] == 0 /*empty */ )) {
                            {
                                if (((!all_entries) && (entry_cite_ptr >= old_num_cites))) {
                                    cite_loc =
                                        str_lookup(buffer, buf_ptr1, (buf_ptr2 - buf_ptr1), 9 /*cite_ilk */ , true);
                                    if ((!hash_found)) {
                                        ilk_info[lc_cite_loc] = cite_loc;
                                        ilk_info[cite_loc] = entry_cite_ptr;
                                        cite_list[entry_cite_ptr] = hash_text[cite_loc];
                                        hash_found = true;
                                    }
                                }
                            }
                            goto lab26;
                        }
                    } else if ((!entry_exists[entry_cite_ptr])) {
                        {
                            ex_buf_ptr = 0;
                            tmp_ptr = str_start[cite_info[entry_cite_ptr]];
                            tmp_end_ptr = str_start[cite_info[entry_cite_ptr] + 1];
                            while ((tmp_ptr < tmp_end_ptr)) {

                                ex_buf[ex_buf_ptr] = str_pool[tmp_ptr];
                                ex_buf_ptr = ex_buf_ptr + 1;
                                tmp_ptr = tmp_ptr + 1;
                            }
                            lower_case(ex_buf, 0,
                                       (str_start[cite_info[entry_cite_ptr] + 1] -
                                        str_start[cite_info[entry_cite_ptr]]));
                            lc_xcite_loc =
                                str_lookup(ex_buf, 0,
                                           (str_start[cite_info[entry_cite_ptr] + 1] -
                                            str_start[cite_info[entry_cite_ptr]]), 10 /*lc_cite_ilk */ , false);
                            if ((!hash_found))
                                cite_key_disappeared_confusion();
                        }
                        if ((lc_xcite_loc == lc_cite_loc))
                            goto lab26;
                    }
                    if ((type_list[entry_cite_ptr] == 0 /*empty */ )) {
                        puts_log("The cite list is messed up");
                        print_confusion();
                        longjmp(error_jmpbuf, 1);
                    }

                    puts_log("Repeated entry");
                    bib_err_print();
                    goto exit;
 lab26:                        /*first_time_entry */ ;
                }
            }
            store_entry = true;
            if ((all_entries)) {        /*273: */
                if ((hash_found)) {
                    if ((entry_cite_ptr < all_marker))
                        goto lab22;
                    else {

                        entry_exists[entry_cite_ptr] = true;
                        cite_loc = ilk_info[lc_cite_loc];
                    }
                } else {

                    cite_loc = str_lookup(buffer, buf_ptr1, (buf_ptr2 - buf_ptr1), 9 /*cite_ilk */ , true);
                    if ((hash_found))
                        hash_cite_confusion();
                }
                entry_cite_ptr = cite_ptr;
                add_database_cite(&cite_ptr);
 lab22:                        /*cite_already_set */ ;
            } else if ((!hash_found))
                store_entry = false;
            if ((store_entry)) {        /*274: */
/*dummy_loc:=str_lookup(buffer,buf_ptr1,(buf_ptr2-buf_ptr1),9[cite_ilk],false);
if(not hash_found)then begin begin write(log_file,'Warning--case mismatch, database key "');
write(standard_output,'Warning--case mismatch, database key "');end;print_a_token;begin write(log_file,'", cite key "');
write(standard_output,'", cite key "');end;print_a_pool_str(cite_list[entry_cite_ptr]);begin begin write_ln(log_file,'"');
write_ln(standard_output,'"');end;bib_warn_print;end;end;*/ if ((type_exists))
                    type_list[entry_cite_ptr] = entry_type_loc;
                else {
                    type_list[entry_cite_ptr] = undefined;
                    puts_log("Warning--entry type for \"");
                    print_a_token();
                    puts_log("\" isn't style-file defined\n");
                    bib_warn_print();
                }
            }
        }
    }
    {
        if ((!eat_bib_white_space())) {
            eat_bib_print();
            goto exit;
        }
    }
    {
        while ((buffer[buf_ptr2] != right_outer_delim)) {

            if ((buffer[buf_ptr2] != 44 /*comma */ )) {
                bib_one_of_two_print(44 /*comma */ , right_outer_delim);
                goto exit;
            }
            buf_ptr2 = buf_ptr2 + 1;
            {
                if ((!eat_bib_white_space())) {
                    eat_bib_print();
                    goto exit;
                }
            }
            if ((buffer[buf_ptr2] == right_outer_delim))
                goto loop_exit;
            {
                scan_identifier(61 /*equals_sign */ , 61 /*equals_sign */ , 61 /*equals_sign */ );
                {
                    if (((scan_result == 3 /*white_adjacent */ ) || (scan_result == 1 /*specified_char_adjacent */ ))) ;
                    else {
                        bib_id_print();
                        puts_log("a field name");
                        bib_err_print();
                        goto exit;
                    }
                }
                ;

                store_field = false;
                if ((store_entry)) {
                    lower_case(buffer, buf_ptr1, (buf_ptr2 - buf_ptr1));
                    field_name_loc = str_lookup(buffer, buf_ptr1, (buf_ptr2 - buf_ptr1), 11 /*bst_fn_ilk */ , false);
                    if ((hash_found)) {

                        if ((fn_type[field_name_loc] == 4 /*field */ ))
                            store_field = true;
                    }
                }
                {
                    if ((!eat_bib_white_space())) {
                        eat_bib_print();
                        goto exit;
                    }
                }
                if ((buffer[buf_ptr2] != 61 /*equals_sign */ )) {
                    bib_equals_sign_print();
                    goto exit;
                }
                buf_ptr2 = buf_ptr2 + 1;
            }
            {
                if ((!eat_bib_white_space())) {
                    eat_bib_print();
                    goto exit;
                }
            }
            if ((!scan_and_store_the_field_value_and_eat_white()))
                goto exit;
        }
 loop_exit:
        buf_ptr2 = buf_ptr2 + 1;
    }
 exit: ;
}

void bst_read_command(void)
{
    if ((read_seen)) {
        puts_log("Illegal, another read command");
        bst_err_print_and_look_for_blank_line();
        goto exit;
    }
    read_seen = true;
    if ((!entry_seen)) {
        puts_log("Illegal, read command before entry command");
        bst_err_print_and_look_for_blank_line();
        goto exit;
    }
    sv_ptr1 = buf_ptr2;
    sv_ptr2 = last;
    tmp_ptr = sv_ptr1;
    while ((tmp_ptr < sv_ptr2)) {

        sv_buffer[tmp_ptr] = buffer[tmp_ptr];
        tmp_ptr = tmp_ptr + 1;
    }
    {
        {
            {
                check_field_overflow(num_fields * num_cites);
                field_ptr = 0;
                while ((field_ptr < max_fields)) {

                    field_info[field_ptr] = 0 /*missing */ ;
                    field_ptr = field_ptr + 1;
                }
            }
            {
                cite_ptr = 0;
                while ((cite_ptr < max_cites)) {

                    type_list[cite_ptr] = 0 /*empty */ ;
                    cite_info[cite_ptr] = 0 /*any_value */ ;
                    cite_ptr = cite_ptr + 1;
                }
                old_num_cites = num_cites;
                if ((all_entries)) {
                    cite_ptr = all_marker;
                    while ((cite_ptr < old_num_cites)) {

                        cite_info[cite_ptr] = cite_list[cite_ptr];
                        entry_exists[cite_ptr] = false;
                        cite_ptr = cite_ptr + 1;
                    }
                    cite_ptr = all_marker;
                } else {

                    cite_ptr = num_cites;
                    all_marker = 0 /*any_value */ ;
                }
            }
        }
        read_performed = true;
        bib_ptr = 0;
        while ((bib_ptr < num_bib_files)) {

            if (verbose) {
                printf_log("Database file #%ld: ", (long) bib_ptr + 1);
                print_bib_name();
            } else {
                fprintf(log_file, "%s%ld%s", "Database file #", (long)bib_ptr + 1, ": ");
                log_pr_bib_name();
            }
            bib_line_num = 0;
            buf_ptr2 = last;
            while ((!eof(bib_file[bib_ptr])))
                get_bib_command_or_entry_and_process();
            a_close(bib_file[bib_ptr]);
            bib_ptr = bib_ptr + 1;
        }
        reading_completed = true;
        ;

        {
            num_cites = cite_ptr;
            num_preamble_strings = preamble_ptr;
            {
                if (((num_cites - 1) * num_fields + crossref_num >= max_fields)) {
                    puts_log("field_info index is out of range");
                    print_confusion();
                    longjmp(error_jmpbuf, 1);
                }
                cite_ptr = 0;
                while ((cite_ptr < num_cites)) {

                    field_ptr = cite_ptr * num_fields + crossref_num;
                    if ((field_info[field_ptr] != 0 /*missing */ )) {

                        if ((find_cite_locs_for_this_cite_key(field_info[field_ptr]))) {
                            cite_loc = ilk_info[lc_cite_loc];
                            field_info[field_ptr] = hash_text[cite_loc];
                            cite_parent_ptr = ilk_info[cite_loc];
                            field_ptr = cite_ptr * num_fields + num_pre_defined_fields;
                            field_end_ptr = field_ptr - num_pre_defined_fields + num_fields;
                            field_parent_ptr = cite_parent_ptr * num_fields + num_pre_defined_fields;
                            while ((field_ptr < field_end_ptr)) {

                                if ((field_info[field_ptr] == 0 /*missing */ ))
                                    field_info[field_ptr] = field_info[field_parent_ptr];
                                field_ptr = field_ptr + 1;
                                field_parent_ptr = field_parent_ptr + 1;
                            }
                        }
                    }
                    cite_ptr = cite_ptr + 1;
                }
            }
            {
                if (((num_cites - 1) * num_fields + crossref_num >= max_fields)) {
                    puts_log("field_info index is out of range");
                    print_confusion();
                    longjmp(error_jmpbuf, 1);
                }
                cite_ptr = 0;
                while ((cite_ptr < num_cites)) {

                    field_ptr = cite_ptr * num_fields + crossref_num;
                    if ((field_info[field_ptr] != 0 /*missing */ )) {

                        if ((!find_cite_locs_for_this_cite_key(field_info[field_ptr]))) {
                            if ((cite_hash_found))
                                hash_cite_confusion();
                            nonexistent_cross_reference_error();
                            field_info[field_ptr] = 0 /*missing */ ;
                        } else {

                            if ((cite_loc != ilk_info[lc_cite_loc]))
                                hash_cite_confusion();
                            cite_parent_ptr = ilk_info[cite_loc];
                            if ((type_list[cite_parent_ptr] == 0 /*empty */ )) {
                                nonexistent_cross_reference_error();
                                field_info[field_ptr] = 0 /*missing */ ;
                            } else {

                                field_parent_ptr = cite_parent_ptr * num_fields + crossref_num;
                                if ((field_info[field_parent_ptr] != 0 /*missing */ )) {        /*283: */
                                    puts_log("Warning--you've nested cross references");
                                    bad_cross_reference_print(cite_list[cite_parent_ptr]);
                                    puts_log("\", which also refers to something\n");
                                    mark_warning();
                                }
                                if (((!all_entries) && (cite_parent_ptr >= old_num_cites)
                                     && (cite_info[cite_parent_ptr] < min_crossrefs)))
                                    field_info[field_ptr] = 0 /*missing */ ;
                            }
                        }
                    }
                    cite_ptr = cite_ptr + 1;
                }
            }
            {
                cite_ptr = 0;
                while ((cite_ptr < num_cites)) {

                    if ((type_list[cite_ptr] == 0 /*empty */ ))
                        print_missing_entry(cite_list[cite_ptr]);
                    else if (((all_entries) || (cite_ptr < old_num_cites) || (cite_info[cite_ptr] >= min_crossrefs))) {
                        if ((cite_ptr > cite_xptr)) {   /*286: */
                            if (((cite_xptr + 1) * num_fields > max_fields)) {
                                puts_log("field_info index is out of range");
                                print_confusion();
                                longjmp(error_jmpbuf, 1);
                            }
                            cite_list[cite_xptr] = cite_list[cite_ptr];
                            type_list[cite_xptr] = type_list[cite_ptr];
                            if ((!find_cite_locs_for_this_cite_key(cite_list[cite_ptr])))
                                cite_key_disappeared_confusion();
                            if (((!cite_hash_found) || (cite_loc != ilk_info[lc_cite_loc])))
                                hash_cite_confusion();
                            ilk_info[cite_loc] = cite_xptr;
                            field_ptr = cite_xptr * num_fields;
                            field_end_ptr = field_ptr + num_fields;
                            tmp_ptr = cite_ptr * num_fields;
                            while ((field_ptr < field_end_ptr)) {

                                field_info[field_ptr] = field_info[tmp_ptr];
                                field_ptr = field_ptr + 1;
                                tmp_ptr = tmp_ptr + 1;
                            }
                        }
                        cite_xptr = cite_xptr + 1;
                    }
                    cite_ptr = cite_ptr + 1;
                }
                num_cites = cite_xptr;
                if ((all_entries)) {    /*287: */
                    cite_ptr = all_marker;
                    while ((cite_ptr < old_num_cites)) {

                        if ((!entry_exists[cite_ptr]))
                            print_missing_entry(cite_info[cite_ptr]);
                        cite_ptr = cite_ptr + 1;
                    }
                }
            }
            {
                entry_ints = XTALLOC((num_ent_ints + 1) * (num_cites + 1), integer);
                int_ent_ptr = 0;
                while ((int_ent_ptr < num_ent_ints * num_cites)) {

                    entry_ints[int_ent_ptr] = 0;
                    int_ent_ptr = int_ent_ptr + 1;
                }
            }
            {
                entry_strs = XTALLOC((num_ent_strs + 1) * (num_cites + 1) * (ent_str_size + 1), ASCII_code);
                str_ent_ptr = 0;
                while ((str_ent_ptr < num_ent_strs * num_cites)) {

                    entry_strs[(str_ent_ptr) * (ent_str_size + 1) + (0)] = 127 /*end_of_string */ ;
                    str_ent_ptr = str_ent_ptr + 1;
                }
            }
            {
                cite_ptr = 0;
                while ((cite_ptr < num_cites)) {

                    cite_info[cite_ptr] = cite_ptr;
                    cite_ptr = cite_ptr + 1;
                }
            }
        }
        read_completed = true;
    }
    buf_ptr2 = sv_ptr1;
    last = sv_ptr2;
    tmp_ptr = buf_ptr2;
    while ((tmp_ptr < last)) {

        buffer[tmp_ptr] = sv_buffer[tmp_ptr];
        tmp_ptr = tmp_ptr + 1;
    }
 exit: ;
}

void bst_reverse_command(void)
{
    if ((!read_seen)) {
        puts_log("Illegal, reverse command before read command");
        bst_err_print_and_look_for_blank_line();
        goto exit;
    }
    {
        if ((!eat_bst_white_space())) {
            eat_bst_print();
            puts_log("reverse");
            bst_err_print_and_look_for_blank_line();
            goto exit;
        }
    }
    {
        if ((buffer[buf_ptr2] != 123 /*left_brace */ )) {
            bst_left_brace_print();
            puts_log("reverse");
            bst_err_print_and_look_for_blank_line();
            goto exit;
        }
        buf_ptr2 = buf_ptr2 + 1;
    }
    {
        if ((!eat_bst_white_space())) {
            eat_bst_print();
            puts_log("reverse");
            bst_err_print_and_look_for_blank_line();
            goto exit;
        }
    }
    {
        scan_identifier(125 /*right_brace */ , 37 /*comment */ , 37 /*comment */ );
        if (((scan_result == 3 /*white_adjacent */ ) || (scan_result == 1 /*specified_char_adjacent */ ))) ;
        else {
            bst_id_print();
            puts_log("reverse");
            bst_err_print_and_look_for_blank_line();
            goto exit;
        }
    }
    {
        ;

        if ((bad_argument_token()))
            goto exit;
    }
    {
        if ((!eat_bst_white_space())) {
            eat_bst_print();
            puts_log("reverse");
            bst_err_print_and_look_for_blank_line();
            goto exit;
        }
    }
    {
        if ((buffer[buf_ptr2] != 125 /*right_brace */ )) {
            bst_right_brace_print();
            puts_log("reverse");
            bst_err_print_and_look_for_blank_line();
            goto exit;
        }
        buf_ptr2 = buf_ptr2 + 1;
    }
    {
        init_command_execution();
        mess_with_entries = true;
        if ((num_cites > 0)) {
            sort_cite_ptr = num_cites;
            do {
                sort_cite_ptr = sort_cite_ptr - 1;
                cite_ptr = cite_info[sort_cite_ptr];
                ;

                execute_fn(fn_loc);
                check_command_execution();
            } while (!((sort_cite_ptr == 0)));
        }
    }
 exit: ;
}

void bst_sort_command(void)
{
    if ((!read_seen)) {
        puts_log("Illegal, sort command before read command");
        bst_err_print_and_look_for_blank_line();
        goto exit;
    }
    {
        ;

        if ((num_cites > 1))
            quick_sort(0, num_cites - 1);
        ;

    }
 exit: ;
}

void bst_strings_command(void)
{
    {
        if ((!eat_bst_white_space())) {
            eat_bst_print();
            puts_log("strings");
            bst_err_print_and_look_for_blank_line();
            goto exit;
        }
    }
    {
        if ((buffer[buf_ptr2] != 123 /*left_brace */ )) {
            bst_left_brace_print();
            puts_log("strings");
            bst_err_print_and_look_for_blank_line();
            goto exit;
        }
        buf_ptr2 = buf_ptr2 + 1;
    }
    {
        if ((!eat_bst_white_space())) {
            eat_bst_print();
            puts_log("strings");
            bst_err_print_and_look_for_blank_line();
            goto exit;
        }
    }
    while ((buffer[buf_ptr2] != 125 /*right_brace */ )) {

        {
            scan_identifier(125 /*right_brace */ , 37 /*comment */ , 37 /*comment */ );
            if (((scan_result == 3 /*white_adjacent */ ) || (scan_result == 1 /*specified_char_adjacent */ ))) ;
            else {
                bst_id_print();
                puts_log("strings");
                bst_err_print_and_look_for_blank_line();
                goto exit;
            }
        }
        {
            ;

            lower_case(buffer, buf_ptr1, (buf_ptr2 - buf_ptr1));
            fn_loc = str_lookup(buffer, buf_ptr1, (buf_ptr2 - buf_ptr1), 11 /*bst_fn_ilk */ , true);
            {
                if ((hash_found)) {
                    already_seen_function_print(fn_loc);
                    goto exit;
                }
            }
            fn_type[fn_loc] = 8 /*str_global_var */ ;
            ilk_info[fn_loc] = num_glb_strs;
            if ((num_glb_strs == max_glob_strs)) {
                BIB_XRETALLOC_NOSET("glb_str_ptr", glb_str_ptr, str_number, max_glob_strs,
                                    max_glob_strs + MAX_GLOB_STRS);
                BIB_XRETALLOC_STRING("global_strs", global_strs, glob_str_size, max_glob_strs,
                                     max_glob_strs + MAX_GLOB_STRS);
                BIB_XRETALLOC("glb_str_end", glb_str_end, integer, max_glob_strs, max_glob_strs + MAX_GLOB_STRS);
                str_glb_ptr = num_glb_strs;
                while ((str_glb_ptr < max_glob_strs)) {

                    glb_str_ptr[str_glb_ptr] = 0;
                    glb_str_end[str_glb_ptr] = 0;
                    str_glb_ptr = str_glb_ptr + 1;
                }
            }
            num_glb_strs = num_glb_strs + 1;
        }
        {
            if ((!eat_bst_white_space())) {
                eat_bst_print();
                puts_log("strings");
                bst_err_print_and_look_for_blank_line();
                goto exit;
            }
        }
    }
    buf_ptr2 = buf_ptr2 + 1;
 exit: ;
}

void get_bst_command_and_process(void)
{
    if ((!scan_alpha())) {
        printf_log("\"%c\" can't start a style-file command", buffer[buf_ptr2]);
        bst_err_print_and_look_for_blank_line();
        goto exit;
    }
    lower_case(buffer, buf_ptr1, (buf_ptr2 - buf_ptr1));
    command_num = ilk_info[str_lookup(buffer, buf_ptr1, (buf_ptr2 - buf_ptr1), 4 /*bst_command_ilk */ , false)];
    if ((!hash_found)) {
        print_a_token();
        puts_log(" is an illegal style-file command");
        bst_err_print_and_look_for_blank_line();
        goto exit;
    }
    switch ((command_num)) {
    case 0:
        bst_entry_command();
        break;
    case 1:
        bst_execute_command();
        break;
    case 2:
        bst_function_command();
        break;
    case 3:
        bst_integers_command();
        break;
    case 4:
        bst_iterate_command();
        break;
    case 5:
        bst_macro_command();
        break;
    case 6:
        bst_read_command();
        break;
    case 7:
        bst_reverse_command();
        break;
    case 8:
        bst_sort_command();
        break;
    case 9:
        bst_strings_command();
        break;
    default:
        puts_log("Unknown style-file command");
        print_confusion();
        longjmp(error_jmpbuf, 1);
        break;
    }
 exit: ;
}

void setup_params(void)
{
    integer bound_default;
    const_string bound_name;

    ent_str_size = ENT_STR_SIZE;
    glob_str_size = GLOB_STR_SIZE;
    max_strings = MAX_STRINGS;

    hash_size = max_strings;
    if (hash_size < 5000 /*HASH_SIZE */ )
        hash_size = 5000 /*HASH_SIZE */ ;
    hash_max = hash_size + hash_base - 1;
    end_of_def = hash_max + 1;
    undefined = hash_max + 1;
}

void compute_hash_prime(void)
{
    integer hash_want;
    integer k;
    integer j;
    integer o;
    integer square;
    integer n;
    boolean j_prime;
    hash_want = (hash_size / 20) * 17;
    j = 1;
    k = 1;
    hash_prime = 2;
    hash_next[k] = hash_prime;
    o = 2;
    square = 9;
    while (hash_prime < hash_want) {

        do {
            j = j + 2;
            if (j == square) {
                hash_text[o] = j;
                j = j + 2;
                o = o + 1;
                square = hash_next[o] * hash_next[o];
            }
            n = 2;
            j_prime = true;
            while ((n < o) && j_prime) {

                while (hash_text[n] < j)
                    hash_text[n] = hash_text[n] + 2 * hash_next[n];
                if (hash_text[n] == j)
                    j_prime = false;
                n = n + 1;
            }
        } while (!(j_prime));
        k = k + 1;
        hash_prime = j;
        hash_next[k] = hash_prime;
    }
}

void initialize(void)
{
    integer i;
    hash_loc k;
    bad = 0;
    if ((min_print_line < 3))
        bad = 1;
    if ((max_print_line <= min_print_line))
        bad = 10 * bad + 2;
    if ((max_print_line >= buf_size))
        bad = 10 * bad + 3;
    if ((hash_prime < 128))
        bad = 10 * bad + 4;
    if ((hash_prime > hash_size))
        bad = 10 * bad + 5;
    if ((hash_base != 1))
        bad = 10 * bad + 6;
    if ((max_strings > hash_size))
        bad = 10 * bad + 7;
    if ((max_cites > max_strings))
        bad = 10 * bad + 8;
    if ((10 /*short_list */  < 2 * 4 /*end_offset */  + 2))
        bad = 100 * bad + 22;
    if ((bad > 0)) {
        fprintf(standard_output, "%ld%s\n", (long)bad, " is a bad bad");
        exit(1);
    }
    history = HISTORY_SPOTLESS;
    {
        register integer for_end;
        i = 0;
        for_end = 127;
        if (i <= for_end)
            do
                lex_class[i] = 5 /*other_lex */ ;
            while (i++ < for_end);
    }
    {
        register integer for_end;
        i = 128;
        for_end = 255;
        if (i <= for_end)
            do
                lex_class[i] = 2 /*alpha */ ;
            while (i++ < for_end);
    }
    {
        register integer for_end;
        i = 0;
        for_end = 31;
        if (i <= for_end)
            do
                lex_class[i] = 0 /*illegal */ ;
            while (i++ < for_end);
    }
    lex_class[127 /*invalid_code */ ] = 0 /*illegal */ ;
    lex_class[9 /*tab */ ] = 1 /*white_space */ ;
    lex_class[13] = 1 /*white_space */ ;
    lex_class[32 /*space */ ] = 1 /*white_space */ ;
    lex_class[126 /*tie */ ] = 4 /*sep_char */ ;
    lex_class[45 /*hyphen */ ] = 4 /*sep_char */ ;
    {
        register integer for_end;
        i = 48;
        for_end = 57;
        if (i <= for_end)
            do
                lex_class[i] = 3 /*numeric */ ;
            while (i++ < for_end);
    }
    {
        register integer for_end;
        i = 65;
        for_end = 90;
        if (i <= for_end)
            do
                lex_class[i] = 2 /*alpha */ ;
            while (i++ < for_end);
    }
    {
        register integer for_end;
        i = 97;
        for_end = 122;
        if (i <= for_end)
            do
                lex_class[i] = 2 /*alpha */ ;
            while (i++ < for_end);
    }
    {
        register integer for_end;
        i = 0;
        for_end = 255;
        if (i <= for_end)
            do
                id_class[i] = 1 /*legal_id_char */ ;
            while (i++ < for_end);
    }
    {
        register integer for_end;
        i = 0;
        for_end = 31;
        if (i <= for_end)
            do
                id_class[i] = 0 /*illegal_id_char */ ;
            while (i++ < for_end);
    }
    id_class[32 /*space */ ] = 0 /*illegal_id_char */ ;
    id_class[9 /*tab */ ] = 0 /*illegal_id_char */ ;
    id_class[34 /*double_quote */ ] = 0 /*illegal_id_char */ ;
    id_class[35 /*number_sign */ ] = 0 /*illegal_id_char */ ;
    id_class[37 /*comment */ ] = 0 /*illegal_id_char */ ;
    id_class[39 /*single_quote */ ] = 0 /*illegal_id_char */ ;
    id_class[40 /*left_paren */ ] = 0 /*illegal_id_char */ ;
    id_class[41 /*right_paren */ ] = 0 /*illegal_id_char */ ;
    id_class[44 /*comma */ ] = 0 /*illegal_id_char */ ;
    id_class[61 /*equals_sign */ ] = 0 /*illegal_id_char */ ;
    id_class[123 /*left_brace */ ] = 0 /*illegal_id_char */ ;
    id_class[125 /*right_brace */ ] = 0 /*illegal_id_char */ ;
    {
        register integer for_end;
        i = 0;
        for_end = 127;
        if (i <= for_end)
            do
                char_width[i] = 0;
            while (i++ < for_end);
    }
    char_width[32] = 278;
    char_width[33] = 278;
    char_width[34] = 500;
    char_width[35] = 833;
    char_width[36] = 500;
    char_width[37] = 833;
    char_width[38] = 778;
    char_width[39] = 278;
    char_width[40] = 389;
    char_width[41] = 389;
    char_width[42] = 500;
    char_width[43] = 778;
    char_width[44] = 278;
    char_width[45] = 333;
    char_width[46] = 278;
    char_width[47] = 500;
    char_width[48] = 500;
    char_width[49] = 500;
    char_width[50] = 500;
    char_width[51] = 500;
    char_width[52] = 500;
    char_width[53] = 500;
    char_width[54] = 500;
    char_width[55] = 500;
    char_width[56] = 500;
    char_width[57] = 500;
    char_width[58] = 278;
    char_width[59] = 278;
    char_width[60] = 278;
    char_width[61] = 778;
    char_width[62] = 472;
    char_width[63] = 472;
    char_width[64] = 778;
    char_width[65] = 750;
    char_width[66] = 708;
    char_width[67] = 722;
    char_width[68] = 764;
    char_width[69] = 681;
    char_width[70] = 653;
    char_width[71] = 785;
    char_width[72] = 750;
    char_width[73] = 361;
    char_width[74] = 514;
    char_width[75] = 778;
    char_width[76] = 625;
    char_width[77] = 917;
    char_width[78] = 750;
    char_width[79] = 778;
    char_width[80] = 681;
    char_width[81] = 778;
    char_width[82] = 736;
    char_width[83] = 556;
    char_width[84] = 722;
    char_width[85] = 750;
    char_width[86] = 750;
    char_width[87] = 1028;
    char_width[88] = 750;
    char_width[89] = 750;
    char_width[90] = 611;
    char_width[91] = 278;
    char_width[92] = 500;
    char_width[93] = 278;
    char_width[94] = 500;
    char_width[95] = 278;
    char_width[96] = 278;
    char_width[97] = 500;
    char_width[98] = 556;
    char_width[99] = 444;
    char_width[100] = 556;
    char_width[101] = 444;
    char_width[102] = 306;
    char_width[103] = 500;
    char_width[104] = 556;
    char_width[105] = 278;
    char_width[106] = 306;
    char_width[107] = 528;
    char_width[108] = 278;
    char_width[109] = 833;
    char_width[110] = 556;
    char_width[111] = 500;
    char_width[112] = 556;
    char_width[113] = 528;
    char_width[114] = 392;
    char_width[115] = 394;
    char_width[116] = 389;
    char_width[117] = 556;
    char_width[118] = 528;
    char_width[119] = 722;
    char_width[120] = 528;
    char_width[121] = 528;
    char_width[122] = 444;
    char_width[123] = 500;
    char_width[124] = 1000;
    char_width[125] = 500;
    char_width[126] = 500;
    {
        register integer for_end;
        k = hash_base;
        for_end = hash_max;
        if (k <= for_end)
            do {
                hash_next[k] = 0 /*empty */ ;
                hash_text[k] = 0;
            }
            while (k++ < for_end);
    }
    hash_used = hash_max + 1;
    pool_ptr = 0;
    str_ptr = 1;
    str_start[str_ptr] = pool_ptr;
    bib_ptr = 0;
    bib_seen = false;
    bst_str = 0;
    bst_seen = false;
    cite_ptr = 0;
    citation_seen = false;
    all_entries = false;
    wiz_def_ptr = 0;
    num_ent_ints = 0;
    num_ent_strs = 0;
    num_fields = 0;
    str_glb_ptr = 0;
    while ((str_glb_ptr < max_glob_strs)) {

        glb_str_ptr[str_glb_ptr] = 0;
        glb_str_end[str_glb_ptr] = 0;
        str_glb_ptr = str_glb_ptr + 1;
    }
    num_glb_strs = 0;
    entry_seen = false;
    read_seen = false;
    read_performed = false;
    reading_completed = false;
    read_completed = false;
    impl_fn_num = 0;
    out_buf_length = 0;
    pre_def_certain_strings();
    get_the_top_level_aux_file_name();
}

void parse_arguments(void)
{
/*
#define n_options ( 4 )
    struct option long_options[n_options + 1];
    integer getopt_return_val;
    int option_index;
    integer current_option;
    verbose = true;
    min_crossrefs = 2;
    current_option = 0;
    long_options[0].name = "terse";
    long_options[0].has_arg = 0;
    long_options[0].flag = &verbose;
    long_options[0].val = 0;
    current_option = current_option + 1;
    long_options[current_option].name = "min-crossrefs";
    long_options[current_option].has_arg = 1;
    long_options[current_option].flag = 0;
    long_options[current_option].val = 0;
    current_option = current_option + 1;
    long_options[current_option].name = "help";
    long_options[current_option].has_arg = 0;
    long_options[current_option].flag = 0;
    long_options[current_option].val = 0;
    current_option = current_option + 1;
    long_options[current_option].name = "version";
    long_options[current_option].has_arg = 0;
    long_options[current_option].flag = 0;
    long_options[current_option].val = 0;
    current_option = current_option + 1;
    long_options[current_option].name = 0;
    long_options[current_option].has_arg = 0;
    long_options[current_option].flag = 0;
    long_options[current_option].val = 0;
    do {
        getopt_return_val = getopt_long_only(argc, argv, "", long_options, &option_index);
        if (getopt_return_val == -1) {
            ;
        } else if (getopt_return_val == 63 {{"?"}} ) {
            usage("bibtex");
        } else if ((strcmp(long_options[option_index].name, "min-crossrefs") == 0)) {
            min_crossrefs = atoi(optarg);
        } else if ((strcmp(long_options[option_index].name, "help") == 0)) {
            usage_help(BIBTEX_HELP, NULL);
        } else if ((strcmp(long_options[option_index].name, "version") == 0)) {
            print_version_and_exit("This is BibTeX, Version 0.99d", "Oren Patashnik", NULL, NULL);
        }
    } while (!(getopt_return_val == -1));
    if ((optind + 1 != argc)) {
        fprintf(stderr, "%s%s\n", "bibtex", ": Need exactly one file argument.");
        usage("bibtex");
    }
*/
}


tt_history_t
bibtex_main_body(void)
{
    standard_output = stdout;
    pool_size = POOL_SIZE;
    buf_size = BUF_SIZE;
    max_bib_files = MAX_BIB_FILES;
    max_glob_strs = MAX_GLOB_STRS;
    max_fields = MAX_FIELDS;
    max_cites = MAX_CITES;
    wiz_fn_space = WIZ_FN_SPACE;
    lit_stk_size = LIT_STK_SIZE;

    setup_params();

    entry_ints = NULL;
    entry_strs = NULL;

    bib_file = XTALLOC(max_bib_files + 1, FILE *);
    bib_list = XTALLOC(max_bib_files + 1, str_number);
    wiz_functions = XTALLOC(wiz_fn_space + 1, hash_ptr2);
    field_info = XTALLOC(max_fields + 1, str_number);
    s_preamble = XTALLOC(max_bib_files + 1, str_number);
    str_pool = XTALLOC(pool_size + 1, ASCII_code);
    buffer = XTALLOC(buf_size + 1, ASCII_code);
    sv_buffer = XTALLOC(buf_size + 1, ASCII_code);
    ex_buf = XTALLOC(buf_size + 1, ASCII_code);
    out_buf = XTALLOC(buf_size + 1, ASCII_code);
    name_tok = XTALLOC(buf_size + 1, buf_pointer);
    name_sep_char = XTALLOC(buf_size + 1, ASCII_code);
    glb_str_ptr = XTALLOC(max_glob_strs, str_number);
    global_strs = XTALLOC(max_glob_strs * (glob_str_size + 1), ASCII_code);
    glb_str_end = XTALLOC(max_glob_strs, integer);
    cite_list = XTALLOC(max_cites + 1, str_number);
    type_list = XTALLOC(max_cites + 1, hash_ptr2);
    entry_exists = XTALLOC(max_cites + 1, boolean);
    cite_info = XTALLOC(max_cites + 1, str_number);
    str_start = XTALLOC(max_strings + 1, pool_pointer);
    hash_next = XTALLOC(hash_max + 1, hash_pointer);
    hash_text = XTALLOC(hash_max + 1, str_number);
    hash_ilk = XTALLOC(hash_max + 1, str_ilk);
    ilk_info = XTALLOC(hash_max + 1, integer);
    fn_type = XTALLOC(hash_max + 1, fn_class);
    lit_stack = XTALLOC(lit_stk_size + 1, integer);
    lit_stk_type = XTALLOC(lit_stk_size + 1, stk_type);

    compute_hash_prime();
    initialize();

    if (setjmp(error_jmpbuf) == 1)
        goto close_up_shop;

    if (verbose)
        puts_log("This is BibTeX, Version 0.99d");
    else
        fputs("This is BibTeX, Version 0.99d", log_file);

    fprintf(log_file, "Capacity: max_strings=%ld, hash_size=%ld, hash_prime=%ld\n",
            (long) max_strings, (long) hash_size, (long) hash_prime);

    if (verbose) {
        puts_log("The top-level auxiliary file: ");
        print_aux_name();
    } else {
        fputs("The top-level auxiliary file: ", log_file);
        log_pr_aux_name();
    }

    while (true) {
        aux_ln_stack[aux_ptr]++;

        if (!input_ln(aux_file[aux_ptr]))
            pop_the_aux_stack();
        else
            get_aux_command_and_process();
    }

    last_check_for_aux_errors();

    if (bst_str == 0)
        goto no_bst_file;

    bst_line_num = 0;
    bbl_line_num = 1;
    buf_ptr2 = last;

    if (setjmp(recover_jmpbuf) == 0) {
        while(true) {
            if (!eat_bst_white_space())
                break;
            get_bst_command_and_process();
        }
    }

    a_close(bst_file);
 no_bst_file:
    a_close(bbl_file);

close_up_shop:
    /*456:*/

    if (read_performed && !reading_completed) {
        printf_log("Aborted at line %ld of file ", (long) bib_line_num);
        print_bib_name();
    }

    switch (history) {
    case HISTORY_SPOTLESS:
        break;
    case HISTORY_WARNING_ISSUED:
        if (err_count == 1)
            puts_log("(There was 1 warning)\n");
        else
            printf_log("(There were %ld warnings)\n", (long) err_count);
        break;
    case HISTORY_ERROR_ISSUED:
        if (err_count == 1)
            puts_log("(There was 1 error message)\n");
        else
            printf_log("(There were %ld error messages)\n", (long) err_count);
        break;
    case HISTORY_FATAL_ERROR:
        puts_log("(That was a fatal error)\n");
        break;
    default:
        puts_log("History is bunk");
        print_confusion();
        break;
    }

    a_close(log_file);
    return history;
}