using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace HULK
{
    public class Subtraction : BinaryExpression
    {
        public override ExpressionKind Kind { get; set; }
        public override object? Value { get; set; }

        public Subtraction(Expression? left, Expression? right, TokenKind op) : base(left, right, op)
        {
            Kind = ExpressionKind.Subtraction;
            Operator = TokenKind.Minus;
        }


        public override void Evaluate()
        {
            LeftNode?.Evaluate();
            RightNode?.Evaluate();
            Value = (double)LeftNode!.Value - (double)RightNode!.Value;
        }
        public override string CheckForSemanticErrors()
        {
            if (LeftNode == null)
            {
                return "Left node is null";
            }
            else if (RightNode == null)
            {
                return "Right node is null";
            }
            else if (LeftNode.Kind != ExpressionKind.Number)
            {
                return "Left or right node is not a number";
            }
            else if (RightNode.Kind != ExpressionKind.Number)
            {
                return "Left or right node is not a number";

            }
            else
                return "";
        }

        public override object GetValue() => Value!;
    }

}