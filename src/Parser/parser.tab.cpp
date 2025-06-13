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
#include <cstring>
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


struct ClassBody {
  std::vector<std::pair<std::string, ExprPtr>> attributes;
  std::vector<StmtPtr> methods;
};



#line 100 "src/Parser/parser.tab.cpp"

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
  YYSYMBOL_TYPE = 40,                      /* TYPE  */
  YYSYMBOL_INHERITS = 41,                  /* INHERITS  */
  YYSYMBOL_NEW = 42,                       /* NEW  */
  YYSYMBOL_SELF = 43,                      /* SELF  */
  YYSYMBOL_BASE = 44,                      /* BASE  */
  YYSYMBOL_DOT = 45,                       /* DOT  */
  YYSYMBOL_ELIF_CONTENT = 46,              /* ELIF_CONTENT  */
  YYSYMBOL_UMINUS = 47,                    /* UMINUS  */
  YYSYMBOL_LET_IN_PREC = 48,               /* LET_IN_PREC  */
  YYSYMBOL_WHILE_PREC = 49,                /* WHILE_PREC  */
  YYSYMBOL_FOR_PREC = 50,                  /* FOR_PREC  */
  YYSYMBOL_YYACCEPT = 51,                  /* $accept  */
  YYSYMBOL_input = 52,                     /* input  */
  YYSYMBOL_program = 53,                   /* program  */
  YYSYMBOL_binding_list = 54,              /* binding_list  */
  YYSYMBOL_binding = 55,                   /* binding  */
  YYSYMBOL_decl = 56,                      /* decl  */
  YYSYMBOL_stmt = 57,                      /* stmt  */
  YYSYMBOL_opt_inherits = 58,              /* opt_inherits  */
  YYSYMBOL_class_body = 59,                /* class_body  */
  YYSYMBOL_member_def = 60,                /* member_def  */
  YYSYMBOL_stmt_list = 61,                 /* stmt_list  */
  YYSYMBOL_ident_list = 62,                /* ident_list  */
  YYSYMBOL_expr = 63,                      /* expr  */
  YYSYMBOL_if_expr = 64,                   /* if_expr  */
  YYSYMBOL_elif_list = 65,                 /* elif_list  */
  YYSYMBOL_argument_list = 66              /* argument_list  */
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
typedef yytype_uint8 yy_state_t;

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
#define YYLAST   643

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  51
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  16
/* YYNRULES -- Number of rules.  */
#define YYNRULES  66
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  160

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   305


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
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   105,   105,   109,   111,   116,   123,   128,   136,   144,
     158,   174,   179,   190,   191,   195,   198,   211,   221,   245,
     259,   281,   296,   297,   304,   305,   306,   310,   311,   312,
     313,   315,   320,   326,   331,   335,   339,   343,   347,   351,
     355,   359,   363,   367,   371,   375,   379,   383,   387,   391,
     395,   399,   411,   415,   419,   420,   456,   460,   463,   466,
     470,   482,   488,   491,   497,   498,   502
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
  "RBRACE", "COMMA", "SEMICOLON", "TYPE", "INHERITS", "NEW", "SELF",
  "BASE", "DOT", "ELIF_CONTENT", "UMINUS", "LET_IN_PREC", "WHILE_PREC",
  "FOR_PREC", "$accept", "input", "program", "binding_list", "binding",
  "decl", "stmt", "opt_inherits", "class_body", "member_def", "stmt_list",
  "ident_list", "expr", "if_expr", "elif_list", "argument_list", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-101)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -101,     4,    49,  -101,     5,   -23,    17,    54,    27,    55,
    -101,  -101,  -101,  -101,   260,   260,  -101,    70,    79,  -101,
    -101,  -101,    48,   531,  -101,    81,     3,  -101,   260,    97,
     260,   260,   260,    72,  -101,   315,    92,    77,    83,  -101,
     260,   260,   260,   260,   260,   260,   260,   260,   260,   260,
     260,   260,   260,   260,   260,   118,   260,   260,     5,   342,
     127,   531,   531,   -30,   369,   136,  -101,   139,  -101,   108,
     141,   117,   260,    99,    99,   -22,   -22,   -22,    35,    35,
      51,    51,   598,   598,    51,    51,   558,   585,   142,   531,
    -101,  -101,   260,   260,  -101,   260,   260,  -101,   -29,   121,
    -101,  -101,  -101,   -25,   260,  -101,   396,   531,   288,    10,
     150,   136,     8,  -101,   531,   260,   260,   124,  -101,   260,
    -101,  -101,    76,    50,   152,  -101,  -101,  -101,   531,   260,
     531,   134,   148,   260,   136,   128,   423,  -101,   450,    78,
     136,   260,  -101,    12,    95,   288,   260,  -101,    13,  -101,
     477,   176,   260,  -101,  -101,  -101,   504,   218,  -101,  -101
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       3,     0,     2,     1,     0,     0,     0,    33,     0,     0,
      29,    30,    27,    28,     0,     0,    22,     0,     0,    57,
      58,     5,     0,    11,    54,     0,     0,     6,     0,     0,
       0,    64,     0,     0,    34,     0,     0,    13,     0,     4,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    52,    65,     0,     0,    24,    50,     0,    31,     0,
       0,     0,    64,    39,    40,    36,    37,    38,    35,    49,
      43,    44,    45,    46,    41,    42,    48,    47,    59,     8,
      51,     7,     0,     0,    32,     0,     0,    25,     0,     0,
      23,    14,    15,     0,     0,    53,     0,    66,     0,     0,
       0,    24,     0,    56,    60,     0,     0,     0,    61,     0,
      22,    26,     0,     0,     0,     9,    16,    55,    62,     0,
      12,     0,     0,     0,    24,     0,     0,    10,     0,     0,
      24,     0,    17,     0,     0,     0,     0,    22,     0,    63,
       0,     0,     0,    22,    21,    19,     0,     0,    20,    18
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -101,  -101,  -101,  -101,   103,  -101,   161,  -101,  -101,  -101,
    -100,   -90,   -14,  -101,    19,    93
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     1,     2,    26,    27,    21,    69,    71,   112,   126,
      36,    98,    23,    24,   118,    63
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      34,    35,    45,    46,     3,    94,   109,    57,    95,   110,
     113,    28,    25,    95,    59,   123,    61,    62,    64,   124,
     131,   122,   119,    55,   146,   152,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    58,    89,    90,   139,   125,   120,   151,   147,   153,
     144,    29,     4,   157,     5,     6,     7,     8,    62,    45,
       9,    32,    33,   133,    10,    11,    12,    13,    30,    14,
      40,    41,    42,    43,    44,    45,    46,    37,   105,   106,
      55,   107,   108,    15,   134,    16,    38,    39,    31,    17,
     114,    18,    19,    20,    56,     4,    55,     5,     6,     7,
       8,   127,   128,    67,    60,   130,    65,    10,    11,    12,
      13,   132,    14,   143,   110,   136,   110,    72,    70,   138,
      42,    43,    44,    45,    46,    88,    15,   145,    16,    68,
     148,    93,   150,   110,    18,    19,    20,     4,   156,     5,
       6,     7,     8,    97,    55,    67,    99,   100,   101,    10,
      11,    12,    13,   102,    14,   111,   104,   121,   129,   135,
     119,    91,   140,    22,   149,   103,     0,     0,    15,     0,
      16,   137,     0,     0,     0,     0,    18,    19,    20,     4,
       0,     5,     6,     7,     8,     0,     0,    67,     0,     0,
       0,    10,    11,    12,    13,     0,    14,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      15,     0,    16,   155,     0,     0,     0,     0,    18,    19,
      20,     4,     0,     5,     6,     7,     8,     0,     0,    67,
       0,     0,     0,    10,    11,    12,    13,     0,    14,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    15,     0,    16,   159,     0,     0,     0,     0,
      18,    19,    20,     4,     0,     5,     6,     7,     8,     0,
       0,     0,     0,     0,     0,    10,    11,    12,    13,     0,
      14,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    15,     0,    16,   116,   117,     0,
       0,     0,    18,    19,    20,     0,     0,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    55,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,     0,
      66,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      55,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,     0,    92,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    55,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,     0,    96,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    55,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
       0,   115,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    55,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,     0,   141,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    55,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,     0,     0,     0,     0,     0,   142,
       0,     0,     0,     0,     0,    55,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,     0,     0,     0,     0,     0,   154,     0,     0,     0,
       0,     0,    55,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,     0,     0,
       0,     0,     0,   158,     0,     0,     0,     0,     0,    55,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    55,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
       0,    54,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    55,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    40,    41,    42,
      43,    44,    45,    46,    47,    48,     0,     0,    51,    52,
      55,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    55
};

