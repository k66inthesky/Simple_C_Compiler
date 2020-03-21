/*
This is a simple function C compiler written by C++.
It was a Processing Language class homework about 2 years ago.
Author: Lana, Chen
Date: March 20th, 2020
*/
# include <stdexcept>      // std::out_of_range
# include <iterator>
# include <iomanip>
# include <stdio.h>
# include <stdlib.h>// atoi
# include <iostream>
# include <vector>
# include <string.h>// strcpy
# include <math.h>
# include <string>

using namespace std;

struct Token {
  string token;
  int lineNo;
  string type;
};

struct Var_and_Type{
  string type;
  string var_name;
  string index;
  bool hasIndex;
};

struct Func_and_Type{
  string type;
  string func_name;
  vector<string> others;
};

// Id_and_Type_and_Value = Var_and_Type_and_Value + Func_and_Type_and_Value
struct Id_and_Type_and_Value{
  bool _int;
  bool _char;
  bool _float;
  bool _bool;
  bool _string;
  bool _void;

  int i_value;
  char c_value;
  float f_value;
  bool b_value;
  string s_value;

  string id_name;
  string index;
  bool hasIndex;
};

Token g_peekToken;
vector<Token> g_getToken;
vector<Token> g_AllTokens; // during do CutToken(), store in AllTokens
vector<Token> g_ID_table;
vector<Token> g_ID_table_local;
vector<Var_and_Type> g_All_Var;
vector<Var_and_Type> g_temp_Var; // 若宣告的文法正確，才存進g_All_Var(temp_var可能有多個var)
Var_and_Type g_one_Var;
vector<Func_and_Type> g_All_Func;
Func_and_Type g_temp_Func; // 若宣告的文法正確，才存進g_All_Func(temp_func只有一個func)
bool g_is_undefined_token; // use for ListVariable()&ListFunction()
string g_undefined_token; // use for ListVariable()&ListFunction()
bool g_is_undefined_error = false; // 在錯誤輸出時根據此變數判斷是否是undefined還是unexpected
string g_done_str = ""; // Done();
int g_now_error_lineNo; // User_Input()的讀到換行
bool g_is_done_stmt = false; // only exist in stmt()'s Done();
vector<Id_and_Type_and_Value> g_All_Var_and_Type_and_Value; // 不用分glocal跟local,因為需用到此變數時表示過了check那關了
vector<Id_and_Type_and_Value> g_All_Func_and_Type_and_Value;
Id_and_Type_and_Value g_one_Id_and_Type_and_Value; // use for temporarity use
vector<Token> g_vec_Token_for_Eval;

void StoreID( Token token ) {
  g_ID_table.push_back( token );
} // StoreID()

void StoreLocalID( Token token ) {
  g_ID_table_local.push_back( token );
} // StoreLocalID()

// 因為老大數據沒有var跟func同名的情況
bool Is_id_exist( Token token ) {
  for ( int i = 0 ; i < g_ID_table.size() ; i++ ) {
    if ( token.token == g_ID_table.at( i ).token )
      return true;
  } // for

  for ( int i = 0 ; i < g_ID_table_local.size() ; i++ ) {
    if ( token.token == g_ID_table_local.at( i ).token )
      return true;
  } // for

  for ( int i = 0 ; i < g_All_Var.size() ; i++ ) {
    if ( token.token == g_All_Var.at( i ).var_name )
      return true;
  } // for

  return false;
} // Is_id_exist()

// 判斷傳進來的字串(.others)是否為id
bool Is_identifier( string str ) {
  string record_str = "";
  record_str = record_str + str[0];
  if ( str[0] == '_' || ( str[0] >= 'A' && str[0] <= 'Z' ) || ( str[0] >= 'a' && str[0] <= 'z' ) ) {
    for ( int i = 1 ; i < str.length() ; i++ ) {
      if ( ( str[i] != '_' ) && ( str[i] < 'A' || str[i] > 'Z' ) &&
           ( str[i] < 'a' || str[i] > 'z' ) && ( str[i] < '0' || str[i] > '9' ) )
        return false;
      record_str = record_str + str[i];
    } // for

    if ( record_str == "if" || record_str == "else" || record_str == "do" || record_str == "while" ||
         record_str == "cin" || record_str == "cout" )
      return false;
    else
      return true;
  } // if

  else
    return false;
} // Is_identifier()

// 判斷傳進來的字串(.others)是否為num
bool Is_num( string str ) {
  string record_str = "";
  record_str = record_str + str[0];
  bool hasDot = false;
  bool isDotRepeat = false;
  if ( str[0] == '.' || ( str[0] >= '0' && str[0] <= '9' ) ) {
    // .或數字開頭('".數字中) //

    if ( str[0] == '.' ) { // start with .
      for ( int i = 1 ; i < str.length() ; i++ ) {
        if ( ( str[i] < '0' || str[i] > '9' ) )
          return false;
      } // for

      return true;
    } // if

    else { // start with 0-9

      for ( int i = 1 ; !isDotRepeat && i < str.length() ; i++ ) {

        if ( str[i] != '.' && ( str[i] < '0' || str[i] > '9' ) )
          return false;

        if ( str[i] == '.' && hasDot )
          isDotRepeat = true; // leave the loop

        if ( str[i] == '.' && !hasDot )
          hasDot = true;

      } // for

      if ( isDotRepeat )
        return false;
      else
        return true;
    } // else


  } // if

  else
    return false;
} // Is_num()

void Clear_g_one_Id_and_Type_and_Value(  ) {
  g_one_Id_and_Type_and_Value._int = false;
  g_one_Id_and_Type_and_Value._char = false;
  g_one_Id_and_Type_and_Value._float = false;
  g_one_Id_and_Type_and_Value._bool = false;
  g_one_Id_and_Type_and_Value._string = false;
  g_one_Id_and_Type_and_Value._void = false;

  g_one_Id_and_Type_and_Value.i_value = -999;
  g_one_Id_and_Type_and_Value.c_value = '\0';
  g_one_Id_and_Type_and_Value.f_value = -999;
  g_one_Id_and_Type_and_Value.b_value = false;
  g_one_Id_and_Type_and_Value.s_value = "";

  g_one_Id_and_Type_and_Value.id_name = "";
  g_one_Id_and_Type_and_Value.index = -99;
  g_one_Id_and_Type_and_Value.hasIndex = false;

} // Clear_g_one_Id_and_Type_and_Value()

// 以下四個func: Priority(), InToPostfix(), Cal(), Eval()是寫給float計算，而Cal_int(), Eval_int()是給int計算
int Priority( Token op ) {
  if ( op.token == "+" || op.token == "-" )
    return 1;
  else if ( op.token == "*" || op.token == "/" || op.token == "%" )
    return 2;
  else
    return 0;
} // Priority()

void InToPostfix( vector<Token> infix, vector<Token> &postfix ) {
    Token _postfix[80];
    Token _infix[80];
    Token _stack[80];
    int i, j, top;

    for ( int k = 0 ; k < 80 ; k++ ) // "-999"用作判斷，結束func()前複製會用到
      _postfix[k].token = "-999";

    for( int k = 0; k < infix.size() ; k++ )
      _infix[k] = infix[k];

    for( i = 0, j = 0, top = 0; i < infix.size() ; i++ ) {
        if ( _infix[i].token == "(" ) // 運算子堆疊
          _stack[++top] = _infix[i];

        else if ( _infix[i].token == "+" || _infix[i].token == "-" ||
                  _infix[i].token == "*" || _infix[i].token == "/" || _infix[i].token == "%" ) {
          while( Priority( _stack[top] ) >= Priority( _infix[i] ) )
            _postfix[j++] = _stack[top--];

          _stack[++top] = _infix[i]; // 存入堆疊
        } // else if

        else if ( _infix[i].token == ")" ) {
          while( _stack[top].token != "(" ) // 遇 ) 輸出至 (
            _postfix[j++] = _stack[top--];

          top--;  // 不輸出 (
        } // else if

        else // 運算元直接輸出
          _postfix[j++] = _infix[i];
  	} // for


    while( top > 0 )
      _postfix[j++] = _stack[top--];

    for ( int i = 0 ; i < infix.size() ; i++ ) {
      if ( _postfix[i].token != "-999" )
        postfix.push_back( _postfix[i] );
    } // for

} // inToPostfix()

float Cal( Token op, float f1, float f2 ) {
  float value1, value2;
  int tmp_int;

  if ( op.token == "+" )
    return f1 + f2;
  else if ( op.token == "-" )
    return f1 - f2;
  else if ( op.token == "*" )
    return f1 * f2;
  else if ( op.token == "/" )
    return f1 / f2;
  else
    return -999; // this case shouldn't exist!!!
} // Cal()

float Eval(  ) {
    vector<Token> postfix;
    Token opnd;
    float _stack[80];
    int record;

    InToPostfix( g_vec_Token_for_Eval, postfix );

    int top, i;
    for( top = 0, i = 0; i < postfix.size() ; i++ ) {
	  if( postfix[i].token == "+" || postfix[i].token == "-" ||
          postfix[i].token == "*" || postfix[i].token == "/" || postfix[i].token == "%" ) {
        _stack[top - 1] = Cal( postfix[i], _stack[top - 1], _stack[top] );
        top--;
	  } // if

      else {
        if ( Is_identifier( postfix[i].token ) ) {
          // turn ids to numbers
          for ( int k = 0 ; k < g_All_Var_and_Type_and_Value.size() ; k++ ) {
            if ( postfix[i].token == g_All_Var_and_Type_and_Value[k].id_name )
              _stack[++top] = g_All_Var_and_Type_and_Value[k].f_value;
          } // for

        } // if

        else {
          opnd = postfix[i];
          _stack[++top] = atof( opnd.token.c_str() ); //將字串中的數字轉換為 float
        } // else

      } // else

    } // for

    return _stack[ top ];
} // Eval()

int Cal_int( Token op, int i1, int i2 ) {
  int value1, value2;
  int tmp_int;

  if ( op.token == "+" )
    return i1 + i2;
  else if ( op.token == "-" )
    return i1 - i2;
  else if ( op.token == "*" )
    return i1 * i2;
  else if ( op.token == "/" )
    return i1 / i2;
  else if ( op.token == "%" )
    return i1 / i2;
  else
    return -999; // this case shouldn't exist!!!
} // Cal_int()

int Eval_int(  ) {
    vector<Token> postfix;
    Token opnd;
    int _stack[80];
    int record;

    InToPostfix( g_vec_Token_for_Eval, postfix );

    int top, i;
    for( top = 0, i = 0; i < postfix.size() ; i++ ) {
	  if( postfix[i].token == "+" || postfix[i].token == "-" ||
          postfix[i].token == "*" || postfix[i].token == "/" || postfix[i].token == "%" ) {
        _stack[top - 1] = Cal( postfix[i], _stack[top - 1], _stack[top] );
        top--;
	  } // if

      else {
        if ( Is_identifier( postfix[i].token ) ) {
          // turn ids to numbers
          for ( int k = 0 ; k < g_All_Var_and_Type_and_Value.size() ; k++ ) {
            if ( postfix[i].token == g_All_Var_and_Type_and_Value[k].id_name )
              _stack[++top] = g_All_Var_and_Type_and_Value[k].i_value;
          } // for

        } // if

        else {
          opnd = postfix[i];
          _stack[++top] = atoi( opnd.token.c_str() ); //將字串中的數字轉換為 int
        } // else

      } // else

    } // for

    return _stack[ top ];
} // Eval_int()

int g_peekTokenNo; // 在第幾個g_AllTokens上
int g_nearest_error_in_which_line;
int g_lineNo;
int g_last_exe_lineNo; // exe_line include: succeed & error lines
bool g_hasError; // 判斷目前為止的副程式有沒有error，有error就true

void User_input() ;
bool Definition( Token &token, int &IsVar_or_Func ) ;
bool Type_specifier( Token &token ) ;
bool Function_definition_or_declarators( Token &token ) ;

bool Rest_of_declarators( Token &token ) ;
bool Function_definition_without_ID( Token &token ) ;
bool Formal_parameter_list( Token &token ) ;
bool Compound_statement( Token &token ) ;

bool Declaration( Token &token ) ;
bool Statement( Token &token ) ;
bool Stmt_for_Compound_stmt( Token &token ) ;
bool Expression( Token &token ) ;
bool Basic_expression( Token &token ) ;

bool Rest_of_Identifier_started_basic_exp( Token &token ) ;
bool Rest_of_PPMM_Identifier_started_basic_exp( Token &token ) ;
bool Sign( Token &token ) ;
bool Actual_parameter_list( Token &token ) ;


bool Assignment_operator( Token &token ) ;
bool Romce_and_romloe( Token &token ) ;
bool Rest_of_maybe_logical_OR_exp( Token &token ) ;
bool Maybe_logical_AND_exp( Token &token ) ;

bool Rest_of_maybe_logical_AND_exp( Token &token ) ;
bool Maybe_bit_OR_exp( Token &token ) ;
bool Rest_of_maybe_bit_OR_exp( Token &token ) ;
bool Maybe_bit_ex_OR_exp( Token &token ) ;

bool Rest_of_maybe_bit_ex_OR_exp( Token &token ) ;
bool Maybe_bit_AND_exp( Token &token ) ;
bool Rest_of_maybe_bit_AND_exp( Token &token ) ;
bool Maybe_equality_exp( Token &token ) ;

bool Rest_of_maybe_equality_exp( Token &token ) ;
bool Maybe_relational_exp( Token &token ) ;
bool Rest_of_maybe_relational_exp( Token &token ) ;
bool Maybe_shift_exp( Token &token ) ;

bool Rest_of_maybe_shift_exp( Token &token ) ;
bool Maybe_additive_exp( Token &token ) ;
bool Rest_of_maybe_additive_exp( Token &token ) ;
bool Maybe_mult_exp( Token &token ) ;

bool Rest_of_maybe_mult_exp( Token &token ) ;
bool Unary_exp( Token &token ) ;
bool Signed_unary_exp( Token &token ) ;
bool Unsigned_unary_exp( Token &token ) ;
void CutToken( ) ;

// 只印var名字
void ListAllVariables( ) {
  Var_and_Type v; // for temporarily use
  bool tmp_bool = false; // for temporarily use
  Var_and_Type tmp_v; // for temporarily use

  // delete those duplicate ones
  for ( int i = 0 ; i < g_All_Var.size() ; i++ ) {
    for ( int j = i + 1 ; j < g_All_Var.size() ; j++ ) {
      tmp_v = g_All_Var.at( i );
      if ( g_All_Var.at( j ).var_name == tmp_v.var_name ) {
        g_All_Var.erase( g_All_Var.begin() + j );
        j--; // avoid to miss check
      } // if

    } // for

  } // for

  // bubble sort
  for ( int i = 0, k = 0 ; i < g_All_Var.size() ; i++, k++ ) {
    for ( int j = 0 ; j < g_All_Var.size() - k ; j++ ) {
      if ( ( j + 1 < g_All_Var.size() ) &&
           ( g_All_Var.at( j ).var_name > g_All_Var.at( j + 1 ).var_name ) ) {
        // swap( g_All_Var.at( j ), g_All_Var.at( j + 1 ) );
        v = g_All_Var.at( j );
        g_All_Var.at( j ) = g_All_Var.at( j + 1 );
        g_All_Var.at( j + 1 ) = v;
      } // if

    } // for

  } // for

  // print
  for ( int i = 0 ; i < g_All_Var.size() ; i++ ) {
    for ( int j = 0 ; j < g_ID_table.size() ; j++ ) {
      if ( g_All_Var.at( i ).var_name == g_ID_table.at( j ).token )
        tmp_bool = true;
    } // for

    if ( tmp_bool )
      cout << g_All_Var.at( i ).var_name << endl;
    tmp_bool = false; // initialize
  } // for

} // ListAllVariables()

// 只印func名字
void ListAllFunctions( ) {
  Func_and_Type f; // for templately use

  // bubble sort
  for ( int i = 0, k = 0 ; i < g_All_Func.size() ; i++, k++ ) {
    for ( int j = 0 ; j < g_All_Func.size() - k ; j++ ) {
      if ( ( j + 1 < g_All_Func.size() ) &&
           ( g_All_Func.at( j ).func_name > g_All_Func.at( j + 1 ).func_name ) ) {
        // swap( g_All_Func.at( j ), g_All_Func.at( j + 1 ) );
        f = g_All_Func.at( j );
        g_All_Func.at( j ) = g_All_Func.at( j + 1 );
        g_All_Func.at( j + 1 ) = f;
      } // if

    } // for

  } // for

  // print
  for ( int i = 0 ; i < g_All_Func.size() ; i++ )
    cout << g_All_Func.at( i ).func_name << "()" << endl;

} // ListAllFunctions()

Token PeekToken() {
  Token token;
  if ( g_AllTokens.size() == 0 ) {
    while ( g_AllTokens.size() == 0 )
      CutToken();
    token = g_AllTokens.at( g_peekTokenNo );
    g_peekTokenNo++;
  } // if

  else if ( g_peekTokenNo < g_AllTokens.size() ) {
    token = g_AllTokens.at( g_peekTokenNo );
    g_peekTokenNo++;
  } // else if

  else {
    while ( g_peekTokenNo >= g_AllTokens.size() )
      CutToken();
    token = g_AllTokens.at( g_peekTokenNo );
    g_peekTokenNo++;
  } // else

  return token;
} // PeekToken()

void GetToken( Token token ) {
  g_getToken.push_back( token );
} // GetToken()

// 刪除最後一個token
void EraseLastToken() {
  g_getToken.erase( g_getToken.end() );
} // EraseLastToken()

