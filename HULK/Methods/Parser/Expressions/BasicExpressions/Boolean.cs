using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace HULK
{
    public class Boolean : BasicExpression
    {
        public override ExpressionKind Kind { get; set; }
        public override object Value { get; set; }

        public Boolean(bool value)
        {
            Value = value;
            Kind = ExpressionKind.Boolean;
        }
        public override void Evaluate()
        {
            throw new NotImplementedException();
        }

        public override object GetValue() => Value;
    } 
    
}