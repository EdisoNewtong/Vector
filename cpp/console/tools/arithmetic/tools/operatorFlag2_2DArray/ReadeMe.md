# The function of the  "operatorFlag2_2DArray"  Folder  
***==Convert to the new 2D Array format from the following old format==***  

//
// Old Format
//
const TokenMgr::OpPairCfg TokenMgr::s_OpPairCfgTable[s_TABLE_SIZE] = {
    /* left,                right,  closeAvaliable,  seperateAvaliable  */
    // ?                      +
    { E_ADD,                E_ADD,       false,         true  },   // ++ is not avaliable ,    + + is avaliable
    { E_MINUS,              E_ADD,       true,          true  },

		...
};



//
// After Converted 
//
Array2D = { 
	{   //  E_ADD
		{  BoolPairInfo( false, true ) },	 // +  + 
		{  BoolPairInfo( true, true ) },	 // +  - 
		  ...
	},
	{   //  E_MINUS
		{  BoolPairInfo( false, true ) },	 // +  + 
		{  BoolPairInfo( true, true ) },	 // +  - 
		  ...
	},

		...

};