void CutToken() {
  char chr;
  string str = "";
  string done_str = ""; // Done+()
  Token one_Token;
  Token tmp_Token;
  int tmp_int = 0;
  bool isAlpha = false;
  bool hasDot = false;
  bool isDotRepeat = false;
  int start = 0;
  bool is_start_zero = false;

  scanf( "%c", &chr );
  str += chr;
  while ( chr != '\n' ) {
    scanf( "%c", &chr );
    str += chr;
  } // while

  for ( int i = 0 ; i < str.length() ; i++ ) {

    // ==============================Identifier===================================== //
    // Identifier : start with _ or A-Z or a-z, exclude int,float,if,while,etc ====//
    if ( str[i] == '_' || ( str[i] >= 'A' && str[i] <= 'Z' ) || ( str[i] >= 'a' && str[i] <= 'z' ) ) {
      tmp_int = i;
      for ( i = i + 1 ; str[i] == '_' || ( str[i] >= 'A' && str[i] <= 'Z' ) ||
            ( str[i] >= 'a' && str[i] <= 'z' ) || ( str[i] >= '0' && str[i] <= '9' ) ; i++ )
        ;

      one_Token.token.assign( str, tmp_int, i-tmp_int ); // means from tmp_int to i-1
      one_Token.lineNo = g_lineNo;

      if ( one_Token.token == "ListAllVariables" )
        one_Token.type = "ListAllVariables";
      else if ( one_Token.token == "ListVariable" )
        one_Token.type = "ListVariable";
      else if ( one_Token.token == "ListAllFunctions" )
        one_Token.type = "ListAllFunctions";
      else if ( one_Token.token == "ListFunction" )
        one_Token.type = "ListFunction";
      else if ( one_Token.token == "cin" )
        one_Token.type = "CIN";
      else if ( one_Token.token == "cout" )
        one_Token.type = "COUT";
      else if ( one_Token.token == "Done" )
        one_Token.type = "DONE";
      else if ( one_Token.token == "true" )
        one_Token.type = "Constant";
      else if ( one_Token.token == "false" )
        one_Token.type = "Constant";
      else if ( one_Token.token == "int" )
        one_Token.type = "INT";
      else if ( one_Token.token == "float" )
        one_Token.type = "FLOAT";
      else if ( one_Token.token == "char" )
        one_Token.type = "CHAR";
      else if ( one_Token.token == "bool" )
        one_Token.type = "BOOL";
      else if ( one_Token.token == "string" )
        one_Token.type = "STRING";
      else if ( one_Token.token == "void" )
        one_Token.type = "VOID";
      else if ( one_Token.token == "if" )
        one_Token.type = "IF";
      else if ( one_Token.token == "else" )
        one_Token.type = "ELSE";
      else if ( one_Token.token == "while" )
        one_Token.type = "WHILE";
      else if ( one_Token.token == "do" )
        one_Token.type = "DO";
      else if ( one_Token.token == "return" )
        one_Token.type = "RETURN";
      else
        one_Token.type = "Identifier";

      g_AllTokens.push_back( one_Token );

      i--; // 目的不要在進下一次迴圈時跳過str[i]

    } // if
    // ==============================Identifier========================================= //

    // ==============================Constant=========================================== //
    // Constant : start with ' or " or . or 0-9 ================= //
    else if ( str[i] == '\'' || str[i] == '"' || str[i] == '.' || ( str[i] >= 48 && str[i] <= 57 ) ) {

      // '開頭('".數字中) //
      if ( str[i] == '\'' ) {
        if ( str[i+1] == '\'' ) {
          one_Token.lineNo = g_lineNo;
          one_Token.token.assign( str, i, 2 ); // means i to i+1
          one_Token.type = "Constant";
          g_AllTokens.push_back( one_Token );
          i = i + 1;
        } // if

        else if ( str[i+2] == '\'' ) {
          one_Token.lineNo = g_lineNo;
          one_Token.token.assign( str, i, 3 ); // means i to i+2
          one_Token.type = "Constant";
          g_AllTokens.push_back( one_Token );
          i = i + 2;
        } // else if

        else if ( str[i+1] == '\\' && str[i+3] == '\'' ) { // eg. '\n' '\t' etc
          one_Token.lineNo = g_lineNo;
          one_Token.token.assign( str, i, 4 ); // means i to i+3
          one_Token.type = "Constant";
          g_AllTokens.push_back( one_Token );
          i = i + 3;
        } // else if

        else {
          tmp_int = i;
          for ( i = i + 1 ; str[i] != ' ' && str[i] != '\t' && str[i] != '\n' && str[i] != '(' &&
                str[i] != '\'' && str[i] != '\"' &&
                str[i] != '[' && str[i] != '{' && str[i] != ')' && str[i] != ']' &&
                str[i] != '}' && str[i] != '+' && str[i] != '-' && str[i] != '*' &&
                str[i] != '/' && str[i] != '%' && str[i] != '^' && str[i] != '>' &&
                str[i] != '<' && str[i] != '=' && str[i] != '!' && str[i] != '&' &&
                str[i] != '|' && str[i] != ';' && str[i] != ',' && str[i] != '?' &&
                str[i] != ':' ; i++ )
            ;
          one_Token.lineNo = g_lineNo;
          one_Token.token.assign( str, tmp_int, i-tmp_int ); // means tmp_int to i-1
          one_Token.type = "Nothing";
          g_AllTokens.push_back( one_Token );
          if ( str[i] == '\n' )
            ;
          i--; // 怕下一次迴圈沒存到str[i]
        } // else

      } // if
      // '開頭('".數字中) //


      // "開頭('".數字中) //
      else if ( str[i] == '"' ) {
        tmp_int = i;
        for ( i = i + 1 ; str[i] != '"' && str[i] != '\n' ; i++ )
          ;
        if ( str[i] == '"' ) {
          one_Token.lineNo = g_lineNo;
          one_Token.token.assign( str, tmp_int, i+1-tmp_int  ); // means tmp_int to i
          one_Token.type = "Constant";
          g_AllTokens.push_back( one_Token );
        } // if
        else { // 表示str[i]是'\n'
          one_Token.lineNo = g_lineNo;
          one_Token.token = "\"";
          one_Token.type = "Nothing";
          g_AllTokens.push_back( one_Token );
          g_lineNo++;
        } // else
      } // else if
      // "開頭('".數字中) //

      // .或數字開頭('".數字中) //
      else if ( str[i] == '.' || ( str[i] >= '0' && str[i] <= '9' ) ) {

        if ( str[i] == '.' ) {
          tmp_int = i;
          if ( str[i+1] == '.' ) {
            one_Token.type = "Nothing";
            one_Token.lineNo = g_lineNo;
            one_Token.token = ".";
            g_AllTokens.push_back( one_Token );
            i = i + 1;
          } // if

          else {
            for ( i = i + 1 ; ( str[i] >= '0' && str[i] <= '9' ) ; i++ )
              ;
            one_Token.type = "Constant";
            one_Token.lineNo = g_lineNo;
            one_Token.token.assign( str, tmp_int, i-tmp_int ); // means tmp_int to i-1
            g_AllTokens.push_back( one_Token );
          } // else

        } // if

        else {
          tmp_int = i;
          for ( i = i + 1 ; !isDotRepeat &&
                ( str[i] == '.' || ( str[i] >= '0' && str[i] <= '9' ) ) ; i++ ) {
            if ( str[i] == '.' && hasDot ) {
              isDotRepeat = true; // leave the loop
              i--; // dont forget to -- ,cuz next time needs ++ before this loop
            } // if

            if ( str[i] == '.' && !hasDot )
              hasDot = true;
          } // for

          one_Token.type = "Constant";
          one_Token.lineNo = g_lineNo;
          one_Token.token.assign( str, tmp_int, i-tmp_int ); // means tmp_int to i-1
          g_AllTokens.push_back( one_Token );
        } // else

        hasDot = false; // initialize
        isDotRepeat = false; // initialize
        i--; // 避免str[i]沒存到、下一回從i開始
      } // else if
      // .或數字開頭('".數字中) //

    } // else if
    // ==============================Constant=========================================== //

    // ===(=== //
    else if ( str[i] == '(' ) {
      one_Token.lineNo = g_lineNo;
      one_Token.token = "(";
      one_Token.type = "(";
      g_AllTokens.push_back( one_Token );
    } // else if

    // ===)=== //
    else if ( str[i] == ')' ) {
      one_Token.lineNo = g_lineNo;
      one_Token.token = ")";
      one_Token.type = ")";
      g_AllTokens.push_back( one_Token );
    } // else if

    // ===[=== //
    else if ( str[i] == '[' ) {
      one_Token.lineNo = g_lineNo;
      one_Token.token = "[";
      one_Token.type = "[";
      g_AllTokens.push_back( one_Token );
    } // else if

    // ===]=== //
    else if ( str[i] == ']' ) {
      one_Token.lineNo = g_lineNo;
      one_Token.token = "]";
      one_Token.type = "]";
      g_AllTokens.push_back( one_Token );
    } // else if

    // ==={=== //
    else if ( str[i] == '{' ) {
      one_Token.lineNo = g_lineNo;
      one_Token.token = "{";
      one_Token.type = "{";
      g_AllTokens.push_back( one_Token );
    } // else if

    // ===}=== //
    else if ( str[i] == '}' ) {
      one_Token.lineNo = g_lineNo;
      one_Token.token = "}";
      one_Token.type = "}";
      g_AllTokens.push_back( one_Token );
    } // else if

    // ==========================   +   ==================================== //
    else if ( str[i] == '+' ) {
      if ( str[i+1] == '=' ) {
        one_Token.lineNo = g_lineNo;
        one_Token.token = "+=";
        one_Token.type = "PE";
        g_AllTokens.push_back( one_Token );
        i++;
      } // if

      else if ( str[i+1] == '+' ) {
        one_Token.lineNo = g_lineNo;
        one_Token.token = "++";
        one_Token.type = "PP";
        g_AllTokens.push_back( one_Token );
        i++;
      } // if

      else {
        one_Token.lineNo = g_lineNo;
        one_Token.token = "+";
        one_Token.type = "+";
        g_AllTokens.push_back( one_Token );
      } // else
    } // else if
    // ===========================   +   ======================================== //

    // ==========================   -   ==================================== //
    else if ( str[i] == '-' ) {
      if ( str[i+1] == '=' ) {
        one_Token.lineNo = g_lineNo;
        one_Token.token = "-=";
        one_Token.type = "ME";
        g_AllTokens.push_back( one_Token );
        i++;
      } // if

      else if ( str[i+1] == '-' ) {
        one_Token.lineNo = g_lineNo;
        one_Token.token = "--";
        one_Token.type = "MM";
        g_AllTokens.push_back( one_Token );
        i++;
      } // if

      else {
        one_Token.lineNo = g_lineNo;
        one_Token.token = "-";
        one_Token.type = "-";
        g_AllTokens.push_back( one_Token );
      } // else
    } // else if
    // ===========================   -   ======================================== //

    // ==========================   *   ==================================== //
    else if ( str[i] == '*' ) {
      if ( str[i+1] == '=' ) {
        one_Token.lineNo = g_lineNo;
        one_Token.token = "*=";
        one_Token.type = "TE";
        g_AllTokens.push_back( one_Token );
        i++;
      } // if

      else {
        one_Token.lineNo = g_lineNo;
        one_Token.token = "*";
        one_Token.type = "*";
        g_AllTokens.push_back( one_Token );
      } // else
    } // else if
    // ===========================   *   ======================================== //

    // ==========================   /   ==================================== //
    else if ( str[i] == '/' ) {
      if ( str[i+1] == '=' ) {
        one_Token.lineNo = g_lineNo;
        one_Token.token = "/=";
        one_Token.type = "DE";
        g_AllTokens.push_back( one_Token );
        i++;
      } // if

      else if ( str[i+1] == '/' ) { // 註解後面的要刪掉
        for ( i = i + 2 ; str[i] != '\n' ; i++ )
          ;
        g_lineNo++;
      } // else if

      else {
        one_Token.lineNo = g_lineNo;
        one_Token.token = "/";
        one_Token.type = "/";
        g_AllTokens.push_back( one_Token );
      } // else
    } // else if
    // ===========================   /   ======================================== //

    // ==========================   %   ==================================== //
    else if ( str[i] == '%' ) {
      if ( str[i+1] == '=' ) {
        one_Token.lineNo = g_lineNo;
        one_Token.token = "%=";
        one_Token.type = "RE";
        g_AllTokens.push_back( one_Token );
        i++;
      } // if

      else {
        one_Token.lineNo = g_lineNo;
        one_Token.token = "%";
        one_Token.type = "%";
        g_AllTokens.push_back( one_Token );
      } // else
    } // else if
    // ===========================   %   ======================================== //

    // ===^=== //
    else if ( str[i] == '^' ) {
      one_Token.lineNo = g_lineNo;
      one_Token.token = "^";
      one_Token.type = "^";
      g_AllTokens.push_back( one_Token );
    } // else if

    // ============================>=========================================== //
    else if ( str[i] == '>' ) {
      if ( str[i+1] == '=' ) {
        one_Token.lineNo = g_lineNo;
        one_Token.token = ">=";
        one_Token.type = "GE";
        g_AllTokens.push_back( one_Token );
        i++;
      } // if

      else if ( str[i+1] == '>' ) {
        one_Token.lineNo = g_lineNo;
        one_Token.token = ">>";
        one_Token.type = "RS";
        g_AllTokens.push_back( one_Token );
        i++;
      } // else if

      else {
        one_Token.lineNo = g_lineNo;
        one_Token.token = ">";
        one_Token.type = ">";
        g_AllTokens.push_back( one_Token );
      } // else
    } // else if
    // ============================>=========================================== //

    // ============================<=========================================== //
    else if ( str[i] == '<' ) {
      if ( str[i+1] == '=' ) {
        one_Token.lineNo = g_lineNo;
        one_Token.token = "<=";
        one_Token.type = "LE";
        g_AllTokens.push_back( one_Token );
        i++;
      } // if

      else if ( str[i+1] == '<' ) {
        one_Token.lineNo = g_lineNo;
        one_Token.token = "<<";
        one_Token.type = "LS";
        g_AllTokens.push_back( one_Token );
        i++;
      } // else if

      else {
        one_Token.lineNo = g_lineNo;
        one_Token.token = "<";
        one_Token.type = "<";
        g_AllTokens.push_back( one_Token );
      } // else
    } // else if
    // ============================<=========================================== //

    // ==========================   =   ==================================== //
    else if ( str[i] == '=' ) {
      if ( str[i+1] == '=' ) {
        one_Token.lineNo = g_lineNo;
        one_Token.token = "==";
        one_Token.type = "EQ";
        g_AllTokens.push_back( one_Token );
        i++;
      } // if

      else {
        one_Token.lineNo = g_lineNo;
        one_Token.token = "=";
        one_Token.type = "=";
        g_AllTokens.push_back( one_Token );
      } // else
    } // else if
    // ===========================   =   ======================================== //

    // ==========================   !   ==================================== //
    else if ( str[i] == '!' ) {
      if ( str[i+1] == '=' ) {
        one_Token.lineNo = g_lineNo;
        one_Token.token = "!=";
        one_Token.type = "NEQ";
        g_AllTokens.push_back( one_Token );
        i++;
      } // if

      else {
        one_Token.lineNo = g_lineNo;
        one_Token.token = "!";
        one_Token.type = "!";
        g_AllTokens.push_back( one_Token );
      } // else
    } // else if
    // ===========================   !   ======================================== //

    // ==========================   &   ==================================== //
    else if ( str[i] == '&' ) {
      if ( str[i+1] == '&' ) {
        one_Token.lineNo = g_lineNo;
        one_Token.token = "&&";
        one_Token.type = "AND";
        g_AllTokens.push_back( one_Token );
        i++;
      } // if

      else {
        one_Token.lineNo = g_lineNo;
        one_Token.token = "&";
        one_Token.type = "&";
        g_AllTokens.push_back( one_Token );
      } // else
    } // else if
    // ===========================   &   ======================================== //

    // ==========================   |   ==================================== //
    else if ( str[i] == '|' ) {
      if ( str[i+1] == '|' ) {
        one_Token.lineNo = g_lineNo;
        one_Token.token = "||";
        one_Token.type = "OR";
        g_AllTokens.push_back( one_Token );
        i++;
      } // if

      else {
        one_Token.lineNo = g_lineNo;
        one_Token.token = "|";
        one_Token.type = "|";
        g_AllTokens.push_back( one_Token );
      } // else
    } // else if
    // ===========================   |   ======================================== //

    // ===;=== //
    else if ( str[i] == ';' ) {
      one_Token.lineNo = g_lineNo;
      one_Token.token = ";";
      one_Token.type = ";";
      g_AllTokens.push_back( one_Token );
    } // else if

    // ===,=== //
    else if ( str[i] == ',' ) {
      one_Token.lineNo = g_lineNo;
      one_Token.token = ",";
      one_Token.type = ",";
      g_AllTokens.push_back( one_Token );
    } // else if

    // ===?=== //
    else if ( str[i] == '?' ) {
      one_Token.lineNo = g_lineNo;
      one_Token.token = "?";
      one_Token.type = "?";
      g_AllTokens.push_back( one_Token );
    } // else if

    // ===:=== //
    else if ( str[i] == ':' ) {
      one_Token.lineNo = g_lineNo;
      one_Token.token = ":";
      one_Token.type = ":";
      g_AllTokens.push_back( one_Token );
    } // else if

    // ==='\n'=== //
    else if ( str[i] == '\n' )
      g_lineNo++;

    else if ( str[i] == ' ' || str[i] == '\t' ) {
      while ( str[i] == ' ' || str[i] == '\t' )
        i++;
      i--;
    } // else if

    // ===#$@等====
    else {
      one_Token.token = str[ i ];
      one_Token.lineNo = g_lineNo;
      one_Token.type = "Nothing";
      g_AllTokens.push_back( one_Token );
    } // else


  } // for



  /*
  // 拿來檢驗:
  for ( int j = 0 ; j < g_AllTokens.size() ; j++ )
    cout << g_AllTokens.at( j ).token << " is at " << g_AllTokens.at( j ).lineNo << " line"
         << " and it's type is " << g_AllTokens.at( j ).type << endl;
  */

} // CutToken()

void PrintError( int error_type, Token token ) {
  int lineNo;
  if ( g_last_exe_lineNo == 0 )
    lineNo = token.lineNo - 1;
  else
    lineNo = token.lineNo - g_last_exe_lineNo ;

  if ( lineNo == 0 )
    lineNo = 1;

  // 下連續兩個if的順序不能調換(因為error的優先次序之分)
  if ( !g_is_undefined_error )
    error_type = 2;
  if ( error_type == 2 && token.type == "Nothing" ) // 文法錯誤也可能是compiler不認得此token，所以error_type是1
    error_type = 1;

  if ( g_is_undefined_token ) {
    cout << "Line " << lineNo << " : undefined identifier : '" << g_undefined_token << "'\n";
    g_is_undefined_token = false; // initialize
    g_undefined_token = ""; // initialize
  } // if

  else if ( g_is_undefined_error ) {
    cout << "Line " << lineNo << " : undefined identifier : '" << token.token << "'\n";
    g_is_undefined_error = false; // initialize
  } // else if

  else {
    if ( error_type == 1 )
      cout << "Line " << lineNo << " : unrecognized token with first char : '" << token.token << "'\n";
    else if ( error_type == 2 )
      cout << "Line " << lineNo << " : unexpected token : '" << token.token << "'\n";
    else if ( error_type == 3 )
      cout << "Line " << lineNo << " : undefined identifier : '" << token.token << "'\n";
  } // else

  g_last_exe_lineNo = token.lineNo;
  g_getToken.clear();
  g_is_undefined_token = false; // initialize
  g_undefined_token = ""; // initialize
  g_is_undefined_error = false; // initialize
} // PrintError()

void PrintSucceed_Stmt( Token token ) {
  cout << "Statement executed ...\n";

  g_last_exe_lineNo = token.lineNo;
  g_getToken.clear();
} // PrintSucceed_Stmt()

// for variable def
void PrintSucceed_Def_Var( int succeed_type, Token token, Var_and_Type v ) {
  if ( succeed_type == 1 )
    cout << "Definition of " << v.var_name <<  " entered ...\n";
  else if ( succeed_type == 2 )
    cout << "New definition of " << v.var_name <<  " entered ...\n";

  g_last_exe_lineNo = token.lineNo;
  g_getToken.clear();
} // PrintSucceed_Def_Var()

// for function def
void PrintSucceed_Def_Func( int succeed_type, Token token, Func_and_Type f ) {
  if ( succeed_type == 1 )
    cout << "Definition of " << f.func_name << "() entered ...\n";
  else if ( succeed_type == 2 )
    cout << "New definition of " << f.func_name << "() entered ...\n";

  g_last_exe_lineNo = token.lineNo;
  g_getToken.clear();
} // PrintSucceed_Def_Func()

