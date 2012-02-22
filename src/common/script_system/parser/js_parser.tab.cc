/* A Bison parser, made by GNU Bison 2.5.  */

/* Skeleton implementation for Bison LALR(1) parsers in C++
   
      Copyright (C) 2002-2011 Free Software Foundation, Inc.
   
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


/* First part of user declarations.  */


/* Line 293 of lalr1.cc  */
#line 39 "js_parser.tab.cc"


#include "js_parser.tab.hh"

/* User implementation prologue.  */

/* Line 299 of lalr1.cc  */
#line 45 "js_parser.yy"

	extern int yylex(yy::JSParser::semantic_type *yylval,yy::JSParser::location_type *yylloc);	


/* Line 299 of lalr1.cc  */
#line 53 "js_parser.tab.cc"

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* FIXME: INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                               \
 do                                                                    \
   if (N)                                                              \
     {                                                                 \
       (Current).begin = YYRHSLOC (Rhs, 1).begin;                      \
       (Current).end   = YYRHSLOC (Rhs, N).end;                        \
     }                                                                 \
   else                                                                \
     {                                                                 \
       (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;        \
     }                                                                 \
 while (false)
#endif

/* Suppress unused-variable warnings by "using" E.  */
#define YYUSE(e) ((void) (e))

/* Enable debugging if requested.  */
#if YYDEBUG

/* A pseudo ostream that takes yydebug_ into account.  */
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)	\
do {							\
  if (yydebug_)						\
    {							\
      *yycdebug_ << Title << ' ';			\
      yy_symbol_print_ ((Type), (Value), (Location));	\
      *yycdebug_ << std::endl;				\
    }							\
} while (false)

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug_)				\
    yy_reduce_print_ (Rule);		\
} while (false)

# define YY_STACK_PRINT()		\
do {					\
  if (yydebug_)				\
    yystack_print_ ();			\
} while (false)

#else /* !YYDEBUG */

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_REDUCE_PRINT(Rule)
# define YY_STACK_PRINT()

#endif /* !YYDEBUG */

#define yyerrok		(yyerrstatus_ = 0)
#define yyclearin	(yychar = yyempty_)

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)


namespace yy {

/* Line 382 of lalr1.cc  */
#line 139 "js_parser.tab.cc"

  /// Build a parser object.
  JSParser::JSParser ()
#if YYDEBUG
     :yydebug_ (false),
      yycdebug_ (&std::cerr)
#endif
  {
  }

  JSParser::~JSParser ()
  {
  }

#if YYDEBUG
  /*--------------------------------.
  | Print this symbol on YYOUTPUT.  |
  `--------------------------------*/

  inline void
  JSParser::yy_symbol_value_print_ (int yytype,
			   const semantic_type* yyvaluep, const location_type* yylocationp)
  {
    YYUSE (yylocationp);
    YYUSE (yyvaluep);
    switch (yytype)
      {
         default:
	  break;
      }
  }


  void
  JSParser::yy_symbol_print_ (int yytype,
			   const semantic_type* yyvaluep, const location_type* yylocationp)
  {
    *yycdebug_ << (yytype < yyntokens_ ? "token" : "nterm")
	       << ' ' << yytname_[yytype] << " ("
	       << *yylocationp << ": ";
    yy_symbol_value_print_ (yytype, yyvaluep, yylocationp);
    *yycdebug_ << ')';
  }
#endif

  void
  JSParser::yydestruct_ (const char* yymsg,
			   int yytype, semantic_type* yyvaluep, location_type* yylocationp)
  {
    YYUSE (yylocationp);
    YYUSE (yymsg);
    YYUSE (yyvaluep);

    YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

    switch (yytype)
      {
  
	default:
	  break;
      }
  }

