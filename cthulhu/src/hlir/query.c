#include "common.h"

#include "base/panic.h"
#include "cthulhu/hlir/hlir.h"
#include "cthulhu/hlir/query.h"

static bool has_name(hlir_kind_t kind)
{
    switch (kind)
    {
    case eHlirStruct:
    case eHlirUnion:
    case eHlirDigit:
    case eHlirBool:
    case eHlirString:
    case eHlirVoid:
    case eHlirClosure:
    case eHlirPointer:
    case eHlirArray:

    case eHlirType:
    case eHlirAlias:

    case eHlirForward:
    case eHlirField:
    case eHlirFunction:

    case eHlirGlobal:
    case eHlirLocal:
    case eHlirParam:

    case eHlirError:

    case eHlirModule:
        return true;

    default:
        return false;
    }
}

static bool has_attribs(hlir_kind_t kind)
{
    switch (kind)
    {
    case eHlirStruct:
    case eHlirUnion:
    case eHlirAlias:
    case eHlirDigit:
    case eHlirBool:
    case eHlirString:
    case eHlirPointer:
    case eHlirVoid:
    case eHlirClosure:

    case eHlirField:
    case eHlirForward:
    case eHlirFunction:

    case eHlirGlobal:
    case eHlirLocal:
    case eHlirParam:
        return true;

    default:
        return false;
    }
}

hlir_kind_t get_hlir_kind(const hlir_t *hlir)
{
    CTASSERT(hlir != NULL);
    return hlir->type;
}

const hlir_t *get_hlir_type(const hlir_t *hlir)
{
    CTASSERT(hlir != NULL);
    return hlir->of;
}

const char *get_hlir_name(const hlir_t *hlir)
{
    CTASSERT(hlir != NULL);
    hlir_kind_t kind = get_hlir_kind(hlir);
    CTASSERTF(has_name(kind), "hlir_t %s has no name", hlir_kind_to_string(kind));

    return hlir->name;
}

const hlir_attributes_t *get_hlir_attributes(const hlir_t *hlir)
{
    CTASSERT(hlir != NULL);
    hlir_kind_t kind = get_hlir_kind(hlir);
    CTASSERTF(has_attribs(kind), "hlir %s has no attributes", hlir_kind_to_string(kind));

    return hlir->attributes;
}

node_t get_hlir_node(const hlir_t *hlir)
{
    CTASSERT(hlir != NULL);
    return hlir->location;
}

const hlir_t *get_hlir_parent(const hlir_t *hlir)
{
    CTASSERT(hlir != NULL);
    return hlir->parentDecl;
}

bool hlir_is(const hlir_t *hlir, hlir_kind_t kind)
{
    return get_hlir_kind(hlir) == kind;
}

bool hlir_will_be(const hlir_t *hlir, hlir_kind_t kind)
{
    return hlir_is(hlir, eHlirForward) && hlir->expected == kind;
}

bool hlis_is_or_will_be(const hlir_t *hlir, hlir_kind_t kind)
{
    return hlir_is(hlir, kind) || hlir_will_be(hlir, kind);
}

///
/// specific
///

bool hlir_is_type(const hlir_t *hlir)
{
    switch (get_hlir_kind(hlir))
    {
    case eHlirStruct:
    case eHlirUnion:
    case eHlirDigit:
    case eHlirBool:
    case eHlirString:
    case eHlirVoid:
    case eHlirType:
    case eHlirAlias:
        return true;

    default:
        return false;
    }
}

bool hlir_is_decl(const hlir_t *hlir)
{
    switch (get_hlir_kind(hlir))
    {
    case eHlirFunction:
    case eHlirGlobal:
        return true;

    default:
        return false;
    }
}

///
/// debugging
///

