// #include <stdio.h>
// #include <stdlib.h>
// #include <assert.h>
// #include <string.h>
// #include <ctype.h>
// #include "tools.hpp"

// struct RecDes
//     {
//     const char* str;
//     size_t* pos;
//     };

// const char s[] = "1000-7*100/(30+5*10-5*(100/50))+1";
// int p = 0;

// int GetG();
// int GetN(RecDes* data);
// int GetE(RecDes* data);
// int GetT(RecDes* data);
// int GetP(RecDes* data);

// int main()
//     {
//     printf("%d\n", GetG());
//     return 0;
//     }

// int GetG()
//     {
//     RecDes data = {};
//     size_t pos = 0;
//     const char s[] = "1000-7*100/(30+5*10-5*(100/50))+1";
//     data.pos = &pos;
//     data.str = s;

//     int val = GetE(&data);
//     return val;
//     }

// int GetN(RecDes* data)
//     {
//     size_t* pos = data->pos;
//     const char* str = data->str;

//     int val = 0;
//     int old_p = *pos;
//     while ('0' <= str[*pos] && str[*pos] <= '9')
//         {
//         val = val * 10 + str[*pos] - '0';
//         *pos++;
//         }
//     assert(old_p != *pos);
//     return val;
//     }

// int GetE(RecDes* data)
//     {
//     size_t* pos = data->pos;
//     const char* str = data->str;

//     int val = GetT(data);
//     while (str[*pos] == '+' || str[*pos] == '-')
//         {
//         char op = str[*pos];
//         *pos++;
//         int val2 = GetT(data);
//         switch (op)
//             {
//             case '+': val += val2; break;
//             case '-': val -= val2; break;
//             default: break;
//             }
//         }
//     return val;
//     }

// int GetT(RecDes* data)
//     {
//     size_t* pos = data->pos;
//     const char* str = data->str;

//     int val = GetP(data);
//     while (str[*pos] == '*' || str[*pos] == '/')
//         {
//         char op = str[*pos];
//         *pos++;
//         int val2 = GetP(data);
//         switch (op)
//             {
//             case '*': val *= val2; break;
//             case '/':
//                 if (val2 == 0)
//                     return ERR_SYNTAX_ERROR;
//                 val /= val2; 
//                 break;
//             default: break;
//             }
//         }
//     return val;
//     }

// int GetP()
//     {
//     if (s[p] == '(')
//         {
//         p++;
//         int val = GetE();
//         p++;
//         // assert(s[p] == ')');
//         return val;
//         }
//     else
//         return GetN();
//     }

