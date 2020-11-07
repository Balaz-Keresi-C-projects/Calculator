#include <iostream>
#include <iomanip>
#include <map>
#include <sstream>

using namespace std;

enum token{
    END, NUMBER, NAME, PRINT=';', PRINT2,
    PLUS='+', MINUS='-', MUL='*', DIV='/',
    ASSIGN='=', LP='(', RP=')', FAKT = '!'
};

map<string, double> constant;
map<string, double> variables;


token cur_tok=PRINT2;

double number_value;
string string_value;

double expr(bool);
double term(bool);
double prim(bool);

void gettoken();

int no_of_errors;
double error(string);

bool first_tok;
bool nans;

istream* input;

int main(int argc, char* argv[])
{

    switch (argc)
    {
        case 1:
            input = &cin;
            break;
        case 2:
            input = new istringstream(argv[1]);
            break;
        default:
            error(" Hiba: Tul sok bemeno parameter");
            return 1;
    }
    constant["pi"] = 3.1415926535897932385;
    constant["e"] = 2.7182818284590452354;

        while(*input)
        {
            first_tok = false;
            nans = false;
            gettoken();
            if(cur_tok == PRINT || cur_tok == PRINT2)  continue;
            if(string_value == "exit" || cur_tok == END) break;
            double res = expr(false);
            if(no_of_errors == 0 && !nans)
            {
                cout<<"ans = "<<setprecision(10)<<res<<"; ";
                variables["ans"]=res;
            }
            else
                no_of_errors = 0;
            cout<<endl;

        }
    cout<<endl;
    return 0;
}

//=========================================
double expr(bool get)
{
    double left = term(get);
    for(;;)
    {
        switch(cur_tok)
        {
            case PLUS:
            {
                left+=term(true);
                break;
            }
            case MINUS:
            {
                left-=term(true);
                break;
            }
            default:
                return left;
        }
    }
}

double term(bool get)
{
    double left = prim(get);
    for(;;)
    {
        switch(cur_tok)
        {
            case MUL:
                left *= prim(true);
                break;
            case DIV:
            {
                double d;
                if( (d=prim(true)) == 0 )
                    return error("Hiba: Nullaval valo osztas");
                else
                    left /= d;
                break;
            }
            default:
                return left;
        }
    }
}

double prim(bool get)
{
    if(get)
        gettoken();
    switch(cur_tok)
    {
        case NUMBER:
        {
            first_tok = true;
            double ret_val;
            ret_val = number_value;
            gettoken();
            while(cur_tok == FAKT)
            {
                for(int i=1; i<number_value-1; i++)
                    ret_val *= number_value-i ;
                number_value = ret_val;
                gettoken();
            }
            return ret_val;
        }
        case NAME:
        {
            string cur_name = string_value;
            gettoken();
            if(cur_tok == ASSIGN)
            {
                if(constant[cur_name] != 0)
                {
                    expr(true);
                    return error("Konstansnak tilos erteket adni");
                }
                else
                    {
                        if(!first_tok) nans = true;
                        variables[cur_name] = expr(true);
                        cout<<cur_name<<" = "<<variables[cur_name]<<"; ";
                        return variables[cur_name];
                    }
            }
            double ret_val;
            if(constant[cur_name] != 0)
            {
                ret_val = constant[cur_name];
                while(cur_tok == FAKT)
                {
                    for(int i=1; i<number_value-1; i++)
                        ret_val *= number_value-i ;
                    number_value = ret_val;
                    gettoken();
                }
                return ret_val;
            }
            ret_val = variables[cur_name];
            while(cur_tok == FAKT)
            {
                for(int i=1; i<number_value-1; i++)
                    ret_val *= number_value-i ;
                number_value = ret_val;
                gettoken();
            }
            return ret_val;
        }
        case MINUS:
        {
            first_tok = true;
            return -prim(true);
        }
        case LP:
        {
            double ret_val;
            ret_val = expr(true);
            if(cur_tok != RP)
                return error("Hiba: ')' hianyzik ");
            gettoken();
            number_value = ret_val;
            while(cur_tok == FAKT)
            {
                for(int i=1; i<number_value-1; i++)
                    ret_val *= number_value-i ;
                number_value = ret_val;
                gettoken();
            }
            return ret_val;
        }
        default:
            return error("Hiba: Elemi szimbolum szukseges");
    }
}

void gettoken()
{
    char cc;
    while( (cc=input->get()) == ' ' || cc == '\t' ) ;
    if((cc>='0' && cc<='9') || cc=='.')
    {
       input->putback(cc);
       *input>>number_value;
       cur_tok = NUMBER;
       return;
    }
    if((cc>='A' && cc<='Z') || (cc>='a' && cc<='z') || cc=='_')
    {
        string_value = "";
        string_value.push_back(cc);
        while(( (cc=input->get()) >='A' && cc<='Z') || (cc>='a' && cc<='z') || cc=='_')
        {
            string_value.push_back(cc);
        }
        input->putback(cc);
        cur_tok = NAME;
        return;
    }
    if( cc=='+' || cc=='-' || cc=='*' || cc=='/' || cc=='=' || cc=='(' || cc==')' || cc==';' || cc=='!')
    {
        cur_tok = (token)cc;
        return;
    }
    if( cc=='\n' )
    {
        cur_tok = PRINT2;
        return;
    }
    if( cc == 0 || cc == -1)
    {
        cur_tok = END;
        return;
    }
    error("Rossz szimbolum");
    cout<<(int)cc;
    cur_tok = PRINT;
}

double error(string error_message)
{
    cerr<<error_message<<endl;
    no_of_errors++;
    return 0;
}
