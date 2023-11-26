using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace HULK
{
    public class AST
    {
        List<Token> tokens;
        int position;
        Token currentToken;

        public AST(List<Token> _tokens)
        {
            this.tokens = _tokens;
            position = 0;
            currentToken = tokens[position];
        }

        public void NextToken()
        {
            position++;
            if (position < tokens.Count)
            {
                currentToken = tokens[position];
            }
        }

        public void NextToken(int n)
        {
            position += n;
            if (position < tokens.Count)
            {
                currentToken = tokens[position];
            }
        }
        public void PreviousToken()
        {
            position--;
            if (position >= 0)
            {
                currentToken = tokens[position];
            }
        }

        public Expression BuildAST()
        {
            if (tokens.Count <= 1)
            {
                //Exepction
                System.Console.WriteLine("Error: No tokens");
            }

            Expression root = GetExpressionLv1();

            return root;
        }

        #region GetExpressionLv
        private Expression GetExpressionLv1()
        {
            Expression Left = GetExpressionLv2();

            while (IsINLv1(currentToken.Kind))
            {
                Token op = currentToken;
                NextToken();
                Expression Right = GetExpressionLv2();
                Left = GetBinaryExpression(Left, op, Right);
            }

            Expression node = Left;
            return node;
        }


        private Expression GetExpressionLv2()
        {
            Expression Left = GetExpressionLv3();

            while (IsINLv2(currentToken.Kind))
            {
                Token op = currentToken;
                NextToken();
                Expression Right = GetExpressionLv3();
                Left = GetBinaryExpression(Left, op, Right);
            }

            Expression node = Left;
            return node;
        }


        private Expression GetExpressionLv3()
        {
            Expression Left = GetLiterals();

            while (IsINLv3(currentToken.Kind))
            {
                Token op = currentToken;
                NextToken();
                Expression Right = GetLiterals();
                Left = GetBinaryExpression(Left, op, Right);
            }

            Expression node = Left;
            return node;
        }

        #endregion
        private Expression GetBinaryExpression(Expression left, Token op, Expression right)
        {

            //esto pincha?
            BinaryExpression binaryExpression;
            
            switch (op.Kind)
            {
                // ===>>> LV3 BinaryOperations <<<===
                case TokenKind.Plus:
                    BinaryExpression Plus = new Addition(left, right, op.Kind);
                    Plus.CheckForSemanticErrors();
                    binaryExpression = Plus;
                    break;
                case TokenKind.Minus:
                    BinaryExpression Minus = new Subtraction(left, right, op.Kind);
                    Minus.CheckForSemanticErrors();
                    binaryExpression = Minus;
                    break;
                case TokenKind.Concatenate:
                    BinaryExpression Concatenate = new Concatenation(left, right, op.Kind);
                    Concatenate.CheckForSemanticErrors();
                    binaryExpression = Concatenate;
                    break;
                // ===>>> EndOfLv3  <<<===

                // ==>> LV2 BinaryOperations <<==
                case TokenKind.Multiply:
                    BinaryExpression Multiply = new Multiplication(left, right, op.Kind);
                    Multiply.CheckForSemanticErrors();
                    binaryExpression = Multiply;
                    break;
                case TokenKind.Divide:
                    BinaryExpression Divide = new Division(left, right, op.Kind);
                    Divide.CheckForSemanticErrors();
                    binaryExpression = Divide;
                    break;
                case TokenKind.Modulus:
                    BinaryExpression Modulus = new Modulus(left, right, op.Kind);
                    Modulus.CheckForSemanticErrors();
                    binaryExpression = Modulus;
                    break;
                // ==>> EndOfLv2  <<==

                // => LV1 BinaryOperations <=
                case TokenKind.Power:
                    BinaryExpression Power = new Power(left, right, op.Kind);
                    Power.CheckForSemanticErrors();
                    binaryExpression = Power;
                    break;
                // => EndOfLv1  <=

                // () Error ()
                default:
                    System.Console.WriteLine("Error: No operator");
                    throw new Exception();
            }

            return binaryExpression;
        }

        #region IsINLv
        private bool IsINLv1(TokenKind kind)
        {
            switch (kind)
            {
                case TokenKind.Plus:
                    return true;
                case TokenKind.Minus:
                    return true;
                case TokenKind.Concatenate:
                    return true;
                default:
                    return false;
            }
        }

        private bool IsINLv2(TokenKind kind)
        {
            switch (kind)
            {
                case TokenKind.Multiply:
                    return true;
                case TokenKind.Divide:
                    return true;
                case TokenKind.Modulus:
                    return true;
                default:
                    return false;
            }
        }

        private bool IsINLv3(TokenKind kind)
        {
            switch (kind)
            {
                case TokenKind.Power:
                    return true;
                default:
                    return false;
            }
        }

        #endregion
        private Expression GetLiterals()
        {
            switch(currentToken.Kind)
            {
                // >>==Most Basic Expression==<<
                case TokenKind.Number:
                    Number numberLiteral = new Number((double)currentToken.GetValue());
                    NextToken();
                    return numberLiteral;
                case TokenKind.String:
                    String stringLiteral = new String((string)currentToken.GetValue());
                    NextToken();
                    return stringLiteral;
                case TokenKind.Bool:
                    Boolean boolLiteral = new Boolean((bool)currentToken.GetValue());
                    NextToken();
                    return boolLiteral;
                // >>==End Of Most Basic Expression==<<

                // () Error ()
                default:
                    System.Console.WriteLine("Error: No literal");
                    throw new Exception();
            }
        }
    }
}