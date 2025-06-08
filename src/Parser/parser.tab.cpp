/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "src/Parser/parser.y"

#include <iostream>
#include <cstdlib>
#include <cmath>
#include "../AST/ast.hpp"
#include "../PrintVisitor/print_visitor.hpp"
#include "../Value/value.hpp"
#include "../Evaluator/evaluator.hpp"

using ExprPtr = std::unique_ptr<Expr>;
using ProgramPtr = std::unique_ptr<Program>;

using namespace std;
extern int yylex();
extern int yylineno;
void yyerror(const char* s);

Program* rootAST = nullptr;

#line 91 "src/Parser/parser.tab.cpp"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "parser.tab.hpp"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_LET = 3,                        /* LET  */
  YYSYMBOL_IN = 4,                         /* IN  */
  YYSYMBOL_WHILE = 5,                      /* WHILE  */
  YYSYMBOL_FOR = 6,                        /* FOR  */
  YYSYMBOL_IDENT = 7,                      /* IDENT  */
  YYSYMBOL_IF = 8,                         /* IF  */
  YYSYMBOL_ELSE = 9,                       /* ELSE  */
  YYSYMBOL_ELIF = 10,                      /* ELIF  */
  YYSYMBOL_FUNCTION = 11,                  /* FUNCTION  */
  YYSYMBOL_ARROW = 12,                     /* ARROW  */
  YYSYMBOL_ASSIGN = 13,                    /* ASSIGN  */
  YYSYMBOL_ASSIGN_DESTRUCT = 14,           /* ASSIGN_DESTRUCT  */
  YYSYMBOL_TRUE = 15,                      /* TRUE  */
  YYSYMBOL_FALSE = 16,                     /* FALSE  */
  YYSYMBOL_NUMBER = 17,                    /* NUMBER  */
  YYSYMBOL_STRING = 18,                    /* STRING  */
  YYSYMBOL_PLUS = 19,                      /* PLUS  */
  YYSYMBOL_MINUS = 20,                     /* MINUS  */
  YYSYMBOL_MULT = 21,                      /* MULT  */
  YYSYMBOL_DIV = 22,                       /* DIV  */
  YYSYMBOL_MOD = 23,                       /* MOD  */
  YYSYMBOL_POW = 24,                       /* POW  */
  YYSYMBOL_CONCAT = 25,                    /* CONCAT  */
  YYSYMBOL_LE = 26,                        /* LE  */
  YYSYMBOL_GE = 27,                        /* GE  */
  YYSYMBOL_EQ = 28,                        /* EQ  */
  YYSYMBOL_NEQ = 29,                       /* NEQ  */
  YYSYMBOL_LESS_THAN = 30,                 /* LESS_THAN  */
  YYSYMBOL_GREATER_THAN = 31,              /* GREATER_THAN  */
  YYSYMBOL_OR = 32,                        /* OR  */
  YYSYMBOL_AND = 33,                       /* AND  */
  YYSYMBOL_LPAREN = 34,                    /* LPAREN  */
  YYSYMBOL_RPAREN = 35,                    /* RPAREN  */
  YYSYMBOL_LBRACE = 36,                    /* LBRACE  */
  YYSYMBOL_RBRACE = 37,                    /* RBRACE  */
  YYSYMBOL_COMMA = 38,                     /* COMMA  */
  YYSYMBOL_SEMICOLON = 39,                 /* SEMICOLON  */
  YYSYMBOL_UMINUS = 40,                    /* UMINUS  */
  YYSYMBOL_YYACCEPT = 41,                  /* $accept  */
  YYSYMBOL_input = 42,                     /* input  */
  YYSYMBOL_program = 43,                   /* program  */
  YYSYMBOL_binding_list = 44,              /* binding_list  */
  YYSYMBOL_binding = 45,                   /* binding  */
  YYSYMBOL_stmt = 46,                      /* stmt  */
  YYSYMBOL_stmt_list = 47,                 /* stmt_list  */
  YYSYMBOL_ident_list = 48,                /* ident_list  */
  YYSYMBOL_expr = 49,                      /* expr  */
  YYSYMBOL_if_expr = 50,                   /* if_expr  */
  YYSYMBOL_elif_list = 51,                 /* elif_list  */
  YYSYMBOL_argument_list = 52              /* argument_list  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   317

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  41
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  12
/* YYNRULES -- Number of rules.  */
#define YYNRULES  50
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  110

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   295


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    72,    72,    76,    77,    85,    90,    98,   106,   111,
     122,   134,   135,   142,   143,   144,   148,   149,   150,   151,
     153,   158,   164,   169,   173,   177,   181,   185,   190,   194,
     198,   202,   206,   210,   214,   218,   222,   226,   230,   235,
     239,   251,   255,   259,   260,   299,   305,   308,   314,   315,
     319
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "LET", "IN", "WHILE",
  "FOR", "IDENT", "IF", "ELSE", "ELIF", "FUNCTION", "ARROW", "ASSIGN",
  "ASSIGN_DESTRUCT", "TRUE", "FALSE", "NUMBER", "STRING", "PLUS", "MINUS",
  "MULT", "DIV", "MOD", "POW", "CONCAT", "LE", "GE", "EQ", "NEQ",
  "LESS_THAN", "GREATER_THAN", "OR", "AND", "LPAREN", "RPAREN", "LBRACE",
  "RBRACE", "COMMA", "SEMICOLON", "UMINUS", "$accept", "input", "program",
  "binding_list", "binding", "stmt", "stmt_list", "ident_list", "expr",
  "if_expr", "elif_list", "argument_list", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-25)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     -25,     8,   103,   -25,     9,   -24,     6,     3,    12,    48,
     -25,   -25,   -25,   -25,   125,   125,   -25,    28,   243,   -25,
      51,    -2,   -25,   125,    64,   125,   125,   125,    40,   -25,
     158,    36,   -25,   125,   125,   125,   125,   125,   125,   125,
     125,   125,   125,   125,   125,   125,   125,   125,   125,   125,
       9,   175,    71,   243,   243,   -20,   192,    69,   -25,   -25,
      39,   -18,   -18,    25,    25,    25,    53,    53,    38,    38,
     286,   286,    38,    38,   258,   273,   243,   243,   -25,   125,
     125,   -25,   125,   125,   -25,    10,   -25,   243,   209,   243,
     143,     2,    74,   125,   125,    55,   -25,   125,   -25,   -25,
     243,   243,   125,   243,    79,   226,   -25,   125,   143,   -25
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       3,     0,     2,     1,     0,     0,     0,    22,     0,     0,
      18,    19,    16,    17,     0,     0,    11,     0,     8,    43,
       0,     0,     5,     0,     0,     0,    48,     0,     0,    23,
       0,     0,     4,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    41,    49,     0,     0,    13,    39,    20,
       0,    28,    29,    25,    26,    27,    24,    38,    32,    33,
      34,    35,    30,    31,    37,    36,     7,    40,     6,     0,
       0,    21,     0,     0,    14,     0,    12,    42,     0,    50,
       0,     0,     0,     0,     0,     0,    45,     0,    11,    15,
      44,    46,     0,    10,     0,     0,     9,     0,     0,    47
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -25,   -25,   -25,   -25,    41,    90,     0,   -25,   -14,   -25,
      -8,   -25
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     1,     2,    21,    22,    60,    31,    85,    18,    19,
      96,    55
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      29,    30,    49,    35,    36,    37,    38,    39,     3,    51,
      23,    53,    54,    56,    97,    81,    20,    25,    82,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    50,    26,    98,     4,
      24,     5,     6,     7,     8,    91,    27,     9,    92,    38,
      39,    10,    11,    12,    13,    28,    14,    33,    34,    35,
      36,    37,    38,    39,    48,    87,    88,    32,    89,    90,
      15,    52,    16,    59,    57,    80,    84,    38,    86,   100,
     101,    99,     4,   103,     5,     6,     7,     8,   105,   102,
       9,    78,    17,   108,    10,    11,    12,    13,   104,    14,
     109,     0,     0,     0,     0,     0,     4,     0,     5,     6,
       7,     8,     0,    15,     9,    16,   106,     0,    10,    11,
      12,    13,     0,    14,     0,     0,     0,     0,     4,     0,
       5,     6,     7,     8,     0,     0,     0,    15,     0,    16,
      10,    11,    12,    13,     0,    14,     0,     0,     0,     0,
       0,     0,    94,    95,     0,     0,     0,     0,     0,    15,
       0,    16,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,     0,    58,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,     0,
      79,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,     0,    83,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,     0,    93,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
       0,   107,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
       0,    47,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    33,    34,    35,    36,    37,
      38,    39,    40,    41,     0,     0,    44,    45
};

