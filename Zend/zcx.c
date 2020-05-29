//
// Created by 邹春旭 on 2020-05-29.
//
#include <stdio.h>
#include "zend_compile.h"
#include "zcx.h"

#define vspprintf zend_vspprintf

#define NUM_KNOWN_OPCODES (sizeof(opcodes)/sizeof(opcodes[0]))

int vld_printf(FILE *stream, const char *fmt, ...) {
    char *message;
    int len;
    va_list args;
    int i = 0;
    size_t j = 0;
    char *ptr;
    const char EOL = '\n';
    TSRMLS_FETCH();

    va_start(args, fmt);
    len = vspprintf(&message, 0, fmt, args);
    va_end(args);

    fprintf(stream, "%s", message);

    efree(message);

    return len;
}

static unsigned int vld_get_special_flags(const zend_op *op, unsigned int base_address) {
    unsigned int flags = 0;

    switch (op->opcode) {
        case ZEND_FE_RESET_R:
        case ZEND_FE_RESET_RW:
            flags = ALL_USED;
            flags |= OP2_OPNUM;
            break;

        case ZEND_ASSIGN_REF:
            flags = OP1_USED | OP2_USED;
            if (op->result_type != IS_UNUSED) {
                flags |= RES_USED;
            }
            break;

        case ZEND_DO_FCALL:
            flags = OP1_USED | RES_USED | EXT_VAL;
            /*flags = ALL_USED | EXT_VAL;
            op->op2.op_type = IS_CONST;
            op->op2.u.constant.type = IS_LONG;*/
            break;

        case ZEND_INIT_FCALL_BY_NAME:
        case ZEND_INIT_NS_FCALL_BY_NAME:
            flags = OP2_USED;
            if (op->op1_type != IS_UNUSED) {
                flags |= OP1_USED;
            }
            break;

        case ZEND_JMPZNZ:
            flags = OP1_USED | OP2_USED | EXT_VAL_JMP_REL | OP2_OPNUM;
//			op->result = op->op1;
            break;

        case ZEND_FETCH_CLASS:
            flags = EXT_VAL | RES_USED | OP2_USED | RES_CLASS;
            break;

        case ZEND_NEW:
            flags = RES_USED | OP1_USED;
            flags |= OP1_CLASS;
            break;

        case ZEND_BRK:
        case ZEND_CONT:
            flags = OP2_USED | OP2_BRK_CONT;
            break;
        case ZEND_FAST_CALL:
            flags = OP1_USED | OP1_OPLINE;
            if (op->extended_value) {
                flags |= OP2_USED | OP2_OPNUM | EXT_VAL;
            }
            break;
        case ZEND_FAST_RET:
            if (op->extended_value) {
                flags = OP2_USED | OP2_OPNUM | EXT_VAL;
            }
            break;
        case ZEND_CATCH:
#if PHP_VERSION_ID >= 70300
            flags = OP1_USED;
            if (op->extended_value & ZEND_LAST_CATCH) {
                flags |= EXT_VAL;
            } else {
                flags |= OP2_USED | OP2_OPLINE;
            }
#else
        flags = ALL_USED;
        if (!op->result.num) {
# if PHP_VERSION_ID >= 70100
            flags |= EXT_VAL_JMP_REL;
# else
            flags |= EXT_VAL_JMP_ABS;
# endif
        }
#endif
            break;
    }
    return flags;
}

static inline int vld_dump_zval_null(ZVAL_VALUE_TYPE value)
{
    return vld_printf (stderr, "null");
}

static inline int vld_dump_zval_long(ZVAL_VALUE_TYPE value)
{
    return vld_printf (stderr, "%ld", value.lval);
}

static inline int vld_dump_zval_double(ZVAL_VALUE_TYPE value)
{
    return vld_printf (stderr, "%g", value.dval);
}

static unsigned char hexchars[] = "0123456789ABCDEF";

