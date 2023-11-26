using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace HULK
{
    public class Concatenation : BinaryExpression
    {
        public override ExpressionKind Kind { get; set; }
        public override object? Value { get; set; }

        public Concatenation(Expression? left, Expression? right, TokenKind op) : base(left, right, op)
        {
            Kind = ExpressionKind.Concatenation;
            Operator = TokenKind.Plus;
        }

        public override void Evaluate()
        {
            LeftNode?.Evaluate();
            RightNode?.Evaluate();
            Value = (string)LeftNode!.Value + (string)RightNode!.Value;
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
            else if (LeftNode.Kind != ExpressionKind.String)
            {
                return "Left or right node is not a string";
            }
            else if (RightNode.Kind != ExpressionKind.String)
            {
                return "Left or right node is not a string";

            }
            else
                return "";
        }

        public override object GetValue() => Value!;
    }
   
}