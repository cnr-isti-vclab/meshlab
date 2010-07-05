
#include "ooc_vector.h"
#include <vcg/math/random_generator.h>


struct Logging * lgn;

// define your data type
struct yourtype{ char a[1000]; void Init(){memset(a,0,1000);}};
struct big{ char a[20000]; void Init(){memset(a,0,20000);}};


int Zero(){return 0;}

//
//int main(int,char**){
//
//		SimpleDb * s = new SimpleDb("simple");
//		s->Create("simple");
//
//		yourtype a;a.Init();
//
//		for(unsigned int j =0 ; j < 1100; ++j){
//				char t[100];
//				sprintf(t,"%d",j);
//				sprintf(a.a,"+++ %d",j);
//				s->Put(t,&a,1000);
//		}
//
//		big b;
//		s->Put("biggo",&b,sizeof(big));
//
//		delete s;
//
//		s = new SimpleDb("simple");
//		s->Open("simple");
//
//
//		for(unsigned int j =0 ; j < 1100; ++j){
//				char t[100];
//				sprintf(t,"%d",j);
//				s->Get(t,&a,sizeof(yourtype));
//				printf("%s \n",a.a);
//		}
//
//		void * tmp;
//		s->Get("biggo", tmp);
//
//		delete s;
//
//}

//
////
int main(int argc, char **){

	lgn = new Logging("ooc_vector_log.txt");


	STAT::Begin(N_STAT);
	printf("BEGIN:::::::::::::::");

	// define the environment for the ooc_vector
	OOCEnv env;
	env.params.blockSizeBytes = 1024;

	// establish a correspondance between name and type in this environment
	env.AddNameTypeBound<yourtype>("yourtype");

	// assign the allocator

		bool crea = (argc>1);

		if(crea)
				{lgn = new Logging("log_crea.txt");	 env.Create("data"); 	}
	else
				{lgn = new Logging("log_usa.txt"); env.Open("data_ID.socm");	}

		// declare the ooc_vector
		ooc_vector<yourtype> vect;

		/*
			assign (OOCEvn  & environment,"name",if_the_vector_with_this_name_does_not_exists_create_it
		*/
		vect.assign(env,"aaaaaaaa",true);



/*
	*/
		if(crea){
		for(unsigned int ii = 0 ; ii < 50000; ++ii)
			{
			yourtype yt; yt.Init();
			*(int* )&yt.a[0] = ii;
			vect.push_back(yt);
		}
		}else
		{
				for(unsigned int ii = 0 ; ii < 50000; ++ii){
				yourtype yt = vect[ii];
				assert(*(int* )&yt.a[0] == ii);
		}
		}
		env.Close();
}