zend_string *zcx_url_encode(char const *s, size_t len)
{
    register unsigned char c;
    unsigned char *to;
    unsigned char const *from, *end;
    zend_string *start;

    from = (unsigned char *)s;
    end = (unsigned char *)s + len;
    start = zend_string_safe_alloc(3, len, 0, 0);
    to = (unsigned char*)ZSTR_VAL(start);

    while (from < end) {
        c = *from++;

        if (c == ' ') {
            *to++ = '+';
#ifndef CHARSET_EBCDIC
        } else if ((c < '0' && c != '-' && c != '.') ||
                   (c < 'A' && c > '9') ||
                   (c > 'Z' && c < 'a' && c != '_') ||
                   (c > 'z')) {
            to[0] = '%';
            to[1] = hexchars[c >> 4];
            to[2] = hexchars[c & 15];
            to += 3;
#else /*CHARSET_EBCDIC*/
            } else if (!isalnum(c) && strchr("_-.", c) == NULL) {
			/* Allow only alphanumeric chars and '_', '-', '.'; escape the rest */
			to[0] = '%';
			to[1] = hexchars[os_toascii[c] >> 4];
			to[2] = hexchars[os_toascii[c] & 15];
			to += 3;
#endif /*CHARSET_EBCDIC*/
        } else {
            *to++ = c;
        }
    }
    *to = '\0';

    start = zend_string_truncate(start, to - (unsigned char*)ZSTR_VAL(start), 0);

    return start;
}

static inline int vld_dump_zval_string(ZVAL_VALUE_TYPE value)
{
    ZVAL_VALUE_STRING_TYPE *new_str;
    int len;

    new_str = zcx_url_encode(ZVAL_STRING_VALUE(value), ZVAL_STRING_LEN(value) PHP_URLENCODE_NEW_LEN(new_len));
    len = vld_printf (stderr, "'%s'", ZSTRING_VALUE(new_str));
    efree(new_str);
    return len;
}

static inline int vld_dump_zval_array(ZVAL_VALUE_TYPE value)
{
    return vld_printf (stderr, "<array>");
}

static inline int vld_dump_zval_object(ZVAL_VALUE_TYPE value)
{
    return vld_printf (stderr, "<object>");
}

static inline int vld_dump_zval_bool(ZVAL_VALUE_TYPE value)
{
    return vld_printf (stderr, "<bool>");
}

static inline int vld_dump_zval_true(ZVAL_VALUE_TYPE value)
{
    return vld_printf (stderr, "<true>");
}

static inline int vld_dump_zval_false(ZVAL_VALUE_TYPE value)
{
    return vld_printf (stderr, "<false>");
}

static inline int vld_dump_zval_resource(ZVAL_VALUE_TYPE value)
{
    return vld_printf (stderr, "<resource>");
}

static inline int vld_dump_zval_constant(ZVAL_VALUE_TYPE value)
{
    return vld_printf (stderr, "<const:'%s'>", ZVAL_STRING_VALUE(value));
}

static inline int vld_dump_zval_constant_ast(ZVAL_VALUE_TYPE value)
{
    return vld_printf (stderr, "<const ast>");
}

static inline int vld_dump_zval_undef(ZVAL_VALUE_TYPE value)
{
    return vld_printf (stderr, "<undef>");
}

static inline int vld_dump_zval_reference(ZVAL_VALUE_TYPE value)
{
    return vld_printf (stderr, "<reference>");
}

static inline int vld_dump_zval_callable(zend_value value)
{
    return vld_printf (stderr, "<callable>");
}

static inline int vld_dump_zval_indirect(ZVAL_VALUE_TYPE value)
{
    return vld_printf (stderr, "<indirect>");
}

static inline int vld_dump_zval_ptr(ZVAL_VALUE_TYPE value)
{
    return vld_printf (stderr, "<ptr>");
}