void User_input() {
  Token token;
  string done_str = "";
  g_last_exe_lineNo = 0;
  Token tmp_token;
  int isVar_or_Func = 0; // default = 0
  bool tmp_bool = false, tmp_bool_2 = false, tmp_bool_3 = false;
  int tmp_int = 0, tmp_int_3 = 0;
  string tmp_str = "";

  int i_value;
  float f_value;
  char c_value;
  string s_value;
  bool b_value = false;
  bool _int = false, _float = false, _char = false, _string = false, _bool = false;

  CutToken();
  token = PeekToken();

  while ( !g_is_done_stmt ) {

    cout << "> ";
    if ( token.type == "INT" || token.type == "FLOAT" || token.type == "CHAR" ||
         token.type == "BOOL" || token.type == "STRING" || token.type == "VOID" ) {
      g_getToken.clear(); // 每次用前先清空，以免存到上一次存過的
      g_temp_Var.clear(); // initialize
      g_temp_Func.others.clear(); // initialize
      g_ID_table_local.clear(); // initialize
      if ( Definition( token, isVar_or_Func ) ) {
        GetToken( token );
        if ( isVar_or_Func == 2 ) {
          if ( g_All_Func.size() == 0 ) {
            PrintSucceed_Def_Func( 1, token, g_temp_Func );
            g_All_Func.push_back( g_temp_Func );
            tmp_token.token = g_temp_Func.func_name;
            g_ID_table.push_back( tmp_token );

          } // if

          else {
            for ( int i = 0 ; i < g_All_Func.size() ; i++ ) {
              if ( g_temp_Func.func_name == g_All_Func.at( i ).func_name ) {
                tmp_bool_2 = true;
                tmp_int = i;
              } // if

            } // for

            if ( tmp_bool_2 ) {
              PrintSucceed_Def_Func( 2, token, g_temp_Func );
              g_All_Func.erase( g_All_Func.begin() + tmp_int ); // New Def需刪掉舊定義的func
              tmp_int = 0; // initialize
              tmp_bool_2 = false; // initialize

              // delete the old one
              for ( int i = 0 ; i < g_All_Func_and_Type_and_Value.size() ; i++ ) {
                if ( g_temp_Func.func_name == g_All_Func_and_Type_and_Value.at( i ).id_name ) {
                  g_All_Func_and_Type_and_Value.erase( g_All_Func_and_Type_and_Value.begin() + i );
                  i = 1000; // force to leave the loop
                } // if

              } // for

            } // if

            else
              PrintSucceed_Def_Func( 1, token, g_temp_Func );

            g_All_Func.push_back( g_temp_Func );
            tmp_token.token = g_temp_Func.func_name;
            g_ID_table.push_back( tmp_token );

          } // else

          // ==================================================================
            if ( g_temp_Func.type == "int" )
              g_one_Id_and_Type_and_Value._int = true;
            else if ( g_temp_Func.type == "float" )
              g_one_Id_and_Type_and_Value._float = true;
            else if ( g_temp_Func.type == "char" )
              g_one_Id_and_Type_and_Value._char = true;
            else if ( g_temp_Func.type == "string" )
              g_one_Id_and_Type_and_Value._string = true;
            else if ( g_temp_Func.type == "bool" )
              g_one_Id_and_Type_and_Value._bool = true;
            else if ( g_temp_Func.type == "void" )
              g_one_Id_and_Type_and_Value._void = true;
            g_one_Id_and_Type_and_Value.id_name = g_temp_Func.func_name;
            g_All_Func_and_Type_and_Value.push_back( g_one_Id_and_Type_and_Value );
            Clear_g_one_Id_and_Type_and_Value();
            // ==================================================================

        } // if

        else if ( isVar_or_Func == 1 ) {

          if ( g_All_Var.size() == 0 ) {
            for ( int j = 0 ; j < g_temp_Var.size() ; j++ ) {
              g_All_Var.push_back( g_temp_Var.at( j ) );
              tmp_token.token = g_temp_Var.at( j ).var_name;
              g_ID_table.push_back( tmp_token );
              PrintSucceed_Def_Var( 1, token, g_temp_Var.at( j ) );
              // --------------------------------------------------------
              if ( g_temp_Var.at( j ).type == "int" )
                g_one_Id_and_Type_and_Value._int = true;
              else if ( g_temp_Var.at( j ).type == "float" )
                g_one_Id_and_Type_and_Value._float = true;
              else if ( g_temp_Var.at( j ).type == "char" )
                g_one_Id_and_Type_and_Value._char = true;
              else if ( g_temp_Var.at( j ).type == "string" )
                g_one_Id_and_Type_and_Value._string = true;
              else if ( g_temp_Var.at( j ).type == "bool" )
                g_one_Id_and_Type_and_Value._bool = true;
              else if ( g_temp_Var.at( j ).type == "void" )
                g_one_Id_and_Type_and_Value._void = true;
              g_one_Id_and_Type_and_Value.id_name = g_temp_Var.at( j ).var_name;
              g_All_Var_and_Type_and_Value.push_back( g_one_Id_and_Type_and_Value );
              Clear_g_one_Id_and_Type_and_Value();
              // --------------------------------------------------------
            } // for

          } // if

          else {
            for ( int i = 0 ; i < g_temp_Var.size() ; i++ ) {
              for ( int j = 0 ; j < g_All_Var.size() ; j++ ) {
                if ( g_temp_Var.at( i ).var_name == g_All_Var.at( j ).var_name ) {
                  tmp_bool = true;
                  g_All_Var.erase( g_All_Var.begin() + j ); // New Def需刪掉舊定義的var
                } // if

              } // for

              if ( tmp_bool )
                PrintSucceed_Def_Var( 2, token, g_temp_Var.at( i ) );
              else
                PrintSucceed_Def_Var( 1, token, g_temp_Var.at( i ) );
              tmp_bool = false; // initialize

              // delete the old one
              for ( int k = 0 ; k < g_All_Var_and_Type_and_Value.size() ; k++ ) {
                if ( g_temp_Var.at( i ).var_name == g_All_Var_and_Type_and_Value.at( k ).id_name ) {
                  g_All_Var_and_Type_and_Value.erase( g_All_Var_and_Type_and_Value.begin() + k );
                  k = 1000; // force to leave the loop
                } // if

              } // for

            } // for

            for ( int j = 0 ; j < g_temp_Var.size() ; j++ ) { // 加上新def的var
              g_All_Var.push_back( g_temp_Var.at( j ) );
              tmp_token.token = g_temp_Var.at( j ).var_name;
              g_ID_table.push_back( tmp_token );
              // --------------------------------------------------------
              if ( g_temp_Var.at( j ).type == "int" )
                g_one_Id_and_Type_and_Value._int = true;
              else if ( g_temp_Var.at( j ).type == "float" )
                g_one_Id_and_Type_and_Value._float = true;
              else if ( g_temp_Var.at( j ).type == "char" )
                g_one_Id_and_Type_and_Value._char = true;
              else if ( g_temp_Var.at( j ).type == "string" )
                g_one_Id_and_Type_and_Value._string = true;
              else if ( g_temp_Var.at( j ).type == "bool" )
                g_one_Id_and_Type_and_Value._bool = true;
              else if ( g_temp_Var.at( j ).type == "void" )
                g_one_Id_and_Type_and_Value._void = true;
              g_one_Id_and_Type_and_Value.id_name = g_temp_Var.at( j ).var_name;
              g_All_Var_and_Type_and_Value.push_back( g_one_Id_and_Type_and_Value );
              Clear_g_one_Id_and_Type_and_Value();
              // --------------------------------------------------------
            } // for

            g_temp_Var.clear();
          } // else

        } // else if

      } // if

      else {
        if ( token.type == "Nothing" )
          PrintError( 1, token );
        else if ( token.type == "Identifier" && !Is_id_exist( token ) )
          PrintError( 3, token );
        else
          PrintError( 2, token );
        // 直接讀至這行結束為止
        g_now_error_lineNo = token.lineNo;
        while ( g_peekTokenNo < g_AllTokens.size() && token.lineNo == g_now_error_lineNo )
          token = PeekToken();
        // 把宣告失敗的global, local id刪掉(local想說不會這麼倒楣吧～～卡隱藏)
        g_ID_table_local.clear();
      } // else

    } // if

    else {
      g_getToken.clear(); // 每次用前先清空，以免存到上一次存過的
      g_temp_Var.clear(); // initialize
      g_temp_Func.others.clear(); // initialize
      g_ID_table_local.clear(); // initialize
      if ( Statement( token ) ) {
        GetToken( token );
        if ( !g_is_done_stmt ) {



          PrintSucceed_Stmt( token );
        } // if

      } // if

      else {
        if ( token.type == "Nothing" )
          PrintError( 1, token );
        else if ( token.type == "Identifier" && !Is_id_exist( token ) )
          PrintError( 3, token );
        else
          PrintError( 2, token );
        // 直接讀至這行結束為止
        g_now_error_lineNo = token.lineNo;
        while ( g_peekTokenNo < g_AllTokens.size() && token.lineNo == g_now_error_lineNo )
          token = PeekToken();
      } // else

    } // else

    // initialize
    g_one_Var.hasIndex = false;
    g_one_Var.index = "";
    g_one_Var.type = "";
    g_one_Var.var_name = "";
    g_temp_Var.clear();
    g_temp_Func.func_name = "";
    g_temp_Func.type = "";
    g_temp_Func.others.clear();
    g_ID_table_local.clear();
    g_getToken.clear();
    if ( !g_is_done_stmt ) {
      CutToken();
      token = PeekToken();
    } // if

  } // while

} // User_input()

bool Definition( Token &token, int &isVar_or_Func ) {
  /*
  definition
    :           VOID Identifier function_definition_without_ID
    | type_specifier Identifier function_definition_or_declarators
  */
  // 若isVar_or_Func是1則宣告的是var,若是2則是Func
  isVar_or_Func = 0; // default

  if ( token.type == "VOID" ) {
    GetToken( token );
    token = PeekToken();
    if ( token.type == "Identifier" ) {
      GetToken( token );
      token = PeekToken();
      if ( token.type == "(" ) {
        if ( Function_definition_without_ID( token ) ) {
          isVar_or_Func = 2;
          // 刪除所有local id
          g_ID_table_local.clear();
          return true;
        } // if

        else
          return false;
      } // if

      else
        return false;
    } // if

    else
      return false;
  } // if

  else if ( token.type == "INT" || token.type == "CHAR" || token.type == "FLOAT" ||
            token.type == "STRING" || token.type == "BOOL" ) {
    if ( Type_specifier( token ) ) {
      g_one_Var.type = token.token; // 可能是var/func的宣告
      GetToken( token );
      token = PeekToken();
      if ( token.type == "Identifier" ) {
        g_one_Var.var_name = token.token; // 可能是var/func的宣告
        GetToken( token );
        token = PeekToken();
        if ( token.type == "(" || token.type == "[" || token.type == "," || token.type == ";" ) {
          if ( token.type == "(" )
            isVar_or_Func = 2;
          else
            isVar_or_Func = 1;
          if ( Function_definition_or_declarators( token ) ) {
            // 刪除所有local id
            if ( isVar_or_Func == 2 )
              g_ID_table_local.clear();
            return true;
          } // if

          else
            return false;
        } // if

        else
          return false;
      } // if

      else
        return false;
    } // if

    else
      return false;
  } // else if

  else
    return false;

} // Definition()

bool Type_specifier( Token &token ) {
/*
type_specifier
    : INT | CHAR | FLOAT | STRING | BOOL
*/
  if ( token.type == "INT" || token.type == "CHAR" || token.type == "FLOAT" ||
       token.type == "STRING" || token.type == "BOOL" )
    return true;
  else
    return false;
} // Type_specifier()

bool Function_definition_or_declarators( Token &token ) {
/*
function_definition_or_declarators
    : function_definition_without_ID
    | rest_of_declarators
*/
  // IsVar_or_Func: 1 is var, 2 is func
  // vec_var: records of vars' names
  // vec_func: records of all things about this func (eg. (xxx){xxxx} )

  if ( token.type == "(" ) {
    if ( Function_definition_without_ID( token ) ) {
      g_temp_Var.clear(); // g_temp_Var同時存global跟local，要刪local
      g_ID_table_local.clear(); // delete local
      return true;
    } // if

    else
      return false;
  } // if

  else if ( token.type == "[" || token.type == "," || token.type == ";" ) {
    if ( Rest_of_declarators( token ) ) {
      return true;
    } // if

    else
      return false;
  } // else if

  else
    return false;
} // Function_definition_or_declarators()

bool Rest_of_declarators( Token &token ) {
/*
  rest_of_declarators
    : [ '[' Constant ']' ]
      { ',' Identifier [ '[' Constant ']' ] } ';'
*/
  if ( token.type == "[" ) {
    GetToken( token );
    token = PeekToken();
    if ( token.type == "Constant" ) {
      g_one_Var.index = token.token;
      GetToken( token );
      token = PeekToken();
      if ( token.type == "]" ) {
        g_one_Var.hasIndex = true;
        GetToken( token );
        token = PeekToken();
        while ( token.type == "," ) {
          g_temp_Var.push_back( g_one_Var );
          g_one_Var.hasIndex = false; // initialize(important)!!!
          g_one_Var.index = ""; // initialize(important)!!!
          GetToken( token );
          token = PeekToken();
          if ( token.type == "Identifier" ) {
            g_one_Var.var_name = token.token;
            GetToken( token );
            token = PeekToken();
            if ( token.type == "[" ) {
              GetToken( token );
              token = PeekToken();
              if ( token.type == "Constant" ) {
                g_one_Var.index = token.token;
                GetToken( token );
                token = PeekToken();
                if ( token.type == "]" ) {
                  g_one_Var.hasIndex = true;
                  GetToken( token );
                  token = PeekToken();
                } // if

                else
                  return false;
              } // if

              else
                return false;
            } // if

            if ( token.type == ";" ) {
              g_temp_Var.push_back( g_one_Var );
              return true;
            } // if

            else if ( token.type == "," )
              ;
            else
              return false;
          } // if

          else
            return false;
        } // while

        if ( token.type == ";" ) {
          g_temp_Var.push_back( g_one_Var );
          return true;
        } // if

        else
          return false;
      } // if

      else
        return false;
    } // if

    else
      return false;
  } // if

  else if ( token.type == "," ) {
    while ( token.type == "," ) {
      g_temp_Var.push_back( g_one_Var );
      g_one_Var.hasIndex = false; // initialize(important)!!!
      g_one_Var.index = ""; // initialize(important)!!!
      GetToken( token );
      token = PeekToken();
      if ( token.type == "Identifier" ) {
        g_one_Var.var_name = token.token;
        GetToken( token );
        token = PeekToken();
        if ( token.type == "[" ) {
          GetToken( token );
          token = PeekToken();
          if ( token.type == "Constant" ) {
            g_one_Var.index = token.token;
            GetToken( token );
            token = PeekToken();
            if ( token.type == "]" ) {
              g_one_Var.hasIndex = true;
              GetToken( token );
              token = PeekToken();
            } // if

            else
              return false;
          } // if

          else
            return false;
        } // if

        else if ( token.type == ";" ) {
          g_temp_Var.push_back( g_one_Var );
          return true;
        } // else if

      } // if

      else
        return false;
    } // while

    if ( token.type == ";" ) {
      g_temp_Var.push_back( g_one_Var );
      return true;
    } // if

    else
      return false;
  } // else if

  else if ( token.type == ";" ) {
    g_temp_Var.push_back( g_one_Var );
    return true;
  } // else if

  else
    return false;


} // Rest_of_declarators()

bool Function_definition_without_ID( Token &token ) {
/*
function_definition_without_ID
    : '(' [ VOID | formal_parameter_list ] ')' compound_statement
*/
  if ( token.type == "(" ) {
    GetToken( token );
    token = PeekToken();
    if ( token.type == "VOID"  ) {
      GetToken( token );
      token = PeekToken();
      if ( token.type == ")" ) {
        GetToken( token );
        token = PeekToken();
        if ( token.type == "{" ) {
          if ( Compound_statement( token ) ) {
            g_temp_Func.type = g_getToken.at( 0 ).token;
            g_temp_Func.func_name = g_getToken.at( 1 ).token;
            for ( int i = 2 ; i < g_getToken.size() ; i++ )
              g_temp_Func.others.push_back( g_getToken.at( i ).token );
            g_getToken.clear();
            return true;
          } // if

          else
            return false;
        } // if

        else
          return false;
      } // if

      else
        return false;
    } // if

    else if ( token.type == "INT" || token.type == "CHAR" || token.type == "FLOAT" ||
              token.type == "STRING" || token.type == "BOOL" ) {
      if ( Formal_parameter_list( token ) ) {
        GetToken( token );
        token = PeekToken();
        if ( token.type == ")" ) {
          GetToken( token );
          token = PeekToken();
          if ( token.type == "{" ) {
            if ( Compound_statement( token ) ) {
              g_temp_Func.type = g_getToken.at( 0 ).token;
              g_temp_Func.func_name = g_getToken.at( 1 ).token;
              for ( int i = 2 ; i < g_getToken.size() ; i++ )
                g_temp_Func.others.push_back( g_getToken.at( i ).token );
              g_getToken.clear();
              return true;
            } // if

            else
              return false;
          } // if

          else
            return false;
        } // if

        else
          return false;
      } // if

      else
        return false;
    } // else if

    else if ( token.type == ")" ) {
      GetToken( token );
      token = PeekToken();
      if ( token.type == "{" ) {
        if ( Compound_statement( token ) ) {
          g_temp_Func.type = g_getToken.at( 0 ).token;
          g_temp_Func.func_name = g_getToken.at( 1 ).token;
          for ( int i = 2 ; i < g_getToken.size() ; i++ )
            g_temp_Func.others.push_back( g_getToken.at( i ).token );
          g_getToken.clear();
          return true;
        } // if

        else
          return false;
      } // if

      else
        return false;
    } // else if

    else
      return false;
  } // if

  else
    return false;

} // Function_definition_without_ID()

bool Formal_parameter_list( Token &token ) {
/*
  formal_parameter_list
    : type_specifier [ '&' ] Identifier [ '[' Constant ']' ]
      { ',' type_specifier [ '&' ] Identifier [ '[' Constant ']' ] }
*/
  bool tmp = false;
  if ( token.type == "INT" || token.type == "FLOAT" || token.type == "CHAR" ||
       token.type == "BOOL" || token.type == "STRING" ) {
    if ( Type_specifier( token ) ) {
      GetToken( token );
      token = PeekToken();
      if ( token.type == "&" ) {
        GetToken( token );
        token = PeekToken();
        if ( token.type == "Identifier" ) {
          StoreLocalID( token );
          GetToken( token );
          token = PeekToken();
          if ( token.type == "[" ) {
            GetToken( token );
            token = PeekToken();
            if ( token.type == "Constant" ) {
              GetToken( token );
              token = PeekToken();
              if ( token.type == "]" ) {
                GetToken( token );
                token = PeekToken();
                while ( token.type == "," ) {
                  GetToken( token );
                  token = PeekToken();
                  if ( token.type == "INT" || token.type == "FLOAT" || token.type == "CHAR" ||
                       token.type == "BOOL" || token.type == "STRING" ) {
                    if ( Type_specifier( token ) ) {
                      GetToken( token );
                      token = PeekToken();
                      if ( token.type == "&" ) {
                        GetToken( token );
                        token = PeekToken();
                        if ( token.type == "Identifier" ) {
                          StoreLocalID( token );
                          GetToken( token );
                          token = PeekToken();
                          if ( token.type == "[" ) {
                            GetToken( token );
                            token = PeekToken();
                            if ( token.type == "Constant" ) {
                              GetToken( token );
                              token = PeekToken();
                              if ( token.type == "]" )
                                return true;
                              else
                                return false;
                            } // if

                            else
                              return false;
                          } // if

                        } // if

                        else
                          return false;
                      } // if

                      else if ( token.type == "Identifier" ) {
                        StoreLocalID( token );
                        GetToken( token );
                        token = PeekToken();
                        if ( token.type == "[" ) {
                          GetToken( token );
                          token = PeekToken();
                          if ( token.type == "Constant" ) {
                            GetToken( token );
                            token = PeekToken();
                            if ( token.type == "]" )
                              ;
                            else
                              return false;
                          } // if

                          else
                            return false;
                        } // if

                      } // else if

                      else
                        return false;
                    } // if

                    else
                      return false;
                  } // if

                  else
                    return false;
                } // while

                EraseLastToken();
                g_peekTokenNo -= 2;
                token = PeekToken();
                return true;
              } // if

              else
                return false;
            } // if

            else
              return false;
          } // if

          else if ( token.type == "," ) {
            while ( token.type == "," ) {
              GetToken( token );
              token = PeekToken();
              if ( token.type == "INT" || token.type == "FLOAT" || token.type == "CHAR" ||
                   token.type == "BOOL" || token.type == "STRING" ) {
                if ( Type_specifier( token ) ) {
                  GetToken( token );
                  token = PeekToken();
                  if ( token.type == "&" ) {
                    GetToken( token );
                    token = PeekToken();
                    if ( token.type == "Identifier" ) {
                      StoreLocalID( token );
                      GetToken( token );
                      token = PeekToken();
                      if ( token.type == "[" ) {
                        GetToken( token );
                        token = PeekToken();
                        if ( token.type == "Constant" ) {
                          GetToken( token );
                          token = PeekToken();
                          if ( token.type == "]" ) {
                            GetToken( token );
                            token = PeekToken();
                          } // if

                          else
                            return false;
                        } // if

                        else
                          return false;
                      } // if


                      EraseLastToken();
                      g_peekTokenNo -= 2; // 多讀
                      token = PeekToken();
                      return true;


                    } // if

                    else
                      return false;
                  } // if

                  else if ( token.type == "Identifier" ) {
                    StoreLocalID( token );
                    GetToken( token );
                    token = PeekToken();
                    if ( token.type == "[" ) {
                      GetToken( token );
                      token = PeekToken();
                      if ( token.type == "Constant" ) {
                        GetToken( token );
                        token = PeekToken();
                        if ( token.type == "]" ) {
                          GetToken( token );
                          token = PeekToken();
                        } // if

                        else
                          return false;
                      } // if

                      else
                        return false;
                    } // if

                    EraseLastToken();
                    g_peekTokenNo -= 2; // 多讀
                    token = PeekToken();
                    return true;


                  } // else if

                  else
                    return false;
                } // if

                else
                  return false;
              } // if

              else
                return false;
            } // while

            EraseLastToken();
            g_peekTokenNo -= 2;
            token = PeekToken();
            return true;
          } // else if

          else {
            g_peekTokenNo -= 2;
            token = PeekToken();
            EraseLastToken();
            return true;
          } // else

        } // if

        else
          return false;
      } // if

      else if ( token.type == "Identifier" ) {
        StoreLocalID( token );
        GetToken( token );
        token = PeekToken();
        if ( token.type == "[" ) {
          GetToken( token );
          token = PeekToken();
          if ( token.type == "Constant" ) {
            GetToken( token );
            token = PeekToken();
            if ( token.type == "]" ) {
              GetToken( token );
              token = PeekToken();
              while ( token.type == "," ) {
                GetToken( token );
                token = PeekToken();
                if ( token.type == "INT" || token.type == "FLOAT" || token.type == "CHAR" ||
                     token.type == "BOOL" || token.type == "STRING" ) {
                  if ( Type_specifier( token ) ) {
                    GetToken( token );
                    token = PeekToken();
                    if ( token.type == "&" ) {
                      GetToken( token );
                      token = PeekToken();
                      if ( token.type == "Identifier" ) {
                        StoreLocalID( token );
                        GetToken( token );
                        token = PeekToken();
                        if ( token.type == "[" ) {
                          GetToken( token );
                          token = PeekToken();
                          if ( token.type == "Constant" ) {
                            GetToken( token );
                            token = PeekToken();
                            if ( token.type == "]" ) {
                              GetToken( token );
                              token = PeekToken();
                            } // if

                            else
                              return false;
                          } // if

                          else
                            return false;
                        } // if

                      } // if

                      else
                        return false;
                    } // if

                    else if ( token.type == "Identifier" ) {
                      StoreLocalID( token );
                      GetToken( token );
                      token = PeekToken();
                      if ( token.type == "[" ) {
                        GetToken( token );
                        token = PeekToken();
                        if ( token.type == "Constant" ) {
                          GetToken( token );
                          token = PeekToken();
                          if ( token.type == "]" ) {
                            GetToken( token );
                            token = PeekToken();
                          } // if

                          else
                            return false;
                        } // if

                        else
                          return false;
                      } // if

                    } // else if

                    else
                      return false;
                  } // if

                  else
                    return false;
                } // if

                else
                  return false;
              } // while

              EraseLastToken();
              g_peekTokenNo -= 2;
              token = PeekToken();
              return true;
            } // if

            else
              return false;
          } // if

          else
            return false;
        } // if

        else if ( token.type == "," ) {
          while ( token.type == "," ) {
            tmp = true;
            GetToken( token );
            token = PeekToken();
            if ( token.type == "INT" || token.type == "FLOAT" || token.type == "CHAR" ||
                 token.type == "BOOL" || token.type == "STRING" ) {
              if ( Type_specifier( token ) ) {
                GetToken( token );
                token = PeekToken();
                if ( token.type == "&" ) {
                  GetToken( token );
                  token = PeekToken();
                  if ( token.type == "Identifier" ) {
                    StoreLocalID( token );
                    GetToken( token );
                    token = PeekToken();
                    if ( token.type == "[" ) {
                      GetToken( token );
                      token = PeekToken();
                      if ( token.type == "Constant" ) {
                        GetToken( token );
                        token = PeekToken();
                        if ( token.type == "]" ) {
                          GetToken( token );
                          token = PeekToken();
                        } // if

                        else
                          return false;
                      } // if

                      else
                        return false;
                    } // if

                    EraseLastToken();
                    g_peekTokenNo -= 2; // 多讀
                    token = PeekToken();
                    return true;
                  } // if

                  else
                    return false;
                } // if

                else if ( token.type == "Identifier" ) {
                  StoreLocalID( token );
                  GetToken( token );
                  token = PeekToken();
                  if ( token.type == "[" ) {
                    GetToken( token );
                    token = PeekToken();
                    if ( token.type == "Constant" ) {
                      GetToken( token );
                      token = PeekToken();
                      if ( token.type == "]" ) {
                        GetToken( token );
                        token = PeekToken();
                      } // if

                      else
                        return false;
                    } // if

                    else
                      return false;
                  } // if

                  EraseLastToken();
                  g_peekTokenNo -= 2; // 多讀
                  token = PeekToken();
                  return true;
                } // else if

                else
                  return false;
              } // if

              else
                return false;
            } // if

            else
              return false;
          } // while

          EraseLastToken();
          g_peekTokenNo -= 2;
          token = PeekToken();
          return true;
        } // else if

        EraseLastToken();
        g_peekTokenNo -= 2;
        token = PeekToken();
        return true;

      } // else if

      else
        return false;
    } // if

    else
      return false;
  } // if
  else
    return false;


} // Formal_parameter_list()

