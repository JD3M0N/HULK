using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.ComponentModel;

namespace HULK
{
    public static class DataCenter
    {
        //the diccionary where we save the user's numeric variables.
        public static Dictionary<string, double> UserVars;
        //Dictionary to keep the methods
        public static Dictionary<string, List<Token>> UserMethods;


        //Method to initialize the data center class
        public static void DataCenterAwake()
        {
            //key => variables || values => variables values
            Dictionary<string, double> userVars= new Dictionary<string, double>();
            UserVars = userVars;
            //key => methods || values => token list that makes the method
            Dictionary<string, List<Token>> userMethods= new Dictionary<string, List<Token>>();
            UserMethods = userMethods;
        }
    }
}