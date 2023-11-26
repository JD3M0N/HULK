namespace HULK
{

    public class Token
    {
        /* Token Type: This attribute represents the type of the token. 
        It could be a keyword, identifier, operator, delimiter, literal, or any other type of token that the language supports. 
        The token type is essential for the parser to understand how to handle the token in the context of the language's syntax.
        */
        public string TokeType;
        /*Value: This attribute stores the actual text or value of the token. 
        For example, if the token is an identifier, the value could be the name of the variable.
        If the token is a literal, the value could be the actual literal value.
        */
        public string Value;

        public Token(string tokenType, string value)
        {
            TokeType = tokenType;
            Value = value;
        }

    }
}