bool Compound_statement( Token &token ) {
/*
compound_statement
    : '{' { declaration | statement } '}'
*/
  if ( token.type == "{" ) {
    GetToken( token );
    token = PeekToken();
    while ( token.type == "INT" || token.type == "CHAR" || token.type == "FLOAT" ||
            token.type == "STRING" || token.type == "BOOL" ||
            token.type == ";" || token.type == "Identifier" || token.type == "PP" ||
            token.type == "MM" ||
            token.type == "+" || token.type == "-" || token.type == "!" ||
            token.type == "Constant" || token.type == "(" ||
            token.type == "RETURN" || token.type == "{" || token.type == "IF" ||
            token.type == "WHILE" || token.type == "DO" ||
            token.type == "CIN" || token.type == "COUT" || token.type == "DONE" ) {
      if ( token.type == "INT" || token.type == "CHAR" || token.type == "FLOAT" ||
           token.type == "STRING" || token.type == "BOOL" ) {
        if ( Declaration( token ) )
          GetToken( token );

        else
          return false;
      } // if

      else {
        if ( Statement( token ) )
          GetToken( token );
        else
          return false;
      } // else

      token = PeekToken();
    } // while

    if ( token.type == "}" ) {
      GetToken( token );
      return true;
    } // if

    else
      return false;
  } // if

  else
    return false;

} // Compound_statement()

bool Declaration( Token &token ) {
// 只有compound_stmt()會呼叫到，會宣告區域變數，所以id要存在區域變數內
/*
declaration
    : type_specifier Identifier rest_of_declarators
*/
  Token one_token;
  if ( token.type == "INT" || token.token == "CHAR" || token.type == "BOOL" ||
       token.type == "STRING" || token.type == "FLOAT" ) {
    if ( Type_specifier( token ) ) {
      g_one_Var.type = token.token;
      GetToken( token );
      token = PeekToken();
      if ( token.type == "Identifier" ) {
        g_one_Var.var_name = token.token;
        GetToken( token );
        token = PeekToken();
        if ( token.type == "[" || token.type == "," || token.type == ";" ) {
          if ( Rest_of_declarators( token ) ) {

            if ( g_temp_Var.size() > 0 ) {
              for ( int i = 0 ; i < g_temp_Var.size() ; i++ ) {
                one_token.token = g_temp_Var.at( i ).var_name;
                g_ID_table_local.push_back( one_token );
              } // for

            } // if

            return true;
          } // if

          else
            return false;
        } // if

        else
          return false;
      } // if

      else
        return false;
    } // if

    else
      return false;
  } // if

  else
    return false;
} // Declaration()

