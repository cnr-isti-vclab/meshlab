#ifndef __BOOLVECTOR__
#define __BOOLVECTOR__

#include <vector>

struct BoolVector{
	BoolVector():mode(false){}

	/* mode = 0, the set of marked elements are contained in the vector marked_elements		*/
	/* mode = 1, the set of marked elements correspond to truth value containedin is_marked */
	bool mode;		

	std::vector<unsigned int> marked_elements;
	std::vector<bool> is_marked;

	void SetMode(bool _mode){
								if(mode==_mode) return;
								 
								if(!mode){
									if(!marked_elements.empty()){
										std::sort(marked_elements.begin(),marked_elements.end());
										int max_value = marked_elements.back();
										is_marked.resize(max_value+1);
										unsigned int i = 0, j = 0;
										for(std::vector<unsigned int>::iterator  id = marked_elements.begin(); id != marked_elements.end();++id,++i){
											for( ; j < (*id);++j)  is_marked[j] = false;
											is_marked[(*id)] = true;
											++j;
										}
									}
								}else{
									marked_elements.clear();
									unsigned int i = 0;
									for(i = 0; i <is_marked.size(); ++i)
										if(is_marked[i])
											marked_elements.push_back(i);
								}
								mode = _mode;
	}

	void SetAsVectorOfBool(){SetMode(true);};
	void SetAsVectorOfMarked(){SetMode(false);};

	bool IsMarked(const unsigned int & i){
		assert(mode);						// disallow query when the vector is used (inefficient)
	
		return	(i >= is_marked.size())?	// if the index is greater of than the size of the vector it is
				false:is_marked[i];			// false for sure (i.e. it has not been marked 
	}

	void SetMarked(const unsigned int & i, bool value){
		if(!mode){ 
			RAssert(value);			// disallow unsetting when the vector is used
			// if(value)
			  marked_elements.push_back(i); 
			return;
		}else{
			if(i >= is_marked.size())
				is_marked.resize(i+1);
			if(!is_marked[i] && value)         // marked_elements is kept valid as long as the are only setting to true
				marked_elements.push_back(i); 
			is_marked[i] = value;	
		}
	}

	void Clear(){
		is_marked.clear();
		marked_elements.clear();
		}
	bool Empty(){
		if(mode) return is_marked.empty();
		else return marked_elements.empty();
		}

	bool IsModeVectorOfMarked(){return !mode;}
	bool IsModeVectorOfBool(){return mode;}
};

#endif
