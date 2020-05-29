//
// Created by 邹春旭 on 2020-05-29.
//

#include "zend_compile.h"

#ifndef PHP7_ZCX_H
#define PHP7_ZCX_H

// flags used in the op array list
#define OP1_USED   1<<0
#define OP2_USED   1<<1
#define RES_USED   1<<2

#define NONE_USED  0
#define ALL_USED   0x7

#define OP1_OPLINE   1<<3
#define OP2_OPLINE   1<<4
#define OP1_OPNUM    1<<5
#define OP2_OPNUM    1<<6
#define OP_FETCH     1<<7
#define EXT_VAL      1<<8
#define NOP2_OPNUM   1<<9
#define OP2_BRK_CONT 1<<10
#define OP1_CLASS    1<<11
#define RES_CLASS    1<<12
#define OP2_JMP_ARRAY    1<<13

#define SPECIAL    0xff

// special op-type flags
#define VLD_IS_OPLINE 1<<20
#define VLD_IS_OPNUM  1<<21
#define VLD_IS_CLASS  1<<22
#define OP2_INCLUDE   1<<23
#define EXT_VAL_JMP_REL   1<<24
#define EXT_VAL_JMP_ABS   1<<25
#define VLD_IS_JMP_ARRAY  1<<26

typedef struct _op_usage {
    const char  *name;
    unsigned int flags;
} op_usage;