bool Statement( Token &token ) {
// stmt()用bool是因為是遞迴
/*
statement
    : ';'     // the null statement
    | expression ';'  // expression here should not be empty
    | RETURN [ expression ] ';'
    | compound_statement
    | IF '(' expression ')' statement [ ELSE statement ]
    | WHILE '(' expression ')' statement
    | DO statement WHILE '(' expression ')' ';'
*/
  string var_name; // use for only one place, for templatly use
  string var_type; // use for only one place, for templatly use
  string func_name; // use for only one place, for templatly use
  int tmp_int = 0; // use for only one place, for templatly use
  bool t = false; // use for only one place, for templatly use
  bool tmp_bool = false; // use for only one place, for templatly use
  Token tmp_token; // use for only one place, for templatly use
  g_lineNo = token.lineNo;
  int tmp_int2 = 0;
  int record = 2; // 在ListFunction那，紀錄著每行前面要空幾格
  bool justoneline = false; // for ListFunction
  bool tmp_bool_2 = false;
  bool tmp_bool_3 = false;
  int tmp_int3 = 0, tmp_int4 = 0, tmp_int5 = 0;
  int getToken_start_num = 0; // use in stmt()--else if<expr>; , for setting id value

  // -------------------------------------------------------------------------------
  // use in <expr>; for setting id value
  int i_value;
  float f_value;
  char c_value;
  string s_value;
  bool b_value = false;
  bool _int = false, _float = false, _char = false, _string = false, _bool = false;
  Token tmp_token_2;
  int tmp_int_4 = 2;
  string tmp_str_2 = "";
  int num_of_LP = 0; // 計(的數量，推至)的數量
  Token tmp_token_3;
  // ------------------------------------------------------------------------------

  if ( token.type == "Nothing" )
    return false;

  else if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
    g_is_undefined_error = true;
    return false;
  } // else if

  else if ( token.type == "DONE" ) {
    GetToken( token );
    token = PeekToken();
    if ( token.type == "(" ) {
      GetToken( token );
      token = PeekToken();
      if ( token.type == "Identifier" || token.type == "Constant" || token.type == ")" ) {
        if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
          g_is_undefined_error = true;
          return false;
        } // if

        if ( token.type == "Identifier" || token.type == "Constant" ) {
          if ( Expression( token ) ) {
            GetToken( token );
            token = PeekToken();
            if ( token.type == ")" ) {
              GetToken( token );
              token = PeekToken();
              if ( token.type == ";" )
                return true;
              else if ( token.type == "+" || token.type == "-" || token.type == "*" ||
                        token.type == "/" || token.type == "%" || token.type == "PP" ||
                        token.type == "MM" ) {
                if ( Expression( token ) ) {
                  GetToken( token );
                  token = PeekToken();
                  if ( token.type == ";" )
                    return true;
                  else
                    return false;
                } // if

                else
                  return false;
              } // else if

              else
                return false;
            } // if

            else
              return false;
          } // if

          else
            return false;
        } // if

        else { // token.type == ")"
          GetToken( token );
          token = PeekToken();
          if ( token.type == ";" ) {
            g_is_done_stmt = true;
            return true;
          } // if

          else if ( token.type == "+" || token.type == "-" || token.type == "*" ||
                    token.type == "/" || token.type == "%" || token.type == "PP" ||
                    token.type == "MM" ) {
            if ( Expression( token ) ) {
              if ( token.type == ";" )
                return true;
              GetToken( token );
              token = PeekToken();
              if ( token.type == ";" )
                return true;
              else
                return false;
            } // if

            else
              return false;
          } // else if

          else
            return false;
        } // else

      } // if

      else
        return false;
    } // if

    else
      return false;
  } // else if

  else if ( token.type == "ListAllVariables" ) {
    token = PeekToken();
    if ( token.type == "(" ) {
      token = PeekToken();
      if ( token.type == ")" ) {
        token = PeekToken();
        if ( token.type == ";" ) {
          ListAllVariables();
          return true;
        } // if

        else
          return false;
      } // if

      else
        return false;
    } // if

    else
      return false;
  } // else if

  else if ( token.type == "ListVariable" ) {
    token = PeekToken();
    if ( token.type == "(" ) {
      token = PeekToken();
      if ( token.type == "Constant" && token.token.substr( 0, 1 ) == "\"" ) { // 避免是Cons但可能是數字情況發生
        tmp_token = token;
        var_name = token.token.substr( 1, token.token.size() - 2 ); // "abc" 取 abc
        token = PeekToken();
        if ( token.type == ")" ) {
          token = PeekToken();
          if ( token.type == ";" ) {
            for ( int i = 0 ; i < g_All_Var.size() ; i++ ) {
              if ( var_name == g_All_Var.at( i ).var_name ) {
                tmp_bool = true;
                tmp_int = i;
              } // if

            } // for


            if ( tmp_bool ) {
              if ( g_All_Var.at( tmp_int ).hasIndex )
                cout << g_All_Var.at( tmp_int ).type << " " << g_All_Var.at( tmp_int ).var_name << "[ "
                     << g_All_Var.at( tmp_int ).index << " ] ;" << endl;
              else
                cout << g_All_Var.at( tmp_int ).type << " " << g_All_Var.at( tmp_int ).var_name << " ;\n";
              return true;
            } // if

            else {
              g_is_undefined_token = true;
              g_undefined_token = var_name;
              token = tmp_token;
              return false; // undefined not unexpected
            } // else

          } // if

          else
            return false;
        } // if

        else
          return false;

      } // if

      else
        return false;
    } // if

    else
      return false;
  } // else if

  else if ( token.type == "ListAllFunctions" ) {
    token = PeekToken();
    if ( token.type == "(" ) {
      token = PeekToken();
      if ( token.type == ")" ) {
        token = PeekToken();
        if ( token.type == ";" ) {
          ListAllFunctions();
          return true;
        } // if

        else
          return false;
      } // if

      else
        return false;
    } // if

    else
      return false;


  } // else if

  else if ( token.type == "ListFunction" ) {
    token = PeekToken();
    if ( token.type == "(" ) {
      token = PeekToken();
      if ( token.type == "Constant" && token.token.substr( 0, 1 ) == "\"" ) { // 避免是Cons但可能是數字情況發生
        tmp_token = token;
        func_name = token.token.substr( 1, token.token.size() - 2 ); // "abc" 取 abc
        token = PeekToken();
        if ( token.type == ")" ) {
          token = PeekToken();
          if ( token.type == ";" ) {
            for ( int i = 0 ; i < g_All_Func.size() ; i++ ) {
              if ( func_name == g_All_Func.at( i ).func_name ) {
                tmp_bool = true;
                tmp_int = i;
              } // if

            } // for

            // 以下輸出func內容 ================================================================
            if ( tmp_bool ) {
              cout << g_All_Func.at( tmp_int ).type << " " << g_All_Func.at( tmp_int ).func_name;
              cout << g_All_Func.at( tmp_int ).others.at( 0 ); // (
              if ( g_All_Func.at( tmp_int ).others.at( 1 ) == ")" ) // check if it is }, then enter 2 space
                cout << "  " << g_All_Func.at( tmp_int ).others.at( 1 ) ;
              else
                cout << " " << g_All_Func.at( tmp_int ).others.at( 1 ) ; // else, only enter 1 space
              for ( tmp_int2 = 2 ; tmp_int2 < g_All_Func.at( tmp_int ).others.size() &&
                    g_All_Func.at( tmp_int ).others.at( tmp_int2 ) != "{" ; tmp_int2 ++ ) {
                if ( g_All_Func.at( tmp_int ).others.at( tmp_int2 ) == "[" ||
                     g_All_Func.at( tmp_int ).others.at( tmp_int2 ) == "," )
                  cout << g_All_Func.at( tmp_int ).others.at( tmp_int2 ) ;
                else if ( g_All_Func.at( tmp_int ).others.at( tmp_int2 ) == "&" ) {
                  cout << " " << g_All_Func.at( tmp_int ).others.at( tmp_int2 );
                  tmp_int2++;
                  cout << g_All_Func.at( tmp_int ).others.at( tmp_int2 );
                } // else if

                else
                  cout << " " << g_All_Func.at( tmp_int ).others.at( tmp_int2 ) ;
              } // for

              cout << " " << g_All_Func.at( tmp_int ).others.at( tmp_int2 ) << "\n"; // {
              // 以上全為func定義第一行


              for ( int i =  tmp_int2 + 1 ; i < g_All_Func.at( tmp_int ).others.size() ; i++ ) {


                if ( g_All_Func.at( tmp_int ).others.at( i ) == "if" ||
                     g_All_Func.at( tmp_int ).others.at( i ) == "while" ) {
                  for ( int x = 1 ; x < record ; x++ )
                    cout << " ";
                  cout << " " << g_All_Func.at( tmp_int ).others.at( i ); // if or while
                  tmp_int3 = i + 1;
                  tmp_int4 = 0;
                  for ( i = i + 1 ; i < g_All_Func.at( tmp_int ).others.size() &&
                        g_All_Func.at( tmp_int ).others.at( i ) != "{" &&
                        g_All_Func.at( tmp_int ).others.at( i ) != ";" ; i++ ) {
                    if ( g_All_Func.at( tmp_int ).others.at( i ) == ")" )
                      tmp_int4++; // use tmp_int4 to record the numbers of )
                  } // for

                  if ( g_All_Func.at( tmp_int ).others.at( i ) == "{" ) {
                    for ( int j = 0, i = tmp_int3 ; j < tmp_int4 &&
                          i < g_All_Func.at( tmp_int ).others.size() ; i++ ) {
                      if ( g_All_Func.at( tmp_int ).others.at( i ) == "[" ||
                           g_All_Func.at( tmp_int ).others.at( i ) == "++" ||
                           g_All_Func.at( tmp_int ).others.at( i ) == "--" )
                        cout << g_All_Func.at( tmp_int ).others.at( i ); // [ ++ -- are special
                      else if ( g_All_Func.at( tmp_int ).others.at( i ) == ")" ) {
                        cout << " " << g_All_Func.at( tmp_int ).others.at( i );
                        j++;
                      } // else if

                      else if ( g_All_Func.at( tmp_int ).others.at( i ) == "(" ) {
                        if ( Is_identifier( g_All_Func.at( tmp_int ).others.at( i - 1 ) ) )
                          cout << g_All_Func.at( tmp_int ).others.at( i );
                        else
                          cout << " " << g_All_Func.at( tmp_int ).others.at( i );
                      } // else if

                      else
                        cout << " " << g_All_Func.at( tmp_int ).others.at( i );
                    } // for

                    cout << " " << g_All_Func.at( tmp_int ).others.at( i ) << endl; // {
                    record += 2;
                  } // if

                  else {
                    for ( int j = 0, i = tmp_int3 ; j < tmp_int4 &&
                          i < g_All_Func.at( tmp_int ).others.size() ; i++ ) {
                      if ( g_All_Func.at( tmp_int ).others.at( i ) == "[" ||
                           g_All_Func.at( tmp_int ).others.at( i ) == "++" ||
                           g_All_Func.at( tmp_int ).others.at( i ) == "--" )
                        cout << g_All_Func.at( tmp_int ).others.at( i ); // [ ++ -- are special
                      else if ( g_All_Func.at( tmp_int ).others.at( i ) == ")" ) {
                        cout << " " << g_All_Func.at( tmp_int ).others.at( i );
                        j++;
                      } // else if

                      else if ( g_All_Func.at( tmp_int ).others.at( i ) == "(" ) {
                        if ( Is_identifier( g_All_Func.at( tmp_int ).others.at( i - 1 ) ) )
                          cout << g_All_Func.at( tmp_int ).others.at( i );
                        else
                          cout << " " << g_All_Func.at( tmp_int ).others.at( i );
                      } // else if

                      else
                        cout << " " << g_All_Func.at( tmp_int ).others.at( i );
                      tmp_int5 = i;
                    } // for

                    if ( g_All_Func.at( tmp_int ).others.at( tmp_int5 + 1 ) == ";" ) {
                      cout << " " << g_All_Func.at( tmp_int ).others.at( i ) << endl;
                    } // if

                    else {
                      cout << endl;
                      record += 2;
                      for ( int x = 1 ; x < record ; x++ )
                        cout << " ";

                      for ( i = tmp_int5 + 1 ; i < g_All_Func.at( tmp_int ).others.size() &&
                            g_All_Func.at( tmp_int ).others.at( i ) != ";" ; i++ ) {
                        if ( g_All_Func.at( tmp_int ).others.at( i ) == "[" ||
                             g_All_Func.at( tmp_int ).others.at( i ) == "++" ||
                             g_All_Func.at( tmp_int ).others.at( i ) == "--" )
                          cout << g_All_Func.at( tmp_int ).others.at( i ); // [ ++ -- are special
                        else if ( g_All_Func.at( tmp_int ).others.at( i ) == "(" ) {
                          if ( Is_identifier( g_All_Func.at( tmp_int ).others.at( i - 1 ) ) )
                            cout << g_All_Func.at( tmp_int ).others.at( i );
                          else
                            cout << " " << g_All_Func.at( tmp_int ).others.at( i );
                        } // else if

                        else
                          cout << " " << g_All_Func.at( tmp_int ).others.at( i );
                      } // for

                      cout << " " << g_All_Func.at( tmp_int ).others.at( i ) << endl; // ;
                      record -= 2;
                    } // else

                  } // else

                  tmp_int3 = 0;
                  tmp_int4 = 0;
                  tmp_int5 = 0;
                } // if

                else if ( g_All_Func.at( tmp_int ).others.at( i ) == "else" ) {
                  for ( int x = 1 ; x < record ; x++ )
                    cout << " ";
                  cout << " " << g_All_Func.at( tmp_int ).others.at( i ); // else
                  i++;
                  if ( g_All_Func.at( tmp_int ).others.at( i ) == ";" )
                    cout << " " << g_All_Func.at( tmp_int ).others.at( i ) << endl;

                  else if ( g_All_Func.at( tmp_int ).others.at( i ) == "{" ) {
                    cout << " " << g_All_Func.at( tmp_int ).others.at( i ) << endl;
                    record += 2;
                  } // else if

                  else {
                    cout << endl;
                    record += 2;
                    for ( int x = 1 ; x < record ; x++ )
                      cout << " ";
                    for ( ; i < g_All_Func.at( tmp_int ).others.size() &&
                          g_All_Func.at( tmp_int ).others.at( i ) != ";" ; i++ ) {
                      if ( g_All_Func.at( tmp_int ).others.at( i ) == "[" ||
                           g_All_Func.at( tmp_int ).others.at( i ) == "++" ||
                           g_All_Func.at( tmp_int ).others.at( i ) == "--" )
                        cout << g_All_Func.at( tmp_int ).others.at( i ); // [ ++ -- are special
                      else if ( g_All_Func.at( tmp_int ).others.at( i ) == "(" ) {
                        if ( Is_identifier( g_All_Func.at( tmp_int ).others.at( i - 1 ) ) )
                          cout << g_All_Func.at( tmp_int ).others.at( i );
                        else
                          cout << " " << g_All_Func.at( tmp_int ).others.at( i );
                      } // else if

                      else
                        cout << " " << g_All_Func.at( tmp_int ).others.at( i );
                    } // for

                    cout << " " << g_All_Func.at( tmp_int ).others.at( i ) << endl; // ;
                    record -= 2;
                  } // else

                } // else if

                else if ( g_All_Func.at( tmp_int ).others.at( i ) == "do" ) {
                  for ( int x = 1 ; x < record ; x++ )
                    cout << " ";
                  cout << " " << g_All_Func.at( tmp_int ).others.at( i );
                  if ( i + 1 < g_All_Func.at( tmp_int ).others.size() &&
                       g_All_Func.at( tmp_int ).others.at( i + 1 ) == "{" ) {
                    cout << " " << g_All_Func.at( tmp_int ).others.at( i + 1 ) << endl;
                    record += 2;
                    i++;
                  } // if

                  else {
                    i = i + 1 ;
                    for ( ; i < g_All_Func.at( tmp_int ).others.size() &&
                          g_All_Func.at( tmp_int ).others.at( i ) != ";" ; i++ ) {
                      if ( g_All_Func.at( tmp_int ).others.at( i ) == "[" ||
                           g_All_Func.at( tmp_int ).others.at( i ) == "++" ||
                           g_All_Func.at( tmp_int ).others.at( i ) == "--" )
                        cout << g_All_Func.at( tmp_int ).others.at( i );
                      else if ( g_All_Func.at( tmp_int ).others.at( i ) == "(" ) {
                        if ( Is_identifier( g_All_Func.at( tmp_int ).others.at( i - 1 ) ) )
                          cout << g_All_Func.at( tmp_int ).others.at( i );
                        else
                          cout << " " << g_All_Func.at( tmp_int ).others.at( i );
                      } // else if

                      else
                        cout << " " << g_All_Func.at( tmp_int ).others.at( i );
                    } // for

                    cout << " " << g_All_Func.at( tmp_int ).others.at( i ) << endl;
                  } // else

                } // else if

                else if ( g_All_Func.at( tmp_int ).others.at( i ) == "}" ) {
                  record -= 2;
                  for ( int x = 1 ; x < record ; x++ )
                    cout << " ";
                  if ( i == g_All_Func.at( tmp_int ).others.size() - 1 ) // if } is the last one char
                    cout << g_All_Func.at( tmp_int ).others.at( i ) << endl;
                  else
                    cout << " " << g_All_Func.at( tmp_int ).others.at( i ) << endl;
                } // else if

                else {
                  for ( int x = 1 ; x < record ; x++ )
                    cout << " ";
                  for ( ; i < g_All_Func.at( tmp_int ).others.size() &&
                        g_All_Func.at( tmp_int ).others.at( i ) != ";" ; i++ ) {
                    if ( g_All_Func.at( tmp_int ).others.at( i ) == "[" ||
                         g_All_Func.at( tmp_int ).others.at( i ) == "++" ||
                         g_All_Func.at( tmp_int ).others.at( i ) == "--" )
                      cout << g_All_Func.at( tmp_int ).others.at( i );
                    else if ( g_All_Func.at( tmp_int ).others.at( i ) == "(" ) {
                      if ( Is_identifier( g_All_Func.at( tmp_int ).others.at( i - 1 ) ) )
                        cout << g_All_Func.at( tmp_int ).others.at( i );
                      else
                        cout << " " << g_All_Func.at( tmp_int ).others.at( i );
                    } // else if

                    else if ( g_All_Func.at( tmp_int ).others.at( i ) == "&" ) {
                      cout << " " << g_All_Func.at( tmp_int ).others.at( i );
                      i++;
                      cout << g_All_Func.at( tmp_int ).others.at( i );
                    } // else if

                    else
                      cout << " " << g_All_Func.at( tmp_int ).others.at( i );
                  } // for

                  cout << " " << g_All_Func.at( tmp_int ).others.at( i ) << endl;
                } // else

              } // for

              return true;
            } // if

            else {
              g_is_undefined_token = true;
              g_undefined_token = func_name;
              token = tmp_token;
              return false; // undefined not unexpected
            } // else

          } // if

          else
            return false;
        } // if

        else
          return false;

      } // if

      else
        return false;
    } // if

    else
      return false;
  } // else if

  else if ( token.type == "CIN" ) {
    GetToken( token );
    token = PeekToken();
    if ( token.type == "RS" ) {
      while ( token.type == "RS" ) {
        GetToken( token );
        token = PeekToken();
        if ( token.type == "Identifier" || token.type == "PP" || token.type == "MM" ||
             token.type == "+" || token.type == "-" || token.type == "!" ||
             token.type == "Constant" || token.type == "(" ) {
          if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
            g_is_undefined_error = true;
            return false;
          } // if

          if ( Expression( token ) ) {
            GetToken( token );
            token = PeekToken();
          } // if

          else
            return false;
        } // if

        else
          return false;
      } // while

      if ( token.type == ";" ) {
        return true;
      } // if

      else
        return false;
    } // if

    else
      return false;
  } // else if

  else if ( token.type == "COUT" ) {
    GetToken( token );
    token = PeekToken();
    if ( token.type == "LS" ) {
      while ( token.type == "LS" ) {
        GetToken( token );
        token = PeekToken();
        if ( token.type == "Identifier" || token.type == "PP" || token.type == "MM" ||
             token.type == "+" || token.type == "-" || token.type == "!" ||
             token.type == "Constant" || token.type == "(" ) {
          if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
            g_is_undefined_error = true;
            return false;
          } // if

          if ( Expression( token ) ) {
            GetToken( token );
            token = PeekToken();
          } // if

          else
            return false;
        } // if

        else
          return false;
      } // while

      if ( token.type == ";" ) {
        return true;
      } // if

      else
        return false;
    } // if

    else
      return false;
  } // else if

  else if ( token.type == ";" ) {
    return true;
  } // else if

  else if ( token.type == "RETURN" ) {
    GetToken( token );
    token = PeekToken();
    if ( token.token == ";" ) // return ;
      return true;
    if ( token.type == "Identifier" || token.type == "PP" || token.type == "MM" ||
         token.type == "+" || token.type == "-" || token.type == "!" ||
         token.type == "Constant" || token.type == "(" ) {
      if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
        g_is_undefined_error = true;
        return false;
      } // if

      if ( Expression( token ) ) {
        GetToken( token );
        token = PeekToken();
        if ( token.type == ";" ) {
          return true;
        } // if

        else
          return false;
      } // if

      else
        return false;
    } // if

    else
      return false;
  } // else if

  else if ( token.type == "IF" ) {
    GetToken( token );
    token = PeekToken();
    if ( token.type == "(" ) {
      GetToken( token );
      token = PeekToken();
      if ( token.type == "Identifier" || token.type == "PP" || token.type == "MM" ||
           token.type == "+" || token.type == "-" || token.type == "!" ||
           token.type == "Constant" || token.type == "(" ) {
        if ( Expression( token ) ) {
          GetToken( token );
          token = PeekToken();
          if ( token.type == ")" ) {
            GetToken( token );
            token = PeekToken();
            if ( token.type == ";" || token.type == "Identifier" || token.type == "PP" ||
                 token.type == "MM" || token.type == "+" || token.type == "-" ||
                 token.type == "!" || token.type == "Constant" || token.type == "(" ||
                 token.type == "RETURN" || token.type == "{" || token.type == "IF" ||
                 token.type == "WHILE" || token.type == "DO" ||
                 token.type == "CIN" || token.type == "COUT" || token.type == "DONE" ) {
              if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
                g_is_undefined_error = true;
                return false;
              } // if

              if ( token.type == "{" )
                tmp_bool_3 = true;
              if ( Statement( token ) ) {
                if ( !tmp_bool_3 )
                  GetToken( token );
                tmp_bool_3 = false; // initial
                token = PeekToken();
                if ( token.type == "ELSE" ) {
                  GetToken( token );
                  token = PeekToken();
                  if ( token.type == ";" || token.type == "Identifier" || token.type == "PP" ||
                       token.type == "MM" || token.type == "+" || token.type == "-" ||
                       token.type == "!" || token.type == "Constant" || token.type == "(" ||
                       token.type == "RETURN" || token.type == "{" || token.type == "IF" ||
                       token.type == "WHILE" || token.type == "DO" ||
                       token.type == "CIN" || token.type == "COUT" || token.type == "DONE" ) {
                    if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
                      g_is_undefined_error = true;
                      return false;
                    } // if

                    if ( token.type == "{" )
                      tmp_bool_3 = true;
                    if ( Statement( token ) ) {
                      if ( tmp_bool_3 )
                        EraseLastToken();
                      tmp_bool_3 = false; // initial
                      return true;
                    } // if

                    else
                      return false;
                  } // if

                  else
                    return false;
                } // if

                else {
                  EraseLastToken();
                  g_peekTokenNo -= 2;
                  token = PeekToken();
                  return true;
                } // else

              } // if

              else
                return false;
            } // if

            else
              return false;
          } // if

          else
            return false;
        } // if

        else
          return false;
      } // if

      else
        return false;
    } // if

    else
      return false;

  } // else if

  else if ( token.type == "WHILE" ) {
    GetToken( token );
    token = PeekToken();
    if ( token.type == "(" ) {
      GetToken( token );
      token = PeekToken();
      if ( token.type == "Identifier" || token.type == "PP" || token.type == "MM" ||
           token.type == "+" || token.type == "-" || token.type == "!" ||
           token.type == "Constant" || token.type == "(" ) {
        if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
          g_is_undefined_error = true;
          return false;
        } // if

        if ( Expression( token ) ) {
          GetToken( token );
          token = PeekToken();
          if ( token.type == ")" ) {
            GetToken( token );
            token = PeekToken();
            if ( token.type == ";" || token.type == "Identifier" || token.type == "PP" ||
                 token.type == "MM" || token.type == "+" || token.type == "-" ||
                 token.type == "!" || token.type == "Constant" || token.type == "(" ||
                 token.type == "RETURN" || token.type == "{" || token.type == "IF" ||
                 token.type == "WHILE" || token.type == "DO" ||
                 token.type == "CIN" || token.type == "COUT" || token.type == "DONE" ) {
              if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
                g_is_undefined_error = true;
                return false;
              } // if

              if ( token.type == "{" )
                tmp_bool_3 = true;
              if ( Statement( token ) ) {
                if ( tmp_bool_3 )
                  EraseLastToken();

                tmp_bool_3 = false; // initial
                return true;
              } // if

              else
                return false;
            } // if

            else
              return false;
          } // if

          else
            return false;
        } // if

        else
          return false;
      } // if

      else
        return false;
    } // if

    else
      return false;
  } // else if

  else if ( token.type == "DO" ) {
    GetToken( token );
    token = PeekToken();
    if ( token.type == ";" || token.type == "Identifier" || token.type == "PP" ||
         token.type == "MM" || token.type == "+" || token.type == "-" ||
         token.type == "!" || token.type == "Constant" || token.type == "(" ||
         token.type == "RETURN" || token.type == "{" || token.type == "IF" ||
         token.type == "WHILE" || token.type == "DO" ||
         token.type == "CIN" || token.type == "COUT" || token.type == "DONE" ) {
      if ( token.type == "Identifier" && !Is_id_exist( token ) )
        return false;
      if ( token.type == "{" )
        tmp_bool_3 = true;
      if ( Statement( token ) ) {
        if ( !tmp_bool_3 )
          GetToken( token );
        tmp_bool_3 = false; // initial
        token = PeekToken();
        if ( token.type == "WHILE" ) {
          GetToken( token );
          token = PeekToken();
          if ( token.type == "(" ) {
            GetToken( token );
            token = PeekToken();
            if ( token.type == "Identifier" || token.type == "PP" || token.type == "MM" ||
                 token.type == "+" || token.type == "-" ||
                 token.type == "!" || token.type == "Constant" || token.type == "(" ) {
              if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
                g_is_undefined_error = true;
                return false;
              } // if

              if ( Expression( token ) ) {
                GetToken( token );
                token = PeekToken();
                if ( token.type == ")" ) {
                  GetToken( token );
                  token = PeekToken();
                  if ( token.type == ";" )
                    return true;
                  else
                    return false;
                } // if

                else
                  return false;
              } // if

              else
                return false;
            } // if

            else
              return false;

          } // if

          else
            return false;
        } // if

        else
          return false;
      } // if

      else
        return false;
    } // if

    else
      return false;
  } // else if

  else if ( token.type == "{" ) {
    if ( token.type == "{" ) {
      if ( Compound_statement( token ) ) {
        return true;
      } // if

      else
        return false;
    } // if

    else
      return false;
  } // else if

  else if ( token.type == "Identifier" || token.type == "PP" || token.type == "MM" ||
            token.type == "+" || token.type == "-" ||
            token.type == "!" || token.type == "Constant" || token.type == "(" ) {

    // 設定getToken_start_num ===========================================================================
      if ( token.type == "Identifier" && g_getToken.size() > 0 )
        getToken_start_num = g_getToken.size() - 1;
      else if ( token.type == "Identifier" && g_getToken.size() == 0 )
        getToken_start_num = 0;
      else if ( token.type == "(" ) {
        getToken_start_num = 0 ; // initialize
        num_of_LP = 0; // initialize
        num_of_LP++;

        // 以下四行for()勿改，就是要算有幾個
        for ( int i = g_peekTokenNo ; i < g_AllTokens.size() &&
              g_AllTokens[i].type != "Identifier" ; i++ ) {
          getToken_start_num++;
          num_of_LP++;
        } // for

        getToken_start_num++;


      } // else if
      // 設定getToken_start_num ===========================================================================

    if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
      g_is_undefined_error = true;
      return false;
    } // if

    if ( Expression( token ) ) {
      GetToken( token );
      token = PeekToken();

      if ( token.type == ";" ) {

        // return true前先 update id value
        // 設定assign========================================================================================
        if ( getToken_start_num >= 0 &&
             ( g_getToken[ getToken_start_num ].type == "Identifier" ||
               Is_num( g_getToken[ getToken_start_num ].token ) ) ) {
          // examine which type the id is

          for ( int i = 0 ; i < g_All_Var_and_Type_and_Value.size() ; i++ ) {
            if ( g_getToken[getToken_start_num].token == g_All_Var_and_Type_and_Value[i].id_name ) {
              if ( g_All_Var_and_Type_and_Value[i]._int )
                _int = true;
              else if ( g_All_Var_and_Type_and_Value[i]._float )
                _float = true;
              else if ( g_All_Var_and_Type_and_Value[i]._char )
                _char = true;
              else if ( g_All_Var_and_Type_and_Value[i]._string )
                _string = true;
              else if ( g_All_Var_and_Type_and_Value[i]._bool )
                _bool = true;
            } // if

          } // for

          if ( _float ) {
            if ( g_getToken.size() > 1 && g_getToken[ getToken_start_num + 1 ].type == "=" ) { // 右assign給左
              tmp_token_2 = g_getToken[getToken_start_num]; // record before delete
              // throwaway id & =
              for ( int i = getToken_start_num + 2; i < g_getToken.size() ; i++ )
                g_vec_Token_for_Eval.push_back( g_getToken[i] );

              // 刪除後面幾個), id=前有幾個(就有幾個)
              for ( int i = 0 ; i < num_of_LP ; i++ )
                g_vec_Token_for_Eval.erase( g_vec_Token_for_Eval.end() - 1 );


              f_value = Eval( );

              // update its value( the most important part!! )
              for ( int w = 0 ; w < g_All_Var_and_Type_and_Value.size() ; w++ ) {
                if ( tmp_token_2.token == g_All_Var_and_Type_and_Value[w].id_name )
                  g_All_Var_and_Type_and_Value[w].f_value = f_value;
              } // for

            } // if

          } // if

          else if ( _int ) {
            if ( g_getToken.size() > 1 && g_getToken[ getToken_start_num + 1 ].type == "=" ) { // 右assign給左
              tmp_token_2 = g_getToken[getToken_start_num]; // record before delete

              // throwaway id & =
              for ( int i = getToken_start_num + 2; i < g_getToken.size() ; i++ )
                g_vec_Token_for_Eval.push_back( g_getToken[i] );

              // 刪除後面幾個), id=前有幾個(就有幾個)
              for ( int i = 0 ; i < num_of_LP ; i++ )
                g_vec_Token_for_Eval.erase( g_vec_Token_for_Eval.end() - 1 );

              i_value = Eval_int( );
              cout<<"update value = "<<i_value<<endl;

              // update its value( the most important part!! )
              for ( int w = 0 ; w < g_All_Var_and_Type_and_Value.size() ; w++ ) {
                if ( tmp_token_2.token == g_All_Var_and_Type_and_Value[w].id_name )
                  g_All_Var_and_Type_and_Value[w].i_value = i_value;
              } // for

            } // if

          } // else if

          else if ( _char ) {

            // c; or c=xxx;
            // c = ((...(('x'))...));

            if ( g_getToken[ getToken_start_num + 1 ].token == "=" ) {

              if ( g_getToken[ getToken_start_num + 2 ].token != "(" )
                tmp_int_4 = getToken_start_num + 2;
              else {
                for ( int k = getToken_start_num + 2 ; k < g_getToken.size() && g_getToken[ k ].token == "(" ; k++ )
                  tmp_int_4 = k; // use tmp_int_4 to record, and it needs to add 1 later
                tmp_int_4++;
              } // else

              if ( g_getToken[ tmp_int_4 ].token.c_str()[1] == '\\' ) { // '\n' , '\t' etc
                if ( g_getToken[ tmp_int_4 ].token.c_str()[2] == 't' )
                  c_value = '\t';
                else if ( g_getToken[ tmp_int_4 ].token.c_str()[2] == 'n' )
                  c_value = '\n';
                else if ( g_getToken[ tmp_int_4 ].token.c_str()[2] == 'r' )
                  c_value = '\r';
              } // if

              else // normal char, like 'c' , 'a' , ' ' etc
                c_value = g_getToken[ tmp_int_4 ].token.c_str()[1];

              // update its value( the most important part!! )
              for ( int k = 0 ; k < g_All_Var_and_Type_and_Value.size() ; k++ ) {
                if ( g_All_Var_and_Type_and_Value[k].id_name == g_getToken[ getToken_start_num ].token )
                  g_All_Var_and_Type_and_Value[k].c_value = c_value;
              } // for

            } // if

          } // else if

          else if ( _string ) {
            // s; s=s1;
            // s+s1+s2; s=s+s1+s2;
            // string直接用.type == constant做判斷，因為測資不會有coersion error
            if ( g_getToken[ getToken_start_num + 1 ].token == "=" ) { // else 不是設定就不管了
              if ( g_getToken[ getToken_start_num + 2 ].type == "Constant" ||
                   g_getToken[ getToken_start_num + 2 ].type == "Identifier" ) {
                if ( g_getToken[ getToken_start_num + 2 ].type == "Identifier" ) {
                  for ( int w = 0 ; w < g_All_Var_and_Type_and_Value.size() ; w++ ) {
                    if ( g_getToken[ getToken_start_num + 2 ].token == g_All_Var_and_Type_and_Value[w].id_name )
                      s_value = g_All_Var_and_Type_and_Value[w].s_value;
                  } // for

                } // if

                else
                  s_value.assign( g_getToken[ getToken_start_num + 2 ].token, 1,
                                  g_getToken[ getToken_start_num + 2 ].token.length() - 2 );

                // means from 1 to i-1

                for ( int k = getToken_start_num + 3 ; g_getToken.size() - getToken_start_num > 3 &&
                      g_getToken[ k ].token == "+" &&
                      k + 1 < g_getToken.size() ; k++ ) {
                  k++;
                  if ( g_getToken[ k ].type == "Identifier" ) {
                    for ( int w = 0 ; w < g_All_Var_and_Type_and_Value.size() ; w++ ) {
                      if ( g_getToken[ k ].token == g_All_Var_and_Type_and_Value[w].id_name )
                        tmp_str_2 = g_All_Var_and_Type_and_Value[w].s_value;
                    } // for

                  } // if

                  else if ( g_getToken[ k ].type == "Constant" )
                    tmp_str_2.assign( g_getToken[ k ].token, 1, g_getToken[ k ].token.length() - 2 );

                  s_value = s_value + tmp_str_2;
                  tmp_str_2 = ""; // initialize

                } // for



              } // if

              // update its value( the most important part!! )
              for ( int w = 0 ; w < g_All_Var_and_Type_and_Value.size() ; w++ ) {
                if ( g_getToken[ getToken_start_num ].token == g_All_Var_and_Type_and_Value[w].id_name )
                  g_All_Var_and_Type_and_Value[w].s_value = s_value;
              } // for

              tmp_str_2 = ""; // initialize

            } // if

          } // else if

          else if ( _bool ) {
            // ( b = f1 > f2 )
            // ( b = f1&&f2||f3 )
            if ( g_getToken.size() > 1 && g_getToken[ getToken_start_num + 1 ].type == "=" ) {
              tmp_token_2 = g_getToken[getToken_start_num]; // record before delete
              // throwaway id & =
              for ( int i = getToken_start_num + 2; i < g_getToken.size() ; i++ )
                g_vec_Token_for_Eval.push_back( g_getToken[i] );

              // 刪除後面幾個), id=前有幾個(就有幾個)
              for ( int i = 0 ; i < num_of_LP ; i++ )
                g_vec_Token_for_Eval.erase( g_vec_Token_for_Eval.end() - 1 );

              if ( g_vec_Token_for_Eval[0].type == "Identifier" ) {
                _int = false; // initial
                _float = false;
                _char = false;
                _string = false;
                _bool = false;
                for ( int i = 0 ; i < g_All_Var_and_Type_and_Value.size() ; i++ ) {
                  if ( g_vec_Token_for_Eval[0].token == g_All_Var_and_Type_and_Value[i].id_name ) {
                    if ( g_All_Var_and_Type_and_Value[i]._int )
                      _int = true;
                    else if ( g_All_Var_and_Type_and_Value[i]._float )
                      _float = true;
                    else if ( g_All_Var_and_Type_and_Value[i]._char )
                      _char = true;
                    else if ( g_All_Var_and_Type_and_Value[i]._string )
                      _string = true;
                    else if ( g_All_Var_and_Type_and_Value[i]._bool )
                      _bool = true;
                  } // if

                } // for

                if ( _int ) {

                } // if

                else if ( _float ) {

                } // else if

                else if ( _char ) {

                } // else if

                else if ( _string ) {

                } // else if

                else if ( _bool ) {

                } // else if

              } // if


            } // if


          } // else if

          _int = false; // initial
          _float = false;
          _char = false;
          _string = false;
          _bool = false;
          g_vec_Token_for_Eval.clear();
          num_of_LP = 0;
        } // if
        // 設定assign========================================================================================


        return true;
      } // if

      else
        return false;
    } // if

    else
      return false;
  } // else if

  else
    return false;

} // Statement()

bool Expression( Token &token ) {
/*
expression
    : basic_expression { ',' basic_expression }
*/
  bool tmp = false;
  if ( token.type == "Identifier" || token.type == "PP" || token.type == "MM" ||
       token.type == "+" || token.type == "-" || token.type == "!" ||
       token.type == "Constant" || token.type == "(" ) {
    if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
      g_is_undefined_error = true;
      return false;
    } // if

    if ( Basic_expression( token ) ) {
      GetToken( token );
      token = PeekToken();
      while ( token.type == "," ) {
        GetToken( token );
        token = PeekToken();
        if ( token.type == "Identifier" || token.type == "PP" || token.type == "MM" ||
             token.type == "+" || token.type == "-" || token.type == "!" ||
             token.type == "Constant" || token.type == "(" ) {
          if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
            g_is_undefined_error = true;
            return false;
          } // if

          if ( Basic_expression( token ) ) {
            GetToken( token );
            token = PeekToken();
          } // if

          else
            return false;
        } // if

        else
          return false;
      } // while

      g_peekTokenNo -= 2;
      token = PeekToken();
      EraseLastToken();
      return true;
    } // if

    else
      return false;
  } // if

  else
    return false;

} // Expression()