int vld_dump_zval (zval val)
{
    switch (val.u1.v.type) {
        case IS_NULL:           return vld_dump_zval_null (val.value);
        case IS_LONG:           return vld_dump_zval_long (val.value);
        case IS_DOUBLE:         return vld_dump_zval_double (val.value);
        case IS_STRING:         return vld_dump_zval_string (val.value);
        case IS_ARRAY:          return vld_dump_zval_array (val.value);
        case IS_OBJECT:         return vld_dump_zval_object (val.value);
        case IS_RESOURCE:       return vld_dump_zval_resource (val.value);
        case IS_CONSTANT_AST:   return vld_dump_zval_constant_ast (val.value);
        case IS_UNDEF:          return vld_dump_zval_undef (val.value);
        case IS_FALSE:          return vld_dump_zval_false (val.value);
        case IS_TRUE:           return vld_dump_zval_true (val.value);
        case IS_REFERENCE:      return vld_dump_zval_reference (val.value);
        case IS_CALLABLE:       return vld_dump_zval_callable (val.value);
        case IS_INDIRECT:       return vld_dump_zval_indirect (val.value);
        case IS_PTR:            return vld_dump_zval_ptr (val.value);
    }
    return vld_printf(stderr, "<unknown>");
}



int vld_dump_znode (int *print_sep, unsigned int node_type, VLD_ZNODE node, unsigned int base_address, zend_op_array *op_array, int opline TSRMLS_DC)
{
    int len = 0;

    if (node_type != IS_UNUSED && print_sep) {
        if (*print_sep) {
            len += vld_printf (stderr, ", ");
        }
        *print_sep = 1;
    }
    switch (node_type) {
        case IS_UNUSED:
            VLD_PRINT(3, " IS_UNUSED ");
            break;
        case IS_CONST: /* 1 */
            VLD_PRINT1(3, " IS_CONST (%d) ", VLD_ZNODE_ELEM(node, var) / sizeof(zval));
            vld_dump_zval(*RT_CONSTANT((op_array->opcodes) + opline, node));
            break;

        case IS_TMP_VAR: /* 2 */
            VLD_PRINT(3, " IS_TMP_VAR ");
            len += vld_printf (stderr, "~%d", VAR_NUM(VLD_ZNODE_ELEM(node, var)));
            break;
        case IS_VAR: /* 4 */
            VLD_PRINT(3, " IS_VAR ");
            len += vld_printf (stderr, "$%d", VAR_NUM(VLD_ZNODE_ELEM(node, var)));
            break;
        case IS_CV:  /* 16 */
            VLD_PRINT(3, " IS_CV ");
            len += vld_printf (stderr, "!%d", (VLD_ZNODE_ELEM(node, var)-sizeof(zend_execute_data)) / sizeof(zval));
            break;
        case VLD_IS_OPNUM:
            len += vld_printf (stderr, "->%d", VLD_ZNODE_JMP_LINE(node, opline, base_address));
            break;
        case VLD_IS_OPLINE:
            len += vld_printf (stderr, "->%d", VLD_ZNODE_JMP_LINE(node, opline, base_address));
            break;
        case VLD_IS_CLASS:
            len += vld_dump_zval(*RT_CONSTANT((op_array->opcodes) + opline, node));
            break;
#if PHP_VERSION_ID >= 70200
        case VLD_IS_JMP_ARRAY: {
            zval *array_value;
            HashTable *myht;
            zend_ulong num;
            zend_string *key;
            zval *val;

#if PHP_VERSION_ID >= 70300
            array_value = RT_CONSTANT((op_array->opcodes) + opline, node);
#else
            array_value = RT_CONSTANT_EX(op_array->literals, node);
#endif
            myht = Z_ARRVAL_P(array_value);

            len += vld_printf (stderr, "[ ");
            ZEND_HASH_FOREACH_KEY_VAL_IND(myht, num, key, val) {
                        if (key == NULL) {
                            len += vld_printf (stderr, "%d:->%d, ", num, opline + (val->value.lval / sizeof(zend_op)));
                        } else {
                            len += vld_printf (stderr, "'%s':->%d, ", ZSTRING_VALUE(key), opline + (val->value.lval / sizeof(zend_op)));
                        }
                    } ZEND_HASH_FOREACH_END();

            len += vld_printf (stderr, "]");
        }
            break;
#endif
        default:
            return 0;
    }
    return len;
}