static const op_usage opcodes[] = {
        /*  0 */	{ "NOP", NONE_USED },
        /*  1 */	{ "ADD", ALL_USED },
        /*  2 */	{ "SUB", ALL_USED },
        /*  3 */	{ "MUL", ALL_USED },
        /*  4 */	{ "DIV", ALL_USED },
        /*  5 */	{ "MOD", ALL_USED },
        /*  6 */	{ "SL", ALL_USED },
        /*  7 */	{ "SR", ALL_USED },
        /*  8 */	{ "CONCAT", ALL_USED },
        /*  9 */	{ "BW_OR", ALL_USED },
        /*  10 */	{ "BW_AND", ALL_USED },
        /*  11 */	{ "BW_XOR", ALL_USED },
        /*  12 */	{ "BW_NOT", RES_USED | OP1_USED },
        /*  13 */	{ "BOOL_NOT", RES_USED | OP1_USED },
        /*  14 */	{ "BOOL_XOR", ALL_USED },
        /*  15 */	{ "IS_IDENTICAL", ALL_USED },
        /*  16 */	{ "IS_NOT_IDENTICAL", ALL_USED },
        /*  17 */	{ "IS_EQUAL", ALL_USED },
        /*  18 */	{ "IS_NOT_EQUAL", ALL_USED },
        /*  19 */	{ "IS_SMALLER", ALL_USED },
        /*  20 */	{ "IS_SMALLER_OR_EQUAL", ALL_USED },
        /*  21 */	{ "CAST", ALL_USED | EXT_VAL },
#if PHP_VERSION_ID >= 70400
/*  22 */	{ "ASSIGN", ALL_USED },
	/*  23 */	{ "ASSIGN_DIM", ALL_USED },
	/*  24 */	{ "ASSIGN_OBJ", ALL_USED },
	/*  25 */	{ "ASSIGN_STATIC_PROP", ALL_USED },
	/*  26 */	{ "ASSIGN_OP", ALL_USED | EXT_VAL },
	/*  27 */	{ "ASSIGN_DIM_OP", ALL_USED | EXT_VAL },
	/*  28 */	{ "ASSIGN_OBJ_OP", ALL_USED | EXT_VAL },
	/*  29 */	{ "ASSIGN_STATIC_PROP_OP", ALL_USED | EXT_VAL },
	/*  30 */	{ "ASSIGN_REF", SPECIAL },
	/*  31 */	{ "QM_ASSIGN", RES_USED | OP1_USED },
	/*  32 */	{ "ASSIGN_OBJ_REF", ALL_USED },
	/*  33 */	{ "ASSIGN_STATIC_PROP_REF", ALL_USED },
#else
        /*  22 */	{ "QM_ASSIGN", RES_USED | OP1_USED },
        /*  23 */	{ "ASSIGN_ADD", ALL_USED | EXT_VAL },
        /*  24 */	{ "ASSIGN_SUB", ALL_USED | EXT_VAL },
        /*  25 */	{ "ASSIGN_MUL", ALL_USED | EXT_VAL },
        /*  26 */	{ "ASSIGN_DIV", ALL_USED | EXT_VAL },
        /*  27 */	{ "ASSIGN_MOD", ALL_USED | EXT_VAL },
        /*  28 */	{ "ASSIGN_SL", ALL_USED | EXT_VAL },
        /*  29 */	{ "ASSIGN_SR", ALL_USED | EXT_VAL },
        /*  30 */	{ "ASSIGN_CONCAT", ALL_USED | EXT_VAL },
        /*  31 */	{ "ASSIGN_BW_OR", ALL_USED | EXT_VAL },
        /*  32 */	{ "ASSIGN_BW_AND", ALL_USED | EXT_VAL },
        /*  33 */	{ "ASSIGN_BW_XOR", ALL_USED | EXT_VAL },
#endif
        /*  34 */	{ "PRE_INC", OP1_USED | RES_USED },
        /*  35 */	{ "PRE_DEC", OP1_USED | RES_USED },
        /*  36 */	{ "POST_INC", OP1_USED | RES_USED },
        /*  37 */	{ "POST_DEC", OP1_USED | RES_USED },
#if PHP_VERSION_ID >= 70400
/*  38 */	{ "PRE_INC_STATIC_PROP", ALL_USED },
	/*  39 */	{ "PRE_DEC_STATIC_PROP", ALL_USED },
	/*  40 */	{ "POST_INC_STATIC_PROP", ALL_USED },
	/*  41 */	{ "POST_DEC_STATIC_PROP", ALL_USED },
#else
        /*  38 */	{ "ASSIGN", ALL_USED },
        /*  39 */	{ "ASSIGN_REF", SPECIAL },
        /*  40 */	{ "ECHO", OP1_USED },
# if PHP_VERSION_ID < 70100
        /*  41 */	{ "UNKNOWN [41]", ALL_USED },
# else
        /*  41 */	{ "GENERATOR_CREATE", RES_USED },
# endif
#endif
        /*  42 */	{ "JMP", OP1_USED | OP1_OPLINE },
        /*  43 */	{ "JMPZ", OP1_USED | OP2_USED | OP2_OPLINE },
        /*  44 */	{ "JMPNZ", OP1_USED | OP2_USED | OP2_OPLINE },
        /*  45 */	{ "JMPZNZ", SPECIAL },
        /*  46 */	{ "JMPZ_EX", ALL_USED | OP2_OPLINE },
        /*  47 */	{ "JMPNZ_EX", ALL_USED | OP2_OPLINE },
        /*  48 */	{ "CASE", ALL_USED },
#if PHP_VERSION_ID < 70200
        /*  49 */	{ "SWITCH_FREE", RES_USED | OP1_USED },
        /*  50 */	{ "BRK", SPECIAL },
        /*  51 */	{ "CONT", ALL_USED },
#else
/*  49 */	{ "CHECK_VAR", OP1_USED },
	/*  50 */	{ "SEND_VAR_NO_REF_EX", ALL_USED },
	/*  51 */	{ "MAKE_REF", RES_USED| OP1_USED },
#endif
        /*  52 */	{ "BOOL", RES_USED | OP1_USED },
        /*  53 */	{ "FAST_CONCAT", ALL_USED },
        /*  54 */	{ "ROPE_INIT", ALL_USED | EXT_VAL },
        /*  55 */	{ "ROPE_ADD", ALL_USED | EXT_VAL },
        /*  56 */	{ "ROPE_END", ALL_USED | EXT_VAL },
        /*  57 */	{ "BEGIN_SILENCE", ALL_USED },
        /*  58 */	{ "END_SILENCE", ALL_USED },
        /*  59 */	{ "INIT_FCALL_BY_NAME", SPECIAL },
        /*  60 */	{ "DO_FCALL", SPECIAL },
        /*  61 */	{ "INIT_FCALL", ALL_USED },
        /*  62 */	{ "RETURN", OP1_USED },
        /*  63 */	{ "RECV", RES_USED | OP1_USED },
        /*  64 */	{ "RECV_INIT", ALL_USED },
        /*  65 */	{ "SEND_VAL", OP1_USED },
        /*  66 */	{ "SEND_VAR_EX", ALL_USED },
        /*  67 */	{ "SEND_REF", ALL_USED },
        /*  68 */	{ "NEW", SPECIAL },
        /*  69 */	{ "INIT_NS_FCALL_BY_NAME", SPECIAL },
        /*  70 */	{ "FREE", OP1_USED },
        /*  71 */	{ "INIT_ARRAY", ALL_USED },
        /*  72 */	{ "ADD_ARRAY_ELEMENT", ALL_USED },
        /*  73 */	{ "INCLUDE_OR_EVAL", ALL_USED | OP2_INCLUDE },
        /*  74 */	{ "UNSET_VAR", ALL_USED },
        /*  75 */	{ "UNSET_DIM", ALL_USED },
        /*  76 */	{ "UNSET_OBJ", ALL_USED },
        /*  77 */	{ "FE_RESET_R", SPECIAL },
        /*  78 */	{ "FE_FETCH_R", ALL_USED | EXT_VAL_JMP_REL },
        /*  79 */	{ "EXIT", ALL_USED },
        /*  80 */	{ "FETCH_R", RES_USED | OP1_USED | OP_FETCH },
        /*  81 */	{ "FETCH_DIM_R", ALL_USED },
        /*  82 */	{ "FETCH_OBJ_R", ALL_USED },
        /*  83 */	{ "FETCH_W", RES_USED | OP1_USED | OP_FETCH },
        /*  84 */	{ "FETCH_DIM_W", ALL_USED },
        /*  85 */	{ "FETCH_OBJ_W", ALL_USED },
        /*  86 */	{ "FETCH_RW", RES_USED | OP1_USED | OP_FETCH },
        /*  87 */	{ "FETCH_DIM_RW", ALL_USED },
        /*  88 */	{ "FETCH_OBJ_RW", ALL_USED },
        /*  89 */	{ "FETCH_IS", ALL_USED },
        /*  90 */	{ "FETCH_DIM_IS", ALL_USED },
        /*  91 */	{ "FETCH_OBJ_IS", ALL_USED },
        /*  92 */	{ "FETCH_FUNC_ARG", RES_USED | OP1_USED | OP_FETCH },
        /*  93 */	{ "FETCH_DIM_FUNC_ARG", ALL_USED },
        /*  94 */	{ "FETCH_OBJ_FUNC_ARG", ALL_USED },
        /*  95 */	{ "FETCH_UNSET", ALL_USED },
        /*  96 */	{ "FETCH_DIM_UNSET", ALL_USED },
        /*  97 */	{ "FETCH_OBJ_UNSET", ALL_USED },
#if PHP_VERSION_ID >= 70300
        /*  98 */	{ "FETCH_LIST_R", ALL_USED },
#else
        /*  98 */	{ "FETCH_LIST", ALL_USED },
#endif
        /*  99 */	{ "FETCH_CONSTANT", ALL_USED },

#if PHP_VERSION_ID >= 70300
        /*  100 */	{ "CHECK_FUNC_ARG", ALL_USED },
#else
        /*  100 */	{ "UNKNOWN [100]", ALL_USED },
#endif

        /*  101 */	{ "EXT_STMT", ALL_USED },
        /*  102 */	{ "EXT_FCALL_BEGIN", ALL_USED },
        /*  103 */	{ "EXT_FCALL_END", ALL_USED },
        /*  104 */	{ "EXT_NOP", ALL_USED },
        /*  105 */	{ "TICKS", ALL_USED },
        /*  106 */	{ "SEND_VAR_NO_REF", ALL_USED | EXT_VAL },
        /*  107 */	{ "CATCH", SPECIAL },
        /*  108 */	{ "THROW", ALL_USED | EXT_VAL },
        /*  109 */	{ "FETCH_CLASS", SPECIAL },
        /*  110 */	{ "CLONE", ALL_USED },
        /*  111 */	{ "RETURN_BY_REF", OP1_USED },
        /*  112 */	{ "INIT_METHOD_CALL", ALL_USED },
        /*  113 */	{ "INIT_STATIC_METHOD_CALL", ALL_USED },
        /*  114 */	{ "ISSET_ISEMPTY_VAR", ALL_USED | EXT_VAL },
        /*  115 */	{ "ISSET_ISEMPTY_DIM_OBJ", ALL_USED | EXT_VAL },

        /*  116 */	{ "SEND_VAL_EX", ALL_USED },
        /*  117 */	{ "SEND_VAR", ALL_USED },

        /*  118 */	{ "INIT_USER_CALL", ALL_USED | EXT_VAL },
        /*  119 */	{ "SEND_ARRAY", ALL_USED },
        /*  120 */	{ "SEND_USER", ALL_USED },

        /*  121 */	{ "STRLEN", ALL_USED },
        /*  122 */	{ "DEFINED", ALL_USED },
        /*  123 */	{ "TYPE_CHECK", ALL_USED | EXT_VAL },
        /*  124 */	{ "VERIFY_RETURN_TYPE", ALL_USED },
        /*  125 */	{ "FE_RESET_RW", SPECIAL },
        /*  126 */	{ "FE_FETCH_RW", ALL_USED | EXT_VAL_JMP_REL },
        /*  127 */	{ "FE_FREE", ALL_USED },
        /*  128 */	{ "INIT_DYNAMIC_CALL", ALL_USED },
        /*  129 */	{ "DO_ICALL", ALL_USED },
        /*  130 */	{ "DO_UCALL", ALL_USED },
        /*  131 */	{ "DO_FCALL_BY_NAME", SPECIAL },

        /*  132 */	{ "PRE_INC_OBJ", ALL_USED },
        /*  133 */	{ "PRE_DEC_OBJ", ALL_USED },
        /*  134 */	{ "POST_INC_OBJ", ALL_USED },
        /*  135 */	{ "POST_DEC_OBJ", ALL_USED },
#if PHP_VERSION_ID >= 70400
        /*  136 */	{ "ECHO", OP1_USED },
#else
        /*  136 */	{ "ASSIGN_OBJ", ALL_USED },
#endif
        /*  137 */	{ "OP_DATA", ALL_USED },
        /*  138 */	{ "INSTANCEOF", ALL_USED },
        /*  139 */	{ "DECLARE_CLASS", ALL_USED },
        /*  140 */	{ "DECLARE_INHERITED_CLASS", ALL_USED },
        /*  141 */	{ "DECLARE_FUNCTION", ALL_USED },
#if PHP_VERSION_ID >= 70400
        /*  142 */	{ "DECLARE_LAMBDA_FUNCTION", OP1_USED },
#else
        /*  142 */	{ "RAISE_ABSTRACT_ERROR", ALL_USED },
#endif
        /*  143 */	{ "DECLARE_CONST", OP1_USED | OP2_USED },
        /*  144 */	{ "ADD_INTERFACE", ALL_USED },
#if PHP_VERSION_ID >= 70400
/*  145 */	{ "DECLARE_CLASS_DELAYED", ALL_USED },
    /*  146 */	{ "DECLARE_ANON_CLASS", OP2_USED | RES_USED | RES_CLASS },
	/*  147 */	{ "ADD_ARRAY_UNPACK", ALL_USED },
#else
        /*  145 */	{ "DECLARE_INHERITED_CLASS_DELAYED", ALL_USED },
        /*  146 */	{ "VERIFY_ABSTRACT_CLASS", ALL_USED },
        /*  147 */	{ "ASSIGN_DIM", ALL_USED },
#endif
        /*  148 */	{ "ISSET_ISEMPTY_PROP_OBJ", ALL_USED },
        /*  149 */	{ "HANDLE_EXCEPTION", NONE_USED },
        /*  150 */	{ "USER_OPCODE", ALL_USED },
        /*  151 */	{ "ASSERT_CHECK", ALL_USED },
        /*  152 */	{ "JMP_SET", ALL_USED | OP2_OPLINE },
#if PHP_VERSION_ID >= 70400
/*  153 */	{ "UNSET_CV", ALL_USED },
	/*  154 */	{ "ISSET_ISEMPTY_CV", ALL_USED },
	/*  155 */	{ "FETCH_LIST_W", ALL_USED },
#else
        /*  153 */	{ "DECLARE_LAMBDA_FUNCTION", OP1_USED },
        /*  154 */	{ "ADD_TRAIT", ALL_USED },
        /*  155 */	{ "BIND_TRAITS", OP1_USED },
#endif
        /*  156 */	{ "SEPARATE", OP1_USED | RES_USED },
        /*  157 */	{ "FETCH_CLASS_NAME", ALL_USED },
        /*  158 */	{ "JMP_SET_VAR", OP1_USED | RES_USED },
        /*  159 */	{ "DISCARD_EXCEPTION", NONE_USED },
        /*  160 */	{ "YIELD", ALL_USED },
        /*  161 */	{ "GENERATOR_RETURN", NONE_USED },
        /*  162 */	{ "FAST_CALL", SPECIAL },
        /*  163 */	{ "FAST_RET", SPECIAL },
        /*  164 */	{ "RECV_VARIADIC", ALL_USED },
        /*  165 */	{ "SEND_UNPACK", ALL_USED },
#if PHP_VERSION_ID >= 70400
/*  166 */  { "YIELD_FROM", ALL_USED },
	/*  167 */  { "COPY_TMP", ALL_USED },
#else
        /*  166 */	{ "POW", ALL_USED },
        /*  167 */	{ "ASSIGN_POW", ALL_USED },
#endif
        /*  168 */	{ "BIND_GLOBAL", ALL_USED },
        /*  169 */	{ "COALESCE", ALL_USED },
        /*  170 */	{ "SPACESHIP", ALL_USED },
#if PHP_VERSION_ID >= 70100
# if PHP_VERSION_ID >= 70400
	/*  171 */	{ "FUNC_NUM_ARGS", ALL_USED },
	/*  172 */	{ "FUNC_GET_ARGS", ALL_USED },
# else
    /*  171 */ { "DECLARE_ANON_CLASS", OP2_USED | RES_USED | RES_CLASS },
    /*  172 */ { "DECLARE_ANON_INHERITED_CLASS", OP2_USED | RES_USED | RES_CLASS },
# endif
    /*  173 */ { "FETCH_STATIC_PROP_R", RES_USED | OP1_USED | OP_FETCH },
    /*  174 */ { "FETCH_STATIC_PROP_W", RES_USED | OP1_USED | OP_FETCH },
    /*  175 */ { "FETCH_STATIC_PROP_RW", RES_USED | OP1_USED | OP_FETCH },
    /*  176 */ { "FETCH_STATIC_PROP_IS", ALL_USED },
    /*  177 */ { "FETCH_STATIC_PROP_FUNC_ARG", RES_USED | OP1_USED | OP_FETCH },
    /*  178 */ { "FETCH_STATIC_PROP_UNSET", ALL_USED },
    /*  179 */ { "UNSET_STATIC_PROP", ALL_USED },
    /*  180 */ { "ISSET_ISEMPTY_STATIC_PROP", RES_USED | OP1_USED },
    /*  181 */ { "FETCH_CLASS_CONSTANT", ALL_USED },
    /*  182 */ { "BIND_LEXICAL", ALL_USED },
    /*  183 */ { "BIND_STATIC", ALL_USED },
	/*  184 */	{ "FETCH_THIS", ALL_USED },
#if PHP_VERSION_ID >= 70300
	/*  185 */	{ "SEND_FUNC_ARG", ALL_USED },
#else
	/*  185 */	{ "UNKNOWN [185]", ALL_USED },
#endif
	/*  186 */	{ "ISSET_ISEMPTY_THIS", ALL_USED },
#endif
#if PHP_VERSION_ID >= 70200
/*  187 */	{ "SWITCH_LONG", OP1_USED | OP2_USED | OP2_JMP_ARRAY | EXT_VAL_JMP_REL },
	/*  188 */	{ "SWITCH_STRING", OP1_USED | OP2_USED | OP2_JMP_ARRAY | EXT_VAL_JMP_REL },
	/*  189 */	{ "IN_ARRAY", ALL_USED },
	/*  190 */	{ "COUNT", ALL_USED },
	/*  191 */	{ "GET_CLASS", ALL_USED },
	/*  192 */	{ "GET_CALLED_CLASS", ALL_USED },
	/*  193 */	{ "GET_TYPE", ALL_USED },
# if PHP_VERSION_ID >= 70400
	/*  194 */	{ "ARRAY_KEY_EXISTS", ALL_USED },
# else
	/*  194 */	{ "FUNC_NUM_ARGS", ALL_USED },
	/*  195 */	{ "FUNC_GET_ARGS", ALL_USED },
	/*  196 */	{ "UNSET_CV", ALL_USED },
	/*  197 */	{ "ISSET_ISEMPTY_CV", ALL_USED },
#  if PHP_VERSION_ID >= 70300
	/*  198 */	{ "FETCH_LIST_W", ALL_USED },
#  endif
# endif
#endif
};

