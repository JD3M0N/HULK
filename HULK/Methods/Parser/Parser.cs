// using System.Security.Principal;
// using System.Linq.Expressions;
// using System;
// using System.Collections.Generic;
// using System.Linq;
// using System.Threading.Tasks;

// namespace HULK
// {
//     public class Parser
//     {
        
//         /* work on
//         let number = 42, text = "The meaning of life is" in print(text @ number);
//         Que es equivalente a:

//         let number = 42 in (let text = "The meaning of life is" in (print(text @ number)));
//         */
//         public static List<Token> Parsing(string[] semiTokens)
//         {
//             List<Token> tokens = new List<Token>();

//             //First step is to check is the parentheses are balanced
//             if(!BP(semiTokens))
//             {
//                 return tokens;
//             }

//             //Checking if it has the EOL
//             if(semiTokens[semiTokens.Length - 1] != ";")
//             {
//                 //EXEPTION
//                 System.Console.WriteLine("Your are missing a ';' at the end of the input");
//                 return tokens;
//             }

//             //Now lets tokenize
//             for (int i = 0; i < semiTokens.Length - 1; i++)
//             {
//                 //Cheking keywords
//                 if(semiTokens[i] == "print" || semiTokens[i] == "function" || semiTokens[i] == "if" || semiTokens[i] == "else" || semiTokens[i] == "sin" || semiTokens[i] == "cos")
//                 {
//                     //type - value
//                     tokens.Add(new Token("keyword", semiTokens[i]));
//                 }
//                 //Cheking the let - in
//                 //if(semiTokens[i] == "let" && semiTokens[i + 4] )
//             }

//             return tokens;
//         }

//         //Valid Let - In
//         public bool VLI(string[] semiTokens)
//         {
//             //count the variables, the '=' and the values, the have to macth to be a valid let in

//             return true;
//         }

//         //(B)alanced (P)arentheses
//         public static bool BP(string[] input)
//         {
//             int counter = 0;
//             for (int i = 0; i < input.Length; i++)
//             {
//                 if(input[i] == "(") counter++;
//                 if(input[i] == ")")
//                 {
//                     if (counter == 0) 
//                     {
//                         //EXEPTION
//                         System.Console.WriteLine($"You are missing one '(' before the token '{input[i]}' in position {i}");
//                         return false;
//                     }
//                     counter--;
//                 }
//             }

//             if(counter != 0)
//             {
//                 //EXEPTION
//                 System.Console.WriteLine($"You are missing {counter} ')'");
//                 return false;
//             }

//             return true;
//         }
//     }
// }