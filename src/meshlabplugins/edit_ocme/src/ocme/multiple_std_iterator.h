

/*
stupid class to allow to run over a list of containers with the same type in a single "for" cycle

example:

	// take two vectors of int
	std::vector<int> a,b; 

	// fill them
	for(int i = 0; i <10 ; ++i)
		a.push_back(i);

	for(int i = 0; i <10 ; ++i)
		b.push_back(i);

	// create a MCont
	MCont<std::vector<int> > mc(a);
	mc.Plus(b);

	// run over all the elements as if they were in one vector (Oh my GOodness)
	for(MIte<std::vector<int> > i = mc.begin(); i != mc.end(); ++i) 
	{printf("%d\n",(*i));}
	

*/

template <class CONT_TYPE>
struct MCont;

template <class CONT_TYPE>
struct MIte{

	MIte(){}
	MIte(MCont<CONT_TYPE> * _cnt, const int & _ci, typename CONT_TYPE::iterator  _ite):cnt(_cnt),ci(_ci),ite(_ite){}

		void operator++(){
		ite++;
		if(ite == cnt->conts[ci]->end())
		{
			if( ci != cnt->conts.size()-1){
				ci++;
				ite = cnt->conts[ci]->begin();
				}
		}
	}
	
	typename CONT_TYPE::value_type operator*(){
			return (*ite);
		}
	
	bool operator !=( const MIte &r) const { 
		return	( cnt !=r.cnt) || (ci!=r.ci) || (ite!=r.ite);	
	}

	MIte operator =( const MIte &r) const { 
		ite = r.ite;
		ci = r.ci;
		cnt = r.cnt;
		return (*this);
	}
	typename CONT_TYPE::iterator ite;
	int ci;
	MCont<CONT_TYPE> * cnt;

};

template <class CONT_TYPE>
struct MCont{

	MCont( CONT_TYPE & c){
		Plus (c);
	}
		
	std::vector<CONT_TYPE*> conts;


	MCont & Plus( CONT_TYPE & c){
		conts.push_back(&c);
		return *this;
	}

	MIte<CONT_TYPE>   begin()  { return MIte<CONT_TYPE>(this,0,conts[0]->begin());}
	MIte<CONT_TYPE>   end()    { return MIte<CONT_TYPE>(this,conts.size()-1,conts.back()->end());}

};