static const char *kKindNames[eHlirTotal] = {
    [eHlirLiteralDigit] = "digit-literal",
    [eHlirLiteralBool] = "bool-literal",
    [eHlirLiteralString] = "string-literal",

    [eHlirName] = "name",
    [eHlirUnary] = "unary",
    [eHlirBinary] = "binary",
    [eHlirCompare] = "compare",
    [eHlirCall] = "call",

    [eHlirStmts] = "stmt-list",
    [eHlirBranch] = "branch",
    [eHlirLoop] = "loop",
    [eHlirAssign] = "assign",
    [eHlirReturn] = "return",

    [eHlirStruct] = "struct-type",
    [eHlirUnion] = "union-type",
    [eHlirDigit] = "digit-type",
    [eHlirBool] = "bool-type",
    [eHlirString] = "string-type",
    [eHlirVoid] = "void-type",
    [eHlirClosure] = "closure-type",
    [eHlirPointer] = "pointer-type",
    [eHlirArray] = "array-type",
    [eHlirType] = "metatype",
    [eHlirAlias] = "alias-type",

    [eHlirLocal] = "local",
    [eHlirParam] = "param",
    [eHlirGlobal] = "global",

    [eHlirForward] = "forward",
    [eHlirFunction] = "function",
    [eHlirModule] = "module",

    [eHlirField] = "field",

    [eHlirError] = "internal-error",
};

static const char *kDigitNames[eDigitTotal] = {
    [eChar] = "char", [eShort] = "short", [eInt] = "int", [eLong] = "long", [eIntSize] = "size", [eIntPtr] = "intptr",
};

static const char *kSignNames[eSignTotal] = {[eSigned] = "signed", [eUnsigned] = "unsigned"};

const char *hlir_kind_to_string(hlir_kind_t kind)
{
    return kKindNames[kind];
}

const char *hlir_sign_to_string(sign_t sign)
{
    return kSignNames[sign];
}

const char *hlir_digit_to_string(digit_t digit)
{
    return kDigitNames[digit];
}

const hlir_t *hlir_follow_type(const hlir_t *hlir)
{
    if (hlir_is(hlir, eHlirAlias) && !hlir->newtype)
    {
        return hlir_follow_type(hlir->alias);
    }

    if (hlir_is(hlir, eHlirParam))
    {
        return hlir_follow_type(get_hlir_type(hlir));
    }

    return hlir;
}

const hlir_t *hlir_real_type(const hlir_t *hlir)
{
    if (hlir_is(hlir, eHlirAlias))
    {
        return hlir_follow_type(hlir->alias);
    }

    if (hlir_is(hlir, eHlirParam))
    {
        return hlir_follow_type(get_hlir_type(hlir));
    }

    return hlir;
}

bool hlir_types_equal(const hlir_t *lhs, const hlir_t *rhs)
{
    const hlir_t *actualLhs = hlir_follow_type(lhs);
    const hlir_t *actualRhs = hlir_follow_type(rhs);

    if ((lhs == rhs) || (actualLhs == actualRhs))
    {
        return true;
    }

    hlir_kind_t lhsKind = get_hlir_kind(actualLhs);
    hlir_kind_t rhsKind = get_hlir_kind(actualRhs);
    if (lhsKind != rhsKind)
    {
        return false;
    }

    switch (lhsKind)
    {
    case eHlirDigit:
        return actualLhs->width == actualRhs->width && actualLhs->sign == actualRhs->sign;
    case eHlirString: // TODO: update this when we have multiple string encodings
    case eHlirBool:
    case eHlirVoid:
        return true;

    case eHlirFunction: {
        vector_t *lhsParams = closure_params(actualLhs);
        vector_t *rhsParams = closure_params(actualRhs);
        if (vector_len(lhsParams) != vector_len(rhsParams))
        {
            return false;
        }

        if (closure_variadic(actualLhs) != closure_variadic(actualRhs))
        {
            return false;
        }

        for (size_t i = 0; i < vector_len(lhsParams); i++)
        {
            const hlir_t *lhsParam = vector_get(lhsParams, i);
            const hlir_t *rhsParam = vector_get(rhsParams, i);
            if (!hlir_types_equal(lhsParam, rhsParam))
            {
                return false;
            }
        }
        
        return true;
    }

    case eHlirAlias:
    case eHlirError: // TODO: are errors always equal, or never equal?
        return false;

    default:
        CTASSERTF(false, "unknown type %s", hlir_kind_to_string(lhsKind));
        return false;
    }
}
