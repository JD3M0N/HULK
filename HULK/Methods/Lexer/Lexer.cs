
using Microsoft.VisualBasic.CompilerServices;
using System.Text;
using System.ComponentModel.DataAnnotations;
using System.Data;
using System.Text.RegularExpressions;
using System.Security.Cryptography.X509Certificates;

namespace HULK
{
    public class Lexer
    {
        //The variable "input" is the sentence that is going to be split or scanned.
        public string Input;
        public int currentIndex;
        public char currentChar;
        
        public Lexer(string input)
        {
            Input = input;
            currentIndex = 0;
            currentChar = Input[currentIndex];

        }

        void Move()
        {
            currentIndex++;

            if (currentIndex >= Input.Length)
                return;

            currentChar = Input[currentIndex];
        }

        void Move(int positions)
        {
            currentIndex += positions;
            if (currentIndex >= Input.Length)
                return;

            currentChar = Input[currentIndex];
        }

        char LookAhead()
        {

            if (currentIndex + 1 >= Input.Length)
                return ' ';
            return Input[currentIndex + 1];
        }

        char LookBack()
        {
            if (currentIndex - 1 < 0)
                return ' ';
            return Input[currentIndex - 1];
        }


        public List<Token> Tokenizer()
        {

            List<Token> tokens = new List<Token>();
            
            while (currentIndex < Input.Length)
            {
                if (char.IsWhiteSpace(currentChar))
                    Move();

                if (currentChar == '"')
                    tokens.Add(String());

                if (char.IsDigit(currentChar))
                    tokens.Add(Number());

                if (IsLetter(currentChar))
                    tokens.Add(Identifier());

                if (IsOperator(currentChar))
                    tokens.Add(Operator());

                if (IsPunctuator(currentChar))
                    tokens.Add(Punctuator());
            }

            if(tokens.Last().Kind != TokenKind.Semicolon)
            {
                Console.WriteLine($"!lexical error: expressions must finish with \"{TokenKind.Semicolon}\"");
                throw new Exception();
            }

            tokens.Add(new CommonToken(TokenKind.EOF));

            return tokens;
        }

        private Token String()
        {
            string str = string.Empty;
            Move();

            while (currentIndex < Input.Length)
            {
                str += currentChar;
                Move();
                if (currentChar == '"')
                {
                    Move();
                    return new DataToken(TokenKind.String, str);
                }
            }

            Console.WriteLine($"!lexical error: string literal is incomplete at {currentIndex}.");
            throw new Exception();
        }

        private Token Number()
        {
            string number = string.Empty;
            bool flag = false;
            //3.2.3

            while (currentIndex < Input.Length && (char.IsDigit(currentChar) || currentChar == '.'))
            {
                if (currentChar == '.')
                {
                    if (flag)
                    {
                        Console.WriteLine($"!lexical error: pon esto tu");
                        throw new Exception();
                    }
                    else
                    {
                        flag = true;
                        number += currentChar;
                        Move();
                        continue;
                    }
                }

                if (IsLetter(LookAhead()))
                {
                    Console.WriteLine($"!lexical error: pon esto tu");
                    throw new Exception();
                }

                number += currentChar;
                Move();
            }



            return new DataToken(TokenKind.Number, double.Parse(number));

        }

        private Token Identifier()
        {
            string id = string.Empty;

            while (IsLetterOrDigit(currentChar))
            {
                id += currentChar;
                Move();
            }

            if (IsKeyWord(id))
                return KeyWord(id);

            return new DataToken(TokenKind.Identifier, id);
        }

        private Token Operator()
        {
            if (currentChar == '+')
            {
                Move();
                return new CommonToken(TokenKind.Plus);
            }

            else if (currentChar == '-')
            {
                Move();
                return new CommonToken(TokenKind.Minus);
            }

            else if (currentChar == '*')
            {
                Move();
                return new CommonToken(TokenKind.Multiply);
            }

            else if (currentChar == '/')
            {
                Move();
                return new CommonToken(TokenKind.Divide);
            }

            else if (currentChar == '%')
            {
                Move();
                return new CommonToken(TokenKind.Modulus);
            }

            else if (currentChar == '^')
            {
                Move();
                return new CommonToken(TokenKind.Power);
            }

            else if (currentChar == '=' && LookAhead() == '=')
            {
                Move(2);
                return new CommonToken(TokenKind.EqualsTo);
            }

            else if (currentChar == '<' && LookAhead() == '=')
            {
                Move(2);
                return new CommonToken(TokenKind.LesserOrEquals);
            }

            else if (currentChar == '>' && LookAhead() == '=')
            {
                Move(2);
                return new CommonToken(TokenKind.GreatherOrEquals);
            }

            else if (currentChar == '!' && LookAhead() == '=')
            {
                Move(2);
                return new CommonToken(TokenKind.NotEqualsTo);
            }

            else if (currentChar == '=' && LookAhead() == '>')
            {
                Move(2);
                return new CommonToken(TokenKind.Lambda);
            }

            else if (currentChar == '<')
            {
                Move();
                return new CommonToken(TokenKind.LesserThan);
            }

            else if (currentChar == '>')
            {
                Move();
                return new CommonToken(TokenKind.GreatherThan);
            }

            else if (currentChar == '!')
            {
                Move();
                return new CommonToken(TokenKind.Not);
            }
            else if (currentChar == '@')
            {
                Move();
                return new CommonToken(TokenKind.Concatenate);
            }
            else 
            {
                Move();
                return new CommonToken(TokenKind.Equals);
            }
        }

        private Token Punctuator()
        {
            switch (currentChar)
            {
                case '(':
                    Move();
                    return new CommonToken(TokenKind.LeftParenthesis);
                case ')':
                    Move();
                    return new CommonToken(TokenKind.RightParenthesis);
                case ';':
                    Move();
                    return new CommonToken(TokenKind.Semicolon);
                default:
                    Move();
                    return new CommonToken(TokenKind.Comma);
            }
        }

        private Token KeyWord(string identifier)
        {
            switch (identifier)
            {
                case "true":
                    return new DataToken(TokenKind.Bool, true);

                case "false":
                    return new DataToken(TokenKind.Bool, false);

                case "function":
                    return new CommonToken(TokenKind.FunctionKeyWord);
                    
                case "let":
                    return new CommonToken(TokenKind.LetKeyWord);

                case "in":
                    return new CommonToken(TokenKind.InKeyWord);

                case "if":
                    return new CommonToken(TokenKind.IfKeyWord);

                default:
                    return new CommonToken(TokenKind.ElseKeyWord);
            }
        }

        private bool IsOperator(char currentChar)
        {
            List<char> operators = new List<char>() { '+', '-', '*', '/', '%', '^', '=', '>', '<', '!', '@' };
            return operators.Contains(currentChar);
        }

        private bool IsPunctuator(char currentChar)
        {
            List<char> punctuators = new List<char>() { '(', ')', ',', ';' };
            return punctuators.Contains(currentChar);
        }

        private bool IsKeyWord(string identifier)
        {
            List<string> keywords = new List<string>() { "true", "false", "if", "else", "function", "let", "in" };
            return keywords.Contains(identifier);
        }


        private static bool IsLetterOrDigit(char c) => char.IsLetterOrDigit(c) || c == '_';

        private static bool IsLetter(char c) => char.IsLetter(c) || c == '_';
    }
}