bool Basic_expression( Token &token ) {
/*
basic_expression
    : Identifier rest_of_Identifier_started_basic_exp
    | ( PP | MM ) Identifier rest_of_PPMM_Identifier_started_basic_exp
    | sign { sign } signed_unary_exp romce_and_romloe
    | ( Constant | '(' expression ')' ) romce_and_romloe
*/

  if ( token.type == "Identifier" ) {
    if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
      g_is_undefined_error = true;
      return false;
    } // if

    GetToken( token );
    token = PeekToken();
    if ( token.type == "[" || token.type == "=" || token.type == "TE" ||
         token.type == "DE" || token.type == "RE" || token.type == "PE" ||
         token.type == "ME" || token.type == "PP" || token.type == "MM" ||
         token.type == "(" || token.type == "*" || token.type == "/" || token.type == "%" ||
         token.type == "+" || token.type == "-" ||
         token.type == "OR" || token.type == "AND" ||
         token.type == "|" || token.type == "^" || token.type == "&" ||
         token.type == "EQ" || token.type == "NEQ" ||
         token.type == "<" || token.type == ">" || token.type == "LE" || token.type == "GE" ||
         token.type == "LS" || token.type == "RS" || token.type == "?" ) {
      if ( Rest_of_Identifier_started_basic_exp( token ) )
        return true;
      else
        return false;
    } // if

    else {
      g_peekTokenNo -= 2;
      token = PeekToken();
      EraseLastToken();
      return true;
    } // else

  } // if

  else if ( token.type == "PP" || token.type == "MM" ) {
    GetToken( token );
    token = PeekToken();
    if ( token.type == "Identifier" ) {
      if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
        g_is_undefined_error = true;
        return false;
      } // if

      GetToken( token );
      token = PeekToken();
      if ( token.type == "[" || token.type == "*" || token.type == "/" || token.type == "%" ||
           token.type == "+" || token.type == "-" ||
           token.type == "OR" || token.type == "AND" ||
           token.type == "|" || token.type == "^" || token.type == "&" ||
           token.type == "EQ" || token.type == "NEQ" ||
           token.type == "<" || token.type == ">" || token.type == "LE" || token.type == "GE" ||
           token.type == "LS" || token.type == "RS" ) {
        if ( Rest_of_PPMM_Identifier_started_basic_exp( token ) )
          return true;
        else
          return false;
      } // if

      else {
        g_peekTokenNo -= 2;
        token = PeekToken();
        EraseLastToken();
        return true;
      } // else

    } // if

    else
      return false;
  } // else if

  else if ( token.type == "+" || token.type == "-" || token.type == "!" ) {
    if ( Sign( token ) ) {
      GetToken( token );
      token = PeekToken();
      while ( Sign( token ) ) {
        GetToken( token );
        token = PeekToken();
      } // while

      if ( token.type == "Identifier" || token.type == "Constant" || token.type == "(" ) {
        if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
          g_is_undefined_error = true;
          return false;
        } // if

        if ( Signed_unary_exp( token ) ) {
          GetToken( token );
          token = PeekToken();
          if ( token.type == "*" || token.type == "/" || token.type == "%" ||
               token.type == "+" || token.type == "-" ||
               token.type == "OR" || token.type == "AND" ||
               token.type == "|" || token.type == "^" || token.type == "&" ||
               token.type == "EQ" || token.type == "NEQ" ||
               token.type == "<" || token.type == ">" || token.type == "LE" || token.type == "GE" ||
               token.type == "LS" || token.type == "RS" ) {
            if ( Romce_and_romloe( token ) )
              return true;
            else
              return false;
          } // if

          g_peekTokenNo -= 2;
          token = PeekToken();
          EraseLastToken();
          return true;
        } // if

        else
          return false;
      } // if

      else
        return false;
    } // if

    else
      return false;

  } // else if

  else if ( token.type == "Constant" || token.type == "(" ) {
    if ( token.type == "Constant" ) {
      GetToken( token );
      token = PeekToken();
      if ( token.type == "*" || token.type == "/" || token.type == "%" ||
           token.type == "+" || token.type == "-" ||
           token.type == "OR" || token.type == "AND" ||
           token.type == "|" || token.type == "^" || token.type == "&" ||
           token.type == "EQ" || token.type == "NEQ" ||
           token.type == "<" || token.type == ">" || token.type == "LE" || token.type == "GE" ||
           token.type == "LS" || token.type == "RS" ) {
        if ( Romce_and_romloe( token ) )
          return true;
        else
          return false;
      } // if

      // cuz maybe Romce_and_romloe() is EMPTY!!!
      else {
        g_peekTokenNo -= 2;
        token = PeekToken();
        EraseLastToken();
        return true;
      } // else

    } // if

    else { // token.type == "("
      GetToken( token );
      token = PeekToken();
      if ( token.type == "Identifier" || token.type == "PP" || token.type == "MM" ||
           token.type == "+" || token.type == "-" || token.type == "!" ||
           token.type == "Constant" || token.type == "(" ) {
        if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
          g_is_undefined_error = true;
          return false;
        } // if

        if ( Expression( token ) ) {
          GetToken( token );
          token = PeekToken();
          if ( token.type == ")" ) {
            GetToken( token );
            token = PeekToken();
            if ( token.type == "*" || token.type == "/" || token.type == "%" ||
                 token.type == "+" || token.type == "-" ||
                 token.type == "OR" || token.type == "AND" ||
                 token.type == "|" || token.type == "^" || token.type == "&" ||
                 token.type == "EQ" || token.type == "NEQ" ||
                 token.type == "<" || token.type == ">" || token.type == "LE" || token.type == "GE" ||
                 token.type == "LS" || token.type == "RS" ||
                 token.type == "?" || token.type == ":" ) {
              if ( Romce_and_romloe( token ) )
                return true;
              else
                return false;
            } // if

            // cuz maybe Romce_and_romloe() is EMPTY!!!
            else {
              g_peekTokenNo -= 2;
              token = PeekToken();
              EraseLastToken();
              return true;
            } // else

          } // if

          else
            return false;
        } // if

        else
          return false;
      } // if

      else
        return false;
    } // else

  } // else if

  else
    return false;
} // Basic_expression()

bool Rest_of_Identifier_started_basic_exp( Token &token ) {
/*
rest_of_Identifier_started_basic_exp
    : [ '[' expression ']' ]
      ( assignment_operator basic_expression | [ PP | MM ] romce_and_romloe )
    | '(' [ actual_parameter_list ] ')' romce_and_romloe
*/
  Token tmp_token;
  Token t;
  if ( token.type == "[" ) {
    GetToken( token );
    token = PeekToken();
    if ( token.type == "Identifier" || token.type == "PP" || token.type == "MM" ||
         token.type == "+" || token.type == "-" || token.type == "!" ||
         token.type == "Constant" || token.type == "(" ) {
      if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
        g_is_undefined_error = true;
        return false;
      } // if

      if ( Expression( token ) ) {
        GetToken( token );
        token = PeekToken();
        if ( token.type == "]" ) {
          GetToken( token );
          token = PeekToken();
          if ( token.type == "=" || token.type == "TE" || token.type == "DE" ||
               token.type == "RE" || token.type == "PE" || token.type == "ME" ) {
            if ( Assignment_operator( token ) ) {
              GetToken( token );
              token = PeekToken();
              if ( token.type == "Identifier" || token.type == "PP" || token.type == "MM" ||
                   token.type == "+" || token.type == "-" || token.type == "!" ||
                   token.type == "Constant" || token.type == "(" ) {
                if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
                  g_is_undefined_error = true;
                  return false;
                } // if

                if ( Basic_expression( token ) )
                  return true;
                else
                  return false;
              } // if

              else
                return false;
            } // if

            else
              return false;
          } // if

          else if ( token.type == "PP" || token.type == "MM" ) {
            GetToken( token );
            token = PeekToken();
            if ( token.type == "*" || token.type == "/" || token.type == "%" ||
                 token.type == "+" || token.type == "-" ||
                 token.type == "OR" || token.type == "AND" ||
                 token.type == "|" || token.type == "^" || token.type == "&" ||
                 token.type == "EQ" || token.type == "NEQ" ||
                 token.type == "<" || token.type == ">" || token.type == "LE" || token.type == "GE" ||
                 token.type == "LS" || token.type == "RS" ) {
              if ( Romce_and_romloe( token ) )
                return true;
              else
                return false;
            } // if

            // cuz maybe Romce_and_romloe() is EMPTY!!!
            else {
              g_peekTokenNo -= 2;
              token = PeekToken();
              EraseLastToken();
              return true;
            } // else

          } // else if

          else if ( token.type == "*" || token.type == "/" || token.type == "%" ||
                    token.type == "+" || token.type == "-" ||
                    token.type == "OR" || token.type == "AND" ||
                    token.type == "|" || token.type == "^" || token.type == "&" ||
                    token.type == "EQ" || token.type == "NEQ" ||
                    token.type == "<" || token.type == ">" || token.type == "LE" || token.type == "GE" ||
                    token.type == "LS" || token.type == "RS" ) {
            if ( Romce_and_romloe( token ) )
              return true;
            else
              return false;
          } // else if

          // cuz maybe Romce_and_romloe() is EMPTY!!!
          else {
            g_peekTokenNo -= 2;
            token = PeekToken();
            EraseLastToken();
            return true;
          } // else

        } // if

        else
          return false;
      } // if

      else
        return false;
    } // if

    else
      return false;
  } // if

  else if ( token.type == "=" || token.type == "TE" || token.type == "DE" || token.type == "RE" ||
            token.type == "PE" || token.type == "ME" ) {
    if ( Assignment_operator( token ) ) {
      GetToken( token );
      token = PeekToken();
      if ( token.type == "Identifier" || token.type == "PP" || token.type == "MM" ||
           token.type == "+" || token.type == "-" || token.type == "!" ||
           token.type == "Constant" || token.type == "(" ) {
        if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
          g_is_undefined_error = true;
          return false;
        } // if

        if ( Basic_expression( token ) ) {
          return true;
        } // if

        else
          return false;
      } // if

      else
        return false;
    } // if

    else
      return false;
  } // else if


  else if ( token.type == "PP" || token.type == "MM" ) {
    GetToken( token );
    token = PeekToken();
    if ( token.type == "*" || token.type == "/" || token.type == "%" ||
         token.type == "+" || token.type == "-" ||
         token.type == "OR" || token.type == "AND" ||
         token.type == "|" || token.type == "^" || token.type == "&" ||
         token.type == "EQ" || token.type == "NEQ" ||
         token.type == "<" || token.type == ">" || token.type == "LE" || token.type == "GE" ||
         token.type == "LS" || token.type == "RS" ) {
      if ( Romce_and_romloe( token ) )
        return true;
      else
        return false;
    } // if

    // cuz maybe Romce_and_romloe() is EMPTY!!!
    else {
      g_peekTokenNo -= 2;
      token = PeekToken();
      EraseLastToken();
      return true;
    } // else

  } // else if


  else if ( token.type == "*" || token.type == "/" || token.type == "%" ||
            token.type == "+" || token.type == "-" ||
            token.type == "OR" || token.type == "AND" ||
            token.type == "|" || token.type == "^" || token.type == "&" ||
            token.type == "EQ" || token.type == "NEQ" ||
            token.type == "<" || token.type == ">" || token.type == "LE" || token.type == "GE" ||
            token.type == "LS" || token.type == "RS" || token.type == "?" || token.type == ":" ) {
    if ( Romce_and_romloe( token ) )
      return true;
    else
      return false;
  } // else if

  else if ( token.type == "(" ) {
    GetToken( token );
    token = PeekToken();
    if ( token.type == "Identifier" || token.type == "PP" || token.type == "MM" ||
         token.type == "+" || token.type == "-" || token.type == "!" ||
         token.type == "Constant" || token.type == "(" ) {
      if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
        g_is_undefined_error = true;
        return false;
      } // if

      if ( Actual_parameter_list( token ) ) {
        GetToken( token );
        token = PeekToken();
        if ( token.type == ")" ) {
          GetToken( token );
          token = PeekToken();
          if ( token.type == "*" || token.type == "/" || token.type == "%" ||
               token.type == "+" || token.type == "-" ||
               token.type == "OR" || token.type == "AND" ||
               token.type == "|" || token.type == "^" || token.type == "&" ||
               token.type == "EQ" || token.type == "NEQ" ||
               token.type == "<" || token.type == ">" || token.type == "LE" || token.type == "GE" ||
               token.type == "LS" || token.type == "RS" ) {
            if ( Romce_and_romloe( token ) )
              return true;
            else
              return false;
          } // if

          // cuz maybe Romce_and_romloe() is EMPTY!!!
          else {
            g_peekTokenNo -= 2;
            token = PeekToken();
            EraseLastToken();
            return true;
          } // else

        } // if

        else
          return false;
      } // if

      else
        return false;
    } // if

    else if ( token.type == ")" ) {
      GetToken( token );
      token = PeekToken();
      if ( token.type == "*" || token.type == "/" || token.type == "%" ||
           token.type == "+" || token.type == "-" ||
           token.type == "OR" || token.type == "AND" ||
           token.type == "|" || token.type == "^" || token.type == "&" ||
           token.type == "EQ" || token.type == "NEQ" ||
           token.type == "<" || token.type == ">" || token.type == "LE" || token.type == "GE" ||
           token.type == "LS" || token.type == "RS" ) {
        if ( Romce_and_romloe( token ) )
          return true;
        else
          return false;
      } // if

      // cuz maybe Romce_and_romloe() is EMPTY!!!
      else {
        g_peekTokenNo -= 2;
        token = PeekToken();
        EraseLastToken();
        return true;
      } // else

    } // else if

    else
      return false;
  } // else if


  else
    return false;

} // Rest_of_Identifier_started_basic_exp()


bool Rest_of_PPMM_Identifier_started_basic_exp( Token &token ) {
  /*
  rest_of_PPMM_Identifier_started_basic_exp
    : [ '[' expression ']' ] romce_and_romloe
  */
  if ( token.type == "[" ) {
    GetToken( token );
    token = PeekToken();
    if ( token.type == "Identifier" || token.type == "PP" || token.type == "MM" || token.type == "+" ||
         token.type == "-" || token.type == "!" || token.type == "Constant" || token.type == "(" ) {
      if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
        g_is_undefined_error = true;
        return false;
      } // if

      if ( Expression( token ) ) {
        GetToken( token );
        token = PeekToken();
        if ( token.type == "]" ) {
          GetToken( token );
          token = PeekToken();
          if ( token.type == "*" || token.type == "/" || token.type == "%" ||
               token.type == "+" || token.type == "-" ||
               token.type == "OR" || token.type == "AND" ||
               token.type == "|" || token.type == "^" || token.type == "&" ||
               token.type == "EQ" || token.type == "NEQ" ||
               token.type == "<" || token.type == ">" || token.type == "LE" || token.type == "GE" ||
               token.type == "LS" || token.type == "RS" ) {
            if ( Romce_and_romloe( token ) )
              return true;
            else
              return false;
          } // if

          // cuz maybe Romce_and_romloe() is EMPTY!!!
          else {
            g_peekTokenNo -= 2;
            token = PeekToken();
            EraseLastToken();
            return true;
          } // else

        } // if

        else
          return false;
      } // if

      else
        return false;
    } // if

    else
      return false;
  } // if

  else if ( token.type == "*" || token.type == "/" || token.type == "%" ||
            token.type == "+" || token.type == "-" ||
            token.type == "OR" || token.type == "AND" ||
            token.type == "|" || token.type == "^" || token.type == "&" ||
            token.type == "EQ" || token.type == "NEQ" ||
            token.type == "<" || token.type == ">" || token.type == "LE" || token.type == "GE" ||
            token.type == "LS" || token.type == "RS" ) {
    if ( Romce_and_romloe( token ) )
      return true;
    else
      return false;
  } // else if

  // cuz maybe Romce_and_romloe() is EMPTY!!!
  else {
    g_peekTokenNo -= 2;
    token = PeekToken();
    EraseLastToken();
    return true;
  } // else

} // Rest_of_PPMM_Identifier_started_basic_exp()

bool Sign( Token &token ) {
/*
sign
    : '+' | '-' | '!'
*/
  if ( token.type == "+" || token.type == "-" || token.type == "!" )
    return true;
  else
    return false;
} // Sign()

bool Actual_parameter_list( Token &token ) {
/*
actual_parameter_list
    : basic_expression { ',' basic_expression }
*/
  if ( token.type == "Identifier" || token.type == "PP" || token.type == "MM" || token.type == "+" ||
       token.type == "-" || token.type == "!" || token.type == "Constant" || token.type == "(" ) {
    if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
      g_is_undefined_error = true;
      return false;
    } // if

    if ( Basic_expression( token ) ) {
      GetToken( token );
      token = PeekToken();
      while ( token.type == "," ) {
        GetToken( token );
        token = PeekToken();
        if ( token.type == "Identifier" || token.type == "PP" || token.type == "MM" || token.type == "+" ||
             token.type == "-" || token.type == "!" || token.type == "Constant" || token.type == "(" ) {
          if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
            g_is_undefined_error = true;
            return false;
          } // if

          if ( Basic_expression( token ) ) {
            GetToken( token );
            token = PeekToken();
          } // if

          else
            return false;
        } // if

        else
          return false;
      } // while

      g_peekTokenNo -= 2;
      token = PeekToken();
      EraseLastToken();
      return true;
    } // if

    else
      return false;
  } // if

  else
    return false;
} // Actual_parameter_list()

bool Assignment_operator( Token &token ) {
/*
assignment_operator
    : '=' | TE | DE | RE | PE | ME

*/
  if ( token.type == "=" || token.type == "TE" || token.type == "DE" ||
       token.type == "RE" || token.type == "PE" || token.type == "ME" )
    return true;
  else
    return false;
} // Assignment_operator()

bool Romce_and_romloe( Token &token ) {
/*
rest_of_maybe_conditional_exp_and_rest_of_maybe_logical_OR_exp // 即romce_and_romloe
    : rest_of_maybe_logical_OR_exp [ '?' basic_expression ':' basic_expression ]
*/
  if ( token.type == "+" || token.type == "-" || token.type == "*" || token.type == "/" ||
       token.type == "%" ||
       token.type == "LS" || token.type == "RS" ||
       token.type == "<" || token.type == ">" || token.type == "LE" || token.type == "GE" ||
       token.type == "EQ" || token.type == "NEQ" || token.type == "&" || token.type == "^" ||
       token.type == "|" || token.type == "AND" || token.type == "OR" || token.type == "?" ) {
    if ( token.type == "?" ) {
      GetToken( token );
      token = PeekToken();
      if ( token.type == "Identifier" || token.type == "PP" || token.type == "MM" ||
           token.type == "+" || token.type == "-" || token.type == "!" ||
           token.type == "Constant" || token.type == "(" ) {
        if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
          g_is_undefined_error = true;
          return false;
        } // if

        if ( Basic_expression( token ) ) {
          GetToken( token );
          token = PeekToken();
          if ( token.type == ":" ) {
            GetToken( token );
            token = PeekToken();
            if ( token.type == "Identifier" || token.type == "PP" || token.type == "MM" ||
                 token.type == "+" || token.type == "-" || token.type == "!" ||
                 token.type == "Constant" || token.type == "(" ) {

              if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
                g_is_undefined_error = true;
                return false;
              } // if

              if ( Basic_expression( token ) )
                return true;
              else
                return false;
            } // if

            else
              return false;
          } // if

          else
            return false;
        } // if

        else
          return false;
      } // if

      else
        return false;
    } // if

    if ( Rest_of_maybe_logical_OR_exp( token ) ) {
      GetToken( token );
      token = PeekToken();
      if ( token.type == "?" ) {
        GetToken( token );
        token = PeekToken();
        if ( token.type == "Identifier" || token.type == "PP" || token.type == "MM" || token.type == "+" ||
             token.type == "-" || token.type == "!" || token.type == "Constant" || token.type == "(" ) {
          if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
            g_is_undefined_error = true;
            return false;
          } // if

          if ( Basic_expression( token ) ) {
            GetToken( token );
            token = PeekToken();
            if ( token.type == ":" ) {
              GetToken( token );
              token = PeekToken();
              if ( token.type == "Identifier" || token.type == "PP" ||
                   token.type == "MM" || token.type == "+" ||
                   token.type == "-" || token.type == "!" ||
                   token.type == "Constant" || token.type == "(" ) {
                if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
                  g_is_undefined_error = true;
                  return false;
                } // if

                if ( Basic_expression( token ) )
                  return true;
                else
                  return false;
              } // if

              else
                return false;
            } // if

            else
              return false;
          } // if

          else
            return false;
        } // if

        else
          return false;
      } // if

      g_peekTokenNo -= 2;
      token = PeekToken();
      EraseLastToken();
      return true;
    } // if

    else
      return false;
  } // if

  else
    return false;
} // Romce_and_romloe()