static const yytype_int16 yycheck[] =
{
      14,    15,    24,    25,     0,    35,    35,     4,    38,    38,
      35,    34,     7,    38,    28,     7,    30,    31,    32,    11,
     120,   111,    12,    45,    12,    12,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    38,    56,    57,   134,    37,    36,   147,    36,    36,
     140,    34,     3,   153,     5,     6,     7,     8,    72,    24,
      11,    34,     7,    13,    15,    16,    17,    18,    14,    20,
      19,    20,    21,    22,    23,    24,    25,     7,    92,    93,
      45,    95,    96,    34,    34,    36,     7,    39,    34,    40,
     104,    42,    43,    44,    13,     3,    45,     5,     6,     7,
       8,   115,   116,    11,     7,   119,    34,    15,    16,    17,
      18,    35,    20,    35,    38,   129,    38,    34,    41,   133,
      21,    22,    23,    24,    25,     7,    34,   141,    36,    37,
      35,     4,   146,    38,    42,    43,    44,     3,   152,     5,
       6,     7,     8,     7,    45,    11,     7,    39,     7,    15,
      16,    17,    18,    36,    20,    34,    14,     7,    34,     7,
      12,    58,    34,     2,   145,    72,    -1,    -1,    34,    -1,
      36,    37,    -1,    -1,    -1,    -1,    42,    43,    44,     3,
      -1,     5,     6,     7,     8,    -1,    -1,    11,    -1,    -1,
      -1,    15,    16,    17,    18,    -1,    20,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      34,    -1,    36,    37,    -1,    -1,    -1,    -1,    42,    43,
      44,     3,    -1,     5,     6,     7,     8,    -1,    -1,    11,
      -1,    -1,    -1,    15,    16,    17,    18,    -1,    20,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    34,    -1,    36,    37,    -1,    -1,    -1,    -1,
      42,    43,    44,     3,    -1,     5,     6,     7,     8,    -1,
      -1,    -1,    -1,    -1,    -1,    15,    16,    17,    18,    -1,
      20,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    34,    -1,    36,     9,    10,    -1,
      -1,    -1,    42,    43,    44,    -1,    -1,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    45,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    -1,
      35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    -1,    35,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    -1,    35,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    45,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      -1,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    45,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    -1,    35,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    -1,    -1,    -1,    -1,    -1,    39,
      -1,    -1,    -1,    -1,    -1,    45,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    -1,    -1,    -1,    -1,    -1,    39,    -1,    -1,    -1,
      -1,    -1,    45,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    -1,    -1,
      -1,    -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,    45,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    45,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      -1,    33,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    45,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    -1,    -1,    30,    31,
      45,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    45
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    52,    53,     0,     3,     5,     6,     7,     8,    11,
      15,    16,    17,    18,    20,    34,    36,    40,    42,    43,
      44,    56,    57,    63,    64,     7,    54,    55,    34,    34,
      14,    34,    34,     7,    63,    63,    61,     7,     7,    39,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    45,    13,     4,    38,    63,
       7,    63,    63,    66,    63,    34,    35,    11,    37,    57,
      41,    58,    34,    63,    63,    63,    63,    63,    63,    63,
      63,    63,    63,    63,    63,    63,    63,    63,     7,    63,
      63,    55,    35,     4,    35,    38,    35,     7,    62,     7,
      39,     7,    36,    66,    14,    63,    63,    63,    63,    35,
      38,    34,    59,    35,    63,    35,     9,    10,    65,    12,
      36,     7,    62,     7,    11,    37,    60,    63,    63,    34,
      63,    61,    35,    13,    34,     7,    63,    37,    63,    62,
      34,    35,    39,    35,    62,    63,    12,    36,    35,    65,
      63,    61,    12,    36,    39,    37,    63,    61,    39,    37
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    51,    52,    53,    53,    53,    54,    54,    55,    56,
      56,    57,    57,    58,    58,    59,    59,    60,    60,    60,
      60,    60,    61,    61,    62,    62,    62,    63,    63,    63,
      63,    63,    63,    63,    63,    63,    63,    63,    63,    63,
      63,    63,    63,    63,    63,    63,    63,    63,    63,    63,
      63,    63,    63,    63,    63,    63,    63,    63,    63,    63,
      63,    64,    65,    65,    66,    66,    66
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     0,     3,     2,     1,     3,     3,     6,
       8,     1,     7,     0,     2,     0,     2,     4,     8,     7,
       8,     7,     0,     3,     0,     1,     3,     1,     1,     1,
       1,     3,     4,     1,     2,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     4,     3,     5,     1,     7,     5,     1,     1,     3,
       5,     6,     2,     6,     0,     1,     3
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
#line 105 "src/Parser/parser.y"
            { rootAST = (yyvsp[0].prog); }
#line 1345 "src/Parser/parser.tab.cpp"
    break;

  case 3: /* program: %empty  */
#line 109 "src/Parser/parser.y"
                            { (yyval.prog) = new Program(); }
#line 1351 "src/Parser/parser.tab.cpp"
    break;

  case 4: /* program: program stmt SEMICOLON  */
#line 111 "src/Parser/parser.y"
                           {
        (yyvsp[-2].prog)->stmts.emplace_back(StmtPtr((yyvsp[-1].stmt)));
        (yyval.prog) = (yyvsp[-2].prog);
    }
#line 1360 "src/Parser/parser.tab.cpp"
    break;

  case 5: /* program: program decl  */
#line 116 "src/Parser/parser.y"
                 {
        (yyvsp[-1].prog)->stmts.emplace_back(StmtPtr((yyvsp[0].stmt)));
        (yyval.prog) = (yyvsp[-1].prog);
    }
#line 1369 "src/Parser/parser.tab.cpp"
    break;

  case 6: /* binding_list: binding  */
#line 123 "src/Parser/parser.y"
            {
          (yyval.bindings) = new std::vector<std::pair<std::string, Expr*>>();
          (yyval.bindings)->push_back(*(yyvsp[0].binding));
          delete (yyvsp[0].binding);
      }
#line 1379 "src/Parser/parser.tab.cpp"
    break;

  case 7: /* binding_list: binding_list COMMA binding  */
#line 128 "src/Parser/parser.y"
                                 {
          (yyvsp[-2].bindings)->push_back(*(yyvsp[0].binding));
          delete (yyvsp[0].binding);
          (yyval.bindings) = (yyvsp[-2].bindings);
      }
#line 1389 "src/Parser/parser.tab.cpp"
    break;

  case 8: /* binding: IDENT ASSIGN expr  */
#line 136 "src/Parser/parser.y"
                        {
          (yyval.binding) = new std::pair<std::string, Expr*>(std::string((yyvsp[-2].str)), (yyvsp[0].expr));
          free((yyvsp[-2].str));
      }
#line 1398 "src/Parser/parser.tab.cpp"
    break;

  case 9: /* decl: TYPE IDENT opt_inherits LBRACE class_body RBRACE  */
#line 144 "src/Parser/parser.y"
                                                     {
        ClassBody* cb = (yyvsp[-1].class_body);
        (yyval.stmt) = new ClassDecl(
                 std::string((yyvsp[-4].str)),
                 std::string((yyvsp[-3].str)),
                 std::move(cb->attributes),
                 std::move(cb->methods)
             );
        delete cb;
        free((yyvsp[-4].str));
        free((yyvsp[-3].str));
    }
#line 1415 "src/Parser/parser.tab.cpp"
    break;

  case 10: /* decl: FUNCTION IDENT LPAREN ident_list RPAREN LBRACE stmt_list RBRACE  */
#line 158 "src/Parser/parser.y"
                                                                    {
        auto args  = std::move(*(yyvsp[-4].str_list));  delete (yyvsp[-4].str_list);
        auto block = std::make_unique<Program>();
        block->stmts = std::move(*(yyvsp[-1].stmts)); delete (yyvsp[-1].stmts);

        (yyval.stmt) = new FunctionDecl(
                 std::string((yyvsp[-6].str)),
                 std::move(args),
                 std::move(block)
             );
        free((yyvsp[-6].str));
    }
#line 1432 "src/Parser/parser.tab.cpp"
    break;

  case 11: /* stmt: expr  */
#line 175 "src/Parser/parser.y"
    {
      (yyval.stmt) = new ExprStmt( ExprPtr((yyvsp[0].expr)) );
    }
#line 1440 "src/Parser/parser.tab.cpp"
    break;

  case 12: /* stmt: FUNCTION IDENT LPAREN ident_list RPAREN ARROW expr  */
#line 179 "src/Parser/parser.y"
                                                        {
          std::vector<std::string> args = std::move(*(yyvsp[-3].str_list));
          delete (yyvsp[-3].str_list);

          (yyval.stmt) = new FunctionDecl(std::string((yyvsp[-5].str)), std::move(args), StmtPtr(new ExprStmt(ExprPtr((yyvsp[0].expr)))));
          free((yyvsp[-5].str));
      }
#line 1452 "src/Parser/parser.tab.cpp"
    break;

  case 13: /* opt_inherits: %empty  */
#line 190 "src/Parser/parser.y"
                              { (yyval.str) = strdup("Object"); }
#line 1458 "src/Parser/parser.tab.cpp"
    break;

  case 14: /* opt_inherits: INHERITS IDENT  */
#line 191 "src/Parser/parser.y"
                            { (yyval.str) = (yyvsp[0].str); }
#line 1464 "src/Parser/parser.tab.cpp"
    break;

  case 15: /* class_body: %empty  */
#line 195 "src/Parser/parser.y"
                   {
          (yyval.class_body) = new ClassBody();
      }
#line 1472 "src/Parser/parser.tab.cpp"
    break;

  case 16: /* class_body: class_body member_def  */
#line 198 "src/Parser/parser.y"
                            {
          ClassBody* cb = (yyvsp[-1].class_body);
          if ((yyvsp[0].member_def)->isAttribute) {
              cb->attributes.push_back(std::move((yyvsp[0].member_def)->attr));
          } else {
              cb->methods.push_back(std::move((yyvsp[0].member_def)->method));
          }
          delete (yyvsp[0].member_def);
          (yyval.class_body) = cb;
    }
#line 1487 "src/Parser/parser.tab.cpp"
    break;

  case 17: /* member_def: IDENT ASSIGN expr SEMICOLON  */
#line 211 "src/Parser/parser.y"
                                               {
        (yyval.member_def) = new MemberDef{
            true,
            { std::string((yyvsp[-3].str)), ExprPtr((yyvsp[-1].expr)) },      // <<< ExprPtr
            nullptr
        };
        free((yyvsp[-3].str));
    }
#line 1500 "src/Parser/parser.tab.cpp"
    break;

  case 18: /* member_def: FUNCTION IDENT LPAREN ident_list RPAREN LBRACE stmt_list RBRACE  */
#line 221 "src/Parser/parser.y"
                                                                    {
        /* posiciones
            1: FUNCTION
            2: IDENT
            3: LPAREN
            4: ident_list   <<< argumentos
            5: RPAREN
            6: LBRACE
            7: stmt_list    <<< cuerpo
            8: RBRACE
        */
        auto args = std::move(*(yyvsp[-4].str_list)); delete (yyvsp[-4].str_list);     // <<< $4, no $5
        auto block = std::make_unique<Program>();
        block->stmts = std::move(*(yyvsp[-1].stmts)); delete (yyvsp[-1].stmts);  // <<< $7, no $8

        FunctionDecl* fn = new FunctionDecl(
                               (yyvsp[-6].str),
                               std::move(args),
                               std::move(block));
        (yyval.member_def) = new MemberDef{ false, {}, StmtPtr(fn) };
        free((yyvsp[-6].str));
    }
#line 1527 "src/Parser/parser.tab.cpp"
    break;

  case 19: /* member_def: IDENT LPAREN ident_list RPAREN LBRACE stmt_list RBRACE  */
#line 245 "src/Parser/parser.y"
                                                           {
        auto args  = std::move(*(yyvsp[-4].str_list)); delete (yyvsp[-4].str_list);
        auto block = std::make_unique<Program>();
        block->stmts = std::move(*(yyvsp[-1].stmts)); delete (yyvsp[-1].stmts);

        FunctionDecl* fn = new FunctionDecl(
                               (yyvsp[-6].str),
                               std::move(args),
                               std::move(block));
        (yyval.member_def) = new MemberDef{ false, {}, StmtPtr(fn) };
        free((yyvsp[-6].str));
    }
#line 1544 "src/Parser/parser.tab.cpp"
    break;

  case 20: /* member_def: FUNCTION IDENT LPAREN ident_list RPAREN ARROW expr SEMICOLON  */
#line 259 "src/Parser/parser.y"
                                                                 {
        /* posiciones
            1: FUNCTION
            2: IDENT
            3: LPAREN
            4: ident_list   <<< argumentos
            5: RPAREN
            6: ARROW
            7: expr         <<< cuerpo
        */
        auto args = std::move(*(yyvsp[-4].str_list)); delete (yyvsp[-4].str_list);     // <<< $4, no $5
        ExprStmt* body = new ExprStmt(ExprPtr((yyvsp[-1].expr)));

        FunctionDecl* fn = new FunctionDecl(
                               (yyvsp[-6].str),
                               std::move(args),
                               StmtPtr(body));
        (yyval.member_def) = new MemberDef{ false, {}, StmtPtr(fn) };
        free((yyvsp[-6].str));
    }
#line 1569 "src/Parser/parser.tab.cpp"
    break;

  case 21: /* member_def: IDENT LPAREN ident_list RPAREN ARROW expr SEMICOLON  */
#line 281 "src/Parser/parser.y"
                                                        {
        auto argsVec = std::move(*(yyvsp[-4].str_list)); delete (yyvsp[-4].str_list);
        ExprStmt* body = new ExprStmt(ExprPtr((yyvsp[-1].expr)));

        FunctionDecl* fn = new FunctionDecl(
                               (yyvsp[-6].str),
                               std::move(argsVec),
                               StmtPtr(body));
        (yyval.member_def) = new MemberDef{ false, {}, StmtPtr(fn) };
        free((yyvsp[-6].str));
    }
#line 1585 "src/Parser/parser.tab.cpp"
    break;

  case 22: /* stmt_list: %empty  */
#line 296 "src/Parser/parser.y"
                   { (yyval.stmts) = new std::vector<StmtPtr>(); }
#line 1591 "src/Parser/parser.tab.cpp"
    break;

  case 23: /* stmt_list: stmt_list stmt SEMICOLON  */
#line 297 "src/Parser/parser.y"
                               {
        (yyvsp[-2].stmts)->emplace_back(StmtPtr((yyvsp[-1].stmt)));
        (yyval.stmts) = (yyvsp[-2].stmts);
    }
#line 1600 "src/Parser/parser.tab.cpp"
    break;

  case 24: /* ident_list: %empty  */
#line 304 "src/Parser/parser.y"
                   { (yyval.str_list) = new std::vector<std::string>(); }
#line 1606 "src/Parser/parser.tab.cpp"
    break;

  case 25: /* ident_list: IDENT  */
#line 305 "src/Parser/parser.y"
            { (yyval.str_list) = new std::vector<std::string>(); (yyval.str_list)->push_back((yyvsp[0].str)); free((yyvsp[0].str)); }
#line 1612 "src/Parser/parser.tab.cpp"
    break;

  case 26: /* ident_list: ident_list COMMA IDENT  */
#line 306 "src/Parser/parser.y"
                             { (yyvsp[-2].str_list)->push_back((yyvsp[0].str)); free((yyvsp[0].str)); (yyval.str_list) = (yyvsp[-2].str_list); }
#line 1618 "src/Parser/parser.tab.cpp"
    break;

  case 27: /* expr: NUMBER  */
#line 310 "src/Parser/parser.y"
                        { (yyval.expr) = (yyvsp[0].expr); }
#line 1624 "src/Parser/parser.tab.cpp"
    break;

  case 28: /* expr: STRING  */
#line 311 "src/Parser/parser.y"
                        { (yyval.expr) = (yyvsp[0].expr); }
#line 1630 "src/Parser/parser.tab.cpp"
    break;

  case 29: /* expr: TRUE  */
#line 312 "src/Parser/parser.y"
                        { (yyval.expr) = (yyvsp[0].expr); }
#line 1636 "src/Parser/parser.tab.cpp"
    break;

  case 30: /* expr: FALSE  */
#line 313 "src/Parser/parser.y"
                        { (yyval.expr) = (yyvsp[0].expr); }
#line 1642 "src/Parser/parser.tab.cpp"
    break;

  case 31: /* expr: LBRACE stmt_list RBRACE  */
#line 315 "src/Parser/parser.y"
                              {
          (yyval.expr) = new ExprBlock(std::move(*(yyvsp[-1].stmts)));
          delete (yyvsp[-1].stmts);
      }
#line 1651 "src/Parser/parser.tab.cpp"
    break;

  case 32: /* expr: IDENT LPAREN argument_list RPAREN  */
#line 320 "src/Parser/parser.y"
                                        {
          (yyval.expr) = new CallExpr(std::string((yyvsp[-3].str)), std::move(*(yyvsp[-1].expr_list)));
          delete (yyvsp[-1].expr_list);
          free((yyvsp[-3].str));
      }
#line 1661 "src/Parser/parser.tab.cpp"
    break;

  case 33: /* expr: IDENT  */
#line 326 "src/Parser/parser.y"
            {
          (yyval.expr) = new VariableExpr(std::string((yyvsp[0].str)));
          free((yyvsp[0].str));
      }
#line 1670 "src/Parser/parser.tab.cpp"
    break;

  case 34: /* expr: MINUS expr  */
#line 331 "src/Parser/parser.y"
                              {
          (yyval.expr) = new UnaryExpr(UnaryExpr::OP_NEG, ExprPtr((yyvsp[0].expr)));
      }
#line 1678 "src/Parser/parser.tab.cpp"
    break;

  case 35: /* expr: expr POW expr  */
#line 335 "src/Parser/parser.y"
                    {
          (yyval.expr) = new BinaryExpr(BinaryExpr::OP_POW, ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
      }
#line 1686 "src/Parser/parser.tab.cpp"
    break;

  case 36: /* expr: expr MULT expr  */
#line 339 "src/Parser/parser.y"
                     {
          (yyval.expr) = new BinaryExpr(BinaryExpr::OP_MUL, ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
      }
#line 1694 "src/Parser/parser.tab.cpp"
    break;

  case 37: /* expr: expr DIV expr  */
#line 343 "src/Parser/parser.y"
                    {
          (yyval.expr) = new BinaryExpr(BinaryExpr::OP_DIV, ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
      }
#line 1702 "src/Parser/parser.tab.cpp"
    break;

  case 38: /* expr: expr MOD expr  */
#line 347 "src/Parser/parser.y"
                               {
          (yyval.expr) = new BinaryExpr(BinaryExpr::OP_MOD, ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
      }
#line 1710 "src/Parser/parser.tab.cpp"
    break;

  case 39: /* expr: expr PLUS expr  */
#line 351 "src/Parser/parser.y"
                     {
          (yyval.expr) = new BinaryExpr(BinaryExpr::OP_ADD, ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
      }
#line 1718 "src/Parser/parser.tab.cpp"
    break;

  case 40: /* expr: expr MINUS expr  */
#line 355 "src/Parser/parser.y"
                      {
          (yyval.expr) = new BinaryExpr(BinaryExpr::OP_SUB, ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
      }
#line 1726 "src/Parser/parser.tab.cpp"
    break;

  case 41: /* expr: expr LESS_THAN expr  */
#line 359 "src/Parser/parser.y"
                          {
          (yyval.expr) = new BinaryExpr(BinaryExpr::OP_LT, ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
      }
#line 1734 "src/Parser/parser.tab.cpp"
    break;

  case 42: /* expr: expr GREATER_THAN expr  */
#line 363 "src/Parser/parser.y"
                             {
          (yyval.expr) = new BinaryExpr(BinaryExpr::OP_GT, ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
      }
#line 1742 "src/Parser/parser.tab.cpp"
    break;

  case 43: /* expr: expr LE expr  */
#line 367 "src/Parser/parser.y"
                   {
          (yyval.expr) = new BinaryExpr(BinaryExpr::OP_LE, ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
      }
#line 1750 "src/Parser/parser.tab.cpp"
    break;

  case 44: /* expr: expr GE expr  */
#line 371 "src/Parser/parser.y"
                   {
          (yyval.expr) = new BinaryExpr(BinaryExpr::OP_GE, ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
      }
#line 1758 "src/Parser/parser.tab.cpp"
    break;

  case 45: /* expr: expr EQ expr  */
#line 375 "src/Parser/parser.y"
                   {
          (yyval.expr) = new BinaryExpr(BinaryExpr::OP_EQ, ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
      }
#line 1766 "src/Parser/parser.tab.cpp"
    break;

  case 46: /* expr: expr NEQ expr  */
#line 379 "src/Parser/parser.y"
                    {
          (yyval.expr) = new BinaryExpr(BinaryExpr::OP_NEQ, ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
      }
#line 1774 "src/Parser/parser.tab.cpp"
    break;

  case 47: /* expr: expr AND expr  */
#line 383 "src/Parser/parser.y"
                    {
          (yyval.expr) = new BinaryExpr(BinaryExpr::OP_AND, ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
      }
#line 1782 "src/Parser/parser.tab.cpp"
    break;

  case 48: /* expr: expr OR expr  */
#line 387 "src/Parser/parser.y"
                   {
          (yyval.expr) = new BinaryExpr(BinaryExpr::OP_OR, ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
      }
#line 1790 "src/Parser/parser.tab.cpp"
    break;

  case 49: /* expr: expr CONCAT expr  */
#line 391 "src/Parser/parser.y"
                       {
        (yyval.expr) = new BinaryExpr(BinaryExpr::OP_CONCAT, ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
    }
#line 1798 "src/Parser/parser.tab.cpp"
    break;

  case 50: /* expr: LPAREN expr RPAREN  */
#line 395 "src/Parser/parser.y"
                         {
          (yyval.expr) = (yyvsp[-1].expr);
      }
#line 1806 "src/Parser/parser.tab.cpp"
    break;

  case 51: /* expr: LET binding_list IN expr  */
#line 399 "src/Parser/parser.y"
                                                 {
          Expr* result = (yyvsp[0].expr);
          auto& list = *(yyvsp[-2].bindings);

          for (auto it = list.rbegin(); it != list.rend(); ++it) {
              result = new LetExpr(it->first, ExprPtr(it->second), std::make_unique<ExprStmt>(ExprPtr(result)));
          }

          delete (yyvsp[-2].bindings);
          (yyval.expr) = result;      
        }
#line 1822 "src/Parser/parser.tab.cpp"
    break;

  case 52: /* expr: IDENT ASSIGN_DESTRUCT expr  */
#line 411 "src/Parser/parser.y"
                                 {
          (yyval.expr) = new AssignExpr(std::string((yyvsp[-2].str)), ExprPtr((yyvsp[0].expr)));
          free((yyvsp[-2].str));
      }
#line 1831 "src/Parser/parser.tab.cpp"
    break;

  case 53: /* expr: WHILE LPAREN expr RPAREN expr  */
#line 415 "src/Parser/parser.y"
                                                     {
      (yyval.expr) = new WhileExpr(ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
    }
#line 1839 "src/Parser/parser.tab.cpp"
    break;

  case 55: /* expr: FOR LPAREN IDENT IN expr RPAREN expr  */
#line 420 "src/Parser/parser.y"
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
#line 1879 "src/Parser/parser.tab.cpp"
    break;

  case 56: /* expr: NEW IDENT LPAREN argument_list RPAREN  */
#line 456 "src/Parser/parser.y"
                                            {
        (yyval.expr) = new NewExpr(std::string((yyvsp[-3].str)), std::move(*(yyvsp[-1].expr_list)));
        free((yyvsp[-3].str)); delete (yyvsp[-1].expr_list);
    }
#line 1888 "src/Parser/parser.tab.cpp"
    break;

  case 57: /* expr: SELF  */
#line 460 "src/Parser/parser.y"
           {
        (yyval.expr) = new SelfExpr();
    }
#line 1896 "src/Parser/parser.tab.cpp"
    break;

  case 58: /* expr: BASE  */
#line 463 "src/Parser/parser.y"
           {
        (yyval.expr) = new BaseExpr();
    }
#line 1904 "src/Parser/parser.tab.cpp"
    break;

  case 59: /* expr: expr DOT IDENT  */
#line 466 "src/Parser/parser.y"
                     {
        (yyval.expr) = new MemberAccessExpr( ExprPtr((yyvsp[-2].expr)), std::string((yyvsp[0].str)) );
        free((yyvsp[0].str));
    }
#line 1913 "src/Parser/parser.tab.cpp"
    break;

  case 60: /* expr: expr DOT IDENT ASSIGN_DESTRUCT expr  */
#line 470 "src/Parser/parser.y"
                                            {
        // ATAJO: expr := MemberAssignExpr(object, member, value)
        (yyval.expr) = new MemberAssignExpr(
                 ExprPtr((yyvsp[-4].expr)),
                 std::string((yyvsp[-2].str)),
                 ExprPtr((yyvsp[0].expr))
             );
        free((yyvsp[-2].str));
    }
#line 1927 "src/Parser/parser.tab.cpp"
    break;

  case 61: /* if_expr: IF LPAREN expr RPAREN expr elif_list  */
#line 482 "src/Parser/parser.y"
                                         {
        (yyval.expr) = new IfExpr(ExprPtr((yyvsp[-3].expr)), ExprPtr((yyvsp[-1].expr)), ExprPtr((yyvsp[0].expr)));
    }
#line 1935 "src/Parser/parser.tab.cpp"
    break;

  case 62: /* elif_list: ELSE expr  */
#line 488 "src/Parser/parser.y"
                                 {
        (yyval.expr) = (yyvsp[0].expr);
    }
#line 1943 "src/Parser/parser.tab.cpp"
    break;

  case 63: /* elif_list: ELIF LPAREN expr RPAREN expr elif_list  */
#line 491 "src/Parser/parser.y"
                                             {
        (yyval.expr) = new IfExpr(ExprPtr((yyvsp[-3].expr)), ExprPtr((yyvsp[-1].expr)), ExprPtr((yyvsp[0].expr)));
    }
#line 1951 "src/Parser/parser.tab.cpp"
    break;

  case 64: /* argument_list: %empty  */
#line 497 "src/Parser/parser.y"
                   { (yyval.expr_list) = new std::vector<ExprPtr>(); }
#line 1957 "src/Parser/parser.tab.cpp"
    break;

  case 65: /* argument_list: expr  */
#line 498 "src/Parser/parser.y"
           {
          (yyval.expr_list) = new std::vector<ExprPtr>();
          (yyval.expr_list)->emplace_back(ExprPtr((yyvsp[0].expr)));
      }
#line 1966 "src/Parser/parser.tab.cpp"
    break;

  case 66: /* argument_list: argument_list COMMA expr  */
#line 502 "src/Parser/parser.y"
                               {
          (yyvsp[-2].expr_list)->emplace_back(ExprPtr((yyvsp[0].expr)));
          (yyval.expr_list) = (yyvsp[-2].expr_list);
      }
#line 1975 "src/Parser/parser.tab.cpp"
    break;


#line 1979 "src/Parser/parser.tab.cpp"

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

#line 509 "src/Parser/parser.y"


void yyerror(const char* s) {
    std::cerr << "Parse error: " << s << " at line " << yylineno << std::endl;
}