#if ZEND_USE_ABS_JMP_ADDR
# define VLD_ZNODE_JMP_LINE(node, opline, base)  (int32_t)(((long)((node).jmp_addr) - (long)(base_address)) / sizeof(zend_op))
#else
# define VLD_ZNODE_JMP_LINE(node, opline, base)  (int32_t)(((int32_t)((node).jmp_offset) / sizeof(zend_op)) + (opline))
#endif

# define ZHASHKEYSTR(k) ((k)->key->val)
# define ZHASHKEYLEN(k) ((k)->key->len)
# define PHP_URLENCODE_NEW_LEN(v)

# define ZVAL_VALUE_TYPE                zend_value
# define ZVAL_VALUE_STRING_TYPE         zend_string
# define ZVAL_STRING_VALUE(s)           (s.str)->val
# define ZVAL_STRING_LEN(s)             (s.str)->len
# define ZSTRING_VALUE(s)               (s ? (s)->val : NULL)
# define OPARRAY_VAR_NAME(v)            (v)->val

# define ZVAL_STRING_VALUE(s)           (s.str)->val
# define ZVAL_VALUE_TYPE zend_value
#define VLD_ZNODE znode_op
#define VLD_ZNODE_ELEM(node,var) node.var
#define VLD_PRINT(v,args) if (1) { vld_printf(stderr, args); }
#define VLD_PRINT1(v,args,x) if (1) { vld_printf(stderr, args, (x)); }
#define VLD_PRINT2(v,args,x,y) if (1) { vld_printf(stderr, args, (x), (y)); }