bool Rest_of_maybe_logical_OR_exp( Token &token ) {
/*
rest_of_maybe_logical_OR_exp
    : rest_of_maybe_logical_AND_exp { OR maybe_logical_AND_exp }
*/
  if ( token.type == "+" || token.type == "-" || token.type == "*" || token.type == "/" ||
       token.type == "%" ||
       token.type == "LS" || token.type == "RS" ||
       token.type == "<" || token.type == ">" || token.type == "LE" || token.type == "GE" ||
       token.type == "EQ" || token.type == "NEQ" || token.type == "&" || token.type == "^" ||
       token.type == "|" || token.type == "AND" || token.type == "OR" || token.type == "?" ) {

    if ( token.type == "OR" ) {
      while ( token.type == "OR" ) {
        GetToken( token );
        token = PeekToken();
        if ( token.type == "+" || token.type == "-" || token.type == "!" || token.type == "Identifier" ||
             token.type == "Constant" || token.type == "(" || token.type == "PP" || token.type == "MM" ) {
          if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
            g_is_undefined_error = true;
            return false;
          } // if

          if ( Maybe_logical_AND_exp( token ) ) {
            GetToken( token );
            token = PeekToken();
          } // if

          else
            return false;
        } // if

        else
          return false;
      } // while

      g_peekTokenNo -= 2;
      token = PeekToken();
      EraseLastToken();
      return true;
    } // if


    if ( Rest_of_maybe_logical_AND_exp( token ) ) {
      GetToken( token );
      token = PeekToken();
      while ( token.type == "OR" ) {
        GetToken( token );
        token = PeekToken();
        if ( token.type == "+" || token.type == "-" || token.type == "!" || token.type == "Identifier" ||
             token.type == "Constant" || token.type == "(" || token.type == "PP" || token.type == "MM" ) {
          if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
            g_is_undefined_error = true;
            return false;
          } // if

          if ( Maybe_logical_AND_exp( token ) ) {
            GetToken( token );
            token = PeekToken();
          } // if

          else
            return false;
        } // if

        else
          return false;
      } // while

      g_peekTokenNo -= 2;
      token = PeekToken();
      EraseLastToken();
      return true;
    } // if

    else
      return false;
  } // if

  else
    return false;
} // Rest_of_maybe_logical_OR_exp()

bool Maybe_logical_AND_exp( Token &token ) {
/*
maybe_logical_AND_exp
    : maybe_bit_OR_exp { AND maybe_bit_OR_exp }
*/
  if ( token.type == "+" || token.type == "-" || token.type == "!" || token.type == "Identifier" ||
       token.type == "Constant" || token.type == "(" || token.type == "PP" || token.type == "MM" ||
       token.type == "LS" || token.type == "RS" ||
       token.type == "<" || token.type == ">" || token.type == "LE" || token.type == "GE" ||
       token.type == "EQ" || token.type == "NEQ" || token.type == "&" || token.type == "^" ||
       token.type == "|" || token.type == "AND" ) {
    if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
      g_is_undefined_error = true;
      return false;
    } // if

    if ( Maybe_bit_OR_exp( token ) ) {
      GetToken( token );
      token = PeekToken();
      while ( token.type == "AND" ) {
        GetToken( token );
        token = PeekToken();
        if ( token.type == "+" || token.type == "-" || token.type == "!" || token.type == "Identifier" ||
             token.type == "Constant" || token.type == "(" || token.type == "PP" || token.type == "MM" ||
             token.type == "LS" || token.type == "RS" ||
             token.type == "<" || token.type == ">" || token.type == "LE" || token.type == "GE" ||
             token.type == "EQ" || token.type == "NEQ" || token.type == "&" || token.type == "^" ||
             token.type == "|" ) {
          if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
            g_is_undefined_error = true;
            return false;
          } // if

          if ( Maybe_bit_OR_exp( token ) ) {
            GetToken( token );
            token = PeekToken();
          } // if

          else
            return false;
        } // if

        else
          return false;
      } // while

      g_peekTokenNo -= 2;
      token = PeekToken();
      EraseLastToken();
      return true;
    } // if

    else
      return false;
  } // if

  else
    return false;

} // Maybe_logical_AND_exp()

bool Rest_of_maybe_logical_AND_exp( Token &token ) {
/*
rest_of_maybe_logical_AND_exp
    : rest_of_maybe_bit_OR_exp { AND maybe_bit_OR_exp }
*/
  if ( token.type == "+" || token.type == "-" || token.type == "*" || token.type == "/" ||
       token.type == "%" ||
       token.type == "LS" || token.type == "RS" ||
       token.type == "<" || token.type == ">" || token.type == "LE" || token.type == "GE" ||
       token.type == "EQ" || token.type == "NEQ" || token.type == "&" || token.type == "^" ||
       token.type == "|" || token.type == "AND" || token.type == "OR" || token.type == "?" ) {

    if ( token.type == "AND" ) {
      while ( token.type == "AND" ) {
        GetToken( token );
        token = PeekToken();
        if ( token.type == "+" || token.type == "-" || token.type == "!" || token.type == "Identifier" ||
             token.type == "Constant" || token.type == "(" || token.type == "PP" || token.type == "MM" ) {
          if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
            g_is_undefined_error = true;
            return false;
          } // if

          if ( Maybe_bit_OR_exp( token ) ) {
            GetToken( token );
            token = PeekToken();
          } // if

          else
            return false;
        } // if

        else
          return false;
      } // while

      g_peekTokenNo -= 2;
      token = PeekToken();
      EraseLastToken();
      return true;
    } // if

    if ( Rest_of_maybe_bit_OR_exp( token ) ) {
      GetToken( token );
      token = PeekToken();
      while ( token.type == "AND" ) {
        GetToken( token );
        token = PeekToken();
        if ( token.type == "+" || token.type == "-" || token.type == "!" || token.type == "Identifier" ||
             token.type == "Constant" || token.type == "(" || token.type == "PP" || token.type == "MM" ) {
          if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
            g_is_undefined_error = true;
            return false;
          } // if

          if ( Maybe_bit_OR_exp( token ) ) {
            GetToken( token );
            token = PeekToken();
          } // if

          else
            return false;
        } // if

        else
          return false;
      } // while

      g_peekTokenNo -= 2;
      token = PeekToken();
      EraseLastToken();
      return true;
    } // if

    else
      return false;
  } // if

  else
    return false;
} // Rest_of_maybe_logical_AND_exp()

bool Maybe_bit_OR_exp( Token &token ) {
/*
maybe_bit_OR_exp
    : maybe_bit_ex_OR_exp { '|' maybe_bit_ex_OR_exp }
*/
  if ( token.type == "+" || token.type == "-" || token.type == "!" || token.type == "Identifier" ||
       token.type == "Constant" || token.type == "(" || token.type == "PP" || token.type == "MM" ||
       token.type == "LS" || token.type == "RS" ||
       token.type == "<" || token.type == ">" || token.type == "LE" || token.type == "GE" ||
       token.type == "EQ" || token.type == "NEQ" || token.type == "&" || token.type == "^" ||
       token.type == "|" ) {
    if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
      g_is_undefined_error = true;
      return false;
    } // if

    if ( Maybe_bit_ex_OR_exp( token ) ) {
      GetToken( token );
      token = PeekToken();
      while ( token.type == "|" ) {
        GetToken( token );
        token = PeekToken();
        if ( token.type == "+" || token.type == "-" || token.type == "!" || token.type == "Identifier" ||
             token.type == "Constant" || token.type == "(" || token.type == "PP" || token.type == "MM" ||
             token.type == "LS" || token.type == "RS" ||
             token.type == "<" || token.type == ">" || token.type == "LE" || token.type == "GE" ||
             token.type == "EQ" || token.type == "NEQ" || token.type == "&" || token.type == "^" ) {
          if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
            g_is_undefined_error = true;
            return false;
          } // if

          if ( Maybe_bit_ex_OR_exp( token ) ) {
            GetToken( token );
            token = PeekToken();
          } // if

          else
            return false;
        } // if

        else
          return false;
      } // while

      g_peekTokenNo -= 2;
      token = PeekToken();
      EraseLastToken();
      return true;
    } // if

    else
      return false;
  } // if

  else
    return false;

} // Maybe_bit_OR_exp()

bool Rest_of_maybe_bit_OR_exp( Token &token ) {
/*
rest_of_maybe_bit_OR_exp
    : rest_of_maybe_bit_ex_OR_exp { '|' maybe_bit_ex_OR_exp }
*/
  if ( token.type == "+" || token.type == "-" || token.type == "*" || token.type == "/" ||
       token.type == "%" ||
       token.type == "LS" || token.type == "RS" ||
       token.type == "<" || token.type == ">" || token.type == "LE" || token.type == "GE" ||
       token.type == "EQ" || token.type == "NEQ" || token.type == "&" || token.type == "^" ||
       token.type == "|" || token.type == "AND" || token.type == "OR" || token.type == "?"  ) {

    if ( token.type == "|" ) {
      while ( token.type == "|" ) {
        GetToken( token );
        token = PeekToken();
        if ( token.type == "+" || token.type == "-" || token.type == "!" || token.type == "Identifier" ||
             token.type == "Constant" || token.type == "(" || token.type == "PP" || token.type == "MM" ) {
          if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
            g_is_undefined_error = true;
            return false;
          } // if

          if ( Maybe_bit_ex_OR_exp( token ) ) {
            GetToken( token );
            token = PeekToken();
          } // if

          else
            return false;
        } // if

        else
          return false;
      } // while

      g_peekTokenNo -= 2;
      token = PeekToken();
      EraseLastToken();
      return true;
    } // if

    if ( Rest_of_maybe_bit_ex_OR_exp( token ) ) {
      GetToken( token );
      token = PeekToken();
      while ( token.type == "|" ) {
        GetToken( token );
        token = PeekToken();
        if ( token.type == "+" || token.type == "-" || token.type == "!" ||
             token.type == "Identifier" || token.type == "Constant" || token.type == "(" ||
             token.type == "PP" || token.type == "MM" ) {
          if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
            g_is_undefined_error = true;
            return false;
          } // if

          if ( Maybe_bit_ex_OR_exp( token ) ) {
            GetToken( token );
            token = PeekToken();
          } // if

          else
            return false;
        } // if

        else
          return false;
      } // while

      g_peekTokenNo -= 2;
      token = PeekToken();
      EraseLastToken();
      return true;
    } // if

    else
      return false;
  } // if

  else
    return false;

} // Rest_of_maybe_bit_OR_exp()

bool Maybe_bit_ex_OR_exp( Token &token ) {
/*
maybe_bit_ex_OR_exp
    : maybe_bit_AND_exp { '^' maybe_bit_AND_exp }
*/
  if ( token.type == "+" || token.type == "-" || token.type == "!" || token.type == "Identifier" ||
       token.type == "Constant" || token.type == "(" || token.type == "PP" || token.type == "MM" ||
       token.type == "LS" || token.type == "RS" ||
       token.type == "<" || token.type == ">" || token.type == "LE" || token.type == "GE" ||
       token.type == "EQ" || token.type == "NEQ" || token.type == "&" ) {
    if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
      g_is_undefined_error = true;
      return false;
    } // if

    if ( Maybe_bit_AND_exp( token ) ) {
      GetToken( token );
      token = PeekToken();
      while ( token.type == "^" ) {
        GetToken( token );
        token = PeekToken();
        if ( token.type == "+" || token.type == "-" || token.type == "!" || token.type == "Identifier" ||
             token.type == "Constant" || token.type == "(" || token.type == "PP" || token.type == "MM" ||
             token.type == "LS" || token.type == "RS" ||
             token.type == "<" || token.type == ">" || token.type == "LE" || token.type == "GE" ||
             token.type == "EQ" || token.type == "NEQ" || token.type == "&" ) {
          if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
            g_is_undefined_error = true;
            return false;
          } // if

          if ( Maybe_bit_AND_exp( token ) ) {
            GetToken( token );
            token = PeekToken();
          } // if

          else
            return false;
        } // if

        else
          return false;
      } // while

      g_peekTokenNo -= 2;
      token = PeekToken();
      EraseLastToken();
      return true;
    } // if

    else
      return false;
  } // if

  else
    return false;
} // Maybe_bit_ex_OR_exp()

bool Rest_of_maybe_bit_ex_OR_exp( Token &token ) {
/*
rest_of_maybe_bit_ex_OR_exp
    : rest_of_maybe_bit_AND_exp { '^' maybe_bit_AND_exp }
*/
  if ( token.type == "+" || token.type == "-" || token.type == "*" || token.type == "/" ||
       token.type == "%" ||
       token.type == "LS" || token.type == "RS" ||
       token.type == "<" || token.type == ">" || token.type == "LE" || token.type == "GE" ||
       token.type == "EQ" || token.type == "NEQ" || token.type == "&" || token.type == "^" ||
       token.type == "|" || token.type == "AND" || token.type == "OR" || token.type == "?" ) {

    if ( token.type == "^" ) {
      while ( token.type == "^" ) {
        GetToken( token );
        token = PeekToken();
        if ( token.type == "+" || token.type == "-" || token.type == "!" || token.type == "Identifier" ||
             token.type == "Constant" || token.type == "(" || token.type == "PP" || token.type == "MM" ) {
          if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
            g_is_undefined_error = true;
            return false;
          } // if

          if ( Maybe_bit_AND_exp( token ) ) {
            GetToken( token );
            token = PeekToken();
          } // if

          else
            return false;
        } // if

        else
          return false;
      } // while

      g_peekTokenNo -= 2;
      token = PeekToken();
      EraseLastToken();
      return true;
    } // if

    if ( Rest_of_maybe_bit_AND_exp( token ) ) {
      GetToken( token );
      token = PeekToken();
      while ( token.type == "^" ) {
        GetToken( token );
        token = PeekToken();
        if ( token.type == "+" || token.type == "-" || token.type == "!" || token.type == "Identifier" ||
             token.type == "Constant" || token.type == "(" || token.type == "PP" || token.type == "MM" ) {
          if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
            g_is_undefined_error = true;
            return false;
          } // if

          if ( Maybe_bit_AND_exp( token ) ) {
            GetToken( token );
            token = PeekToken();
          } // if

          else
            return false;
        } // if

        else
          return false;
      } // while

      g_peekTokenNo -= 2;
      token = PeekToken();
      EraseLastToken();
      return true;
    } // if

    else
      return false;
  } // if

  else
    return false;
} // Rest_of_maybe_bit_ex_OR_exp()

bool Maybe_bit_AND_exp( Token &token ) {
/*
maybe_bit_AND_exp
    : maybe_equality_exp { '&' maybe_equality_exp }
*/
  if ( token.type == "+" || token.type == "-" || token.type == "!" || token.type == "Identifier" ||
       token.type == "Constant" || token.type == "(" || token.type == "PP" || token.type == "MM" ||
       token.type == "LS" || token.type == "RS" ||
       token.type == "<" || token.type == ">" || token.type == "LE" || token.type == "GE" ||
       token.type == "EQ" || token.type == "NEQ" ) {
    if ( token.type == "Identifier" && !Is_id_exist( token ) )
      return false;
    if ( Maybe_equality_exp( token ) ) {
      GetToken( token );
      token = PeekToken();
      while ( token.type == "&" ) {
        GetToken( token );
        token = PeekToken();
        if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
          g_is_undefined_error = true;
          return false;
        } // if

        if ( token.type == "+" || token.type == "-" || token.type == "!" || token.type == "Identifier" ||
             token.type == "Constant" || token.type == "(" || token.type == "PP" || token.type == "MM" ||
             token.type == "LS" || token.type == "RS" ||
             token.type == "<" || token.type == ">" || token.type == "LE" || token.type == "GE" ||
             token.type == "EQ" || token.type == "NEQ" ) {
          if ( Maybe_equality_exp( token ) ) {
            token = PeekToken();
          } // if

          else
            return false;
        } // if

        else
          return false;
      } // while

      g_peekTokenNo -= 2;
      token = PeekToken();
      EraseLastToken();
      return true;
    } // if

    else
      return false;
  } // if

  else
    return false;
} // Maybe_bit_AND_exp()

bool Rest_of_maybe_bit_AND_exp( Token &token ) {
/*
rest_of_maybe_bit_AND_exp
    : rest_of_maybe_equality_exp { '&' maybe_equality_exp }
*/
  if ( token.type == "+" || token.type == "-" || token.type == "*" || token.type == "/" ||
       token.type == "%" ||
       token.type == "LS" || token.type == "RS" ||
       token.type == "<" || token.type == ">" || token.type == "LE" || token.type == "GE" ||
       token.type == "EQ" || token.type == "NEQ" || token.type == "&" || token.type == "^" ||
       token.type == "|" || token.type == "AND" || token.type == "OR" || token.type == "?" ) {
    if ( token.type == "&" ) {
      while ( token.type == "&" ) {
        GetToken( token );
        token = PeekToken();
        if ( token.type == "+" || token.type == "-" || token.type == "!" || token.type == "Identifier" ||
             token.type == "Constant" || token.type == "(" || token.type == "PP" || token.type == "MM" ) {
          if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
            g_is_undefined_error = true;
            return false;
          } // if

          if ( Maybe_equality_exp( token ) ) {
            GetToken( token );
            token = PeekToken();
          } // if

          else
            return false;
        } // if

        else
          return false;
      } // while

      g_peekTokenNo -= 2;
      token = PeekToken();
      EraseLastToken();
      return true;
    } // if

    if ( Rest_of_maybe_equality_exp( token ) ) {
      GetToken( token );
      token = PeekToken();
      while ( token.type == "&" ) {
        GetToken( token );
        token = PeekToken();
        if ( token.type == "+" || token.type == "-" || token.type == "!" || token.type == "Identifier" ||
             token.type == "Constant" || token.type == "(" || token.type == "PP" || token.type == "MM" ) {
          if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
            g_is_undefined_error = true;
            return false;
          } // if

          if ( Maybe_equality_exp( token ) ) {
            GetToken( token );
            token = PeekToken();
          } // if

          else
            return false;
        } // if

        else
          return false;
      } // while

      g_peekTokenNo -= 2;
      token = PeekToken();
      EraseLastToken();
      return true;
    } // if

    else
      return false;
  } // if

  else
    return false;
} // Rest_of_maybe_bit_AND_exp()

bool Maybe_equality_exp( Token &token ) {
/*
maybe_equality_exp
    : maybe_relational_exp
      { ( EQ | NEQ ) maybe_relational_exp}
*/
  if ( token.type == "+" || token.type == "-" || token.type == "!" || token.type == "Identifier" ||
       token.type == "Constant" || token.type == "(" || token.type == "PP" || token.type == "MM" ||
       token.type == "LS" || token.type == "RS" ||
       token.type == "<" || token.type == ">" || token.type == "LE" || token.type == "GE" ) {
    if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
      g_is_undefined_error = true;
      return false;
    } // if

    if ( Maybe_relational_exp( token ) ) {
      GetToken( token );
      token = PeekToken();
      while ( token.type == "EQ" || token.type == "NEQ" ) {
        GetToken( token );
        token = PeekToken();
        if ( token.type == "+" || token.type == "-" || token.type == "!" || token.type == "Identifier" ||
             token.type == "Constant" || token.type == "(" || token.type == "PP" || token.type == "MM" ||
             token.type == "LS" || token.type == "RS" ||
             token.type == "<" || token.type == ">" || token.type == "LE" || token.type == "GE" ) {
          if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
            g_is_undefined_error = true;
            return false;
          } // if

          if ( Maybe_relational_exp( token ) ) {
            GetToken( token );
            token = PeekToken();
          } // if

          else
            return false;
        } // if

        else
          return false;
      } // while

      g_peekTokenNo -= 2;
      token = PeekToken();
      EraseLastToken();
      return true;
    } // if

    else
      return false;
  } // if

  else
    return false;
} // Maybe_equality_exp()

bool Rest_of_maybe_equality_exp( Token &token ) {
/*
rest_of_maybe_equality_exp
    : rest_of_maybe_relational_exp
      { ( EQ | NEQ ) maybe_relational_exp }
*/
  if ( token.type == "+" || token.type == "-" || token.type == "*" || token.type == "/" ||
       token.type == "%" ||
       token.type == "LS" || token.type == "RS" ||
       token.type == "<" || token.type == ">" || token.type == "LE" || token.type == "GE" ||
       token.type == "EQ" || token.type == "NEQ" || token.type == "&" || token.type == "^" ||
       token.type == "|" || token.type == "AND" || token.type == "OR" || token.type == "?" ) {

    if ( token.type == "EQ" || token.type == "NEQ" ) {
      while ( token.type == "EQ" || token.type == "NEQ" ) {
        GetToken( token );
        token = PeekToken();
        if ( token.type == "+" || token.type == "-" || token.type == "!" || token.type == "Identifier" ||
             token.type == "Constant" || token.type == "(" || token.type == "PP" || token.type == "MM" ) {
          if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
            g_is_undefined_error = true;
            return false;
          } // if

          if ( Maybe_relational_exp( token ) ) {
            GetToken( token );
            token = PeekToken();
          } // if

          else
            return false;
        } // if

        else
          return false;
      } // while

      g_peekTokenNo -= 2;
      token = PeekToken();
      EraseLastToken();
      return true;
    } // if

    if ( Rest_of_maybe_relational_exp( token ) ) {
      GetToken( token );
      token = PeekToken();
      while ( token.type == "EQ" || token.type == "NEQ" ) {
        GetToken( token );
        token = PeekToken();
        if ( token.type == "+" || token.type == "-" || token.type == "!" || token.type == "Identifier" ||
             token.type == "Constant" || token.type == "(" || token.type == "PP" || token.type == "MM" ) {
          if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
            g_is_undefined_error = true;
            return false;
          } // if

          if ( Maybe_relational_exp( token ) ) {
            GetToken( token );
            token = PeekToken();
          } // if

          else
            return false;
        } // if

        else
          return false;
      } // while

      g_peekTokenNo -= 2;
      token = PeekToken();
      EraseLastToken();
      return true;
    } // if

    else
      return false;
  } // if

  else
    return false;
} // Rest_of_maybe_equality_exp()