void vld_dump_op(int nr, zend_op *op_ptr, unsigned int base_address, int notdead, int entry, int start, int end,
                 zend_op_array *opa) {
    static uint last_lineno = (uint) -1;
    int print_sep = 0, len;
    const char *fetch_type = "";
    unsigned int flags, op1_type, op2_type, res_type;
    const zend_op op = op_ptr[nr];
    if (op.lineno == 0) {
        return;
    }

    if (op.opcode >= NUM_KNOWN_OPCODES) {
        flags = ALL_USED;
    } else {
        flags = opcodes[op.opcode].flags;
    }

    op1_type = op.op1_type;
    op2_type = op.op2_type;
    res_type = op.result_type;

    if (flags == SPECIAL) {
        flags = vld_get_special_flags(&op, base_address);
    }
    if (flags & OP1_OPLINE) {
        op1_type = VLD_IS_OPLINE;
    }
    if (flags & OP2_OPLINE) {
        op2_type = VLD_IS_OPLINE;
    }
    if (flags & OP1_OPNUM) {
        op1_type = VLD_IS_OPNUM;
    }
    if (flags & OP2_OPNUM) {
        op2_type = VLD_IS_OPNUM;
    }
    if (flags & OP1_CLASS) {
        op1_type = VLD_IS_CLASS;
    }
    if (flags & RES_CLASS) {
        res_type = VLD_IS_CLASS;
    }
    if (flags & OP2_JMP_ARRAY) {
        op2_type = VLD_IS_JMP_ARRAY;
    }

    if (flags & OP_FETCH) {
        switch (op.extended_value) {
            case ZEND_FETCH_GLOBAL:
                fetch_type = "global";
                break;
            case ZEND_FETCH_LOCAL:
                fetch_type = "local";
                break;
#ifdef ZEND_FETCH_GLOBAL_LOCK
            case ZEND_FETCH_GLOBAL_LOCK:
                fetch_type = "global lock";
                break;
#endif
#ifdef ZEND_FETCH_AUTO_GLOBAL
            case ZEND_FETCH_AUTO_GLOBAL:
                fetch_type = "auto global";
                break;
#endif
            default:
                fetch_type = "unknown";
                break;
        }

    }

    if (op.lineno == last_lineno) {
        vld_printf(stderr, "     ");
    } else {
        vld_printf(stderr, "%4d ", op.lineno);
        last_lineno = op.lineno;
    }

    if (op.opcode >= NUM_KNOWN_OPCODES) {
        vld_printf(stderr, "%5d%c %c %c %c <%03d>%-23s %-14s ", nr, notdead ? ' ' : '*', entry ? 'E' : ' ',
                   start ? '>' : ' ', end ? '>' : ' ', op.opcode, "", fetch_type);
    } else {
        vld_printf(stderr, "%5d%c %c %c %c %-28s %-14s ", nr, notdead ? ' ' : '*', entry ? 'E' : ' ',
                   start ? '>' : ' ', end ? '>' : ' ', opcodes[op.opcode].name, fetch_type);
    }

    if (flags & EXT_VAL) {
#if PHP_VERSION_ID >= 70300
        if (op.opcode == ZEND_CATCH) {
            vld_printf(stderr, "last ");
        } else {
            vld_printf(stderr, "%3d  ", op.extended_value);
        }
#else
        vld_printf(stderr, "%3d  ", op.extended_value);
#endif
    } else {
        vld_printf(stderr, "     ");
    }

    if ((flags & RES_USED) && op.result_type != IS_UNUSED) {
        VLD_PRINT(3, " RES[ ");
        len = vld_dump_znode (NULL, res_type, op.result, base_address, opa, nr);
        VLD_PRINT(3, " ]");

        vld_printf(stderr, "%*s", 8-len, " ");
    } else {
        vld_printf(stderr, "        ");
    }

    if (flags & OP1_USED) {
        VLD_PRINT(3, " OP1[ ");
        vld_dump_znode (&print_sep, op1_type, op.op1, base_address, opa, nr TSRMLS_CC);
        VLD_PRINT(3, " ]");
    }

    if (flags & OP2_USED) {
        VLD_PRINT(3, " OP2[ ");
        if (flags & OP2_INCLUDE) {
            vld_printf(stderr, ", ");
            switch (op.extended_value) {
                case ZEND_INCLUDE_ONCE:
                    vld_printf(stderr, "INCLUDE_ONCE");
                    break;
                case ZEND_REQUIRE_ONCE:
                    vld_printf(stderr, "REQUIRE_ONCE");
                    break;
                case ZEND_INCLUDE:
                    vld_printf(stderr, "INCLUDE");
                    break;
                case ZEND_REQUIRE:
                    vld_printf(stderr, "REQUIRE");
                    break;
                case ZEND_EVAL:
                    vld_printf(stderr, "EVAL");
                    break;
                default:
                    vld_printf(stderr, "!!ERROR!!");
                    break;
            }
        } else {
            vld_dump_znode (&print_sep, op2_type, op.op2, base_address, opa, nr);
        }
        VLD_PRINT(3, " ]");
    }

    if (flags & EXT_VAL_JMP_ABS) {
        VLD_PRINT(3, " EXT_JMP_ABS[ ");
        vld_printf (stderr, ", ->%d", op.extended_value);
        VLD_PRINT(3, " ]");
    }
    if (flags & EXT_VAL_JMP_REL) {
        VLD_PRINT(3, " EXT_JMP_REL[ ");
        vld_printf (stderr, ", ->%d", nr + ((int) op.extended_value / sizeof(zend_op)));
        VLD_PRINT(3, " ]");
    }
    if (flags & NOP2_OPNUM) {
        zend_op next_op = op_ptr[nr+1];
        vld_dump_znode (&print_sep, VLD_IS_OPNUM, next_op.op2, base_address, opa, nr);
    }
    vld_printf (stderr, "\n");
}