static const yytype_int8 yycheck[] =
{
      14,    15,     4,    21,    22,    23,    24,    25,     0,    23,
      34,    25,    26,    27,    12,    35,     7,    14,    38,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    38,    34,    36,     3,
      34,     5,     6,     7,     8,    35,    34,    11,    38,    24,
      25,    15,    16,    17,    18,     7,    20,    19,    20,    21,
      22,    23,    24,    25,    13,    79,    80,    39,    82,    83,
      34,     7,    36,    37,    34,     4,     7,    24,    39,    93,
      94,     7,     3,    97,     5,     6,     7,     8,   102,    34,
      11,    50,     2,   107,    15,    16,    17,    18,    98,    20,
     108,    -1,    -1,    -1,    -1,    -1,     3,    -1,     5,     6,
       7,     8,    -1,    34,    11,    36,    37,    -1,    15,    16,
      17,    18,    -1,    20,    -1,    -1,    -1,    -1,     3,    -1,
       5,     6,     7,     8,    -1,    -1,    -1,    34,    -1,    36,
      15,    16,    17,    18,    -1,    20,    -1,    -1,    -1,    -1,
      -1,    -1,     9,    10,    -1,    -1,    -1,    -1,    -1,    34,
      -1,    36,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    -1,    35,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    -1,
      35,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    -1,    35,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    -1,    35,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      -1,    35,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      -1,    33,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    -1,    -1,    30,    31
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    42,    43,     0,     3,     5,     6,     7,     8,    11,
      15,    16,    17,    18,    20,    34,    36,    46,    49,    50,
       7,    44,    45,    34,    34,    14,    34,    34,     7,    49,
      49,    47,    39,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    13,     4,
      38,    49,     7,    49,    49,    52,    49,    34,    35,    37,
      46,    49,    49,    49,    49,    49,    49,    49,    49,    49,
      49,    49,    49,    49,    49,    49,    49,    49,    45,    35,
       4,    35,    38,    35,     7,    48,    39,    49,    49,    49,
      49,    35,    38,    35,     9,    10,    51,    12,    36,     7,
      49,    49,    34,    49,    47,    49,    37,    35,    49,    51
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    41,    42,    43,    43,    44,    44,    45,    46,    46,
      46,    47,    47,    48,    48,    48,    49,    49,    49,    49,
      49,    49,    49,    49,    49,    49,    49,    49,    49,    49,
      49,    49,    49,    49,    49,    49,    49,    49,    49,    49,
      49,    49,    49,    49,    49,    50,    51,    51,    52,    52,
      52
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     0,     3,     1,     3,     3,     1,     8,
       7,     0,     3,     0,     1,     3,     1,     1,     1,     1,
       3,     4,     1,     2,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       4,     3,     5,     1,     7,     6,     2,     6,     0,     1,
       3
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* input: program  */
#line 72 "src/Parser/parser.y"
            { rootAST = (yyvsp[0].prog); }
#line 1235 "src/Parser/parser.tab.cpp"
    break;

  case 3: /* program: %empty  */
#line 76 "src/Parser/parser.y"
                      { (yyval.prog) = new Program(); }
#line 1241 "src/Parser/parser.tab.cpp"
    break;

  case 4: /* program: program stmt SEMICOLON  */
#line 78 "src/Parser/parser.y"
    {
      (yyvsp[-2].prog)->stmts.emplace_back( StmtPtr((yyvsp[-1].stmt)) );
      (yyval.prog) = (yyvsp[-2].prog);
    }
#line 1250 "src/Parser/parser.tab.cpp"
    break;

  case 5: /* binding_list: binding  */
#line 85 "src/Parser/parser.y"
            {
          (yyval.bindings) = new std::vector<std::pair<std::string, Expr*>>();
          (yyval.bindings)->push_back(*(yyvsp[0].binding));
          delete (yyvsp[0].binding);
      }
#line 1260 "src/Parser/parser.tab.cpp"
    break;

  case 6: /* binding_list: binding_list COMMA binding  */
#line 90 "src/Parser/parser.y"
                                 {
          (yyvsp[-2].bindings)->push_back(*(yyvsp[0].binding));
          delete (yyvsp[0].binding);
          (yyval.bindings) = (yyvsp[-2].bindings);
      }
#line 1270 "src/Parser/parser.tab.cpp"
    break;

  case 7: /* binding: IDENT ASSIGN expr  */
#line 98 "src/Parser/parser.y"
                        {
          (yyval.binding) = new std::pair<std::string, Expr*>(std::string((yyvsp[-2].str)), (yyvsp[0].expr));
          free((yyvsp[-2].str));
      }
#line 1279 "src/Parser/parser.tab.cpp"
    break;

  case 8: /* stmt: expr  */
#line 107 "src/Parser/parser.y"
    {
      (yyval.stmt) = new ExprStmt( ExprPtr((yyvsp[0].expr)) );
    }
#line 1287 "src/Parser/parser.tab.cpp"
    break;

  case 9: /* stmt: FUNCTION IDENT LPAREN ident_list RPAREN LBRACE stmt_list RBRACE  */
#line 111 "src/Parser/parser.y"
                                                                    {
          std::vector<std::string> args = std::move(*(yyvsp[-4].str_list));
          delete (yyvsp[-4].str_list);

          auto block = std::make_unique<Program>();
          block->stmts = std::move(*(yyvsp[-1].stmts));
          delete (yyvsp[-1].stmts);

          (yyval.stmt) = new FunctionDecl(std::string((yyvsp[-6].str)), std::move(args), std::move(block));
          free((yyvsp[-6].str));
      }
#line 1303 "src/Parser/parser.tab.cpp"
    break;

  case 10: /* stmt: FUNCTION IDENT LPAREN ident_list RPAREN ARROW expr  */
#line 122 "src/Parser/parser.y"
                                                        {
          std::vector<std::string> args = std::move(*(yyvsp[-3].str_list));
          delete (yyvsp[-3].str_list);

          (yyval.stmt) = new FunctionDecl(std::string((yyvsp[-5].str)), std::move(args), StmtPtr(new ExprStmt(ExprPtr((yyvsp[0].expr)))));
          free((yyvsp[-5].str));
      }
#line 1315 "src/Parser/parser.tab.cpp"
    break;

  case 11: /* stmt_list: %empty  */
#line 134 "src/Parser/parser.y"
                   { (yyval.stmts) = new std::vector<StmtPtr>(); }
#line 1321 "src/Parser/parser.tab.cpp"
    break;

  case 12: /* stmt_list: stmt_list stmt SEMICOLON  */
#line 135 "src/Parser/parser.y"
                               {
        (yyvsp[-2].stmts)->emplace_back(StmtPtr((yyvsp[-1].stmt)));
        (yyval.stmts) = (yyvsp[-2].stmts);
    }
#line 1330 "src/Parser/parser.tab.cpp"
    break;

  case 13: /* ident_list: %empty  */
#line 142 "src/Parser/parser.y"
                   { (yyval.str_list) = new std::vector<std::string>(); }
#line 1336 "src/Parser/parser.tab.cpp"
    break;

  case 14: /* ident_list: IDENT  */
#line 143 "src/Parser/parser.y"
            { (yyval.str_list) = new std::vector<std::string>(); (yyval.str_list)->push_back((yyvsp[0].str)); free((yyvsp[0].str)); }
#line 1342 "src/Parser/parser.tab.cpp"
    break;

  case 15: /* ident_list: ident_list COMMA IDENT  */
#line 144 "src/Parser/parser.y"
                             { (yyvsp[-2].str_list)->push_back((yyvsp[0].str)); free((yyvsp[0].str)); (yyval.str_list) = (yyvsp[-2].str_list); }
#line 1348 "src/Parser/parser.tab.cpp"
    break;

  case 16: /* expr: NUMBER  */
#line 148 "src/Parser/parser.y"
                        { (yyval.expr) = (yyvsp[0].expr); }
#line 1354 "src/Parser/parser.tab.cpp"
    break;

  case 17: /* expr: STRING  */
#line 149 "src/Parser/parser.y"
                        { (yyval.expr) = (yyvsp[0].expr); }
#line 1360 "src/Parser/parser.tab.cpp"
    break;

  case 18: /* expr: TRUE  */
#line 150 "src/Parser/parser.y"
                        { (yyval.expr) = (yyvsp[0].expr); }
#line 1366 "src/Parser/parser.tab.cpp"
    break;

  case 19: /* expr: FALSE  */
#line 151 "src/Parser/parser.y"
                        { (yyval.expr) = (yyvsp[0].expr); }
#line 1372 "src/Parser/parser.tab.cpp"
    break;

  case 20: /* expr: LBRACE stmt_list RBRACE  */
#line 153 "src/Parser/parser.y"
                              {
          (yyval.expr) = new ExprBlock(std::move(*(yyvsp[-1].stmts)));
          delete (yyvsp[-1].stmts);
      }
#line 1381 "src/Parser/parser.tab.cpp"
    break;

  case 21: /* expr: IDENT LPAREN argument_list RPAREN  */
#line 158 "src/Parser/parser.y"
                                        {
          (yyval.expr) = new CallExpr(std::string((yyvsp[-3].str)), std::move(*(yyvsp[-1].expr_list)));
          delete (yyvsp[-1].expr_list);
          free((yyvsp[-3].str));
      }
#line 1391 "src/Parser/parser.tab.cpp"
    break;

  case 22: /* expr: IDENT  */
#line 164 "src/Parser/parser.y"
            {
          (yyval.expr) = new VariableExpr(std::string((yyvsp[0].str)));
          free((yyvsp[0].str));
      }
#line 1400 "src/Parser/parser.tab.cpp"
    break;

  case 23: /* expr: MINUS expr  */
#line 169 "src/Parser/parser.y"
                              {
          (yyval.expr) = new UnaryExpr(UnaryExpr::OP_NEG, ExprPtr((yyvsp[0].expr)));
      }
#line 1408 "src/Parser/parser.tab.cpp"
    break;

  case 24: /* expr: expr POW expr  */
#line 173 "src/Parser/parser.y"
                    {
          (yyval.expr) = new BinaryExpr(BinaryExpr::OP_POW, ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
      }
#line 1416 "src/Parser/parser.tab.cpp"
    break;

  case 25: /* expr: expr MULT expr  */
#line 177 "src/Parser/parser.y"
                     {
          (yyval.expr) = new BinaryExpr(BinaryExpr::OP_MUL, ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
      }
#line 1424 "src/Parser/parser.tab.cpp"
    break;

  case 26: /* expr: expr DIV expr  */
#line 181 "src/Parser/parser.y"
                    {
          (yyval.expr) = new BinaryExpr(BinaryExpr::OP_DIV, ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
      }
#line 1432 "src/Parser/parser.tab.cpp"
    break;

  case 27: /* expr: expr MOD expr  */
#line 185 "src/Parser/parser.y"
                               {
          (yyval.expr) = new BinaryExpr(BinaryExpr::OP_MOD, ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
      }
#line 1440 "src/Parser/parser.tab.cpp"
    break;

  case 28: /* expr: expr PLUS expr  */
#line 190 "src/Parser/parser.y"
                     {
          (yyval.expr) = new BinaryExpr(BinaryExpr::OP_ADD, ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
      }
#line 1448 "src/Parser/parser.tab.cpp"
    break;

  case 29: /* expr: expr MINUS expr  */
#line 194 "src/Parser/parser.y"
                      {
          (yyval.expr) = new BinaryExpr(BinaryExpr::OP_SUB, ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
      }
#line 1456 "src/Parser/parser.tab.cpp"
    break;

  case 30: /* expr: expr LESS_THAN expr  */
#line 198 "src/Parser/parser.y"
                          {
          (yyval.expr) = new BinaryExpr(BinaryExpr::OP_LT, ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
      }
#line 1464 "src/Parser/parser.tab.cpp"
    break;

  case 31: /* expr: expr GREATER_THAN expr  */
#line 202 "src/Parser/parser.y"
                             {
          (yyval.expr) = new BinaryExpr(BinaryExpr::OP_GT, ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
      }
#line 1472 "src/Parser/parser.tab.cpp"
    break;

  case 32: /* expr: expr LE expr  */
#line 206 "src/Parser/parser.y"
                   {
          (yyval.expr) = new BinaryExpr(BinaryExpr::OP_LE, ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
      }
#line 1480 "src/Parser/parser.tab.cpp"
    break;

  case 33: /* expr: expr GE expr  */
#line 210 "src/Parser/parser.y"
                   {
          (yyval.expr) = new BinaryExpr(BinaryExpr::OP_GE, ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
      }
#line 1488 "src/Parser/parser.tab.cpp"
    break;

  case 34: /* expr: expr EQ expr  */
#line 214 "src/Parser/parser.y"
                   {
          (yyval.expr) = new BinaryExpr(BinaryExpr::OP_EQ, ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
      }
#line 1496 "src/Parser/parser.tab.cpp"
    break;

  case 35: /* expr: expr NEQ expr  */
#line 218 "src/Parser/parser.y"
                    {
          (yyval.expr) = new BinaryExpr(BinaryExpr::OP_NEQ, ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
      }
#line 1504 "src/Parser/parser.tab.cpp"
    break;

  case 36: /* expr: expr AND expr  */
#line 222 "src/Parser/parser.y"
                    {
          (yyval.expr) = new BinaryExpr(BinaryExpr::OP_AND, ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
      }
#line 1512 "src/Parser/parser.tab.cpp"
    break;

  case 37: /* expr: expr OR expr  */
#line 226 "src/Parser/parser.y"
                   {
          (yyval.expr) = new BinaryExpr(BinaryExpr::OP_OR, ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
      }
#line 1520 "src/Parser/parser.tab.cpp"
    break;

  case 38: /* expr: expr CONCAT expr  */
#line 230 "src/Parser/parser.y"
                       {
        // Creamos un BinaryExpr con OP_CONCAT
        (yyval.expr) = new BinaryExpr(BinaryExpr::OP_CONCAT, ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
    }
#line 1529 "src/Parser/parser.tab.cpp"
    break;

  case 39: /* expr: LPAREN expr RPAREN  */
#line 235 "src/Parser/parser.y"
                         {
          (yyval.expr) = (yyvsp[-1].expr);
      }
#line 1537 "src/Parser/parser.tab.cpp"
    break;

  case 40: /* expr: LET binding_list IN expr  */
#line 239 "src/Parser/parser.y"
                               {
          Expr* result = (yyvsp[0].expr);
          auto& list = *(yyvsp[-2].bindings);

          for (auto it = list.rbegin(); it != list.rend(); ++it) {
              result = new LetExpr(it->first, ExprPtr(it->second), std::make_unique<ExprStmt>(ExprPtr(result)));
          }

          delete (yyvsp[-2].bindings);
          (yyval.expr) = result;      
        }
#line 1553 "src/Parser/parser.tab.cpp"
    break;

  case 41: /* expr: IDENT ASSIGN_DESTRUCT expr  */
#line 251 "src/Parser/parser.y"
                                 {
          (yyval.expr) = new AssignExpr(std::string((yyvsp[-2].str)), ExprPtr((yyvsp[0].expr)));
          free((yyvsp[-2].str));
      }
#line 1562 "src/Parser/parser.tab.cpp"
    break;

  case 42: /* expr: WHILE LPAREN expr RPAREN expr  */
#line 255 "src/Parser/parser.y"
                                    {
      (yyval.expr) = new WhileExpr(ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
    }
#line 1570 "src/Parser/parser.tab.cpp"
    break;

  case 44: /* expr: FOR LPAREN IDENT IN expr RPAREN expr  */
#line 260 "src/Parser/parser.y"
                                           {
        auto argsNext = std::vector<ExprPtr>();
        argsNext.push_back(std::make_unique<VariableExpr>("__iter"));
        ExprPtr callNext = std::make_unique<CallExpr>("next", std::move(argsNext));

        auto argsCurrent = std::vector<ExprPtr>();
        argsCurrent.push_back(std::make_unique<VariableExpr>("__iter"));
        ExprPtr callCurrent = std::make_unique<CallExpr>("current", std::move(argsCurrent));

        ExprPtr bodyFor = ExprPtr((yyvsp[0].expr));

        Expr* innerLetRaw = new LetExpr(
            std::string((yyvsp[-4].str)),                      // nombre de la variable (x)
            std::move(callCurrent),               // initializer = current(__iter)
            StmtPtr(new ExprStmt(std::move(bodyFor))) // cuerpo = ExprStmt(bodyFor)
        );

        Expr* whileRaw = new WhileExpr(
            std::move(callNext),
            ExprPtr(innerLetRaw)
        );

        auto argsIter = std::vector<ExprPtr>();
        argsIter.push_back( ExprPtr((yyvsp[-2].expr)) );
        ExprPtr callIter = std::make_unique<CallExpr>("iter", std::move(argsIter));

        Expr* outerLetRaw = new LetExpr(
            "__iter",
            std::move(callIter),
            StmtPtr(new ExprStmt( ExprPtr(whileRaw) ))
        );

        free((yyvsp[-4].str));  // liberar el IDENT
        (yyval.expr) = outerLetRaw;
    }
#line 1610 "src/Parser/parser.tab.cpp"
    break;

  case 45: /* if_expr: IF LPAREN expr RPAREN expr elif_list  */
#line 299 "src/Parser/parser.y"
                                         {
        (yyval.expr) = new IfExpr(ExprPtr((yyvsp[-3].expr)), ExprPtr((yyvsp[-1].expr)), ExprPtr((yyvsp[0].expr)));
    }
#line 1618 "src/Parser/parser.tab.cpp"
    break;

  case 46: /* elif_list: ELSE expr  */
#line 305 "src/Parser/parser.y"
              {
        (yyval.expr) = (yyvsp[0].expr);
    }
#line 1626 "src/Parser/parser.tab.cpp"
    break;

  case 47: /* elif_list: ELIF LPAREN expr RPAREN expr elif_list  */
#line 308 "src/Parser/parser.y"
                                             {
        (yyval.expr) = new IfExpr(ExprPtr((yyvsp[-3].expr)), ExprPtr((yyvsp[-1].expr)), ExprPtr((yyvsp[0].expr)));
    }
#line 1634 "src/Parser/parser.tab.cpp"
    break;

  case 48: /* argument_list: %empty  */
#line 314 "src/Parser/parser.y"
                   { (yyval.expr_list) = new std::vector<ExprPtr>(); }
#line 1640 "src/Parser/parser.tab.cpp"
    break;

  case 49: /* argument_list: expr  */
#line 315 "src/Parser/parser.y"
           {
          (yyval.expr_list) = new std::vector<ExprPtr>();
          (yyval.expr_list)->emplace_back(ExprPtr((yyvsp[0].expr)));
      }
#line 1649 "src/Parser/parser.tab.cpp"
    break;

  case 50: /* argument_list: argument_list COMMA expr  */
#line 319 "src/Parser/parser.y"
                               {
          (yyvsp[-2].expr_list)->emplace_back(ExprPtr((yyvsp[0].expr)));
          (yyval.expr_list) = (yyvsp[-2].expr_list);
      }
#line 1658 "src/Parser/parser.tab.cpp"
    break;


#line 1662 "src/Parser/parser.tab.cpp"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 326 "src/Parser/parser.y"


void yyerror(const char* s) {
    std::cerr << "Parse error: " << s << " at line " << yylineno << std::endl;
}
