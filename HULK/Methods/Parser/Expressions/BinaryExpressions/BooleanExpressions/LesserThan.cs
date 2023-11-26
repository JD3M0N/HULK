using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace HULK
{
    public class LesserThan : BinaryExpression
    {
        public override ExpressionKind Kind { get; set; }
        public override object? Value { get; set; }

        public LesserThan(Expression left, Expression right, TokenKind operation) : base(left, right, operation)
        {
            Kind = ExpressionKind.LesserThan;
            Operator = TokenKind.LesserThan;
        }

        public override void Evaluate()
        {
            LeftNode?.Evaluate();
            RightNode?.Evaluate();
            Value = (double)LeftNode!.Value < (double)RightNode!.Value;
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