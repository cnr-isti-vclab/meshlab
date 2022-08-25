#ifndef _Q_BOOL_
#define _Q_BOOL_

/*
stupid class for a bool in temporla mark style
*/
struct FBool{
	FBool ():globalCounter(NULL),localCounter(0){ };
	FBool ( unsigned int * _globalCounter):globalCounter(_globalCounter){}

	bool operator ()(){ return (localCounter)?(localCounter == *globalCounter):false;} 
	void operator = (bool value){   localCounter=(value)? *globalCounter:0;} 
	bool operator !() { return  ! (*this)();} 
	const unsigned int & LocalCounter(){return localCounter;} 	 
private:
	unsigned int * globalCounter;	// global counter
	unsigned int localCounter;	// local counter
};

#endif