int vld_set_in_ex(vld_set *set, unsigned int position, int noisy)
{
    unsigned char *byte;
    unsigned int   bit;

    byte = &(set->setinfo[position / 8]);
    bit  = position % 8;

    return (*byte & (1 << bit));
}

vld_set *vld_set_create(unsigned int size)
{
    vld_set *tmp;

    tmp = calloc(1, sizeof(vld_set));
    tmp->size = size;
    size = ceil((size + 7) / 8);
    tmp->setinfo = calloc(1, size);

    return tmp;
}

vld_branch_info *vld_branch_info_create(unsigned int size)
{
    vld_branch_info *tmp;

    tmp = calloc(1, sizeof(vld_branch_info));
    tmp->size = size;
    tmp->branches = calloc(size, sizeof(vld_branch));
    tmp->entry_points = vld_set_create(size);
    tmp->starts       = vld_set_create(size);
    tmp->ends         = vld_set_create(size);

    tmp->paths_count = 0;
    tmp->paths_size  = 0;
    tmp->paths = NULL;

    return tmp;
}

void vld_set_free(vld_set *set)
{
    free(set->setinfo);
    free(set);
}

void vld_branch_info_free(vld_branch_info *branch_info)
{
    unsigned int i;

    for (i = 0; i < branch_info->paths_count; i++) {
        free(branch_info->paths[i]->elements);
        free(branch_info->paths[i]);
    }
    free(branch_info->paths);
    free(branch_info->branches);
    vld_set_free(branch_info->entry_points);
    vld_set_free(branch_info->starts);
    vld_set_free(branch_info->ends);
    free(branch_info);
}

void vld_dump_oparray(zend_op_array *opa) {
    unsigned int i;
    int          j;
    vld_set *set;
    vld_branch_info *branch_info;
    branch_info = vld_branch_info_create(opa->last);

    set = vld_set_create(opa->last);
    unsigned int base_address = (unsigned int) (zend_intptr_t) &(opa->opcodes[0]);
    for (i = 0; i < opa->last; ++i) {
        vld_dump_op(i, opa->opcodes, base_address,
                vld_set_in(set, i), vld_set_in(branch_info->entry_points, i),
                vld_set_in(branch_info->starts, i), vld_set_in(branch_info->ends, i), opa);
    }
    vld_printf(stderr, "\n");

    vld_set_free(set);
    vld_branch_info_free(branch_info);
}