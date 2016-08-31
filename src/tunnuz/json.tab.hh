/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

#ifndef YY_YY_USERS_MILOYIP_GITHUB_NATIVEJSON_BENCHMARK_SRC_TUNNUZ_JSON_TAB_HH_INCLUDED
# define YY_YY_USERS_MILOYIP_GITHUB_NATIVEJSON_BENCHMARK_SRC_TUNNUZ_JSON_TAB_HH_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 25 "/Users/miloyip/github/nativejson-benchmark/thirdparty/tunnuz/json.y" /* yacc.c:1915  */
 #include "json_st.hh" 

#line 47 "/Users/miloyip/github/nativejson-benchmark/src/tunnuz/json.tab.hh" /* yacc.c:1915  */

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    COMMA = 258,
    COLON = 259,
    SQUARE_BRACKET_L = 260,
    SQUARE_BRACKET_R = 261,
    CURLY_BRACKET_L = 262,
    CURLY_BRACKET_R = 263,
    DOUBLE_QUOTED_STRING = 264,
    SINGLE_QUOTED_STRING = 265,
    NUMBER_I = 266,
    NUMBER_F = 267,
    BOOLEAN = 268,
    NULL_T = 269
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 28 "/Users/miloyip/github/nativejson-benchmark/thirdparty/tunnuz/json.y" /* yacc.c:1915  */

    // "Pure" types
    long long int int_v;
    long double float_v;
    bool bool_v;
    bool null_p;
    char* string_v;
    
    // Pointers to more complex classes
    JSON::Object* object_p;
    JSON::Array* array_p;
    JSON::Value* value_p;

#line 88 "/Users/miloyip/github/nativejson-benchmark/src/tunnuz/json.tab.hh" /* yacc.c:1915  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_USERS_MILOYIP_GITHUB_NATIVEJSON_BENCHMARK_SRC_TUNNUZ_JSON_TAB_HH_INCLUDED  */