bool Maybe_relational_exp( Token &token ) {
/*
maybe_relational_exp
    : maybe_shift_exp
      { ( '<' | '>' | LE | GE ) maybe_shift_exp }
*/
  if ( token.type == "+" || token.type == "-" || token.type == "!" || token.type == "Identifier" ||
       token.type == "Constant" || token.type == "(" || token.type == "PP" || token.type == "MM" ||
       token.type == "LS" || token.type == "RS" ||
       token.type == "<" || token.type == ">" || token.type == "LE" || token.type == "GE" ) {
    if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
      g_is_undefined_error = true;
      return false;
    } // if

    if ( Maybe_shift_exp( token ) ) {
      GetToken( token );
      token = PeekToken();
      while ( token.type == "<" || token.type == ">" || token.type == "LE" || token.type == "GE" ) {
        GetToken( token );
        token = PeekToken();
        if ( token.type == "+" || token.type == "-" || token.type == "!" || token.type == "Identifier" ||
             token.type == "Constant" || token.type == "(" || token.type == "PP" || token.type == "MM" ||
             token.type == "LS" || token.type == "RS" ) {
          if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
            g_is_undefined_error = true;
            return false;
          } // if

          if ( Maybe_shift_exp( token ) ) {
            GetToken( token );
            token = PeekToken();
          } // if

          else
            return false;
        } // if

        else
          return false;
      } // while

      g_peekTokenNo -= 2;
      token = PeekToken();
      EraseLastToken();
      return true;
    } // if

    else
      return false;
  } // if

  else
    return false;
} // Maybe_relational_exp()

bool Rest_of_maybe_relational_exp( Token &token ) {
/*
rest_of_maybe_relational_exp
    : rest_of_maybe_shift_exp
      { ( '<' | '>' | LE | GE ) maybe_shift_exp }
*/
  if ( token.type == "+" || token.type == "-" || token.type == "*" || token.type == "/" ||
       token.type == "%" ||
       token.type == "LS" || token.type == "RS" ||
       token.type == "<" || token.type == ">" || token.type == "LE" || token.type == "GE" ||
       token.type == "EQ" || token.type == "NEQ" || token.type == "&" || token.type == "^" ||
       token.type == "|" || token.type == "AND" || token.type == "OR" || token.type == "?" ) {
    if ( token.type == ">" || token.type == "<" || token.type == "GE" ||
         token.type == "LE" ) {
      while ( token.type == "<" || token.type == ">" || token.type == "LE" || token.type == "GE" ) {
        GetToken( token );
        token = PeekToken();
        if ( token.type == "+" || token.type == "-" || token.type == "!" || token.type == "Identifier" ||
             token.type == "Constant" || token.type == "(" || token.type == "PP" || token.type == "MM" ) {
          if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
            g_is_undefined_error = true;
            return false;
          } // if

          if ( Maybe_shift_exp( token ) ) {
            GetToken( token );
            token = PeekToken();
          } // if

          else
            return false;
        } // if

        else
          return false;
      } // while

      g_peekTokenNo -= 2;
      token = PeekToken();
      EraseLastToken();
      return true;
    } // if

    if ( Rest_of_maybe_shift_exp( token ) ) {
      GetToken( token );
      token = PeekToken();
      while ( token.type == "<" || token.type == ">" || token.type == "LE" || token.type == "GE" ) {
        GetToken( token );
        token = PeekToken();
        if ( token.type == "+" || token.type == "-" || token.type == "!" || token.type == "Identifier" ||
             token.type == "Constant" || token.type == "(" || token.type == "PP" || token.type == "MM" ) {
          if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
            g_is_undefined_error = true;
            return false;
          } // if

          if ( Maybe_shift_exp( token ) ) {
            GetToken( token );
            token = PeekToken();
          } // if

          else
            return false;
        } // if

        else
          return false;
      } // while

      g_peekTokenNo -= 2;
      token = PeekToken();
      EraseLastToken();
      return true;
    } // if

    else
      return false;
  } // if

  else
    return false;
} // Rest_of_maybe_relational_exp()

bool Maybe_shift_exp( Token &token ) {
/*
maybe_shift_exp
    : maybe_additive_exp { ( LS | RS ) maybe_additive_exp }
*/
  if ( token.type == "+" || token.type == "-" || token.type == "!" || token.type == "Identifier" ||
       token.type == "Constant" || token.type == "(" || token.type == "PP" || token.type == "MM" ) {
    if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
      g_is_undefined_error = true;
      return false;
    } // if

    if ( Maybe_additive_exp( token ) ) {
      GetToken( token );
      token = PeekToken();
      while ( token.type == "LS" || token.type == "RS" ) {
        GetToken( token );
        token = PeekToken();
        if ( token.type == "+" || token.type == "-" || token.type == "!" || token.type == "Identifier" ||
             token.type == "Constant" || token.type == "(" || token.type == "PP" || token.type == "MM" ) {
          if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
            g_is_undefined_error = true;
            return false;
          } // if

          if ( Maybe_additive_exp( token ) ) {
            GetToken( token );
            token = PeekToken();
          } // if

          else
            return false;
        } // if

        else
          return false;
      } // while

      g_peekTokenNo -= 2;
      token = PeekToken();
      EraseLastToken();
      return true;
    } // if

    else
      return false;
  } // if

  else
    return false;
} // Maybe_shift_exp()

bool Rest_of_maybe_shift_exp( Token &token ) {
/*
rest_of_maybe_shift_exp
    : rest_of_maybe_additive_exp { ( LS | RS ) maybe_additive_exp }
*/
  if ( token.type == "+" || token.type == "-" || token.type == "*" || token.type == "/" ||
       token.type == "%" ||
       token.type == "LS" || token.type == "RS" ||
       token.type == "<" || token.type == ">" || token.type == "LE" || token.type == "GE" ||
       token.type == "EQ" || token.type == "NEQ" || token.type == "&" || token.type == "^" ||
       token.type == "|" || token.type == "AND" || token.type == "OR" || token.type == "?" ) {

    if ( token.type == "LS" || token.type == "RS" ) {
      while ( token.type == "LS" || token.type == "RS" ) {
        GetToken( token );
        token = PeekToken();
        if ( token.type == "+" || token.type == "-" || token.type == "!" || token.type == "Identifier" ||
             token.type == "Constant" || token.type == "(" || token.type == "PP" || token.type == "MM" ) {
          if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
            g_is_undefined_error = true;
            return false;
          } // if

          if ( Maybe_additive_exp( token ) ) {
            GetToken( token );
            token = PeekToken();
          } // if

          else
            return false;
        } // if

        else
          return false;
      } // while

      g_peekTokenNo -= 2;
      token = PeekToken();
      EraseLastToken();
      return true;
    } // if

    if ( Rest_of_maybe_additive_exp( token ) ) {
      GetToken( token );
      token = PeekToken();
      while ( token.type == "LS" || token.type == "RS" ) {
        GetToken( token );
        token = PeekToken();
        if ( token.type == "+" || token.type == "-" || token.type == "!" || token.type == "Identifier" ||
             token.type == "Constant" || token.type == "(" || token.type == "PP" || token.type == "MM" ) {
          if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
            g_is_undefined_error = true;
            return false;
          } // if

          if ( Maybe_additive_exp( token ) ) {
            GetToken( token );
            token = PeekToken();
          } // if

          else
            return false;
        } // if

        else
          return false;
      } // while

      g_peekTokenNo -= 2;
      token = PeekToken();
      EraseLastToken();
      return true;
    } // if

    else
      return false;
  } // if

  else
    return false;
} // Rest_of_maybe_shift_exp()

bool Maybe_additive_exp( Token &token ) {
/*
maybe_additive_exp
    : maybe_mult_exp { ( '+' | '-' ) maybe_mult_exp }
*/
  if ( token.type == "+" || token.type == "-" || token.type == "!" || token.type == "Identifier" ||
       token.type == "Constant" || token.type == "(" || token.type == "PP" || token.type == "MM" ) {
    if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
      g_is_undefined_error = true;
      return false;
    } // if

    if ( Maybe_mult_exp( token ) ) {
      GetToken( token );
      token = PeekToken();
      while ( token.type == "+" || token.type == "-" ) {
        GetToken( token );
        token = PeekToken();
        if ( token.type == "+" || token.type == "-" || token.type == "!" || token.type == "Identifier" ||
             token.type == "Constant" || token.type == "(" || token.type == "PP" || token.type == "MM" ) {
          if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
            g_is_undefined_error = true;
            return false;
          } // if

          if ( Maybe_mult_exp( token ) ) {
            GetToken( token );
            token = PeekToken();
          } // if

          else
            return false;
        } // if

        else
          return false;
      } // while

      g_peekTokenNo -= 2;
      token = PeekToken();
      EraseLastToken();
      return true;
    } // if

    else
      return false;
  } // if

  else
    return false;
} // Maybe_additive_exp()

bool Rest_of_maybe_additive_exp( Token &token ) {
/*
rest_of_maybe_additive_exp
    : rest_of_maybe_mult_exp { ( '+' | '-' ) maybe_mult_exp }
*/
  bool needEraseLastToken = true;
  if ( token.type == "+" || token.type == "-" || token.type == "*" || token.type == "/" ||
       token.type == "%" ||
       token.type == "LS" || token.type == "RS" ||
       token.type == "<" || token.type == ">" || token.type == "LE" || token.type == "GE" ||
       token.type == "EQ" || token.type == "NEQ" || token.type == "&" || token.type == "^" ||
       token.type == "|" || token.type == "AND" || token.type == "OR" || token.type == "?" ) {

    if ( token.type == "+" || token.type == "-" ) {
      while ( token.type == "+" || token.type == "-" ) {
        GetToken( token );
        token = PeekToken();
        if ( token.type == "+" || token.type == "-" || token.type == "!" || token.type == "Identifier" ||
             token.type == "Constant" || token.type == "(" || token.type == "PP" || token.type == "MM" ) {
          if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
            g_is_undefined_error = true;
            return false;
          } // if

          if ( Maybe_mult_exp( token ) ) {
            GetToken( token );
            token = PeekToken();
          } // if

          else
            return false;
        } // if

        else
          return false;
      } // while

      g_peekTokenNo -= 2;
      token = PeekToken();
      EraseLastToken();
      return true;
    } // if

    if ( Rest_of_maybe_mult_exp( token ) ) {
      GetToken( token );
      token = PeekToken();
      while ( token.type == "+" || token.type == "-" ) {
        GetToken( token );
        token = PeekToken();
        if ( token.type == "+" || token.type == "-" || token.type == "!" || token.type == "Identifier" ||
             token.type == "Constant" || token.type == "(" || token.type == "PP" || token.type == "MM" ) {
          if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
            g_is_undefined_error = true;
            return false;
          } // if

          if ( Maybe_mult_exp( token ) ) {
            GetToken( token );
            token = PeekToken();
          } // if

          else
            return false;
        } // if

        else
          return false;
      } // while

      g_peekTokenNo -= 2;
      token = PeekToken();
      EraseLastToken();
      return true;
    } // if

    else
      return false;
  } // if

  else
    return false;

} // Rest_of_maybe_additive_exp()

bool Maybe_mult_exp( Token &token ) {
/*
maybe_mult_exp
    : unary_exp rest_of_maybe_mult_exp
*/
  Token t;
  if ( token.type == "+" || token.type == "-" || token.type == "!" || token.type == "Identifier" ||
       token.type == "Constant" || token.type == "(" || token.type == "PP" || token.type == "MM" ) {
    if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
      g_is_undefined_error = true;
      return false;
    } // if

    if ( Unary_exp( token ) ) {
      GetToken( token );
      token = PeekToken();
      t = token;
      if ( Rest_of_maybe_mult_exp( token ) ) {
        if ( t.token == token.token ) {
          g_peekTokenNo -= 2;
          token = PeekToken();
          EraseLastToken();
        } // if

        return true;
      } // if

      else
        return false;

    } // if

    else
      return false;
  } // if

  else
    return false;
} // Maybe_mult_exp()

bool Rest_of_maybe_mult_exp( Token &token ) {
/*
rest_of_maybe_mult_exp
    : { ( '*' | '/' | '%' ) unary_exp }  // could be empty !
*/
  bool tmp = false;
  while ( token.type == "*" || token.type == "/" || token.type == "%" ) {
    GetToken( token );
    token = PeekToken();
    if ( token.type == "+" || token.type == "-" || token.type == "!" || token.type == "Identifier" ||
         token.type == "Constant" || token.type == "(" || token.type == "PP" || token.type == "MM" ) {
      if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
        g_is_undefined_error = true;
        return false;
      } // if

      if ( Unary_exp( token ) ) {
        tmp = true;
        GetToken( token );
        token = PeekToken();
      } // if

      else
        return false;
    } // if

    else
      return false;
  } // while

  if ( tmp ) {
    g_peekTokenNo -= 2;
    token = PeekToken();
    EraseLastToken();
  } // if

  return true;
} // Rest_of_maybe_mult_exp()

bool Unary_exp( Token &token ) {
/*
unary_exp
    : sign { sign } signed_unary_exp
    | unsigned_unary_exp
    | ( PP | MM ) Identifier [ '[' expression ']' ]
*/
  if ( token.type == "+" || token.type == "-" || token.type == "!" ) {
    if ( Sign( token ) ) {
      GetToken( token );
      token = PeekToken();
      while ( Sign( token ) ) {
        GetToken( token );
        token = PeekToken();
      } // while

      if ( token.type == "Identifier" || token.type == "Constant" || token.type == "(" ) {
        if ( Signed_unary_exp( token ) )
          return true;
        else
          return false;
      } // if

      else
        return false;
    } // if

    else
      return false;
  } // if

  else if ( token.type == "Identifier" || token.type == "Constant" || token.type == "(" ) {
    if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
      g_is_undefined_error = true;
      return false;
    } // if

    if ( Unsigned_unary_exp( token ) )
      return true;
    else
      return false;
  } // else if

  else if ( token.type == "PP" || token.type == "MM" ) {
    GetToken( token );
    token = PeekToken();
    if ( token.type == "Identifier" ) {
      if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
        g_is_undefined_error = true;
        return false;
      } // if

      GetToken( token );
      token = PeekToken();
      if ( token.type == "[" ) {
        GetToken( token );
        token = PeekToken();
        if ( token.type == "Identifier" || token.type == "PP" || token.type == "MM" || token.type == "+" ||
             token.type == "-" || token.type == "!" || token.type == "Constant" || token.type == "(" ) {
          if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
            g_is_undefined_error = true;
            return false;
          } // if

          if ( Expression( token ) ) {
            GetToken( token );
            token = PeekToken();
            if ( token.type == "]" )
              return true;
            else
              return false;
          } // if

          else
            return false;
        } // if

        else
          return false;
      } // if

      else {
        g_peekTokenNo -= 2;
        token = PeekToken();
        EraseLastToken();
        return true;
      } // else

    } // if

    else
      return false;
  } // else if

  else
    return false;
} // Unary_exp()

bool Signed_unary_exp( Token &token ) {
/*
signed_unary_exp
    : Identifier [ '(' [ actual_parameter_list ] ')'
                   |
                   '[' expression ']'
                 ]
    | Constant
    | '(' expression ')'
*/

  if ( token.type == "Identifier" ) {
    if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
      g_is_undefined_error = true;
      return false;
    } // if

    GetToken( token );
    token = PeekToken();
    if ( token.type == "(" ) {
      GetToken( token );
      token = PeekToken();
      if ( token.type == "Identifier" || token.type == "PP" || token.type == "MM" || token.type == "+" ||
           token.type == "-" || token.type == "!" || token.type == "Constant" || token.type == "(" ) {
        if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
          g_is_undefined_error = true;
          return false;
        } // if

        if ( Actual_parameter_list( token ) ) {
          GetToken( token );
          token = PeekToken();
          if ( token.type == ")" )
            return true;
          else
            return false;
        } // if

        else
          return false;
      } // if

      else if ( token.type == ")" )
        return true;
      else
        return false;
    } // if

    else if ( token.type == "[" ) {
      GetToken( token );
      token = PeekToken();
      if ( token.type == "Identifier" || token.type == "PP" || token.type == "MM" || token.type == "+" ||
           token.type == "-" || token.type == "!" || token.type == "Constant" || token.type == "(" ) {
        if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
          g_is_undefined_error = true;
          return false;
        } // if

        if ( Expression( token ) ) {
          GetToken( token );
          token = PeekToken();
          if ( token.type == "]" )
            return true;
          else
            return false;
        } // if

        else
          return false;
      } // if

      else
        return false;

    } // else if

    else {
      g_peekTokenNo -= 2;
      token = PeekToken();
      EraseLastToken();
      return true;
    } // else

  } // if

  else if ( token.type == "Constant" )
    return true;

  else if ( token.type == "(" ) {
    GetToken( token );
    token = PeekToken();
    if ( token.type == "Identifier" || token.type == "PP" || token.type == "MM" || token.type == "+" ||
         token.type == "-" || token.type == "!" || token.type == "Constant" || token.type == "(" ) {
      if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
        g_is_undefined_error = true;
        return false;
      } // if

      if ( Expression( token ) ) {
        GetToken( token );
        token = PeekToken();
        if ( token.type == ")" )
          return true;
        else
          return false;
      } // if

      else
        return false;
    } // if

    else
      return false;
  } // else if

  else
    return false;
} // Signed_unary_exp()

bool Unsigned_unary_exp( Token &token ) {
/*
unsigned_unary_exp
    : Identifier [
                   '(' [ actual_parameter_list ] ')'
                   |
                   [ '[' expression ']' ] [ ( PP | MM ) ]
                 ]
    | Constant
    | '(' expression ')'
*/
  Token tmp_token;
  if ( token.type == "Identifier" ) {
    if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
      g_is_undefined_error = true;
      return false;
    } // if

    GetToken( token );
    token = PeekToken();
    if ( token.type == "(" ) {
      GetToken( token );
      token = PeekToken();
      if ( token.type == "Identifier" || token.type == "PP" || token.type == "MM" || token.type == "+" ||
           token.type == "-" || token.type == "!" || token.type == "Constant" || token.type == "(" ) {
        if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
          g_is_undefined_error = true;
          return false;
        } // if

        if ( Actual_parameter_list( token ) ) {
          GetToken( token );
          token = PeekToken();
          if ( token.type == ")" )
            return true;
          else
            return false;
        } // if

        else
          return false;
      } // if

      else if ( token.type == ")" )
        return true;
      else
        return false;
    } // if

    else if ( token.type == "[" ) {
      GetToken( token );
      token = PeekToken();
      if ( token.type == "Identifier" || token.type == "PP" || token.type == "MM" || token.type == "+" ||
           token.type == "-" || token.type == "!" || token.type == "Constant" || token.type == "(" ) {
        if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
          g_is_undefined_error = true;
          return false;
        } // if

        if ( Expression( token ) ) {
          GetToken( token );
          token = PeekToken();
          if ( token.type == "]" ) {
            GetToken( token );
            token = PeekToken();
            if ( token.type == "PP" || token.type == "MM" )
              return true;
            else {
              g_peekTokenNo -= 2; // 多讀
              token = PeekToken();
              EraseLastToken();
              return true;
            } // else

          } // if

          else
            return false;
        } // if

        else
          return false;
      } // if

      else
        return false;
    } // else if

    else if ( token.type == "PP" || token.type == "MM" )
      return true;

    else {
      g_peekTokenNo -= 2; // 多讀
      token = PeekToken();
      EraseLastToken();
      return true;
    } // else

  } // if

  else if ( token.type == "Constant" )
    return true;

  else if ( token.type == "(" ) {
    GetToken( token );
    token = PeekToken();
    if ( token.type == "Identifier" || token.type == "PP" || token.type == "MM" || token.type == "+" ||
         token.type == "-" || token.type == "!" || token.type == "Constant" || token.type == "(" ) {
      if ( token.type == "Identifier" && !Is_id_exist( token ) ) {
        g_is_undefined_error = true;
        return false;
      } // if

      if ( Expression( token ) ) {
        GetToken( token );
        token = PeekToken();
        if ( token.type == ")" )
          return true;
        else
          return false;
      } // if

      else
        return false;
    } // if

    else
      return false;
  } // else if

  else
    return false;
} // Unsigned_unary_exp()


int main() {
  g_hasError = false;
  int testNum;
  g_peekTokenNo = 0;
  g_lineNo = 1;
  g_last_exe_lineNo = 0;
  cout << "Input a testNum (Any number is okay):";
  scanf( "%d", &testNum );
  char chr;
  scanf( "%c", &chr ); // read the '\n' after testNum
  cout << "Our-C running ...\n";

  User_input();

  cout << "> Our-C exited ..." << endl;
  return 0;
} // main()
