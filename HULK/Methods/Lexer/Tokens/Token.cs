namespace HULK
{
    public abstract class Token
    {
        abstract public TokenKind Kind { get; set; }

        public Token(TokenKind kind)
        {
            Kind = kind;
        }

        public abstract object GetValue();
    }
}
