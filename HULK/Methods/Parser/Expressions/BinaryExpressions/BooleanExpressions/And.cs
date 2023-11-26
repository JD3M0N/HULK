using System.Linq.Expressions;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace HULK
{
    public class And : BinaryExpression
    {
        public override ExpressionKind Kind { get; set; }
        public override object? Value { get; set; }

        public And(Expression left, Expression right, TokenKind operation) : base(left, right, operation)
        {
            Kind = ExpressionKind.And;
            Operator = TokenKind.And;
        }

        public override void Evaluate()
        {
            LeftNode?.Evaluate();
            RightNode?.Evaluate();
            Value = (bool)LeftNode!.Value && (bool)RightNode!.Value;
        }
        public override string CheckForSemanticErrors()
        {
            if(LeftNode == null)
            {
                return "Left node is null";
            }
            else if(RightNode == null)
            {
                return "Right node is null";
            }
            else if(LeftNode.Kind != ExpressionKind.Boolean)
            {
                return "Left or right node is not a boolean";
            }
            else if(RightNode.Kind != ExpressionKind.Boolean)
            {
                return "Left or right node is not a boolean";
            }
            else
                return "";
        }


        public override object GetValue() => Value!;
    }
}