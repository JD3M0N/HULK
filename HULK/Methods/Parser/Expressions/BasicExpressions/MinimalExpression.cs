using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace HULK
{
    public class MinimalExpression : BasicExpression
    {
        public override ExpressionKind Kind { get; set; }
        public override object Value { get; set; }

        public override void Evaluate()
        {
            throw new NotImplementedException();
        }

        public override object GetValue()
        {
            throw new NotImplementedException();
        }
    }
}