  void
  JSParser::yypop_ (unsigned int n)
  {
    yystate_stack_.pop (n);
    yysemantic_stack_.pop (n);
    yylocation_stack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  JSParser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  JSParser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  JSParser::debug_level_type
  JSParser::debug_level () const
  {
    return yydebug_;
  }

  void
  JSParser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif

  inline bool
  JSParser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  inline bool
  JSParser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  JSParser::parse ()
  {
    /// Lookahead and lookahead in internal form.
    int yychar = yyempty_;
    int yytoken = 0;

    /* State.  */
    int yyn;
    int yylen = 0;
    int yystate = 0;

    /* Error handling.  */
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// Semantic value of the lookahead.
    semantic_type yylval;
    /// Location of the lookahead.
    location_type yylloc;
    /// The locations where the error started and ended.
    location_type yyerror_range[3];

    /// $$.
    semantic_type yyval;
    /// @$.
    location_type yyloc;

    int yyresult;

    YYCDEBUG << "Starting parse" << std::endl;


    /* Initialize the stacks.  The initial state will be pushed in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystate_stack_ = state_stack_type (0);
    yysemantic_stack_ = semantic_stack_type (0);
    yylocation_stack_ = location_stack_type (0);
    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yylloc);

    /* New state.  */
  yynewstate:
    yystate_stack_.push (yystate);
    YYCDEBUG << "Entering state " << yystate << std::endl;

    /* Accept?  */
    if (yystate == yyfinal_)
      goto yyacceptlab;

    goto yybackup;

    /* Backup.  */
  yybackup:

    /* Try to take a decision without lookahead.  */
    yyn = yypact_[yystate];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    /* Read a lookahead token.  */
    if (yychar == yyempty_)
      {
	YYCDEBUG << "Reading a token: ";
	yychar = yylex (&yylval, &yylloc);
      }


    /* Convert token to internal form.  */
    if (yychar <= yyeof_)
      {
	yychar = yytoken = yyeof_;
	YYCDEBUG << "Now at end of input." << std::endl;
      }
    else
      {
	yytoken = yytranslate_ (yychar);
	YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
      }

    /* If the proper action on seeing token YYTOKEN is to reduce or to
       detect an error, take that action.  */
    yyn += yytoken;
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yytoken)
      goto yydefault;

    /* Reduce or error.  */
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
	if (yy_table_value_is_error_ (yyn))
	  goto yyerrlab;
	yyn = -yyn;
	goto yyreduce;
      }

    /* Shift the lookahead token.  */
    YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

