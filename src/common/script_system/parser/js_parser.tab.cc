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
#line 52 "js_parser.yy"

	extern int yylex(yy::JSCacheParser::semantic_type *yylval,yy::JSCacheParser::location_type *yylloc);	


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
  JSCacheParser::JSCacheParser (JSSymbolsCache& symb_yyarg, QList<JSVarDescriptor>& tmp_yyarg)
    :
#if YYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      symb (symb_yyarg),
      tmp (tmp_yyarg)
  {
  }

  JSCacheParser::~JSCacheParser ()
  {
  }

#if YYDEBUG
  /*--------------------------------.
  | Print this symbol on YYOUTPUT.  |
  `--------------------------------*/

  inline void
  JSCacheParser::yy_symbol_value_print_ (int yytype,
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
  JSCacheParser::yy_symbol_print_ (int yytype,
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
  JSCacheParser::yydestruct_ (const char* yymsg,
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
  JSCacheParser::yypop_ (unsigned int n)
  {
    yystate_stack_.pop (n);
    yysemantic_stack_.pop (n);
    yylocation_stack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  JSCacheParser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  JSCacheParser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  JSCacheParser::debug_level_type
  JSCacheParser::debug_level () const
  {
    return yydebug_;
  }

  void
  JSCacheParser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif

  inline bool
  JSCacheParser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  inline bool
  JSCacheParser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  JSCacheParser::parse ()
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
	  case 9:

/* Line 690 of lalr1.cc  */
#line 69 "js_parser.yy"
    {
				qDebug() << "Parsing Error Recovery!!!\n";
			}
    break;

  case 11:

/* Line 690 of lalr1.cc  */
#line 77 "js_parser.yy"
    {
				symb.pushFrame();
			}
    break;

  case 12:

/* Line 690 of lalr1.cc  */
#line 83 "js_parser.yy"
    {
				symb.popFrame();
			}
    break;

  case 13:

/* Line 690 of lalr1.cc  */
#line 89 "js_parser.yy"
    {
							JSVarDescriptor vd(*(yysemantic_stack_[(5) - (2)].s),(yysemantic_stack_[(5) - (3)].ftd));
							symb.currentFrame()[*(yysemantic_stack_[(5) - (2)].s)] = vd;
						}
    break;

  case 14:

/* Line 690 of lalr1.cc  */
#line 96 "js_parser.yy"
    {
							
						}
    break;

  case 15:

/* Line 690 of lalr1.cc  */
#line 100 "js_parser.yy"
    {
							
						}
    break;

  case 20:

/* Line 690 of lalr1.cc  */
#line 114 "js_parser.yy"
    {
					(yyval.ftd) = (yysemantic_stack_[(2) - (2)].ftd);
				}
    break;

  case 21:

/* Line 690 of lalr1.cc  */
#line 120 "js_parser.yy"
    {
				(yyval.ftd) = NULL;
			}
    break;

  case 24:

/* Line 690 of lalr1.cc  */
#line 129 "js_parser.yy"
    {
				JSFunctionTypeDescriptor ftd(*(yysemantic_stack_[(6) - (2)].s));
				symb.globalFunType()[*(yysemantic_stack_[(6) - (2)].s)] = ftd;
			}
    break;

  case 25:

/* Line 690 of lalr1.cc  */
#line 136 "js_parser.yy"
    {
					QString nm;
					if (*(yysemantic_stack_[(5) - (2)].s) == "")
						nm = QUuid::createUuid().toString();
					JSFunctionTypeDescriptor ftd(nm);
				}
    break;

  case 27:

/* Line 690 of lalr1.cc  */
#line 146 "js_parser.yy"
    {
						symb.currentFrame()[*(yysemantic_stack_[(2) - (1)].s)] = JSVarDescriptor(*(yysemantic_stack_[(2) - (1)].s),NULL);
				}
    break;

  case 29:

/* Line 690 of lalr1.cc  */
#line 153 "js_parser.yy"
    {
						symb.currentFrame()[*(yysemantic_stack_[(3) - (3)].s)] = JSVarDescriptor(*(yysemantic_stack_[(3) - (3)].s),NULL);
					}
    break;

  case 30:

/* Line 690 of lalr1.cc  */
#line 159 "js_parser.yy"
    {
				(yyval.ftd) = NULL;
			}
    break;

  case 31:

/* Line 690 of lalr1.cc  */
#line 163 "js_parser.yy"
    {
				(yyval.ftd) = NULL;
			}
    break;

  case 32:

/* Line 690 of lalr1.cc  */
#line 167 "js_parser.yy"
    {
			
				(yyval.ftd) = (yysemantic_stack_[(1) - (1)].ftd);
			}
    break;

  case 33:

/* Line 690 of lalr1.cc  */
#line 172 "js_parser.yy"
    {
				(yyval.ftd) = NULL;
			}
    break;

  case 34:

/* Line 690 of lalr1.cc  */
#line 176 "js_parser.yy"
    {
				(yyval.ftd) = NULL;
			}
    break;

  case 35:

/* Line 690 of lalr1.cc  */
#line 180 "js_parser.yy"
    {
				JSFunctionTypeDescriptor ftd(QUuid::createUuid().toString());
				for(int ii = 0;ii < tmp.size();++ii)
					ftd.varmember[tmp[ii].name] = tmp[ii];
				tmp.clear();
				symb.globalFunType()[ftd.name] = ftd;
				(yyval.ftd) = &symb.globalFunType()[ftd.name];
			}
    break;

  case 36:

/* Line 690 of lalr1.cc  */
#line 189 "js_parser.yy"
    {
				(yyval.ftd) = NULL;
			}
    break;

  case 37:

/* Line 690 of lalr1.cc  */
#line 193 "js_parser.yy"
    {
				(yyval.ftd) = NULL;
			}
    break;

  case 38:

/* Line 690 of lalr1.cc  */
#line 197 "js_parser.yy"
    {
				(yyval.ftd) = NULL;
			}
    break;

  case 39:

/* Line 690 of lalr1.cc  */
#line 201 "js_parser.yy"
    {
				(yyval.ftd) = NULL;
			}
    break;

  case 40:

/* Line 690 of lalr1.cc  */
#line 207 "js_parser.yy"
    {
				(yyval.s) = NULL;
			}
    break;

  case 48:

/* Line 690 of lalr1.cc  */
#line 226 "js_parser.yy"
    {
				(yyval.ftd) = symb.getFunctionType(*(yysemantic_stack_[(5) - (2)].s));
			}
    break;

  case 49:

/* Line 690 of lalr1.cc  */
#line 233 "js_parser.yy"
    {
					
					
				}
    break;

  case 60:

/* Line 690 of lalr1.cc  */
#line 263 "js_parser.yy"
    {
					JSVarDescriptor vd(*(yysemantic_stack_[(6) - (2)].s),(yysemantic_stack_[(6) - (4)].ftd));
					tmp.push_back(vd);
				}
    break;

  case 62:

/* Line 690 of lalr1.cc  */
#line 271 "js_parser.yy"
    {
					JSVarDescriptor vd(*(yysemantic_stack_[(5) - (3)].s),(yysemantic_stack_[(5) - (5)].ftd));
					tmp.push_back(vd);
				}
    break;



/* Line 690 of lalr1.cc  */
#line 660 "js_parser.tab.cc"
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
  JSCacheParser::yysyntax_error_ (int, int)
  {
    return YY_("syntax error");
  }


  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
  const signed char JSCacheParser::yypact_ninf_ = -46;
  const signed char
  JSCacheParser::yypact_[] =
  {
       -46,    17,    85,   -46,     5,    32,   -46,   -46,   -46,   -46,
     -46,   -46,   -46,   -46,   -46,   -46,    32,   -46,   -46,     8,
      37,    29,   -46,   -46,    40,    47,    37,    47,    32,   100,
      42,    43,    32,   -46,    32,   -46,   -46,   -46,   -46,   -46,
     -46,    32,   -46,   -46,   -46,   -46,   -46,     2,   -46,   -46,
      13,    40,    32,   -46,   -46,   -46,   -46,   -46,   100,   -46,
     -46,    48,    45,   -46,     3,    40,   -46,   -46,    60,    47,
     100,   100,    40,   -46,   -46,    54,    62,    32,   -46,    32,
     100,   -46,   -46,    45,    45,    32,    28,   -46,    32,    37,
      45,   -46,    67,   -46,   -46,   -46,    45,   100,   -46,   -46,
     -46,   -46,   -46,    11,    69,   -46,   -46,    32,   -46,    32,
      74,   -46,   100,   -46
  };

  /* YYDEFACT[S] -- default reduction number in state S.  Performed when
     YYTABLE doesn't specify something else to do.  Zero means the
     default is an error.  */
  const unsigned char
  JSCacheParser::yydefact_[] =
  {
         3,     0,     0,     1,     0,     0,    46,    11,    45,    18,
      19,     4,     5,     3,     7,     8,     0,     6,     3,     0,
       0,     0,    42,     9,     0,     0,    21,     0,     0,     0,
       0,     0,     0,    64,    26,    12,    10,    22,    16,    23,
      43,    40,    66,    47,    38,    37,    39,     0,     3,    20,
      30,    31,     0,    32,    33,    34,    35,    36,     0,    14,
      15,    44,     0,    28,     0,     0,    41,    59,     0,     0,
       0,     0,     0,    53,    65,     0,    27,     0,    13,    26,
       0,    49,    53,     0,     0,    55,     0,    24,     0,    21,
       0,    61,    52,    50,    51,    57,     0,     0,    67,    63,
      29,    17,    25,     0,    56,    48,    54,     0,    60,     0,
       0,    58,     0,    62
  };

  /* YYPGOTO[NTERM-NUM].  */
  const signed char
  JSCacheParser::yypgoto_[] =
  {
       -46,   -46,    -9,   -46,   -46,     1,   -25,   -46,   -46,   -46,
     -46,    26,    -8,   -46,   -46,   -46,     9,   -46,   -28,   -46,
       6,    10,   -46,    -5,   -46,   -46,   -46,   -46,    16,     0,
     -46,   -46,   -46,   -46,   -46,   -45,   -34,   -46,   -46
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const signed char
  JSCacheParser::yydefgoto_[] =
  {
        -1,     1,     2,    11,    12,    47,    36,    14,    15,    64,
      16,    37,    38,    17,    18,    48,    62,    76,    82,    65,
      50,    51,    21,    22,    52,    53,    54,    55,    83,    86,
      96,   104,    56,   103,    57,    34,    75,    58,    99
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If YYTABLE_NINF_, syntax error.  */
  const signed char JSCacheParser::yytable_ninf_ = -3;
  const signed char
  JSCacheParser::yytable_[] =
  {
        24,    49,    39,    13,    25,    70,    71,    77,    19,    27,
       6,    26,    20,    28,    29,   107,    35,     3,    28,    78,
      79,    23,    67,    40,    33,    35,    13,    85,    13,    63,
      73,    19,    97,    19,    32,    20,    66,    20,    61,    69,
       6,    98,    68,    29,    81,    30,    31,    72,     4,    93,
      94,    33,    91,    28,     5,     6,   102,     7,    59,    60,
      74,    35,   105,    80,     7,     8,    88,     9,    10,   106,
      13,    97,    89,   109,    63,    19,    87,   112,   108,    20,
      95,   101,    92,   100,   113,    -2,     4,    84,    90,     0,
       0,     0,     5,     6,     0,     7,     0,     0,     0,     0,
       0,     0,   110,     8,   111,     9,    10,    41,     6,    42,
       7,     0,    43,     0,     0,     0,     0,    44,     8,    45,
       0,     0,    46
  };

  /* YYCHECK.  */
  const signed char
  JSCacheParser::yycheck_[] =
  {
         5,    29,    27,     2,    13,    50,    51,     4,     2,    18,
       8,    16,     2,     5,     6,     4,    14,     0,     5,    16,
      65,    16,    47,    28,    11,    14,    25,    72,    27,    34,
      58,    25,     4,    27,     5,    25,    41,    27,    32,    48,
       8,    13,    47,     6,    69,    19,    20,    52,     1,    83,
      84,    11,    80,     5,     7,     8,    90,    10,    16,    16,
      15,    14,    96,     3,    10,    18,     4,    20,    21,    97,
      69,     4,    77,     4,    79,    69,    75,     3,   103,    69,
      85,    89,    82,    88,   112,     0,     1,    71,    79,    -1,
      -1,    -1,     7,     8,    -1,    10,    -1,    -1,    -1,    -1,
      -1,    -1,   107,    18,   109,    20,    21,     7,     8,     9,
      10,    -1,    12,    -1,    -1,    -1,    -1,    17,    18,    19,
      -1,    -1,    22
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned char
  JSCacheParser::yystos_[] =
  {
         0,    24,    25,     0,     1,     7,     8,    10,    18,    20,
      21,    26,    27,    28,    30,    31,    33,    36,    37,    43,
      44,    45,    46,    16,    46,    25,    46,    25,     5,     6,
      34,    34,     5,    11,    58,    14,    29,    34,    35,    29,
      46,     7,     9,    12,    17,    19,    22,    28,    38,    41,
      43,    44,    47,    48,    49,    50,    55,    57,    60,    16,
      16,    43,    39,    46,    32,    42,    46,    29,    46,    25,
      58,    58,    46,    41,    15,    59,    40,     4,    16,    58,
       3,    29,    41,    51,    51,    58,    52,    28,     4,    46,
      39,    41,    52,    59,    59,    46,    53,     4,    13,    61,
      46,    35,    59,    56,    54,    59,    41,     4,    29,     4,
      46,    46,     3,    41
  };

#if YYDEBUG
  /* TOKEN_NUMBER_[YYLEX-NUM] -- Internal symbol number corresponding
     to YYLEX-NUM.  */
  const unsigned short int
  JSCacheParser::yytoken_number_[] =
  {
         0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277
  };
#endif

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
  const unsigned char
  JSCacheParser::yyr1_[] =
  {
         0,    23,    24,    25,    25,    26,    26,    26,    26,    26,
      27,    28,    29,    30,    31,    31,    32,    32,    33,    33,
      34,    35,    35,    36,    37,    38,    39,    39,    40,    40,
      41,    41,    41,    41,    41,    41,    41,    41,    41,    41,
      42,    42,    43,    43,    44,    45,    46,    47,    48,    49,
      50,    50,    51,    52,    52,    53,    53,    54,    54,    55,
      55,    56,    56,    57,    58,    59,    60,    61
  };

  /* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
  const unsigned char
  JSCacheParser::yyr2_[] =
  {
         0,     2,     1,     0,     2,     1,     1,     1,     1,     2,
       3,     1,     1,     5,     3,     3,     0,     4,     1,     1,
       2,     0,     1,     3,     6,     5,     0,     2,     0,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       0,     1,     1,     3,     3,     1,     1,     1,     5,     3,
       4,     4,     2,     0,     3,     0,     2,     0,     3,     2,
       6,     0,     5,     4,     1,     1,     1,     1
  };

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
  /* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
     First, the terminals, then, starting at \a yyntokens_, nonterminals.  */
  const char*
  const JSCacheParser::yytname_[] =
  {
    "$end", "error", "$undefined", "T_COLON", "T_COMMA", "T_DOT", "T_EQ",
  "T_FUNCTION", "T_IDENTIFIER", "T_LBRACE", "T_LBRACKET", "T_LPAREN",
  "T_NEW", "T_RBRACE", "T_RBRACKET", "T_RPAREN", "T_SEMICOLON",
  "T_STRING_LITERAL", "T_THIS", "T_NULL", "T_VAR", "T_CONST",
  "T_NUMERIC_LITERAL", "$accept", "Program", "StatementList", "Statement",
  "Block", "OpenBlock", "CloseBlock", "VariableDeclStatement",
  "AssignmentStatement", "OtherDeclOpt", "DeclTok", "Assignment",
  "InitOpt", "FunctionDecl", "StartFun", "StartOptFun", "OptParamList",
  "OtherOptParamList", "Expression", "OptIdTok", "MemberExpr", "ThisExpr",
  "ThisTok", "IdTok", "NewTok", "NewExpr", "FunctionExpr", "FunctionCall",
  "OptExpressionList", "OtherOptExpressionList", "OptArgumentList",
  "OtherOptArgumentList", "ObjConstExpr", "OtherOptFields", "ArrayExpr",
  "OpenBracket", "CloseBracket", "OpenArrBracket", "CloseArrBracket", 0
  };
#endif

#if YYDEBUG
  /* YYRHS -- A `-1'-separated list of the rules' RHS.  */
  const JSCacheParser::rhs_number_type
  JSCacheParser::yyrhs_[] =
  {
        24,     0,    -1,    25,    -1,    -1,    25,    26,    -1,    27,
      -1,    36,    -1,    30,    -1,    31,    -1,     1,    16,    -1,
      28,    25,    29,    -1,    10,    -1,    14,    -1,    33,    46,
      35,    32,    16,    -1,    43,    34,    16,    -1,    44,    34,
      16,    -1,    -1,    32,     4,    46,    35,    -1,    20,    -1,
      21,    -1,     6,    41,    -1,    -1,    34,    -1,    37,    25,
      29,    -1,     7,    46,    58,    39,    59,    28,    -1,     7,
      42,    58,    39,    59,    -1,    -1,    46,    40,    -1,    -1,
      40,     4,    46,    -1,    43,    -1,    44,    -1,    48,    -1,
      49,    -1,    50,    -1,    55,    -1,    57,    -1,    19,    -1,
      17,    -1,    22,    -1,    -1,    46,    -1,    46,    -1,    43,
       5,    46,    -1,    45,     5,    43,    -1,    18,    -1,     8,
      -1,    12,    -1,    47,    46,    58,    53,    59,    -1,    38,
      25,    29,    -1,    43,    58,    51,    59,    -1,    44,    58,
      51,    59,    -1,    41,    52,    -1,    -1,    52,     4,    41,
      -1,    -1,    46,    54,    -1,    -1,    54,     4,    46,    -1,
      28,    29,    -1,    28,    46,     3,    41,    56,    29,    -1,
      -1,    56,     4,    46,     3,    41,    -1,    60,    41,    52,
      61,    -1,    11,    -1,    15,    -1,     9,    -1,    13,    -1
  };

  /* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
     YYRHS.  */
  const unsigned char
  JSCacheParser::yyprhs_[] =
  {
         0,     0,     3,     5,     6,     9,    11,    13,    15,    17,
      20,    24,    26,    28,    34,    38,    42,    43,    48,    50,
      52,    55,    56,    58,    62,    69,    75,    76,    79,    80,
      84,    86,    88,    90,    92,    94,    96,    98,   100,   102,
     104,   105,   107,   109,   113,   117,   119,   121,   123,   129,
     133,   138,   143,   146,   147,   151,   152,   155,   156,   160,
     163,   170,   171,   177,   182,   184,   186,   188
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned short int
  JSCacheParser::yyrline_[] =
  {
         0,    58,    58,    60,    61,    64,    65,    66,    67,    68,
      74,    76,    82,    88,    95,    99,   105,   106,   109,   110,
     113,   120,   123,   127,   128,   135,   144,   145,   151,   152,
     158,   162,   166,   171,   175,   179,   188,   192,   196,   200,
     207,   210,   213,   214,   217,   219,   221,   223,   225,   232,
     239,   240,   245,   248,   249,   253,   254,   257,   258,   261,
     262,   269,   270,   277,   279,   281,   283,   285
  };

  // Print the state stack on the debug stream.
  void
  JSCacheParser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (state_stack_type::const_iterator i = yystate_stack_.begin ();
	 i != yystate_stack_.end (); ++i)
      *yycdebug_ << ' ' << *i;
    *yycdebug_ << std::endl;
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  JSCacheParser::yy_reduce_print_ (int yyrule)
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
  JSCacheParser::token_number_type
  JSCacheParser::yytranslate_ (int t)
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

  const int JSCacheParser::yyeof_ = 0;
  const int JSCacheParser::yylast_ = 122;
  const int JSCacheParser::yynnts_ = 39;
  const int JSCacheParser::yyempty_ = -2;
  const int JSCacheParser::yyfinal_ = 3;
  const int JSCacheParser::yyterror_ = 1;
  const int JSCacheParser::yyerrcode_ = 256;
  const int JSCacheParser::yyntokens_ = 23;

  const unsigned int JSCacheParser::yyuser_token_number_max_ = 277;
  const JSCacheParser::token_number_type JSCacheParser::yyundef_token_ = 2;


} // yy

/* Line 1136 of lalr1.cc  */
#line 1165 "js_parser.tab.cc"


/* Line 1138 of lalr1.cc  */
#line 287 "js_parser.yy"


namespace yy 
{
	void JSCacheParser::error(location const& loc, const std::string& s)
	{
		//std::cerr << "error at " << loc << ": " << s << "\n";
	}
}