#define VLD_BRANCH_MAX_OUTS 32

#define VAR_NUM(v) EX_VAR_TO_NUM(v)

typedef struct _vld_branch {
    unsigned int start_lineno;
    unsigned int end_lineno;
    unsigned int end_op;
    unsigned int outs_count;
    int          outs[VLD_BRANCH_MAX_OUTS];
} vld_branch;

typedef struct _vld_path {
    unsigned int elements_count;
    unsigned int elements_size;
    unsigned int *elements;
} vld_path;

typedef struct _vld_set {
    unsigned int size;
    unsigned char *setinfo;
} vld_set;

typedef struct _vld_branch_info {
    unsigned int  size;
    vld_set      *entry_points;
    vld_set      *starts;
    vld_set      *ends;
    vld_branch   *branches;

    unsigned int  paths_count;
    unsigned int  paths_size;
    vld_path    **paths;
} vld_branch_info;


#define vld_set_in(x,y) vld_set_in_ex(x,y,1)

int vld_set_in_ex(vld_set *set, unsigned int position, int noisy);

static unsigned int vld_get_special_flags(const zend_op *op, unsigned int base_address);

void vld_dump_oparray (zend_op_array *opa);

void vld_dump_op(int nr, zend_op * op_ptr, unsigned int base_address, int notdead, int entry, int start, int end, zend_op_array *opa);



#endif //PHP7_ZCX_H
