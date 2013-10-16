#include "State.h"


namespace StructureSynth {
	namespace Model {	

		State::State() : 
			matrix(SyntopiaCore::Math::Matrix4f::Identity()), 
			hsv(SyntopiaCore::Math::Vector3f(0,1.0f,1.0f)), 
			alpha(1.0f), previous(0), seed(0)  { 
			
		}

		State& State::operator=(const State& rhs){
			this->matrix = rhs.matrix;
			this->hsv = rhs.hsv;
			this->alpha = rhs.alpha;
			this->maxDepths = rhs.maxDepths;
			this->seed = rhs.seed;
			if (rhs.previous) {
				delete(this->previous);
				this->previous = new PreviousState();
				*(this->previous) = *rhs.previous;
			} else {
				delete(this->previous);
				this->previous = 0;
			}
			return *this;
		}

		void State::setPreviousState(SyntopiaCore::Math::Matrix4f matrix,SyntopiaCore::Math::Vector3f hsv, float alpha) {
			if (previous) {delete (previous); }

			this->previous = new PreviousState();
			this->previous->matrix = matrix;
			this->previous->hsv = hsv;
			this->previous->alpha = alpha;
		}

		
		State::State(const State& rhs) : matrix(rhs.matrix), 
			hsv(rhs.hsv), 
			alpha(rhs.alpha), maxDepths(rhs.maxDepths), previous(0), seed(rhs.seed) {

			if (rhs.previous) {
				delete(this->previous);
				this->previous = new PreviousState();
				*(this->previous) = *rhs.previous;
			} else {
				delete(this->previous);
				this->previous = 0;
			}
		}
		


		State::~State() { 
			delete(previous);
		}
	}
}

