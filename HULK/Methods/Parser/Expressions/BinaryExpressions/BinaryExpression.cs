using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace HULK
{
    public abstract class BinaryExpression : Expression
    {
        public Expression? LeftNode;
        public Expression? RightNode;
        public TokenKind Operator;

        public BinaryExpression(Expression? left, Expression? right, TokenKind op)
        {
            //Kind = kind;
            LeftNode = left;
            RightNode = right;
            Operator = op;
        }

        abstract public string CheckForSemanticErrors();
    }
}