#pragma once

#include <QString>
#include <QProgressDialog>
#include "Rendering/Renderer.h"
#include "RuleSet.h"
#include "State.h"
#include "ColorPool.h"
#include "ExecutionStack.h"

#include "../../SyntopiaCore/Math/Matrix4.h"
#include "../../SyntopiaCore/GLEngine/EngineWidget.h"


namespace StructureSynth {
	namespace Model {	


		using namespace SyntopiaCore;

		/// A Builder executes the rule set on a Renderer object
		class Builder {
		public:
			Builder(Rendering::Renderer* renderTarget, RuleSet* ruleSet, bool verbose);
			~Builder();
			void build();

			void setCommand(QString command, QString param);
			ExecutionStack& getNextStack();
			State& getState() { return state; };
			Rendering::Renderer* getRenderer() { return renderTarget; };
			void increaseObjectCount() { objects++; };

			// True, if the random seed was changed by the builder (by 'set seed <int>')
			bool seedChanged() { return hasSeedChanged; }
			int getNewSeed() { return newSeed; }
			ColorPool* getColorPool() { return colorPool; }
			QVector<GLEngine::Command> getRaytracerCommands() { return raytracerCommands; };
			bool wasCancelled() { return userCancelled; }

		private:
			void recurseBreadthFirst(QProgressDialog& progressDialog, int& maxTerminated, int& minTerminated, int& generationCounter);
			void recurseDepthFirst(QProgressDialog& progressDialog, int& maxTerminated, int& minTerminated, int& generationCounter);
		
			State state;

			bool userCancelled;
			
			ExecutionStack stack;
			ExecutionStack nextStack;
			Rendering::Renderer* renderTarget;
			RuleSet* ruleSet;
			bool verbose;
			int maxGenerations;
			int maxObjects;
			int objects;
			int newSeed;
			bool hasSeedChanged;
			float minDim;
			float maxDim;
			bool syncRandom;
			int initialSeed;
			State* currentState;
			ColorPool* colorPool;
			QVector<GLEngine::Command> raytracerCommands;
		};

		


	}
}