    /* Discard the token being shifted.  */
    yychar = yyempty_;

    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yylloc);

    /* Count tokens shifted since error; after three, turn off error
       status.  */
    if (yyerrstatus_)
      --yyerrstatus_;

    yystate = yyn;
    goto yynewstate;

  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystate];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;

  /*-----------------------------.
  | yyreduce -- Do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    /* If YYLEN is nonzero, implement the default value of the action:
       `$$ = $1'.  Otherwise, use the top of the stack.

       Otherwise, the following line sets YYVAL to garbage.
       This behavior is undocumented and Bison
       users should not rely upon it.  */
    if (yylen)
      yyval = yysemantic_stack_[yylen - 1];
    else
      yyval = yysemantic_stack_[0];

    {
      slice<location_type, location_stack_type> slice (yylocation_stack_, yylen);
      YYLLOC_DEFAULT (yyloc, slice, yylen);
    }
    YY_REDUCE_PRINT (yyn);
    switch (yyn)
      {
	

/* Line 690 of lalr1.cc  */
#line 400 "js_parser.tab.cc"
	default:
          break;
      }
    /* User semantic actions sometimes alter yychar, and that requires
       that yytoken be updated with the new translation.  We take the
       approach of translating immediately before every use of yytoken.
       One alternative is translating here after every semantic action,
       but that translation would be missed if the semantic action
       invokes YYABORT, YYACCEPT, or YYERROR immediately after altering
       yychar.  In the case of YYABORT or YYACCEPT, an incorrect
       destructor might then be invoked immediately.  In the case of
       YYERROR, subsequent parser actions might lead to an incorrect
       destructor call or verbose syntax error message before the
       lookahead is translated.  */
    YY_SYMBOL_PRINT ("-> $$ =", yyr1_[yyn], &yyval, &yyloc);

    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();

    yysemantic_stack_.push (yyval);
    yylocation_stack_.push (yyloc);

    /* Shift the result of the reduction.  */
    yyn = yyr1_[yyn];
    yystate = yypgoto_[yyn - yyntokens_] + yystate_stack_[0];
    if (0 <= yystate && yystate <= yylast_
	&& yycheck_[yystate] == yystate_stack_[0])
      yystate = yytable_[yystate];
    else
      yystate = yydefgoto_[yyn - yyntokens_];
    goto yynewstate;

  /*------------------------------------.
  | yyerrlab -- here on detecting error |
  `------------------------------------*/
  yyerrlab:
    /* Make sure we have latest lookahead translation.  See comments at
       user semantic actions for why this is necessary.  */
    yytoken = yytranslate_ (yychar);

    /* If not already recovering from an error, report this error.  */
    if (!yyerrstatus_)
      {
	++yynerrs_;
	if (yychar == yyempty_)
	  yytoken = yyempty_;
	error (yylloc, yysyntax_error_ (yystate, yytoken));
      }

    yyerror_range[1] = yylloc;
    if (yyerrstatus_ == 3)
      {
	/* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

	if (yychar <= yyeof_)
	  {
	  /* Return failure if at end of input.  */
	  if (yychar == yyeof_)
	    YYABORT;
	  }
	else
	  {
	    yydestruct_ ("Error: discarding", yytoken, &yylval, &yylloc);
	    yychar = yyempty_;
	  }
      }

    /* Else will try to reuse lookahead token after shifting the error
       token.  */
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:

    /* Pacify compilers like GCC when the user code never invokes
       YYERROR and the label yyerrorlab therefore never appears in user
       code.  */
    if (false)
      goto yyerrorlab;

    yyerror_range[1] = yylocation_stack_[yylen - 1];
    /* Do not reclaim the symbols of the rule which action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    yystate = yystate_stack_[0];
    goto yyerrlab1;

  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;	/* Each real token shifted decrements this.  */

    for (;;)
      {
	yyn = yypact_[yystate];
	if (!yy_pact_value_is_default_ (yyn))
	{
	  yyn += yyterror_;
	  if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
	    {
	      yyn = yytable_[yyn];
	      if (0 < yyn)
		break;
	    }
	}

	/* Pop the current state because it cannot handle the error token.  */
	if (yystate_stack_.height () == 1)
	YYABORT;

	yyerror_range[1] = yylocation_stack_[0];
	yydestruct_ ("Error: popping",
		     yystos_[yystate],
		     &yysemantic_stack_[0], &yylocation_stack_[0]);
	yypop_ ();
	yystate = yystate_stack_[0];
	YY_STACK_PRINT ();
      }

    yyerror_range[2] = yylloc;
    // Using YYLLOC is tempting, but would change the location of
    // the lookahead.  YYLOC is available though.
    YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yyloc);

    /* Shift the error token.  */
    YY_SYMBOL_PRINT ("Shifting", yystos_[yyn],
		     &yysemantic_stack_[0], &yylocation_stack_[0]);

    yystate = yyn;
    goto yynewstate;

    /* Accept.  */
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    /* Abort.  */
  yyabortlab:
    yyresult = 1;
    goto yyreturn;

  yyreturn:
    if (yychar != yyempty_)
      {
        /* Make sure we have latest lookahead translation.  See comments
           at user semantic actions for why this is necessary.  */
        yytoken = yytranslate_ (yychar);
        yydestruct_ ("Cleanup: discarding lookahead", yytoken, &yylval,
                     &yylloc);
      }

    /* Do not reclaim the symbols of the rule which action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (yystate_stack_.height () != 1)
      {
	yydestruct_ ("Cleanup: popping",
		   yystos_[yystate_stack_[0]],
		   &yysemantic_stack_[0],
		   &yylocation_stack_[0]);
	yypop_ ();
      }

    return yyresult;
  }

  // Generate an error message.
  std::string
  JSParser::yysyntax_error_ (int, int)
  {
    return YY_("syntax error");
  }


  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
  const signed char JSParser::yypact_ninf_ = -64;
  const signed char
  JSParser::yypact_[] =
  {
       -64,     6,    61,   -64,    -3,     7,   -64,   -64,   -64,   -64,
     -64,   -64,   -64,   -64,   -64,   -64,     7,   -64,    20,    20,
      13,    29,     1,   -64,    20,   106,    30,   -64,   121,   121,
       7,     7,   -64,     7,   -64,   -64,   121,   -64,     7,   -64,
     -64,   -64,   -64,   -64,     7,   -64,    20,    20,     7,   -64,
     -64,   -64,    23,   -64,   -64,   121,    23,   -64,   -64,    23,
     -64,   -64,     3,    20,   -64,    32,    37,    20,   -64,   -64,
     -64,   -64,    36,    43,     7,   -64,     7,   121,   121,     7,
       8,   -64,     7,    30,    23,   -64,   -64,   -64,    23,   -64,
     -64,   106,   -64,   -64,    36,    12,    46,   -64,   -64,   -64,
       7,   -64,     7,   106,    48,   -64,   -64,   121,   -64
  };

  /* YYDEFACT[S] -- default reduction number in state S.  Performed when
     YYTABLE doesn't specify something else to do.  Zero means the
     default is an error.  */
  const unsigned char
  JSParser::yydefact_[] =
  {
         3,     0,     0,     1,     0,     0,    42,    11,    41,    17,
      18,     4,     5,     3,     6,     8,     0,     7,     0,     0,
       0,    38,     0,     9,     0,     0,    19,    59,     0,     0,
       0,     0,    14,    22,    12,    10,     0,    15,    36,    61,
      43,    34,    33,    35,     0,    49,    26,    27,     0,    28,
      29,    30,     0,    31,    32,     0,     0,    40,    39,     0,
      24,    20,     0,     0,    37,     0,    48,     0,    60,    46,
      49,    47,     0,    23,     0,    13,    22,     0,     0,    51,
       0,     3,     0,    19,     0,    56,    50,    53,     0,    62,
      58,     0,    25,    16,     0,     0,    52,    44,    21,     3,
       0,    55,     0,     0,     0,    54,    45,     0,    57
  };

  /* YYPGOTO[NTERM-NUM].  */
  const signed char
  JSParser::yypgoto_[] =
  {
       -64,   -64,    -5,   -64,   -64,    -2,   -63,   -64,   -64,   -64,
     -64,   -29,   -64,   -21,   -64,   -33,   -64,    -1,     0,   -64,
       4,   -64,   -64,   -64,     2,    27,   -13,   -64,   -64,   -64,
     -64,   -64,   -14,   -45,   -64,   -64
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const signed char
  JSParser::yydefgoto_[] =
  {
        -1,     1,     2,    11,    12,    44,    35,    14,    15,    62,
      16,    37,    17,    59,    73,    45,    63,    46,    47,    20,
      21,    48,    49,    50,    51,    52,    66,    88,    96,    53,
      95,    54,    28,    69,    55,    90
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If YYTABLE_NINF_, syntax error.  */
  const signed char JSParser::yytable_ninf_ = -3;
  const signed char
  JSParser::yytable_[] =
  {
        13,    18,    19,    61,    22,    29,     3,    74,    25,    24,
      33,    71,    78,    23,    72,     6,   100,    32,    30,    75,
      26,    89,    70,    13,    18,    19,    34,    22,    98,    57,
      58,    27,   101,    29,    31,    77,    36,    60,    68,    94,
     106,    78,    64,    97,    85,    86,     7,    82,    65,    76,
     102,   107,    67,    79,    93,    84,    56,    80,     0,     0,
       0,    -2,     4,     0,     0,     0,     0,     0,     5,     6,
      81,     7,     0,     0,   108,     0,    91,     0,    83,     8,
      60,     9,    10,    87,     0,     0,    92,     0,     0,    13,
      18,    19,    99,    22,   103,     0,     0,     0,     0,     0,
       0,    13,    18,    19,   104,    22,   105,     4,     0,     0,
       0,     0,     0,     5,     6,     0,     7,     0,     0,     0,
      34,     0,     0,     0,     8,     0,     9,    10,    38,     6,
      39,     7,     0,    40,     0,     0,     0,     0,    41,     8,
      42,     0,     0,    43
  };

  /* YYCHECK.  */
  const signed char
  JSParser::yycheck_[] =
  {
         2,     2,     2,    36,     2,    19,     0,     4,    13,     5,
      24,    56,     4,    16,    59,     8,     4,    16,     5,    16,
      16,    13,    55,    25,    25,    25,    14,    25,    91,    30,
      31,    11,    95,    47,     5,     3,     6,    33,    15,    84,
     103,     4,    38,    88,    77,    78,    10,     4,    44,    63,
       4,     3,    48,    67,    83,    76,    29,    70,    -1,    -1,
      -1,     0,     1,    -1,    -1,    -1,    -1,    -1,     7,     8,
      72,    10,    -1,    -1,   107,    -1,    81,    -1,    74,    18,
      76,    20,    21,    79,    -1,    -1,    82,    -1,    -1,    91,
      91,    91,    94,    91,    99,    -1,    -1,    -1,    -1,    -1,
      -1,   103,   103,   103,   100,   103,   102,     1,    -1,    -1,
      -1,    -1,    -1,     7,     8,    -1,    10,    -1,    -1,    -1,
      14,    -1,    -1,    -1,    18,    -1,    20,    21,     7,     8,
       9,    10,    -1,    12,    -1,    -1,    -1,    -1,    17,    18,
      19,    -1,    -1,    22
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned char
  JSParser::yystos_[] =
  {
         0,    24,    25,     0,     1,     7,     8,    10,    18,    20,
      21,    26,    27,    28,    30,    31,    33,    35,    40,    41,
      42,    43,    47,    16,    43,    25,    43,    11,    55,    55,
       5,     5,    16,    55,    14,    29,     6,    34,     7,     9,
      12,    17,    19,    22,    28,    38,    40,    41,    44,    45,
      46,    47,    48,    52,    54,    57,    48,    40,    40,    36,
      43,    38,    32,    39,    43,    43,    49,    43,    15,    56,
      38,    56,    56,    37,     4,    16,    55,     3,     4,    55,
      49,    28,     4,    43,    36,    38,    38,    43,    50,    13,
      58,    25,    43,    34,    56,    53,    51,    56,    29,    28,
       4,    29,     4,    25,    43,    43,    29,     3,    38
  };

#if YYDEBUG
  /* TOKEN_NUMBER_[YYLEX-NUM] -- Internal symbol number corresponding
     to YYLEX-NUM.  */
  const unsigned short int
  JSParser::yytoken_number_[] =
  {
         0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277
  };
#endif

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
  const unsigned char
  JSParser::yyr1_[] =
  {
         0,    23,    24,    25,    25,    26,    26,    26,    26,    26,
      27,    28,    29,    30,    31,    32,    32,    33,    33,    34,
      34,    35,    36,    36,    37,    37,    38,    38,    38,    38,
      38,    38,    38,    38,    38,    38,    39,    39,    40,    40,
      41,    42,    43,    44,    45,    46,    47,    47,    48,    49,
      49,    50,    50,    51,    51,    52,    53,    53,    54,    55,
      56,    57,    58
  };

  /* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
  const unsigned char
  JSParser::yyr2_[] =
  {
         0,     2,     1,     0,     2,     1,     1,     1,     1,     2,
       3,     1,     1,     5,     2,     0,     4,     1,     1,     0,
       2,     8,     0,     2,     0,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     0,     1,     1,     3,
       3,     1,     1,     1,     5,     8,     4,     4,     2,     0,
       3,     0,     2,     0,     3,     6,     0,     5,     4,     1,
       1,     1,     1
  };

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
  /* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
     First, the terminals, then, starting at \a yyntokens_, nonterminals.  */
  const char*
  const JSParser::yytname_[] =
  {
    "$end", "error", "$undefined", "T_COLON", "T_COMMA", "T_DOT", "T_EQ",
  "T_FUNCTION", "T_IDENTIFIER", "T_LBRACE", "T_LBRACKET", "T_LPAREN",
  "T_NEW", "T_RBRACE", "T_RBRACKET", "T_RPAREN", "T_SEMICOLON",
  "T_STRING_LITERAL", "T_THIS", "T_NULL", "T_VAR", "T_CONST",
  "T_NUMERIC_LITERAL", "$accept", "Program", "StatementList", "Statement",
  "Block", "OpenBlock", "CloseBlock", "VariableDecl",
  "FunctionCallStatement", "OtherDeclOpt", "DeclTok", "InitOpt",
  "FunctionDecl", "OptParamList", "OtherOptParamList", "Expression",
  "OptIdTok", "MemberExpr", "ThisExpr", "ThisTok", "IdTok", "NewTok",
  "NewExpr", "FunctionExpr", "FunctionCall", "OptExpressionList",
  "OtherOptExpressionList", "OptArgumentList", "OtherOptArgumentList",
  "ObjConstExpr", "OtherOptFields", "ArrayExpr", "OpenBracket",
  "CloseBracket", "OpenArrBracket", "CloseArrBracket", 0
  };
#endif

#if YYDEBUG
  /* YYRHS -- A `-1'-separated list of the rules' RHS.  */
  const JSParser::rhs_number_type
  JSParser::yyrhs_[] =
  {
        24,     0,    -1,    25,    -1,    -1,    25,    26,    -1,    27,
      -1,    30,    -1,    35,    -1,    31,    -1,     1,    16,    -1,
      28,    25,    29,    -1,    10,    -1,    14,    -1,    33,    43,
      34,    32,    16,    -1,    47,    16,    -1,    -1,    32,     4,
      43,    34,    -1,    20,    -1,    21,    -1,    -1,     6,    38,
      -1,     7,    43,    55,    36,    56,    28,    25,    29,    -1,
      -1,    43,    37,    -1,    -1,    37,     4,    43,    -1,    40,
      -1,    41,    -1,    45,    -1,    46,    -1,    47,    -1,    52,
      -1,    54,    -1,    19,    -1,    17,    -1,    22,    -1,    -1,
      43,    -1,    43,    -1,    43,     5,    40,    -1,    42,     5,
      40,    -1,    18,    -1,     8,    -1,    12,    -1,    44,    43,
      55,    50,    56,    -1,     7,    39,    55,    36,    56,    28,
      25,    29,    -1,    40,    55,    48,    56,    -1,    41,    55,
      48,    56,    -1,    38,    49,    -1,    -1,    49,     4,    38,
      -1,    -1,    43,    51,    -1,    -1,    51,     4,    43,    -1,
      28,    43,     3,    38,    53,    29,    -1,    -1,    53,     4,
      43,     3,    38,    -1,    57,    38,    49,    58,    -1,    11,
      -1,    15,    -1,     9,    -1,    13,    -1
  };

  /* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
     YYRHS.  */
  const unsigned char
  JSParser::yyprhs_[] =
  {
         0,     0,     3,     5,     6,     9,    11,    13,    15,    17,
      20,    24,    26,    28,    34,    37,    38,    43,    45,    47,
      48,    51,    60,    61,    64,    65,    69,    71,    73,    75,
      77,    79,    81,    83,    85,    87,    89,    90,    92,    94,
      98,   102,   104,   106,   108,   114,   123,   128,   133,   136,
     137,   141,   142,   145,   146,   150,   157,   158,   164,   169,
     171,   173,   175
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned char
  JSParser::yyrline_[] =
  {
         0,    50,    50,    52,    53,    56,    57,    58,    59,    60,
      63,    65,    67,    69,    71,    73,    74,    77,    78,    81,
      82,    85,    87,    88,    91,    92,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   107,   108,   111,   112,
     115,   117,   119,   121,   123,   125,   127,   128,   133,   136,
     137,   141,   142,   145,   146,   149,   151,   152,   155,   157,
     159,   161,   163
  };

  // Print the state stack on the debug stream.
  void
  JSParser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (state_stack_type::const_iterator i = yystate_stack_.begin ();
	 i != yystate_stack_.end (); ++i)
      *yycdebug_ << ' ' << *i;
    *yycdebug_ << std::endl;
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  JSParser::yy_reduce_print_ (int yyrule)
  {
    unsigned int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    /* Print the symbols being reduced, and their result.  */
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
	       << " (line " << yylno << "):" << std::endl;
    /* The symbols being reduced.  */
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
		       yyrhs_[yyprhs_[yyrule] + yyi],
		       &(yysemantic_stack_[(yynrhs) - (yyi + 1)]),
		       &(yylocation_stack_[(yynrhs) - (yyi + 1)]));
  }
#endif // YYDEBUG

  /* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
  JSParser::token_number_type
  JSParser::yytranslate_ (int t)
  {
    static
    const token_number_type
    translate_table[] =
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
      15,    16,    17,    18,    19,    20,    21,    22
    };
    if ((unsigned int) t <= yyuser_token_number_max_)
      return translate_table[t];
    else
      return yyundef_token_;
  }

  const int JSParser::yyeof_ = 0;
  const int JSParser::yylast_ = 143;
  const int JSParser::yynnts_ = 36;
  const int JSParser::yyempty_ = -2;
  const int JSParser::yyfinal_ = 3;
  const int JSParser::yyterror_ = 1;
  const int JSParser::yyerrcode_ = 256;
  const int JSParser::yyntokens_ = 23;

  const unsigned int JSParser::yyuser_token_number_max_ = 277;
  const JSParser::token_number_type JSParser::yyundef_token_ = 2;


} // yy

/* Line 1136 of lalr1.cc  */
#line 905 "js_parser.tab.cc"


/* Line 1138 of lalr1.cc  */
#line 165 "js_parser.yy"


namespace yy 
{
	void JSParser::error(location const& loc, const std::string& s)
	{
		//std::cerr << "error at " << loc << ": " << s << "\n";
	}
}

